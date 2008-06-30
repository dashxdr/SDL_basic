#include <SDL.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>

#include "misc.h"

int convertline(bc *bc, char *put, char *take);

struct cmd {
	char *name;
	void (*func)(bc *, char *line);
};

void gettoken(char *put, int putlen, char **take)
{
int i;
char c;

	i=0;
	while((c=*(*take)))
	{
		if(c==' ' || c=='\n') break;
		if(i<putlen-1)
			put[i++] = c;
		++(*take);
	}
	put[i]=0;
}

int iswhite(char c)
{
	return c==' ';
}

void skipwhite(char **take)
{
char c;

	while((c=**take))
	{
		if(!iswhite(c)) break;
		++*take;
	}
}


void error(bc *bc, char *s, ...)
{
va_list ap;
char temp[1024];
	va_start(ap, s);
	vsnprintf(temp, sizeof(temp), s, ap);
	va_end(ap);
	tprintf(bc, "%s\n", temp);
}

void unknown_command(bc *bc, char *str)
{
	error(bc, "Unknown command '%s'", str);
}

void missing_parameter(bc *bc, char *str)
{
	error(bc, "Command is missing a parameter: %s", str);
}

char *findline(bc *bc, int linenumber)
{
char *p=bc->program;

	while(*p)
	{
		if(atoi(p) == linenumber)
			break;
		while(*p && *p++ != '\n');
	}
	return p;
}

char *findtextline(bc *bc, int linenumber)
{
char *p=bc->program;

	while(*p)
	{
		if(atoi(p) > linenumber)
			break;
		while(*p && *p++ != '\n');
	}
	return p;
}


void deleteline(bc *bc, int lnum)
{
char *p, *p2;
	p=findline(bc, lnum);
	if(*p)
	{
		p2=p;
		while(*p2 && *p2++ != '\n');
		while((*p++ = *p2++));
	}
}

// line assumed not to have '\n' on end
void addline(bc *bc, char *line)
{
int lnum;
char *p;
int len;

	lnum = atoi(line);
	len = strlen(bc->program) + strlen(line) + 1;
	p=findline(bc, lnum);
	if(*p)
	{
		while(*p)
		{
			--len;
			if(*p++ == '\n')
				break;
		}
	}
	if(len > sizeof(bc->program)-1)
	{
		error(bc, "Not enough space to add line to program.");
		return;
	}
	deleteline(bc, lnum);
	p=findtextline(bc, lnum);
	len=strlen(line)+1;
	memmove(p+len, p, strlen(p)+1);
	memcpy(p, line, len-1);
	p[len-1] = '\n';
}


void doexit(bc *bc, char *text)
{
	bc->flags |= BF_QUIT;
}

void doinfo(bc *bc, char *text)
{
int t;
char *p;
	tprintf(bc, "Program name: '%s'\n", bc->filename);
	t=0;
	p=bc->program;
	while(*p)
	{
		if(*p++ == '\n')
			++t;
	}
	tprintf(bc, "Program size: %d bytes, %d line%s\n", strlen(bc->program),
		t, t==1 ? "" : "s");
}

/*
   list
   list 10-50
   list -50
   list 10-
   list 10
*/
void dolist(bc *bc, char *text)
{
char aline[1024];
char *p;
int i;
int min, max;
int v1, v2;
int wantinfo=1;
	min=0;
	max=0x7fffffff;

	if(sscanf(text, "%d-%d", &v1, &v2) == 2)
	{
		min=v1;
		max=v2;
		wantinfo=0;
	} else if(*text=='-' && text[1]>='0' && text[1]<='9')
	{
		max=atoi(text+1);
		wantinfo=0;
	} else if(sscanf(text, "%d", &v1) == 1)
	{
		min=v1;
		while(*text>='0' && *text<='9') ++text;
		if(*text != '-')
			max=v1;
		wantinfo=0;
	}
	if(wantinfo) doinfo(bc,"");

	p=bc->program;
	while(*p)
	{
		i=0;
		while(*p)
		{
			aline[i] = *p;
			if(i<sizeof(aline)-1) ++i;
			if(*p++ == '\n') break;
		}
		aline[i]=0;
		i=atoi(aline);
		if(min<=i && i<=max)
			tprintf(bc, "%s", aline);
	}
}

void doedit(bc *bc, char *text)
{
char *p;
int num;
int i;
	num=atoi(text);
	p=findline(bc, num);
	if(!*p)
	{
		error(bc, "Line %d does not exist.", num);
		return;
	}
	i=0;
	while(*p && *p!='\n')
	{
		if(i<sizeof(bc->workspace)-1)
			bc->workspace[i++] = *p;
		++p;
	}
	bc->workspace[i] = 0;
	bc->flags |= BF_EDIT;
}

#define NAMETAIL ".bas"

void makename(char *name, int len)
{
int t;
int k;
	k=strlen(NAMETAIL);
	t=strlen(name);
	if(t > k && !strcmp(NAMETAIL, name+t-k))
		return;
	if(t+k<len-1)
		strcat(name, NAMETAIL);
}

void dosave(bc *bc, char *text)
{
char filename[128];
int fd;
int len;
	gettoken(filename, sizeof(filename), &text);
	if(!filename[0])
	{
		strncpy(filename, bc->filename, sizeof(filename));
		if(!filename[0])
		{
			missing_parameter(bc, "Must specify filename");
			return;
		}
	}
	makename(filename, sizeof(filename));

	fd=open(filename, O_WRONLY|O_CREAT|O_TRUNC, 0644);
	if(fd<0)
	{
		error(bc, "Can't open file '%s' for writing.", filename);
		return;
	}
	len=write(fd, bc->program, strlen(bc->program));
	close(fd);
	if(len!=strlen(bc->program))
		error(bc, "ERROR: Short write, the entire program wasn't saved.");
	strncpy(bc->filename, filename, sizeof(bc->filename));
}

void doload(bc *bc, char *text)
{
char filename[128];
int fd;
int len;
	gettoken(filename, sizeof(filename), &text);
	if(!filename[0])
	{
		missing_parameter(bc, "Must specify filename");
		return;
	}
	makename(filename, sizeof(filename));
	fd=open(filename, O_RDONLY);
	if(fd<0)
	{
		error(bc, "Can't open file '%s' for reading.", filename);
		return;
	}
	len=read(fd, bc->program, sizeof(bc->program)-1);
	close(fd);
	if(len>=0)
		bc->program[len]=0;
	tprintf(bc, "Loaded %d bytes from file '%s'\n", len, filename);
	snprintf(bc->filename, sizeof(bc->filename), "%s", filename);
}

void donew(bc *bc, char *text)
{
	bc->program[0]=0;
	bc->filename[0]=0;
	tprintf(bc, "Program erased.\n");
	runinit(bc);
}

struct inout {
int in;
int out;
};

struct inout *find_io(struct inout *io, int ionum, int want)
{
int low, high, mid;
	if(ionum<1) return 0;
	low = 0;
	high =ionum;
	for(;;)
	{
		mid = (low+high) >> 1;
		if(mid==low) break;
		if(want < io[mid].in)
			high=mid;
		else
			low=mid;
	}
	if(want == io[mid].in)
		return io+mid;
	else
		return 0;

}

int backstr(char *at, char *comp)
{
	at -= strlen(comp);
	while(*comp)
	{
		if(tolower(*at++) != *comp++)
			return -1;
	}
	return 0;
}

void renumber_lines(bc *bc, struct inout *io, int ionum)
{
char *p1, *take, *put, *e;
#define LN (LINESIZE+20)
char temp[LN], *line;
int ln;
struct inout *iof;
int thisline;

	if(!ionum) return;
	memset(temp, 0, sizeof(temp));
	line=temp+10;
	take=bc->program;
	put=bc->runnable;
#warning -- 10 print "goto 10" would pose a problem for this...
	while(*take)
	{
		p1=line;
		e=line+LINESIZE;
		while(*take && *take++ != '\n')
		{
			if(p1<e)
				*p1++ = take[-1];
		}
		*p1 = 0;
		e=bc->runnable+sizeof(bc->runnable)-5;
		thisline=atoi(line);
		iof=find_io(io, ionum, thisline);
		if(!iof) // we're not renumbering this line
		{
			put += snprintf(put, e-put, "%s\n", line);
			continue;
		}
		p1=line;
		while(isdigit(*p1)) ++p1;
		put += snprintf(put, e-put, "%d", iof->out);
		while(*p1)
		{
			if(backstr(p1, "then") && backstr(p1, "goto") &&
				backstr(p1, "gosub"))
			{
				if(put<e)
					*put++ = *p1++;
				continue;
			}
more:
			while(iswhite(*p1))
				if(put<e)
					*put++ = *p1++;
				else
					++p1;
			if(!isdigit(*p1))
				continue;
			ln=atoi(p1);
			while(isdigit(*p1)) ++p1;
			iof = find_io(io, ionum, ln);
			if(!iof)
			{
				error(bc, "Can't renumber line %d, reference to line %d invalid",
					thisline, ln);
				return;
			}
			put+=snprintf(put, e-put, "%d", iof->out);
			while(iswhite(*p1))
				if(put<e)
					*put++ = *p1++;
				else
					++p1;
			if(*p1 == ',') // on x goto #,#,#...
			{
				if(put<e)
					*put++ = *p1++;
				goto more;
			}

		}
		if(put<e)
			*put++ = '\n';
	}
	*put = 0;
	strcpy(bc->program, bc->runnable);
}

void doren(bc *bc, char *text)
{
struct inout io[MAX_PROGRAM_LINES];
int i;
char *p;
int lc,delta;
int v1, v2;
int n=0;
	if(sscanf(text, "%d,%d", &v1, &v2) == 2)
		n=2;
	else if(sscanf(text, "%d", &v1) == 1)
		n=1;
	if((n>0 && (v1<1 || v1>1000)) ||
			(n>1 && (v2<1 || v2>1000)))
	{
		error(bc, "Values must range from 1 to 1000");
		return;
	}
	if(n==1) v2=v1;
	renumber(bc, v1, v2);
#if 0
	p=bc->program;
	if(n>0) lc = v1;
	else lc=10;
	if(n>1) delta = v2;
	else delta=10;
	for(i=0;*p && i<MAX_PROGRAM_LINES-1;++i)
	{
		io[i].in = atoi(p);
		io[i].out = lc;
		lc += delta;
		while(*p && *p++ != '\n');
	}
	if(i)
		renumber_lines(bc, io, i);
#endif
}

void doparse(bc *bc, char *text)
{
	parse(bc, 0); // don't run it
}

void dopr(bc *bc, char *text)
{
	parse(bc, 1); // run it
}

void dorun(bc *bc, char *text);

struct cmd commandlist[]={
{"list", dolist},
{"edit", doedit},
{"save", dosave},
{"load", doload},
{"info", doinfo},
{"new", donew},
{"scr", donew},
{"exit", doexit},
{"run", dopr},
{"oldrun", dorun},
{"ren", doren},
{"parse", doparse},
{"pr", dopr},
{0, 0}};

void processline(bc *bc, char *line)
{
char token[64], *lp;
struct cmd *cmd;

	lp=line;
	gettoken(token, sizeof(token), &lp);
	skipwhite(&lp);

	cmd=commandlist;
	while(cmd->name)
	{
		if(!strncmp(token, cmd->name, strlen(token)))
		{
			cmd->func(bc, lp);
			return;
		}
		++cmd;
	}
	if(line[0]>='0' && line[0]<='9') // line number
	{
		bc->flags |= BF_NOPROMPT;
		lp=line;
		while(*lp>='0' && *lp<='9') ++lp;
		if(*lp)
			addline(bc, line);
		else
			deleteline(bc, atoi(line));
	} else if(*line)
	{
/*
		char tokenized[1024], *p=tokenized;
		convertline(bc, tokenized, line);

		execute(bc,&p);
		if(bc->flags & BF_RUNERROR)
		{
			bc->flags ^= BF_RUNERROR;
			tprintf(bc, "%s\n", bc->lineerror);
		}
*/
		parseline(bc, line);
	} else
		bc->flags |= BF_NOPROMPT;
}

/***********************************************************************
************************************************************************
************************************************************************
      Stuff for running program
************************************************************************
************************************************************************
************************************************************************/

#define SYNTAX_ERROR          "Syntax error"
#define NON_TERMINATED_STRING "Nonterminated string"
#define NO_SUCH_LINE          "Invalid line number"
#define DUPLICATE_ARRAY       "Duplicate array declaration"
#define INVALID_VARIABLE      "Invalid variable"
#define TOO_MANY_FORS         "Too many nested for loops"
#define NEXT_WITHOUT_FOR      "Next without for"
#define OUT_OF_DATA           "Out of data"
#define INVALID_DATA          "Invalid data"
#define STACK_OVERFLOW        "Stack overflow"
#define BAD_RETURN            "Return without gosub"
#define WRONG_NUMBER          "Wrong number of parameters"
#define SQRT_ERROR            "Can't take square root of negative number"
#define ON_RANGE_ERROR        "'ON' index out of range"
#define BADVALUE              "Illegal value"

void run_error(bc *bc, char *s, ...)
{
va_list ap;
	if(!(bc->flags&BF_RUNERROR))
	{
		va_start(ap, s);
		vsnprintf(bc->lineerror, sizeof(bc->lineerror), s, ap);
		va_end(ap);
		bc->flags |= BF_RUNERROR;
	}
}


int findrunline(bc *bc, int num)
{
struct linepointer *lp;
int low, high, mid;
	low = 0;
	high = bc->numlines;
	lp = bc->lps;
	for(;;)
	{
		mid = (low+high) >> 1;
		if(mid==low) break;
		if(num < lp[mid].linenum)
			high=mid;
		else
			low=mid;
	}
	if(lp[mid].linenum == num)
		return mid;
	else
		return -1;
}

int currentline(bc *bc)
{
int t;
	t=bc->online;
	if(t<bc->numlines)
		t=bc->lps[t].linenum;
	else
		t=-1;
	return t;
}

int comma(bc *bc, char **take)
{
int res=0;
	if(**take==',')
		++*take;
	else
	{
		res=-1;
		run_error(bc, SYNTAX_ERROR);
	}
	return res;
}

#define EXACT_NUM 1
// gather a comma separated list of expressions, store in put
// maxgot = how many we can gather (max), gets filled in with
// how many we got
int comma_list(bc *bc, char **take, double *put, int *maxgot, int flag)
{
int max=*maxgot;
einfo einfo, *ei=&einfo;
int res=0;
	*maxgot = 0;
	while(*maxgot < max)
	{
		ei->flags_in = EXPR_NUMERIC;
		res = expr(bc, take, ei);
		if(res) break;

		put[(*maxgot)++] = ei->value;
		if(**take!=',') break;
		++*take;
	}
	if((flag&EXACT_NUM) && *maxgot != max)
	{
		run_error(bc, WRONG_NUMBER);
		res=-1;
	}
	return res;
}


/***********************************************************************
************************************************************************
************************************************************************
      Statements
************************************************************************
************************************************************************
************************************************************************/


void dorem(bc *bc, char **take)
{
}

void dolet(bc *bc, char **take)
{
//int type;
int res;
//char name[16];
//struct variable *v;
einfo einfo, *ei=&einfo;

	ei->flags_in = EXPR_LET;
	res = expr(bc, take, ei);
	if(ei->type == OT_BSTRING)
		free_bstring(bc, ei->string);
}

void doprint(bc *bc, char **take)
{
einfo einfo, *ei=&einfo;
int res;
int newline=1;

	while(**take && **take != '\n' && **take!=':' &&
		*(unsigned char *)*take!=token_else)
	{
		newline=1;
		if(**take == '@')
		{
			int v;
			++*take;
			ei->flags_in = EXPR_NUMERIC;
			res = expr(bc, take, ei);
			if(res) break;
			v=ei->value;
			v&=1023;
			tprintf(bc, "\033%dx\033%dy", v&63, v>>6);
		} else if(*(unsigned char *)*take == token_tab)
		{
			int t;
			++*take;
			if(**take != '(')
			{
				error(bc, SYNTAX_ERROR);
				break;
			}
			++*take;
			ei->flags_in = EXPR_NUMERIC;
			res = expr(bc, take, ei);
			if(res) break;
			if(**take != ')')
			{
				error(bc, SYNTAX_ERROR);
				break;
			}
			++*take;
			t=ei->value;
			t&=63;

			if(bc->txpos<t)
				tprintf(bc, "\033%dx", t);
		} else
		{
			ei->flags_in = 0;
			res = expr(bc, take, ei);
			if(res) break;
			if(ei->type == OT_DOUBLE)
			{
				if((long)ei->value == ei->value)
					tprintf(bc, " %ld ", (long)ei->value);
				else
					tprintf(bc, " %.2f ", ei->value);
			} else if(ei->type == OT_BSTRING)
			{
				bstring *bs = ei->string;
				if(bs)
					tprintf(bc, "%s", bs->string);
				free_bstring(bc, bs);
			}
		}

		if(!**take || **take==':') break;
		if(**take == '"')
			continue;
		if(**take == ',')
		{
			int t;
			++*take;
			t=(bc->txpos&~15)+16;
			if(t>=bc->txsize)
				tprintf(bc, "\n");
			else
			{
				char spaces[32];
				t-=bc->txpos;
				memset(spaces, ' ', t);
				spaces[t] = 0;
				tprintf(bc, "%s", spaces);
			}
			newline = 0;
			continue;
		}
		if(**take == ';')
		{
			++*take;
			newline=0;
			continue;
		}
		if(*(unsigned char *)*take!=token_else)
			run_error(bc, SYNTAX_ERROR);
		break;
	}
	if(newline)
		tprintf(bc, "\n");
}

void reset_waitbase(bc *bc)
{
	bc->waitbase = SDL_GetTicks();
}

int gather_string(char *put, int putlen, char **take)
{
int in=0;
char ch;
int err=0;
	for(;;)
	{
		ch=*(*take)++;
		if(ch=='"')
			break;
		if(ch=='\\')
			ch=*(*take)++;
		if(!ch || ch=='\n')
		{
			--*take;
			err = -1;
			break;
		}
		if(in<putlen-1)
			put[in++] = ch;
	}
	put[in]=0;
	return err;
}

void doinput(bc *bc, char **take)
{
einfo einfo, *ei=&einfo;
int res;

	if(**take == '"')
	{
		char tmp[1024];
		int res;
		++*take;
		res=gather_string(tmp, sizeof(tmp), take);
		if(res<0)
		{
			run_error(bc, NON_TERMINATED_STRING);
			return;
		}
		tprintf(bc, "%s", tmp);
		if(**take == ';')
			++*take;
	}


	tprintf(bc, "? ");
	typeline(bc, "", 1);
	reset_waitbase(bc);

	ei->flags_in = EXPR_LVALUE;
	res = expr(bc, take, ei);
	if(ei->type == OT_PBSTRING)
	{
		bstring *bs;
		bs = *(bstring **)ei->indirect;
		free_bstring(bc, bs);
		bs = make_bstring(bc, bc->debline, strlen(bc->debline));
		*(bstring **)ei->indirect = bs;
	} else if(ei->type == OT_PDOUBLE)
	{
		if(sscanf(bc->debline, "%lf", (double *)ei->indirect) != 1)
			*(double *)ei->indirect = 0;
	} else run_error(bc, SYNTAX_ERROR);
}

void doif(bc *bc, char **take)
{
einfo einfo, *ei=&einfo;
int res;

	ei->flags_in = EXPR_IF;
	res = expr(bc, take, ei);
	if(ei->type == OT_BSTRING)
	{
		free_bstring(bc, ei->string);
		run_error(bc, SYNTAX_ERROR);
	}
	if(ei->value != 0.0) // take the true side
	{
		if(*(unsigned char *)*take != token_then)
			run_error(bc, SYNTAX_ERROR);
		else
		{
			++*take;
			if(isdigit(**take)) // if X then 20
			{
				int newline;
				newline = findrunline(bc, atoi(*take));
				if(newline<0)
					run_error(bc, NO_SUCH_LINE);
				else
				{
					bc->nextline = newline;
					bc->nextbyte = 0;
				}
			} else
				execute(bc, take);

		}
	} else // look for an else
	{
		int ifc=1;
		int c;

		for(;;)
		{
			c=*(unsigned char *)*take;
			++*take;
			if(!c)
			{
				--*take;
				break;
			}
			if(c==token_if)
				++ifc;
			else if(c==token_else)
			{
				--ifc;
				if(!ifc)
				{
					execute(bc, take);
					break;
				}
			}

		}
	}

}

void dodim(bc *bc, char **take)
{
char name[16];
int type;
int value;
int rank;
int fail=0;
int dimensions[128];
struct variable *v;
int i,j;

	while(!fail)
	{
		type = gather_variable_name(bc, name, take);
		if(type==RANK_INVALID || !(type & RANK_MASK))
		{
			run_error(bc, SYNTAX_ERROR);
			fail=1;
			break;
		}
		rank=0;
		while(!fail)
		{
			if(!isdigit(**take))
			{
				run_error(bc, SYNTAX_ERROR);
				fail=1;
				break;
			}
			value=0;
			while(isdigit(**take))
				value = value * 10 + *(*take)++ - '0';
			dimensions[rank++] = value;
			if(**take==',')
			{
				++*take;
				continue;
			}
			if(**take==')')
			{
				++*take;
				break;
			}
			run_error(bc, SYNTAX_ERROR);
			fail=1;
			break;
		}
		if(fail) break;
		v=find_variable(bc, name);
		if(v)
		{
			run_error(bc, DUPLICATE_ARRAY);
			break;
		}
		v=add_variable(bc, name);
#warning check for out of variables needed
		v->rank = (type & ~RANK_MASK) | rank;
		for(i=0;i<=rank;++i)
		{
			v->dimensions[i]=1;
			for(j=i;j<rank;++j)
				v->dimensions[i] *= dimensions[j];
		}
		if(type & RANK_STRING) // array of strings
			v->array = calloc(v->dimensions[0], sizeof(bstring *));
		else
			v->array = calloc(v->dimensions[0], sizeof(double));
#warning check for out of memory
		if(**take == ',')
		{
			++*take;
			continue;
		}
		if(**take && **take!=':')
			run_error(bc, SYNTAX_ERROR);
		break;
	}
}

struct forinfo *findfor(bc *bc, char *name)
{
int i;
	for(i=0;i<bc->numfors;++i)
		if(!strcmp(name, bc->fors[i].name))
			return bc->fors + i;
	return 0;
}

struct forinfo *newfor(bc *bc)
{
	if(bc->numfors == MAX_FORS)
		return 0;
	return bc->fors + bc->numfors++;
}

void dofor(bc *bc, char **take)
{
char name[16];
int type;
einfo einfo, *ei=&einfo;
int res;
double start_val, end_val, step_val;
struct forinfo *fi;

	type=gather_variable_name(bc, name, take);
	if(type!=RANK_VARIABLE)
	{
		run_error(bc, INVALID_VARIABLE);
		return;
	}
	if(*(unsigned char *)*take != '=')
	{
		run_error(bc, SYNTAX_ERROR);
		return;
	}
	++*take;

	ei->flags_in = 0;
	res = expr(bc, take, ei);
	if(ei->type == OT_BSTRING)
	{
		free_bstring(bc, ei->string);
		run_error(bc, SYNTAX_ERROR);
		return;
	}
	start_val = ei->value;

	if(*(unsigned char *)*take != token_to)
	{
		run_error(bc, SYNTAX_ERROR);
		return;
	}
	++*take;

	ei->flags_in = 0;
	res = expr(bc, take, ei);
	if(ei->type == OT_BSTRING)
	{
		free_bstring(bc, ei->string);
		run_error(bc, SYNTAX_ERROR);
		return;
	}
	end_val = ei->value;

	if(*(unsigned char *)*take == token_step)
	{
		++*take;
		ei->flags_in = 0;
		res = expr(bc, take, ei);
		if(ei->type == OT_BSTRING)
		{
			free_bstring(bc, ei->string);
			run_error(bc, SYNTAX_ERROR);
			return;
		}
		step_val = ei->value;
	} else
		step_val = 1.0;

	fi = findfor(bc, name);
	if(!fi)
		fi=newfor(bc);
	if(!fi)
	{
		run_error(bc, TOO_MANY_FORS);
		return;
	}
	strcpy(fi->name, name);
	fi->step = step_val;
	fi->end = end_val;
	if(**take == ':')
	{
		fi->nextline = bc->online;
		fi->nextbyte = *take + 1 - bc->lps[bc->online].line;
	} else
	{
		fi->nextline = bc->online+1;
		fi->nextbyte = 0;
	}
	set_variable(bc, name, start_val);
}

void donext(bc *bc, char **take)
{
struct forinfo *fi;
char name[16]={0};
int type;
struct variable *v;
int pos;

	if(!bc->numfors)
	{
		run_error(bc, NEXT_WITHOUT_FOR);
		return;
	}
	type=gather_variable_name(bc, name, take);
	if(type!=RANK_VARIABLE && type!=RANK_INVALID && name[0])
	{
		run_error(bc, INVALID_VARIABLE);
		return;
	}
	if(name[0])
	{
		fi=findfor(bc, name);
		if(!fi)
		{
			run_error(bc, NEXT_WITHOUT_FOR);
			return;
		}
	}
	else
		fi=bc->fors+bc->numfors-1;
	v=add_variable(bc, fi->name);
#warning check for zero return

	pos=fi->step>=0.0;
	if((pos && v->value >= fi->end) || (!pos && v->value <= fi->end))
	{
		--bc->numfors;
		memcpy(fi, bc->fors + bc->numfors, sizeof(*fi));
	}
	else
	{
#warning for i=1 to 10:next i  what value does i end up with? 10 or 11?
		set_variable(bc, fi->name, v->value + fi->step);
		bc->nextline = fi->nextline;
		bc->nextbyte = fi->nextbyte;
	}

}

void find_data_line(bc *bc)
{
char *p;
	bc->datatake = 0;
	while(bc->dataline < bc->numlines)
	{
		p=bc->lps[bc->dataline].line;
		while(*p && *(unsigned char *)p != token_data)
			++p;
		if(!*p) // no data on this line
		{
			++bc->dataline;
			continue;
		}
		++p;
		bc->datatake = p;
		break;
	}
}

double get_data_double(bc *bc)
{
einfo einfo, *ei=&einfo;
int res;

	if(!bc->datatake)
		find_data_line(bc);
	if(!bc->datatake)
	{
		run_error(bc, OUT_OF_DATA);
		return 0.0;
	}
	ei->flags_in = 0;
	res = expr(bc, &bc->datatake, ei);
	if(ei->type != OT_DOUBLE)
	{
		run_error(bc, INVALID_DATA "(line %d)",
			bc->lps[bc->dataline].linenum);
		if(ei->type == OT_BSTRING)
			free_bstring(bc, ei->string);
		return 0.0;
	}
	if(*bc->datatake == ',')
		++bc->datatake;
	else
	{
		bc->datatake = 0;
		++bc->dataline;
	}

	return ei->value;
}

void doread(bc *bc, char **take)
{
einfo einfo, *ei=&einfo;
int res;

	for(;;)
	{
		ei->flags_in = EXPR_LVALUE;
		res = expr(bc, take, ei);
		if(ei->type == OT_PBSTRING)
		{
			bstring *bs;
			bs = *(bstring **)ei->indirect;
			free_bstring(bc, bs);
			bs = make_bstring(bc, "foo", 3);
			*(bstring **)ei->indirect = bs;
		} else if(ei->type == OT_PDOUBLE)
		{
			*(double *)ei->indirect = get_data_double(bc);
		} else run_error(bc, SYNTAX_ERROR);
		break;
	}

}

int getdecimal(char **take)
{
int v=0;
	while(isdigit(**take))
		v=v*10 + *(*take)++ - '0';
	return v;
}

void dogoto(bc *bc, char **take)
{
int newline;
	newline = findrunline(bc, getdecimal(take));
	if(newline<0)
		run_error(bc, NO_SUCH_LINE);
	else
	{
		bc->nextline = newline;
		bc->nextbyte = 0;
	}
}

void gosub_push(bc *bc, char **take)
{
	if(bc->gosubsp == GOSUBMAX)
	{
		run_error(bc, STACK_OVERFLOW);
		return;
	}
	if(**take == ':')
	{
		++*take;
		bc->gosubstack[bc->gosubsp].nextline = bc->online;
		bc->gosubstack[bc->gosubsp++].nextbyte =
			*take - bc->lps[bc->online].line;
	} else
	{
		bc->gosubstack[bc->gosubsp].nextline = bc->online+1;
		bc->gosubstack[bc->gosubsp++].nextbyte = 0;
	}
}

void doon(bc *bc, char **take)
{
einfo einfo, *ei=&einfo;
int res=0;
int k;
int type;

	ei->flags_in = EXPR_NUMERIC;
	res = expr(bc, take, ei);
	if(res) return;
	type=*(unsigned char *)*take;
	if(type != token_goto && type != token_gosub)
	{
		run_error(bc, SYNTAX_ERROR);
		return;
	}
	++*take;
	k=ei->value;
	if(k<1)
	{
		run_error(bc, ON_RANGE_ERROR);
		return;
	}
	while(--k > 0)
	{
		while(isdigit(**take)) ++*take;
		if(**take!=',')
		{
			run_error(bc, ON_RANGE_ERROR);
			return;
		}
		++*take;
	}
	if(!isdigit(**take))
	{
		run_error(bc, ON_RANGE_ERROR);
		return;
	}
	dogoto(bc, take);
	if(type == token_gosub)
		gosub_push(bc, take);
}

void dogosub(bc *bc, char **take)
{
	dogoto(bc, take);
	gosub_push(bc, take);
}

void doreturn(bc *bc, char **take)
{
	if(!bc->gosubsp)
	{
		run_error(bc, BAD_RETURN);
		return;
	}
	bc->nextline = bc->gosubstack[--bc->gosubsp].nextline;
	bc->nextbyte = bc->gosubstack[bc->gosubsp].nextbyte;
}

void doend(bc *bc, char **take)
{
	bc->flags |= BF_ENDHIT;
}

void dostop(bc *bc, char **take)
{
	bc->flags |= BF_STOPHIT;
}

void dodata(bc *bc, char **take)
{
}

void doint(bc *bc, double *p)
{
	if(*p<0.0)
		*p=(int)*p - 1;
	else
		*p=(int)*p;
}

void dofix(bc *bc, double *p)
{
	*p=(int)*p;
}

void dosgn(bc *bc, double *p)
{
	*p = *p<0.0 ? -1 : (*p>0.0 ? 1 : 0.0);
}

void dosin(bc *bc, double *p)
{
	*p = sin(*p);
}

void doabs(bc *bc, double *p)
{
	if(*p<0.0) *p = -*p;
}

void dosqr(bc *bc, double *p)
{
	if(*p>=0.0) *p = sqrt(*p);
	else
		run_error(bc, SQRT_ERROR);
}

void docos(bc *bc, double *p)
{
	*p = cos(*p);
}

void dornd(bc *bc, double *p)
{
int r;
	r=*p;
	if(r)
		*p = 1 + rand()%r;
	else
		*p = (rand()&0x7fffffff)/(double)0x7fffffff;
}

void dopow(bc *bc, double *p)
{
	*p = pow(p[0], p[1]);
}

void dolog(bc *bc, double *p)
{
	*p = log(*p);
}

void doexp(bc *bc, double *p)
{
	*p = exp(*p);
}

void dotan(bc *bc, double *p)
{
	*p = tan(*p);
}

void doatn(bc *bc, double *p)
{
	*p = atan(*p);
}

void doatn2(bc *bc, double *p)
{
	*p = atan2(*p, p[1]);
}

void domove(bc *bc, char **take)
{
double list[2];
int res;
int got=2;

	res=comma_list(bc, take, list, &got, EXACT_NUM);
	if(!res)
	{
		bc->gx = list[0];
		bc->gy = list[1];
	}
}

void dopen(bc *bc, char **take)
{
double list[1];
int res;
int got=1;
	res=comma_list(bc, take, list, &got, EXACT_NUM);
	if(!res)
	{
		bc->pen = list[0];
	}
}

void doline(bc *bc, char **take)
{
double list[2];
int res;
int got=2;

	res=comma_list(bc, take, list, &got, EXACT_NUM);
	if(!res)
	{
		stroke(bc, list[0], list[1]);
	}
}

void docircle(bc *bc, char **take)
{
double list[3];
int res;
int got=3;

	res=comma_list(bc, take, list, &got, EXACT_NUM);
	if(!res)
	{
		circle(bc, list[0], list[1], list[2]);
	}
}

void dodisc(bc *bc, char **take)
{
double list[3];
int res;
int got=3;

	res=comma_list(bc, take, list, &got, EXACT_NUM);
	if(!res)
	{
		disc(bc, list[0], list[1], list[2]);
	}
}

void docolor(bc *bc, char **take)
{
double list[4];
int res;
int got=4;

	res=comma_list(bc, take, list, &got, 0);
	if(!res)
	{
		if(got<3)
			run_error(bc, "Command requires 3 or 4 parameters");
		else
		{
			bc->gred = list[0];
			bc->ggreen = list[1];
			bc->gblue = list[2];
			if(got==4)
				bc->galpha = list[3];
			else
				bc->galpha = 255;
		}
	}
}

// x,y,  width, height
void dobox(bc *bc, char **take)
{
double list[4];
int res;
int got=4;

	res=comma_list(bc, take, list, &got, EXACT_NUM);
	if(!res)
	{
		shape_init(bc);
		shape_add(bc, list[0]-list[2], list[1]-list[3], TAG_ONPATH);
		shape_add(bc, list[0]+list[2], list[1]-list[3], TAG_ONPATH);
		shape_add(bc, list[0]+list[2], list[1]+list[3], TAG_ONPATH);
		shape_add(bc, list[0]-list[2], list[1]+list[3], TAG_ONPATH);
		shape_done(bc);
	}
}

// x,y,  width, height
void dorect(bc *bc, char **take)
{
double list[4];
int res;
int got=4;
double pen2=bc->pen/2.0;

	res=comma_list(bc, take, list, &got, EXACT_NUM);
	if(!res)
	{
		shape_init(bc);
		shape_add(bc, list[0]-list[2]-pen2, list[1]-list[3]-pen2, TAG_ONPATH);
		shape_add(bc, list[0]+list[2]+pen2, list[1]-list[3]-pen2, TAG_ONPATH);
		shape_add(bc, list[0]+list[2]+pen2, list[1]+list[3]+pen2, TAG_ONPATH);
		shape_add(bc, list[0]-list[2]-pen2, list[1]+list[3]+pen2, TAG_ONPATH);
		shape_end(bc);
		shape_add(bc, list[0]-list[2]+pen2, list[1]-list[3]+pen2, TAG_ONPATH);
		shape_add(bc, list[0]+list[2]-pen2, list[1]-list[3]+pen2, TAG_ONPATH);
		shape_add(bc, list[0]+list[2]-pen2, list[1]+list[3]-pen2, TAG_ONPATH);
		shape_add(bc, list[0]-list[2]+pen2, list[1]+list[3]-pen2, TAG_ONPATH);
		shape_done(bc);
	}
}

void dosleep(bc *bc, char **take)
{
double list[1];
int res;
int got=1;
int until, diff;

	res=comma_list(bc, take, list, &got, EXACT_NUM);
	if(!res)
	{
		if(list[0]<0) return;
		if(list[0]>2.0) list[0]=2.0;
		until=bc->waitbase + list[0]*1000;

		for(;;)
		{
			diff = until - SDL_GetTicks();
			if(diff<0 || diff>2000)
			{
//printf("%s: diff out of range! %d\n", __FUNCTION__, diff);
				reset_waitbase(bc);
				break;
			}
			SDL_Delay(1);
			scaninput(bc);
		}
	}
}

void dohome(bc *bc)
{
	tprintf(bc, "\0330x\0330y");
}

void docls(bc *bc, char **take)
{
	cleartext(bc);
	dohome(bc);
	fillscreen(bc, 0, 0, 0, 255);
}

void doclear(bc *bc, char **take)
{
//	run_error(bc, SYNTAX_ERROR);
}

void dorandom(bc *bc, char **take)
{
#warning random doesn't really randomize...
}

void dofill(bc *bc, char **take)
{
	cleartext(bc);
	fillscreen(bc, bc->gred, bc->ggreen, bc->gblue, bc->galpha);
}

void dotest(bc *bc)
{
	rendertest(bc);
}

void dospot(bc *bc)
{
	spot(bc);
}

void dolen(bc *bc, struct gen_func_ret *gfr)
{
einfo einfo, *ei=&einfo;
int res;

	ei->flags_in = EXPR_STRING;
	res = expr(bc, gfr->take, ei);

	gfr->type = OT_DOUBLE;

	if(ei->type == OT_BSTRING)
	{
		gfr->value = ei->string->length;
			free_bstring(bc, ei->string);
	}
	else
		gfr->value = 0.0;
}

#define SLEFT 0
#define SMID 1
#define SRIGHT 2

void doleftmidrightstring(bc *bc, struct gen_func_ret *gfr, int type)
{
einfo einfo, *ei=&einfo;
int res;
bstring *s1=0, *s2;
int start=0, len=0;
int v1, v2;

	gfr->type = OT_DOUBLE;

	ei->flags_in = EXPR_STRING;
	res = expr(bc, gfr->take, ei);
	if(ei->type == OT_BSTRING)
		s1 = ei->string;
	if(res) goto err;

	if(comma(bc, gfr->take)) goto err;

	ei->flags_in = EXPR_NUMERIC;
	res = expr(bc, gfr->take, ei);
	if(res) goto err;

	v1 = ei->value;
	if(type == SMID)
	{
		if(comma(bc, gfr->take)) goto err;
		ei->flags_in = EXPR_NUMERIC;
		res = expr(bc, gfr->take, ei);
		if(res) goto err;
		v2 = ei->value;
	}

	if(type == SMID)
		len = v2;
	else
		len = v1;

	if(len > s1->length)
		len = s1->length;
	if(len<0)
		len = 0;

	if(type==SLEFT)
			start = 0;
	else if(type==SRIGHT)
		start = s1->length - len;
	else
	{
		start = v1-1;
		if(start<0)
			start=0;
		if(start+len > s1->length)
			start = s1->length - len;
	}

	s2 = make_bstring(bc, s1->string + start, len);
#warning check if string was allocated

	gfr->string = s2;
	gfr->type = OT_BSTRING;
err:
	free_bstring(bc, s1);
}

void doleftstring(bc *bc, struct gen_func_ret *gfr)
{
	doleftmidrightstring(bc, gfr, SLEFT);
}

void domidstring(bc *bc, struct gen_func_ret *gfr)
{
	doleftmidrightstring(bc, gfr, SMID);
}

void dorightstring(bc *bc, struct gen_func_ret *gfr)
{
	doleftmidrightstring(bc, gfr, SRIGHT);
}

#define TYPE_CHR 0
#define TYPE_STR 1
void dochrstrstring(bc *bc, struct gen_func_ret *gfr, int type)
{
int res;
einfo einfo, *ei=&einfo;
	gfr->type = OT_BSTRING;

	ei->flags_in = EXPR_NUMERIC;
	res = expr(bc, gfr->take, ei);
	if(res)
	{
		gfr->type = OT_DOUBLE;
		gfr->value = 0.0;
	} else
	{
		char t[128];
		gfr->type = OT_BSTRING;
		if(type == TYPE_CHR)
		{
			t[0] = ei->value;
			t[1]=0;
		} else
		{
			if(ei->value != (int)ei->value)
				snprintf(t, sizeof(t), "%.2lf", ei->value);
			else
				snprintf(t, sizeof(t), "%d", (int)ei->value);
		}
		gfr->string = make_bstring(bc, t, strlen(t));
	}
}

void dochrstring(bc *bc, struct gen_func_ret *gfr)
{
	dochrstrstring(bc, gfr, TYPE_CHR);
}
void dostrstring(bc *bc, struct gen_func_ret *gfr)
{
	dochrstrstring(bc, gfr, TYPE_STR);
}

void dostringstring(bc *bc, struct gen_func_ret *gfr)
{
int res;
einfo einfo, *ei=&einfo;
int len;

	gfr->type = OT_DOUBLE;
	gfr->value = 0.0;

	ei->flags_in = EXPR_NUMERIC;
	res = expr(bc, gfr->take, ei);
	if(res)
		return;
	len=ei->value;
	if(len<0)
	{
		error(bc, BADVALUE);
		return;
	}
	if(comma(bc, gfr->take))
		return;
	ei->flags_in = EXPR_STRING;
	res = expr(bc, gfr->take, ei);
	if(res)
		return;
	if(ei->string->length<1)
		error(bc, BADVALUE);
	else
	{
		gfr->string = make_raw_bstring(bc, len);
		memset(gfr->string->string, ei->string->string[0], len);
		gfr->type = OT_BSTRING;
	}
	free_bstring(bc, ei->string);
}


#define TYPE_VAL 0
#define TYPE_ASC 1
void dovalasc(bc *bc, struct gen_func_ret *gfr, int type)
{
int res;
einfo einfo, *ei=&einfo;
bstring *s1=0;
char scopy[64];
int len;
	ei->flags_in = EXPR_STRING;
	res = expr(bc, gfr->take, ei);
	if(ei->type == OT_BSTRING)
		s1 = ei->string;
	if(res) goto err;
	gfr->type = OT_DOUBLE;
	gfr->value = 0.0;
	len=s1->length;

	if(type==TYPE_VAL)
	{
		if(len>sizeof(scopy)-1)
			len=sizeof(scopy)-1;
		strncpy(scopy, s1->string, len);
		scopy[len]=0;
		sscanf(scopy, "%lf", &gfr->value);
	} else if(len>0)
		gfr->value = s1->string[0];
err:
	free_bstring(bc, s1);
}

void doval(bc *bc, struct gen_func_ret *gfr)
{
	dovalasc(bc, gfr, TYPE_VAL);
}

void doasc(bc *bc, struct gen_func_ret *gfr)
{
	dovalasc(bc, gfr, TYPE_ASC);
}


void domousex(bc *bc, struct gen_func_ret *gfr)
{
	gfr->value = bc->mousex;
}

void domousey(bc *bc, struct gen_func_ret *gfr)
{
	gfr->value = bc->mousey;
}

void domouseb(bc *bc, struct gen_func_ret *gfr)
{
	gfr->value = bc->mouseb;
}

void doxsize(bc *bc, struct gen_func_ret *gfr)
{
	gfr->value = bc->xsize;
}

void doysize(bc *bc, struct gen_func_ret *gfr)
{
	gfr->value = bc->ysize;
}

void doticks(bc *bc, struct gen_func_ret *gfr)
{
	gfr->value = SDL_GetTicks() - bc->starttime;
}


void doupdate(bc *bc, char **take)
{
	forceupdate(bc);
}


int token_then;
int token_to;
int token_else;
int token_if;
int token_to;
int token_step;
int token_data;
int token_and;
int token_or;
int token_mod;
int token_goto;
int token_gosub;
int token_tab;

struct stmt {
	char *name;
	void (*func)();
	int token_flags;
	int *token_code;
};

#define TOKEN_FUNCTION        0x1 // flag
#define TOKEN_STATEMENT       0x2 // can execute it
#define TOKEN_2PARS           0x4 // function has 2 parameters
#define TOKEN_3PARS           0x8 // function has 3 parameters
#define TOKEN_GENERAL        0x10 // not numbers only for return + parameters
#define TOKEN_STATUS         0x20 // one of the special symbols

struct stmt statements[]={
{"rem", dorem, TOKEN_STATEMENT, 0},
{"let", dolet, TOKEN_STATEMENT, 0},
{"input", doinput, TOKEN_STATEMENT, 0},
{"print", doprint, TOKEN_STATEMENT, 0},
{"?", doprint, TOKEN_STATEMENT, 0},
{"goto", dogoto, TOKEN_STATEMENT, &token_goto},
{"read", doread, TOKEN_STATEMENT, 0},
{"dim", dodim, TOKEN_STATEMENT, 0},
{"then", 0, 0, &token_then},
{"for", dofor, TOKEN_STATEMENT},
{"to", 0, 0, &token_to},
{"step", 0, 0, &token_step},
{"next", donext, TOKEN_STATEMENT},
{"if", doif, TOKEN_STATEMENT, &token_if},
{"else", 0, 0, &token_else},
{"gosub", dogosub, TOKEN_STATEMENT, &token_gosub},
{"return", doreturn, TOKEN_STATEMENT, 0},
{"end", doend, TOKEN_STATEMENT, 0},
{"stop", dostop, TOKEN_STATEMENT, 0},
{"data", dodata, TOKEN_STATEMENT, &token_data},
{"int", doint, TOKEN_FUNCTION, 0},  // 20
{"fix", dofix, TOKEN_FUNCTION, 0},  // 20
{"sgn", dosgn, TOKEN_FUNCTION, 0},
{"sin", dosin, TOKEN_FUNCTION, 0},
{"cos", docos, TOKEN_FUNCTION, 0},
{"rnd", dornd, TOKEN_FUNCTION, 0},
{"pow", dopow, TOKEN_FUNCTION | TOKEN_2PARS, 0},
{"log", dolog, TOKEN_FUNCTION, 0},
{"exp", doexp, TOKEN_FUNCTION, 0},
{"tan", dotan, TOKEN_FUNCTION, 0},
{"atn2", doatn2, TOKEN_FUNCTION | TOKEN_2PARS, 0},
{"atn", doatn, TOKEN_FUNCTION, 0},
{"abs", doabs, TOKEN_FUNCTION, 0},
{"sqr", dosqr, TOKEN_FUNCTION, 0},
{"move", domove, TOKEN_STATEMENT, 0},
{"pen", dopen, TOKEN_STATEMENT, 0},
{"line", doline, TOKEN_STATEMENT, 0},
{"color", docolor, TOKEN_STATEMENT, 0},
{"clear", doclear, TOKEN_STATEMENT, 0},
{"random", dorandom, TOKEN_STATEMENT, 0},
{"cls", docls, TOKEN_STATEMENT, 0},
{"fill", dofill, TOKEN_STATEMENT, 0},
{"home", dohome, TOKEN_STATEMENT, 0},
{"circle", docircle, TOKEN_STATEMENT, 0},
{"disc", dodisc, TOKEN_STATEMENT, 0},
{"test", dotest, TOKEN_STATEMENT, 0},
{"box", dobox, TOKEN_STATEMENT, 0},
{"rect", dorect, TOKEN_STATEMENT, 0},
{"sleep", dosleep, TOKEN_STATEMENT, 0}, // 40
{"spot", dospot, TOKEN_STATEMENT, 0},
{"len", dolen, TOKEN_FUNCTION|TOKEN_GENERAL, 0},
{"mousex", domousex, TOKEN_STATUS, 0},
{"mousey", domousey, TOKEN_STATUS, 0},
{"mouseb", domouseb, TOKEN_STATUS, 0},
{"xsize", doxsize, TOKEN_STATUS, 0},
{"ysize", doysize, TOKEN_STATUS, 0},
{"ticks", doticks, TOKEN_STATUS, 0},
{"update", doupdate, TOKEN_STATEMENT, 0},
{"and", 0, 0, &token_and},
{"or", 0, 0, &token_or},
{"mod", 0, 0, &token_mod},
{"on", doon, TOKEN_STATEMENT, 0},
{"left$", doleftstring, TOKEN_FUNCTION|TOKEN_GENERAL,0},
{"mid$", domidstring, TOKEN_FUNCTION|TOKEN_GENERAL,0},
{"right$", dorightstring, TOKEN_FUNCTION|TOKEN_GENERAL, 0},
{"chr$", dochrstring, TOKEN_FUNCTION|TOKEN_GENERAL, 0},
{"str$", dostrstring, TOKEN_FUNCTION|TOKEN_GENERAL, 0},
{"string$", dostringstring, TOKEN_FUNCTION|TOKEN_GENERAL, 0},
{"val", doval, TOKEN_FUNCTION|TOKEN_GENERAL, 0},
{"asc", doasc, TOKEN_FUNCTION|TOKEN_GENERAL, 0},
{"tab", 0, 0, &token_tab},
{0,0}};

struct stmt *to_statement(bc *bc, int token)
{
	token = 255-token;
	return (token>=0 && token<bc->numstatements) ? statements+token : 0;
}

int is_numeric_function(bc *bc, int token)
{
struct stmt *s;
	s=to_statement(bc, token);
	return s && (s->token_flags&TOKEN_FUNCTION) &&
		(~s->token_flags&TOKEN_GENERAL);
}

int is_general_function(bc *bc, int token)
{
struct stmt *s;
	s=to_statement(bc, token);
	return s && (s->token_flags&TOKEN_FUNCTION) &&
		(s->token_flags&TOKEN_GENERAL);
}

int function_parameter_count(bc *bc, int token)
{
struct stmt *s;
	s=to_statement(bc, token);
	if(!s || !(s->token_flags & TOKEN_FUNCTION)) return 0;
	return (s->token_flags & TOKEN_3PARS) ? 3 :
		(s->token_flags & TOKEN_2PARS) ? 2 : 1;
}

int is_status(bc *bc, int token)
{
struct stmt *s;
	s=to_statement(bc, token);
	return s && (s->token_flags & TOKEN_STATUS);
}

void (*statement_handler(bc *bc, int token))()
{
struct stmt *s;
	s=to_statement(bc, token);
	return s ? s->func : 0;

}

int convertline(bc *bc, char *put, char *take)
{
int err=0;
struct stmt *s;
char c;
int i;
	while(!err)
	{
		if(*take=='\'') // comment introducer as special case
			break;
		skipwhite(&take);
		c=tolower(*take);
		if(!c || c=='\n') break;
		if(c=='"')
		{
			*put++ = *take++;
			for(;;)
			{
				c=*take;
				if(c=='\\')
				{
					++take;
					*put++ = c;
					c=*take++;
					if(c) {*put++ = c;continue;}
				}
				if(!c || c=='\n')
				{
					run_error(bc, NON_TERMINATED_STRING);
					err=1;
					break;
				}
				++take;
				*put++ = c;
				if(c=='"')
					break;
			}
			continue;
		}
		s=statements;
		while(s->name)
		{
			for(i=0;s->name[i];++i)
			{
				if(s->name[i] != tolower(take[i]))
					break;
			}
			if(!s->name[i]) break;
			++s;
		}
		if(s->name)
		{
			take+=i;
			if(s->func == dorem) // don't bother storing it
				break;
			*put++ = 255 - (s - statements); // convert to token number
			continue;
		}
		*put++ = *take++;
	}
	*put=0;
	return err;
}

void free_variables(bc *bc)
{
int i;
struct variable *v;
	for(i=0;i>bc->numvariables;++i)
	{
		v=bc->vars+i;
		if((v->rank&RANK_MASK) != RANK_VARIABLE && v->array)
		{
			free(v->array);
#warning not enough for bstrings
			v->array = 0;
		}
	}
	bc->numvariables = 0;
}

int token_flags(bc *bc, unsigned char val)
{
	return bc->tokenmap[val];
}

void (*token_handler(bc *bc, unsigned char val))(bc *, char **)
{
	return statements[255-val].func;
}

void execute(bc *bc, char **p)
{
unsigned char f;

	if(0)
	{
		int i;
		unsigned char c;
		printf("%d ", bc->lps[bc->online].linenum);
		for(i=0;(c=(*p)[i]);++i)
		{
			if(c<128) putchar(c);
			else printf("%s", statements[255-c].name);
		}
		printf("\n");
	}

	if((f=*(*(unsigned char **)p)++)>=128)
	{
		struct stmt *s;
		s = statements + 255-f;
		if(s->token_flags & TOKEN_STATEMENT)
			s->func(bc, p);
	} else
	{
		if(*--*p) dolet(bc, p);
	}
}

void runinit(bc *bc)
{
struct stmt *st;
	bc->numlines=0;
	bc->flags = 0;
	bc->dataline = 0;
	bc->datatake = 0;
	bc->gosubsp = 0;
	bc->numfors = 0;
	bc->online = 0;
	bc->nextline = 0;
	bc->nextbyte = 0;
	bc->execute_count = 0;
	bc->gx=0;
	bc->gy=0;
	bc->gred=255;
	bc->ggreen=255;
	bc->gblue=255;
	bc->galpha=255;
	bc->pen = 1.0;
	bc->shape_numpoints = 0;
	bc->shape_numcontours = 0;
	free_variables(bc);

	memset(bc->tokenmap, 0, sizeof(bc->tokenmap));
	st=statements;
	bc->numstatements=0;
	while(st->name)
	{
		bc->tokenmap[255-(st-statements)] = st->token_flags;
		if(st->token_code)
			*st->token_code = 255-(st-statements);
		++st;
		++bc->numstatements;
	}
	bc->starttime = SDL_GetTicks();
	reset_waitbase(bc);
}

void dorun(bc *bc, char *line)
{
char *put;
char *take;
int linenum;
int err;

// ************************ SET UP FOR RUN
	runinit(bc);

	put=bc->runnable;
	take=bc->program;
	while(*take)
	{
		linenum=atoi(take);
		bc->lps[bc->numlines].linenum = linenum;
		bc->lps[bc->numlines].line = put;
		++bc->numlines;
		while(*take>='0' && *take<='9') take++;
		err=convertline(bc, put, take);
		if(err)
		{
			error(bc, "Error before running: %s in line %d", bc->lineerror,
					linenum);
			return;
		}
		put += strlen(put)+1;
		while(*take && *take++!= '\n');
	}
if(0){
int fd;
fd=open("/ram/ttt", O_WRONLY|O_CREAT|O_TRUNC, 0644);
write(fd, bc->runnable, put-bc->runnable);
close(fd);
}

	bc->flags |= BF_RUNNING;

	while(!(bc->flags & (BF_CCHIT | BF_RUNERROR | BF_ENDHIT |
		 BF_STOPHIT | BF_QUIT)))
	{
		if(bc->nextline<0)
		{
			if(*take == ':')
				++take;
			else
			{
				bc->nextline = bc->online+1;
				bc->nextbyte = 0;
			}
		}
		if(bc->nextline>=0)
		{
			bc->online = bc->nextline;
			if(bc->online >= bc->numlines)
			{
				bc->flags |= BF_ENDHIT; // fake an end
				continue;
			}
			take=bc->lps[bc->online].line + bc->nextbyte;
			bc->nextline = -1;
			bc->nextbyte = 0;
		}
		execute(bc, &take);

		++bc->execute_count;
		if(!(bc->execute_count & 1023))
			scaninput(bc);
//if(bc->execute_count%1000000 == 0) printf("%d\n", bc->execute_count);

	}
	bc->flags &= ~BF_RUNNING;

	if(bc->flags & BF_CCHIT)
	{
		tprintf(bc, "\nControl-C stopped on line %d\n", currentline(bc));
		bc->flags &= ~BF_CCHIT;
		flushinput(bc);
	}
	if(bc->flags & BF_RUNERROR)
	{
		error(bc, "Error on line %d: %s", currentline(bc), bc->lineerror);
		bc->flags &= ~BF_RUNERROR;
	}
	if(bc->flags & BF_ENDHIT)
	{
		tprintf(bc, "\nProgram terminated normally\n");
		bc->flags &= ~BF_ENDHIT;
	}
	if(bc->flags & BF_STOPHIT)
	{
		tprintf(bc, "\nStop on line %d\n", currentline(bc));
		bc->flags &= ~BF_STOPHIT;
	}
/*	printf("Elapsed time %.3f seconds.\n",
		(SDL_GetTicks()-bc->starttime)/1000.0);
*/
	tprintf(bc, "Elapsed time %.3f seconds.\n",
		(SDL_GetTicks()-bc->starttime)/1000.0);
	resetupdate(bc);
}