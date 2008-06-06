#include <stdlib.h>
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
oper_assign,
};

#define PRI_08    0x08
#define PRI_0c    0x0c
#define PRI_10    0x10
#define PRI_18    0x18
#define PRI_20    0x20
#define PRI_28    0x28
#define PRI_30    0x30

typedef struct expr_element {
	double value;
	bstring *string;
	int operation;
	int priority;
	int type;
} ee;


typedef struct expr_context {
	bc *bc;
	ee exprstack[EXPRMAX];
	ee *exprsp;
	int exprflag;
	ee tos;
//	int priority;
//	int operation;
//	int type;
//	double operval;
//	bstring *operstring;
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
		ec->exprsp->type = OT_DOUBLE;
	}
	for(;;)
	{
		operand(ec);
		operator(ec);
		if(trytop(ec)) break;

		++ec->exprsp;
		ec->exprsp->value = ec->tos.value;
		ec->exprsp->string = ec->tos.string;
		ec->exprsp->operation = ec->tos.operation;
		ec->exprsp->priority = ec->tos.priority;
	}
	--ec->exprsp;
	ec->ei->string = ec->tos.string;
	ec->ei->value = ec->tos.value;
	ec->ei->type = ec->tos.type;
	return 0;
}

int trytop(ec *ec)
{
	int topop;
	bstring *topstring;
	double topval;

	for(;;)
	{
		if(ec->tos.priority > ec->exprsp->priority)
			return ec->tos.priority==PRI_08;
		topop = ec->exprsp->operation;
		topval = ec->exprsp->value;
		topstring = ec->exprsp->string;
		--ec->exprsp;

		switch(topop)
		{
			case oper_plus: /* + */
				ec->tos.value+=topval;
				break;
			case oper_minus: /* - */
				ec->tos.value=topval-ec->tos.value;
				break;
			case oper_divide: /* / */
				ec->tos.value=topval/ec->tos.value;
				break;
			case oper_multiply: /* * */
				ec->tos.value*=topval;
				break;
//			case oper_or: /* | */
//				ec->tos.value=(int)ec->operval | (int)topval;
//				break;
//			case oper_and: /* & */
//				ec->tos.value=(int)ec->operval | (int)topval;
//				break;
//			case oper_lshift: /* << */
//				ec->tos.value=(int)topval<<(int)ec->operval;
//				break;
//			case oper_rshift: /* >> */
//				ec->tos.value=(int)topval>>(int)ec->operval;
//				break;
			case oper_end: return 1;
		}
	}
}

void operator(ec *ec)
{
uchar ch;
int backup=0;

	ch=get(ec);
	switch(ch)
	{
		case '+': ec->tos.priority=PRI_10;ec->tos.operation=oper_plus;break;
		case '-': ec->tos.priority=PRI_10;ec->tos.operation=oper_minus;break;
		case '/': ec->tos.priority=PRI_18;ec->tos.operation=oper_divide;break;
		case '*': ec->tos.priority=PRI_18;ec->tos.operation=oper_multiply;break;
		case '=':
			if(ec->ei->flags_in & EXPR_LET)
			{
				ec->tos.priority=PRI_0c;ec->tos.operation=oper_assign;
			} else
				backup=1;
			break;
//		case '|': ec->tos.priority=PRI_20;ec->tos.operation=oper_or;break;
//		case '&': ec->tos.priority=PRI_28;ec->tos.operation=oper_and;break;
//		case '<':
//			if(get(ec)!='<') back(ec);
//			ec->tos.priority=PRI_30;ec->tos.operation=6;break;
//		case '>':
//			if(get(ec)!='>') back(ec);
//			ec->tos.priority=PRI_20;ec->tos.operation=7;break;
		default:
			backup=1;break;
	}
	if(backup)
	{
		back(ec);
		ec->tos.priority=PRI_08;
		ec->tos.operation=oper_end;
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

struct variable *add_variable(bc *bc, char *name, int type)
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
	v->string = 0;
	v->array = 0;
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
	}
	put[n]=0;
	return type;
}

bstring *make_bstring(char *string, int length)
{
bstring *bs;
	bs=malloc(length+sizeof(bstring)+1);
#warning check for allocation failure
	if(bs)
	{
		bs->length = length;
		memcpy(bs->string, string, length);
		bs->string[length]=0;
	}
	return bs;
}

void free_bstring(bstring *bs)
{
	if(bs) free(bs);
}

bstring *gather_bstring(ec *ec)
{
char tmp[4096];
int in=0;
char ch;
	for(;;)
	{
		ch=get(ec);
		if(ch=='"')
			break;
		if(ch=='\\')
			ch=get(ec);
		if(!ch || ch=='\n')
		{
			back(ec);
			break;
		}
		if(in<sizeof(tmp))
			tmp[in++] = ch;
	}
	return make_bstring(tmp, in);
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
			ec->tos.value = intpart + fracpart;
		} else
			ec->tos.value = intpart;
		back(ec);
		ec->tos.type = OT_DOUBLE;
	} else if(ch=='\'')
	{
		get(ec);
		ec->tos.value=0.0;
		while((ch=get(ec)))
		{
			if(ch=='\n' || !ch) {back(ec);unbalancedq(ec);break;}
			if(ch=='\'')
				if(get(ec)!='\'') {back(ec);break;}
			ec->tos.value*=256;ec->tos.value+=ch;
		}
		ec->tos.type = OT_DOUBLE;
	} else if(ch=='(')
	{
		int res;
		get(ec);
		res=expr2(ec);
		ec->tos.value=ec->ei->value;
		ec->tos.type = ec->ei->type;
		if(get(ec)!=')') {ec->exprflag|=1;back(ec);}
	} else if(ch=='"')
	{
		get(ec);
		ec->tos.string = gather_bstring(ec);
		ec->tos.type = OT_BSTRING;
	} else
	{
		char name[16];
		struct variable *v;
		int type;

		type=gather_variable_name(ec->bc, name, &ec->pnt);
		if(type != RANK_INVALID)
		{
			v=find_variable(ec->bc, name);
			if(!v)
			{
				if(type&RANK_MASK) // error, we can't make this on the fly
#warning have to deal with this
					;
				else
					v=add_variable(ec->bc, name, type);
			}
#warning check for ran out of variables
			ec->tos.value = v->value;
		}
	}
}
