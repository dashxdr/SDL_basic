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

char *findnextline(bc *bc, int linenumber)
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
	p=findnextline(bc, lnum);
	len=strlen(line)+1;
	memmove(p+len, p, strlen(p)+1);
	memcpy(p, line, len-1);
	p[len-1] = '\n';
}


void doexit(bc *bc, char *text)
{
	bc->flags |= BF_QUIT;
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
	min=0;
	max=0x7fffffff;

	if(sscanf(text, "%d-%d", &v1, &v2) == 2)
	{
		min=v1;
		max=v2;
	} else if(*text=='-' && text[1]>='0' && text[1]<='9')
		max=atoi(text+1);
	else if(sscanf(text, "%d", &v1) == 1)
	{
		min=v1;
		while(*text>='0' && *text<='9') ++text;
		if(*text != '-')
			max=v1;
	}

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

void donew(bc *bc, char *text)
{
	bc->program[0]=0;
	bc->filename[0]=0;
	tprintf(bc, "Program erased.\n");
	runinit(bc);
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
{"run", dorun},
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
		char tokenized[1024], *p=tokenized;
		convertline(bc, tokenized, line);

		execute(bc,&p);
		if(bc->flags & BF_RUNERROR)
		{
			bc->flags ^= BF_RUNERROR;
			tprintf(bc, "%s\n", bc->lineerror);
		}
//		unknown_command(bc, line);
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
		free_bstring(ei->string);
}

void doprint(bc *bc, char **take)
{
einfo einfo, *ei=&einfo;
int res;
int newline=1;

	while(**take && **take != '\n' && *(unsigned char *)*take!=token_else)
	{
		newline=1;
		ei->flags_in = 0;
		res = expr(bc, take, ei);
		if(ei->type == OT_DOUBLE)
		{
			if((long)ei->value == ei->value)
				tprintf(bc, " %ld", (long)ei->value);
			else
				tprintf(bc, " %.16f", ei->value);
		} else if(ei->type == OT_BSTRING)
		{
			bstring *bs = ei->string;
			if(bs)
				tprintf(bc, "%s", bs->string);
			free_bstring(bs);
		}
		if(!**take) break;
		if(**take != ';')
		{
			if(*(unsigned char *)*take!=token_else)
				run_error(bc, SYNTAX_ERROR);
			break;
		}
		newline=0;
		++*take;
	}
	if(newline)
		tprintf(bc, "\n");
}

void doinput(bc *bc, char **take)
{
einfo einfo, *ei=&einfo;
int res;
	tprintf(bc, "? ");
	typeline(bc, "", 1);

	ei->flags_in = EXPR_LVALUE;
	res = expr(bc, take, ei);
	if(ei->type == OT_PBSTRING)
	{
		bstring *bs;
		bs = *(bstring **)ei->indirect;
		free_bstring(bs);
		bs = make_bstring(bc->debline, strlen(bc->debline));
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
		free_bstring(ei->string);
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
					bc->nextline = newline;
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
		if(**take)
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
		free_bstring(ei->string);
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
		free_bstring(ei->string);
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
			free_bstring(ei->string);
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
	fi->nextline = bc->nextline;
	set_variable(bc, name, start_val);
}

void doto(bc *bc, char **take)
{
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
			free_bstring(ei->string);
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
			free_bstring(bs);
			bs = make_bstring("foo", 3);
			*(bstring **)ei->indirect = bs;
		} else if(ei->type == OT_PDOUBLE)
		{
			*(double *)ei->indirect = get_data_double(bc);
		} else run_error(bc, SYNTAX_ERROR);
		break;
	}

}

void donext(bc *bc, char **take)
{
struct forinfo *fi;
char name[16];
int type;
struct variable *v;
int pos;

	if(!bc->numfors)
	{
		run_error(bc, NEXT_WITHOUT_FOR);
		return;
	}
	type=gather_variable_name(bc, name, take);
	if(name[0] && type!=RANK_VARIABLE)
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
		set_variable(bc, fi->name, v->value + fi->step);
		bc->nextline = fi->nextline;
	}



}

void dogoto(bc *bc, char **take)
{
int newline;
int linewant;
	linewant = atoi(*take);
	newline = findrunline(bc, linewant);
	if(newline<0)
		run_error(bc, NO_SUCH_LINE);
	else
		bc->nextline = newline;
}

void dogosub(bc *bc, char **take)
{
	if(bc->gosubsp == GOSUBMAX)
	{
		run_error(bc, STACK_OVERFLOW);
		return;
	}
	bc->gosubstack[bc->gosubsp++] = bc->nextline;
	dogoto(bc, take);
}

void doreturn(bc *bc, char **take)
{
	if(!bc->gosubsp)
	{
		run_error(bc, BAD_RETURN);
		return;
	}
	bc->nextline = bc->gosubstack[--bc->gosubsp];
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

void docos(bc *bc, double *p)
{
	*p = cos(*p);
}

void dornd(bc *bc, double *p)
{
	*p = rand()%((int)*p);
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

	res=comma_list(bc, take, list, &got, EXACT_NUM);
	if(!res)
	{
		if(list[0]<0) return;
		if(list[0]>2.0) list[0]=2.0;
		usleep(list[0]*1000000);
	}
}


void doclear(bc *bc, char **take)
{
	cleartext(bc);
	fillscreen(bc, 0, 0, 0, 255);
}

void dofill(bc *bc, char **take)
{
	cleartext(bc);
	fillscreen(bc, bc->gred, bc->ggreen, bc->gblue, bc->galpha);
}

void dohome(bc *bc)
{
	tprintf(bc, "\0330x\0330y");
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
			free_bstring(ei->string);
	}
	else
		gfr->value = 0.0;

}


int token_then;
int token_to;
int token_else;
int token_if;
int token_to;
int token_step;
int token_data;

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

struct stmt statements[]={
{"rem", dorem, TOKEN_STATEMENT, 0},
{"let", dolet, TOKEN_STATEMENT, 0},
{"input", doinput, TOKEN_STATEMENT, 0},
{"print", doprint, TOKEN_STATEMENT, 0},
{"goto", dogoto, TOKEN_STATEMENT, 0},
{"read", doread, TOKEN_STATEMENT, 0},
{"dim", dodim, TOKEN_STATEMENT, 0},
{"then", 0, 0, &token_then},
{"for", dofor, TOKEN_STATEMENT},
{"to", doto, 0, &token_to},
{"step", 0, 0, &token_step},
{"next", donext, TOKEN_STATEMENT},
{"if", doif, TOKEN_STATEMENT, &token_if},
{"else", 0, 0, &token_else},
{"gosub", dogosub, TOKEN_STATEMENT, 0},
{"return", doreturn, TOKEN_STATEMENT, 0},
{"end", doend, TOKEN_STATEMENT, 0},
{"stop", dostop, TOKEN_STATEMENT, 0},
{"data", dodata, TOKEN_STATEMENT, &token_data},
{"int", doint, TOKEN_FUNCTION, 0},
{"sgn", dosgn, TOKEN_FUNCTION, 0},
{"sin", dosin, TOKEN_FUNCTION, 0},
{"cos", docos, TOKEN_FUNCTION, 0},
{"rnd", dornd, TOKEN_FUNCTION, 0},
{"pow", dopow, TOKEN_FUNCTION | TOKEN_2PARS, 0},
{"log", dornd, TOKEN_FUNCTION, 0},
{"exp", dornd, TOKEN_FUNCTION, 0},
{"move", domove, TOKEN_STATEMENT, 0},
{"pen", dopen, TOKEN_STATEMENT, 0},
{"line", doline, TOKEN_STATEMENT, 0},
{"color", docolor, TOKEN_STATEMENT, 0},
{"clear", doclear, TOKEN_STATEMENT, 0},
{"fill", dofill, TOKEN_STATEMENT, 0},
{"home", dohome, TOKEN_STATEMENT, 0},
{"circle", docircle, TOKEN_STATEMENT, 0},
{"disc", dodisc, TOKEN_STATEMENT, 0},
{"test", dotest, TOKEN_STATEMENT, 0},
{"box", dobox, TOKEN_STATEMENT, 0},
{"rect", dorect, TOKEN_STATEMENT, 0},
{"sleep", dosleep, TOKEN_STATEMENT, 0},
{"spot", dospot, TOKEN_STATEMENT, 0},
{"len", dolen, TOKEN_FUNCTION|TOKEN_GENERAL, 0},
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
	bc->nextline = 0;
	bc->execute_count = 0;
	bc->gx=0;
	bc->gy=0;
	bc->gred=255;
	bc->ggreen=255;
	bc->gblue=255;
	bc->galpha=255;
	bc->pen = 1.0;
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
		char *p;
		updatef(bc);
		scaninput(bc);
		bc->online = bc->nextline;
		if(bc->online >= bc->numlines)
		{
			bc->flags |= BF_ENDHIT; // fake an end
			continue;
		}
		p=bc->lps[bc->online].line;
		++bc->nextline;
		execute(bc, &p);
		++bc->execute_count;
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
	printf("Elapsed time %.3f seconds.\n",
		(SDL_GetTicks()-bc->starttime)/1000.0);
	tprintf(bc, "Elapsed time %.3f seconds.\n",
		(SDL_GetTicks()-bc->starttime)/1000.0);
}
