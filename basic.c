#include <SDL.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

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

void doexit(bc *bc, char *text)
{
	bc->doneflag = 1;
}

struct cmd commandlist[]={
{"exit", doexit},
{0, 0}};

void error(bc *bc, char *s, ...)
{
va_list ap;
char temp[1024];
	va_start(ap, s);
	vsnprintf(temp, sizeof(temp), s, ap);
	va_end(ap);
	tprintf(bc, "%s\n", temp);
}

char *findline(bc *bc, int linenumber)
{
char *p=bc->program;
char text[32];
int tl;

	sprintf(text, "%d", linenumber);
	tl=strlen(text);
	while(*p)
	{
		if(!strncmp(p, text, tl) && iswhite(p[tl]))
			return p;
		while(*p && *p++ != '\n');
	}
	return 0;
}



void addline(bc *bc, char *line)
{
int lnum;
printf("addline\n");
	lnum = atoi(line);
	error(bc, "Not enough space to add line %d", 100);

}

void deleteline(bc *bc, int lnum)
{
char *p, *p2;
	p=findline(bc, lnum);
	if(p)
	{
		p2=p;
		while(*p2 && *p2++ != '\n');
		while((*p++ = *p2++));
	}
}

int processline(bc *bc, char *line)
{
char token[64], *lp;
struct cmd *cmd;

	lp=line;
	gettoken(token, sizeof(token), &lp);
	skipwhite(&lp);

	cmd=commandlist;
	while(cmd->name)
	{
		if(!strcmp(token, cmd->name))
		{
			cmd->func(bc, lp);
			break;
		}
		++cmd;
	}
	if(cmd->name) return 0; // we found a match
	if(token[0]>='0' && token[0]<='9') // line number
	{
		if(*lp)
			addline(bc, line);
		else
			deleteline(bc, atoi(token));
	}

	return 0;
}
