#include <SDL.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>

#include "misc.h"

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
}

void dorun(bc *bc, char *text);

struct cmd commandlist[]={
{"list", dolist},
{"edit", doedit},
{"save", dosave},
{"load", doload},
{"info", doinfo},
{"new", donew},
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
		unknown_command(bc, line);
	else
		bc->flags |= BF_NOPROMPT;

	return;
}

#define SYNTAX_ERROR          "Syntax error"
#define NON_TERMINATED_STRING "Nonterminated string"
#define NO_SUCH_LINE          "Invalid line number"

void run_error(bc *bc, char *s, ...)
{
va_list ap;
	va_start(ap, s);
	vsnprintf(bc->lineerror, sizeof(bc->lineerror), s, ap);
	va_end(ap);
	bc->flags |= BF_RUNERROR;
}


struct stmt {
	char *name;
	void (*func)(bc *, char **take);
};

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

void dorem(bc *bc, char **take)
{
}

void dolet(bc *bc, char **take)
{
}

void doprint(bc *bc, char **take)
{
double value;
	value = expr(bc, take);
	if((long)value == value)
		tprintf(bc, "%ld\n", (long)value);
	else
		tprintf(bc, "%.16f\n", value);
}

void doinput(bc *bc, char **take)
{
}

void dogoto(bc *bc, char **take)
{
int newline;
	newline = findrunline(bc, atoi(*take));
	if(newline<0)
		run_error(bc, NO_SUCH_LINE);
	else
		bc->nextline = newline;
}

void doif(bc *bc, char **take)
{
}

void dodim(bc *bc, char **take)
{
}

void dothen(bc *bc, char **take)
{
}

void dofor(bc *bc, char **take)
{
}

void doto(bc *bc, char **take)
{
}

void doread(bc *bc, char **take)
{
}

void donext(bc *bc, char **take)
{
}

void dogosub(bc *bc, char **take)
{
}

void doreturn(bc *bc, char **take)
{
}

void doend(bc *bc, char **take)
{
}

void dodata(bc *bc, char **take)
{
}

void doint(bc *bc, char **take)
{
}

void dosgn(bc *bc, char **take)
{
}

void dosin(bc *bc, char **take)
{
}

void docos(bc *bc, char **take)
{
}

void dornd(bc *bc, char **take)
{
}


struct stmt statements[]={
{"rem", dorem},
{"let", dolet},
{"input", doinput},
{"print", doprint},
{"goto", dogoto},
{"read", doread},
{"dim", dodim},
{"then", dothen},
{"for", dofor},
{"to", doto},
{"next", doif},
{"if", doif},
{"gosub", dogosub},
{"return", doreturn},
{"end", doend},
{"data", dodata},
{"int", doint},
{"sgn", dosgn},
{"sin", dosin},
{"cos", docos},
{"rnd", dornd},

{0,0}};


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

void dorun(bc *bc, char *line)
{
char *put;
char *take;
int linenum;
int err;

// tokenize program
// delete all variables
// start executing on first line

	put=bc->runnable;
	take=bc->program;
	bc->numlines=0;
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

	if(!bc->numlines) return;
	bc->nextline = 0;
	while(!(bc->flags & (BF_CCHIT | BF_RUNERROR)))
	{
		char *p;
		int f;
		updatef(bc);
		scaninput(bc);
		bc->online = bc->nextline;
		if(bc->online >= bc->numlines)
			break; // out of lines
		p=bc->lps[bc->online].line;
		++bc->nextline;
		if((f=*(unsigned char *)p++)>=128)
		{
			statements[255-f].func(bc, &p);
		} else
		{
			--p;
		}
	}
	if(bc->flags & BF_CCHIT)
	{
		tprintf(bc, "\nControl-C stopped on line %d\n", currentline(bc));
		bc->flags &= ~BF_CCHIT;
	}
	if(bc->flags & BF_RUNERROR)
	{
		error(bc, "Error on line %d: %s", currentline(bc), bc->lineerror);
		bc->flags &= ~BF_RUNERROR;
	}


}
