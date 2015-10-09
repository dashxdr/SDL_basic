/*
   SDL_basic written by David Ashley, released 20080621 under the GPL
   http://www.linuxmotors.com/SDL_basic
   dashxdr@gmail.com
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>
#include <dirent.h>

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
	pruninit(bc);
}

void doren(bc *bc, char *text)
{
int v1=10, v2=10;
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
	if(n<1) v1=10;
	if(n<2) v2=v1;
	renumber(bc, v1, v2);
}

void doparse(bc *bc, char *text)
{
	parse(bc, 0); // don't run it
}

void dopr(bc *bc, char *text)
{
	parse(bc, 1); // run it
}

int nametail(char *name, char *tail)
{
int n, t, d;
	n = strlen(name);
	t = strlen(tail);
	if(n<=t) return -1;
	name += n-t;
	while(!(d = tolower(*name) - tolower(*tail)) && *name)
		++name, ++tail;
	return d;
}

int namecompare(const void *p1, const void *p2)
{
	return strcmp(*(char **)p1, *(char **)p2);
}

#define MAXNAMES 4096
void dodir(bc *bc, char *text)
{
	struct dirent *de;
	DIR *dirp;
	char **names;
	int count = 0;
	int maxw = 0;

	names = malloc(MAXNAMES*sizeof(void *));
	if(!names)
		return;
	dirp = opendir(".");
	if(!dirp)
		return;
	for(;;)
	{
		de = readdir(dirp);
		if(!de) break;
		if(nametail(de->d_name, ".bas")) continue;
		char *t = strdup(de->d_name);
		if(!t) break;
		names[count++] = t;
		int len = strlen(t);
		if(len > maxw) maxw = len;
		if(count==MAXNAMES)
			break;
	}
	closedir(dirp);
	if(!count)
		return;
	qsort(names, count, sizeof(names[0]), namecompare);
	++maxw;
	int numcol = bc->txsize / maxw;
	int numrow = (count + numcol-1) / numcol;

	if((numcol-1) * numrow >= count)
		--numcol;
	int cw = bc->txsize / numcol;
	int i, j;
	for(i=0;i<numrow;++i)
	{
		int at = 0;
		for(j=0;j<numcol;++j)
		{
			int which = i + j*numrow;
			if(which >= count)
				break;
			while(at < j*cw)
			{
				tprintf(bc, " ");
				++at;
			}
			at += tprintf(bc, "%s", names[which]);
		}
		tprintf(bc, "\n");
	}

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
{"ren", doren},
{"parse", doparse},
{"help", dohelp},
{"dir", dodir},
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
		parseline(bc, line);
	} else
		bc->flags |= BF_NOPROMPT;
}
