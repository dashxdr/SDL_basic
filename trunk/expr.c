#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "misc.h"

typedef unsigned char uchar;

#define EXPRMAX 64

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
oper_eq,
oper_ne,
oper_lt,
oper_le,
oper_gt,
oper_ge,

};

#define PRI_08    0x08
#define PRI_0c    0x0c
#define PRI_10    0x10
#define PRI_18    0x18
#define PRI_20    0x20
#define PRI_28    0x28
#define PRI_30    0x30

typedef struct expr_element {
	void *indirect;
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
	ee tos;
	int exprflags;
	char *pnt;
	einfo *ei;
} ec;


int expr2(ec *ec, int flags);
void operand(ec *ec);
void operator(ec *ec);
int trytop(ec *ec);

void unbalancedq(ec *ec)
{
}



inline int at(ec *ec)
{
	return *ec->pnt;
}

inline int get(ec *ec)
{
	return *ec->pnt++;
}

inline int back(ec *ec)
{
	return *--ec->pnt;
}

void expr_error(ec *ec, char *msg)
{
	if(!(ec->ei->flags_out & EXPR_ERROR))
	{
		ec->ei->error = msg;
		ec->ei->flags_out |= EXPR_ERROR;
		run_error(ec->bc, "%s", msg);
	}
}

int expr(bc *bc, char **take, einfo *ei)
{
ec ecl, *ec = &ecl;
int res;

	ei->flags_out = 0;
	ei->error = 0;

	ec->bc = bc;
	ec->pnt = *take;
	ec->exprsp=ec->exprstack;
	ec->ei = ei;
	res = expr2(ec, ei->flags_in);
	*take = ec->pnt;
	if((ei->flags_in & EXPR_LET) && !(ei->flags_out & EXPR_DIDLET))
		expr_error(ec, EXPR_ERR_NOOP);
	return ei->flags_out & EXPR_ERROR;
}
/*uchar opchars[]={'+','-','/','*','|','&','<<','>>','!'};*/

void trythunk(ee *thing)
{
	if(thing->type == OT_PDOUBLE)
	{
		thing->value = *(double *)thing->indirect;
		thing->type = OT_DOUBLE;
	} else if(thing->type == OT_PBSTRING)
	{
		bstring *bs;
		bs = *(bstring **)thing->indirect;
		thing->string = make_bstring(bs->string, bs->length);
		thing->type = OT_BSTRING;
	}
}

int expr2(ec *ec, int flags)
{
int flag_save;
	flag_save = ec->ei->flags_in;
	ec->ei->flags_in = flags;
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
		if(ec->ei->flags_in & EXPR_LVALUE) break; // just want an lvalue
		operator(ec);
		if(trytop(ec)) break;

		++ec->exprsp;
		*ec->exprsp = ec->tos;
	}
	--ec->exprsp;
	if(!(ec->ei->flags_in & EXPR_LVALUE))
		trythunk(&ec->tos);
	ec->ei->string = ec->tos.string;
	ec->ei->value = ec->tos.value;
	ec->ei->type = ec->tos.type;
	ec->ei->indirect = ec->tos.indirect;
	if(ec->ei->flags_in & EXPR_NUMERIC)
	{
		if(ec->ei->type == OT_BSTRING || ec->ei->type == OT_PBSTRING)
		{
			expr_error(ec, EXPR_ERR_NUMERIC);
			if(ec->ei->type == OT_BSTRING)
			{
				free_bstring(ec->ei->string);
				ec->ei->value = 0.0;
				ec->ei->type = OT_DOUBLE;
			}
		}
	}


	ec->ei->flags_in = flag_save;
	return 0;
}

int is_string_type(int type)
{
	return type==OT_BSTRING || type==OT_PBSTRING;
}

int bstring_comp(bstring *left, bstring *right)
{
int min,max;
int res;
	min=left->length;
	max=right->length;
	if(max<min)
		res=min,min=max,max=res;

	res=memcmp(left->string, right->string, min);
	if(res) return res;
	if(min==max) return 0;
	return left->length < right->length ? -1 : 1;
}

int string_comps(ee *left, ee *right)
{
int res=0;
	if(left->type == OT_BSTRING && right->type == OT_BSTRING)
	{
		res = bstring_comp(left->string, right->string);
	} else
		right->value = 0;
	if(right->type == OT_BSTRING);
	{
		free_bstring(right->string);
		right->string = 0;
	}
	right->type = OT_DOUBLE;
	return res;
}

int double_comps(ee *left, ee *right)
{
int res=0;
	if(left->type == OT_DOUBLE && right->type == OT_DOUBLE)
	{
		if(left->value < right->value) res=-1;
		else if(left->value > right->value) res=1;
	}
	right->type = OT_DOUBLE;
	return res;
}


int trytop(ec *ec)
{
ee *left, *right;

	right = & ec->tos;
	for(;;)
	{
		if(right->priority > ec->exprsp->priority)
			return right->priority==PRI_08;
		left = ec->exprsp;
		--ec->exprsp;

		if(is_string_type(left->type) ^ is_string_type(right->type))
			expr_error(ec, EXPR_ERR_MISMATCH);

		trythunk(right);
		if(left->operation != oper_assign)
			trythunk(left);

		if(right->type == OT_DOUBLE)
			switch(left->operation)
			{
				case oper_plus: /* + */
					right->value+=left->value;
					break;
				case oper_minus: /* - */
					right->value=left->value - right->value;
					break;
				case oper_divide: /* / */
					right->value=left->value/right->value;
					break;
				case oper_multiply: /* * */
					right->value*=left->value;
					break;
				case oper_assign: // =
					ec->ei->flags_out |= EXPR_DIDLET;
					if(left->type != OT_PDOUBLE)
						expr_error(ec, EXPR_ERR_BAD_LVALUE);
					else
					{
						*(double *)left->indirect = right->value;
						right->type = OT_DOUBLE;
					}
					break;
				case oper_eq: // comparison =
					right->value = !double_comps(left, right) ? 1.0 : 0;
					break;
				case oper_lt: // comparison <
					right->value = double_comps(left, right)<0 ? 1.0 : 0;
					break;
				case oper_gt: // comparison >
					right->value = double_comps(left, right)>0 ? 1.0 : 0;
					break;
				case oper_le: // comparison <=
					right->value = double_comps(left, right)<=0 ? 1.0 : 0;
					break;
				case oper_ge: // comparison >=
					right->value = double_comps(left, right)>=0 ? 1.0 : 0;
					break;
				case oper_ne: // comparison <>
					right->value = double_comps(left, right) ? 1.0 : 0;
					break;
//				case oper_or: /* | */
//					right->value=(int)left->value | (int)right->value;
//					break;
//				case oper_and: /* & */
//					right->value=(int)left->value | (int)right->value;
//					break;
//				case oper_lshift: /* << */
//					right->value=(int)left->value << (int)right->value;
//					break;
//				case oper_rshift: /* >> */
//					right->value=(int)left->value >> (int)ec->value;
//					break;
				case oper_end: return 1;
			}
		else if(right->type == OT_BSTRING)
			switch(left->operation)
			{
				case oper_assign: // =
					ec->ei->flags_out |= EXPR_DIDLET;
					if(left->type != OT_PBSTRING)
						expr_error(ec, EXPR_ERR_BAD_LVALUE);
					else
					{
						bstring *bs;
						bs = *(bstring **)left->indirect;
						free_bstring(bs);
						bs = right->string;
						*(bstring **)left->indirect = dup_bstring(bs);

						right->type=OT_BSTRING;
					}
					break;
				case oper_plus: // +
					if(left->type == OT_BSTRING && right->type == OT_BSTRING)
					{
						bstring *bs;
						bs = make_raw_bstring(left->string->length +
								right->string->length);
						memcpy(bs->string, left->string->string,
								left->string->length);
						memcpy(bs->string + left->string->length,
								right->string->string,
								right->string->length);
						free_bstring(right->string);
						right->string = bs;
					}
					break;
				case oper_eq: // comparison =
					right->value = !string_comps(left, right) ? 1.0 : 0;
					break;
				case oper_lt: // comparison <
					right->value = string_comps(left, right)<0 ? 1.0 : 0;
					break;
				case oper_gt: // comparison >
					right->value = string_comps(left, right)>0 ? 1.0 : 0;
					break;
				case oper_le: // comparison <=
					right->value = string_comps(left, right)<=0 ? 1.0 : 0;
					break;
				case oper_ge: // comparison >=
					right->value = string_comps(left, right)>=0 ? 1.0 : 0;
					break;
				case oper_ne: // comparison <>
					right->value = string_comps(left, right) ? 1.0 : 0;
					break;
				default:
					expr_error(ec, EXPR_ERR_INVALID);
					break;
				case oper_end: return 1;
			
			}
		else
			expr_error(ec, EXPR_ERR_MISSING);

		if(left->type == OT_BSTRING)
		{
			free_bstring(left->string);
			left->string = 0;
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
			} else if(ec->ei->flags_in & EXPR_IF)
			{
				ec->tos.priority=PRI_0c;ec->tos.operation=oper_eq;
			} else
				backup=1;
			break;
		case '<':
			if((ch=get(ec))=='=')
				{ec->tos.priority=PRI_0c;ec->tos.operation=oper_le;}
			else if(ch=='>')
				{ec->tos.priority=PRI_0c;ec->tos.operation=oper_ne;}
			else
				{back(ec);ec->tos.priority=PRI_0c;ec->tos.operation=oper_lt;}
			break;
		case '>':
			if((ch=get(ec))=='=')
				{ec->tos.priority=PRI_0c;ec->tos.operation=oper_ge;}
			else
				{back(ec);ec->tos.priority=PRI_0c;ec->tos.operation=oper_gt;}
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

static struct varname *find_v_low(bc *bc, char *name)
{
struct varname *vn;
int low, mid, high;
	vn=bc->varnames;
	low=0;
	high=bc->numvariables;
	for(;;)
	{
		mid=(low + high) >> 1;
		if(mid==low) break;
		if(strcmp(name, vn[mid].name)<0)
			high=mid;
		else
			low=mid;
	}
	return vn+mid;
}

struct variable *find_variable(bc *bc, char *name)
{
struct varname *vn;
	vn=find_v_low(bc, name);
	if(bc->numvariables && !strcmp(name, vn->name))
		return vn->var;
	return 0;
}

// can call this, and if the variable doesn't exist it'll be created
struct variable *add_variable(bc *bc, char *name)
{
struct variable *v;
struct varname *vn;
int which;
int t;
	vn=find_v_low(bc, name);
	which=vn-bc->varnames;
	if(bc->numvariables)
	{
		if(!(t=strcmp(name, vn->name)))
			return vn->var;
		if(t>0)
			++vn,++which;
		memmove(vn+1, vn, sizeof(*vn)*(bc->numvariables - which));
	}
	strcpy(vn->name, name);
	vn->var = v = bc->vars + bc->numvariables;

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

bstring *make_raw_bstring(int length)
{
bstring *bs;
	bs=malloc(length + sizeof(bstring) + 1);
	if(bs)
	{
		bs->length = length;
		bs->string[length]=0;
	}
	return bs;
}

bstring *make_bstring(char *string, int length)
{
bstring *bs;
	bs=make_raw_bstring(length);
#warning check for allocation failure
	if(bs)
	{
		memcpy(bs->string, string, length);
	}
	return bs;
}

void set_variable(bc *bc, char *name, double value)
{
struct variable *v;
	v=add_variable(bc, name);
	if(v)
		v->value = value;
}


bstring *dup_bstring(bstring *bs)
{
	return make_bstring(bs->string, bs->length);
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
ee tempop={0}, *newop = &tempop;
int i;
int res;

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
			newop->value = intpart + fracpart;
		} else
			newop->value = intpart;
		back(ec);
		newop->type = OT_DOUBLE;
	} else if(ch=='\'')
	{
		get(ec);
		newop->value=0.0;
		while((ch=get(ec)))
		{
			if(ch=='\n' || !ch) {back(ec);unbalancedq(ec);break;}
			if(ch=='\'')
				if(get(ec)!='\'') {back(ec);break;}
			newop->value*=256;newop->value+=ch;
		}
		newop->type = OT_DOUBLE;
	} else if(ch=='(')
	{
		get(ec);
		res=expr2(ec, 0);
#warning need some more error checks...free up string maybe
		newop->value=ec->ei->value;
		newop->type = ec->ei->type;
		if(get(ec)!=')')
		{
			expr_error(ec, EXPR_ERR_CLOSEPAR);
			back(ec);
		}
	} else if(ch=='"')
	{
		get(ec);
		newop->string = gather_bstring(ec);
		newop->type = OT_BSTRING;
	} else if(is_numeric_function(ec->bc, ch))
	{
		int numpars;
		double pars[8];
		void (*func)();

		get(ec);
		if(get(ec) != '(')
		{
			expr_error(ec, EXPR_ERR_NO_ARRAY);
			back(ec);
			goto crapout;
		}
		numpars = function_parameter_count(ec->bc, ch);
		for(i=0;i<numpars;++i)
		{
			res=expr2(ec, EXPR_NUMERIC);
			pars[i] = ec->ei->value;
#warning need some more error checks...free up string maybe
			if(i+1<numpars)
			{
				if(get(ec)!=',')
				{
					back(ec);
					expr_error(ec, EXPR_ERR_PAR_COUNT);
					break;
				}
			}
		}
		if(get(ec)!=')')
		{
			expr_error(ec, EXPR_ERR_CLOSEPAR);
			back(ec);
		}
		func = statement_handler(ec->bc, ch);
		func(ec->bc, pars);
		newop->value = pars[0];
		newop->type = OT_DOUBLE;
	} else
	{
		char name[16];
		struct variable *v;
		int type;

		type=gather_variable_name(ec->bc, name, &ec->pnt);
		if(type != RANK_INVALID)
		{
			v=add_variable(ec->bc, name);
			if(v && type==RANK_ARRAY && !(v->rank&RANK_MASK))
			{
				expr_error(ec, EXPR_ERR_NO_ARRAY);
				goto crapout;
			}
#warning check for ran out of variables
			if(type&RANK_MASK)
			{
				int indexes[128];
				int dims[128];
				int i,j;
				int rank;
				int res;

				rank = v->rank & RANK_MASK;
				for(i=0;i<rank+1;++i)
					dims[i] = v->dimensions[i];
				for(i=0;i<rank;++i)
				{
					res = expr2(ec, EXPR_NUMERIC);
					if(ec->ei->type != OT_DOUBLE)
					{
						expr_error(ec, EXPR_ERR_BAD_INDEX);
						indexes[i]=0;
					} else
						indexes[i] = ec->ei->value - 1.0;
					if(indexes[i]<0 ||
						 indexes[i]*dims[i+1] >= dims[i])
					{
						expr_error(ec, EXPR_ERR_RANGE_ERROR);
						indexes[i]=0;
					}
					if(i+1<rank) // more indexes coming
					{
						if(at(ec) == ',')
							get(ec);
						else
							expr_error(ec, EXPR_ERR_MISCOUNT "1");
					} else
					{
						if(at(ec) == ')')
							get(ec);
						else
							expr_error(ec, EXPR_ERR_MISCOUNT "2");
					}
				}
				j=0;
				for(i=0;i<rank;++i)
					j+=indexes[i]*v->dimensions[i+1];
				if(type & RANK_STRING)
				{
					bstring **ind;
					ind = (bstring **)v->array + j;
					newop->indirect = ind;
					if(!*ind)
						*ind = make_bstring("",0);
					newop->type = OT_PBSTRING;
				} else
				{
					newop->indirect = (double *)v->array + j;
					newop->type = OT_PDOUBLE;
				}
			} else
			{
				if(type & RANK_STRING)
				{
					if(!v->string)
						v->string = make_bstring("", 0);
					newop->indirect = &v->string;
					newop->type = OT_PBSTRING;
				} else
				{
					newop->indirect = &v->value;
					newop->type = OT_PDOUBLE;
				}
			}
		}
	}
crapout:
	ec->tos = tempop;
}
