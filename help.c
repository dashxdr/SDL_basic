#include <stdlib.h>
#include "misc.h"

typedef struct {
	char *name;
	char *alias;
	char *info;
} help;

help helplist[]={
{"help", 0, "Help tells you information about BASIC statements or commands.\n"},
{"run", 0, "Runs the current program."},
{"list", 0, "Lists lines in the current program.\n"
		"   list           = list entire program\n"
		"   list 100-      = list all lines from 100 on\n"
		"   list -100      = list all lines up to 100\n"
		"   list 100-200   = list all lines from 100 through 200\n"},
{"for", 0, "FOR...NEXT loops:\n"
		"   for i=1 to 10         = loop through 10 times, i goes 1 through 10\n"
		"   for i=2 to 30 step 2  = loop with optional step -- counts by 2's.\n"
		"   end loops with a 'next i'\n"},
{"next", "for", ""},
{"goto", 0, "Use GOTO to jump to another line in the program\n"
		"goto 100          = Start executing code at line 100\n"},
{"gosub", 0, "Use GOSUB...RETURN to execute a subroutine.\n"
		"gosub 100         = run subroutine at line 100\n"
		"                    subroutine must end in a 'return'\n"},
{"return", "gosub", ""},
{"save", 0, "Save the current BASIC program to the filename specified.\n"
		"save test         = save to file 'test.bas'\n"},
{"load", 0, "Load BASIC program from the filename specified.\n"
		"load test         = load file 'test.bas'\n"},
{"sgn", 0, "SGN() returns 1, -1 or 0 depending on the sign of the argument.\n"
		"sgn(0) is 0.  sgn(50) is 1. sgn(-10) is -1.\n"},
{"rnd", 0, "RND() returns a random number.\n"
		"rnd(0) returns a random real number between 0 and 1\n"
		"rnd(50) returns a random integer between 1 and 50\n"},
{"sin", 0, "SIN() returns the sine of the angle argument. The angle is in radians.\n"
		"sin(1.570796) is 1.0\n"},
{"cos", 0, "COS() returns the cosine of the angle argument. The angle is in radians.\n"
		"cos(1.570796) is 0.0\n"},
{"log", 0, "LOG() returns the logarithm, base e, of the argument.\n"},
{"exp", 0, "EXP() returns e raised to the power of the argument.\n"},
{"sqr", 0, "SQR() returns the square root of the argument.\n"},
{"pow", 0, "POW(x,y) returns x raised to the power of y.\n"},
{"int", 0, "INT() removes the fractional part of the number, always moving towards the next lowest integer\n"},
{"fix", 0, "FIX() removes the fractional part of the number, always moving towards 0\n"},
{0,0}
};

int hcomp(const void *p1, const void *p2)
{
	return strcmp((*(help **)p1)->name, (*(help **)p2)->name);
}

#define HTMAX 1000
void listhelp(bc *bc, char *p)
{
help *h;
int online;
help *ht[HTMAX];
int n,i,j, nc, nr;
#define WIDTH 10

	h=helplist;
	for(n=0;n<HTMAX && h->name;++n, ++h)
		ht[n] = h;
	qsort(ht, n, sizeof(ht[0]), hcomp);

	tprintf(bc, "Use: help <subject>\n");
	tprintf(bc, "Help on any of these subjects:\n");
	h=helplist;
	nc = (bc->txsize-1) / WIDTH;
	nr = (n + nc-1)/nc;
	online=0;
	for(i=0;i<nr;++i)
	{
		for(j=i;j<n;j+=nr)
			tprintf(bc, "%-10s", ht[j]->name);
		tprintf(bc, "\n");
	}
}

help *findhelp(char *p)
{
int d=0;
help *h;
int i, len;

	for(;;)
	{
		h=helplist;
		len=strlen(p);
		while(h->name)
		{
			for(i=0;i<len;++i)
				if(tolower(p[i]) != h->name[i])
					break;
				else if(!p[i])
					break;
			if(!p[i])
				break;
			++h;
		}
		if(!h->name)
			break;
		if(!d++ && h->alias)
		{
			p=h->alias;
			continue;
		}
		return h;
	}
	return 0;
}

void dohelp(bc *bc, char *p)
{
help *h;

	if(!*p)
		listhelp(bc, p);
	else
	{
		h=findhelp(p);
		if(h && h->name)
			tprintf(bc, "\n%s:\n\n%s\n", p, h->info);
		else
			tprintf(bc, "No help available on '%s'\n", p);
	}

}
