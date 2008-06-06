#include <ctype.h>
#include <string.h>

#include "misc.h"

typedef unsigned char uchar;

#define EXPRMAX 8

enum expr_operations {
oper_minus,
oper_plus,
oper_multiply,
oper_divide,
oper_and,
oper_or,
oper_xor,
oper_lshift,
oper_rshift,
oper_end,
};

#define PRI_08    0x08
#define PRI_10    0x10
#define PRI_18    0x18
#define PRI_20    0x20
#define PRI_28    0x28
#define PRI_30    0x30


typedef struct expr_element {
	double value;
	char *string;
	int operation;
	int priority;
} ee;


typedef struct expr_context {
	bc *bc;
	ee exprstack[EXPRMAX];
	ee *exprsp;
	int exprflag;
	int oppri;
	int opop;
	double operval;
	char *operstring;
	int exprflags;
	char *pnt;
	einfo *ei;
} ec;


int expr2(ec *ec);
void operand(ec *ec);
void operator(ec *ec);
int trytop(ec *ec);

void unbalancedq(ec *ec)
{
}



int at(ec *ec)
{
	return *ec->pnt;
}

int get(ec *ec)
{
	return *ec->pnt++;
}

int back(ec *ec)
{
	return *--ec->pnt;
}

int expr2(ec *ec);

int expr(bc *bc, char **take, einfo *ei)
{
ec ecl, *ec = &ecl;
int res;

	ei->flags_out = 0;

	ec->bc = bc;
	ec->pnt = *take;
	ec->exprsp=ec->exprstack;
	ec->exprflag=0;
	ec->ei = ei;
	res = expr2(ec);
//	if(ec->exprflag & 1) unbalanced();
//	if(ec->exprflag & 2) badoperation();
	*take = ec->pnt;
	return 0;
}
/*uchar opchars[]={'+','-','/','*','|','&','<<','>>','!'};*/

int expr2(ec *ec)
{
	++ec->exprsp;
	ec->exprsp->priority = 0; // endmark
	if(at(ec)=='-') // unary minus
	{
		get(ec);
		++ec->exprsp;
		ec->exprsp->value = 0.0;
		ec->exprsp->string = 0;
		ec->exprsp->operation = oper_minus;
		ec->exprsp->priority = PRI_10;
	}
	for(;;)
	{
		operand(ec);
		operator(ec);
		if(trytop(ec)) break;

		++ec->exprsp;
		ec->exprsp->value = ec->operval;
		ec->exprsp->string = ec->operstring;
		ec->exprsp->operation = ec->opop;
		ec->exprsp->priority = ec->oppri;
	}
	--ec->exprsp;
	ec->ei->value = ec->operval;
	return 0;
}

int trytop(ec *ec)
{
	int topop;
	char *topstring;
	double topval;

	for(;;)
	{
		if(ec->oppri > ec->exprsp->priority)
			return ec->oppri==PRI_08;
		topop = ec->exprsp->operation;
		topval = ec->exprsp->value;
		topstring = ec->exprsp->string;
		--ec->exprsp;

		switch(topop)
		{
			case oper_plus: /* + */
				ec->operval+=topval;
				break;
			case oper_minus: /* - */
				ec->operval=topval-ec->operval;
				break;
			case oper_divide: /* / */
				ec->operval=topval/ec->operval;
				break;
			case oper_multiply: /* * */
				ec->operval*=topval;
				break;
			case oper_or: /* | */
				ec->operval=(int)ec->operval | (int)topval;
				break;
			case oper_and: /* & */
				ec->operval=(int)ec->operval | (int)topval;
				break;
			case oper_lshift: /* << */
				ec->operval=(int)topval<<(int)ec->operval;
				break;
			case oper_rshift: /* >> */
				ec->operval=(int)topval>>(int)ec->operval;
				break;
			case oper_end: return 1;
		}
	}
}

void operator(ec *ec)
{
uchar ch;

	ch=get(ec);
	switch(ch)
	{
		case '+': ec->oppri=PRI_10;ec->opop=oper_plus;break;
		case '-': ec->oppri=PRI_10;ec->opop=oper_minus;break;
		case '/': ec->oppri=PRI_18;ec->opop=oper_divide;break;
		case '*': ec->oppri=PRI_18;ec->opop=oper_multiply;break;
		case '|': ec->oppri=PRI_20;ec->opop=oper_or;break;
		case '&': ec->oppri=PRI_28;ec->opop=oper_and;break;
		case '<':
			if(get(ec)!='<') back(ec);
			ec->oppri=PRI_30;ec->opop=6;break;
		case '>':
			if(get(ec)!='>') back(ec);
			ec->oppri=PRI_20;ec->opop=7;break;
		default:
			back(ec);ec->oppri=PRI_08;ec->opop=oper_end;
	}
}

/*
+ 010
- 110
/ 218,20f
* 318
| 420
& 528
<< 630
>> 730
. , ( ) white ; 008
*/

struct variable *find_v_low(bc *bc, char *name)
{
struct variable *v;
int low, mid, high;
	v=bc->vars;
	low=0;
	high=bc->numvariables;
	for(;;)
	{
		mid=(low + high) >> 1;
		if(mid==low) break;
		if(strcmp(name, v[mid].name)<0)
			high=mid;
		else
			low=mid;
	}
	return v+mid;
}

struct variable *find_variable(bc *bc, char *name)
{
struct variable *v;
	v=find_v_low(bc, name);
	if(!strcmp(name, v->name))
		return v;
	return 0;
}

struct variable *add_variable(bc *bc, char *name)
{
struct variable *v;
int which;
	v=find_v_low(bc, name);
	which=v-bc->vars;
	memmove(v+1, v, sizeof(*v)*(bc->numvariables - which));
	if(which < bc->numvariables && strcmp(name, v->name)>0)
		++v,++which;
	v->rank = RANK_VARIABLE;
	strcpy(v->name, name);
	v->value = 0.0;
	v->data = 0;
	++ bc->numvariables;
#warning check for out of variables needed
	return v;
}

/*
A
A$
A(
A$(
*/

int gather_variable_name(bc *bc, char *put, char **take)
{
int n;
int type=0;
	n=0;
	put[n] = tolower(*(*take));
	if(!isalpha(put[0]))
		return RANK_INVALID;
	++*take;
	++n;
	put[n] = tolower(*(*take));
	if(isalpha(put[1]) || isdigit(put[1]))
	{
		++*take;++n;
	}
	if(**take == '$')
	{
		put[n++] = *(*take)++;
		type |= RANK_STRING;
	}
	if(**take == '(')
	{
		put[n++] = *(*take)++;
		type |= RANK_ARRAY;
	} else type=RANK_VARIABLE;
	put[n]=0;
	return type;
}

/* fills in operval and opertype, leaves pointer on character stopped on */
void operand(ec *ec)
{
	uchar ch;

	ch=at(ec);
	if((ch>='0' && ch<='9') || ch=='.')
	{
		double intpart;

		intpart = 0.0;
		while(isdigit(ch=get(ec))) {intpart*=10;intpart+=ch-'0';}
		if(ch=='.')
		{
			double digit=0.1;
			double fracpart=0.0;
			while(isdigit(ch=get(ec)))
			{
				fracpart += digit * (ch - '0');
				digit /= 10.0;
			}
			ec->operval = intpart + fracpart;
		} else
			ec->operval = intpart;
		back(ec);
	} else if(ch=='\'')
	{
		get(ec);
		ec->operval=0.0;
		while((ch=get(ec)))
		{
			if(ch=='\n' || !ch) {back(ec);unbalancedq(ec);break;}
			if(ch=='\'')
				if(get(ec)!='\'') {back(ec);break;}
			ec->operval*=256;ec->operval+=ch;
		}
	} else if(ch=='(')
	{
		int res;
		get(ec);
		res=expr2(ec);
		ec->operval=ec->ei->value;
		if(get(ec)!=')') {ec->exprflag|=1;back(ec);}
	}  else
	{
		char name[16];
		struct variable *v;

		if(gather_variable_name(ec->bc, name, &ec->pnt) != RANK_INVALID)
		{
			v=find_variable(ec->bc, name);
			if(!v)
				v=add_variable(ec->bc, name);
#warning check for ran out of variables
			ec->operval = v->value;
		}
	}
}
