#include "misc.h"

typedef struct {
	char *name;
	char *info;
} help;

help helplist[]={
{"run", "Runs the current program."},
{"list", "Lists lines in the current program.\n"
		"   list           = list entire program\n"
		"   list 100-      = list all lines from 100 on\n"
		"   list -100      = list all lines up to 100\n"
		"   list 100-200   = list all lines from 100 through 200\n"},
{0,0}
};

void listhelp(bc *bc, char *p)
{
help *h;
int online;
	tprintf(bc, "Help on any of these subjects:\n");
	h=helplist;
	online=0;
	while(h->name)
	{
		tprintf(bc, "%10s", h->name);
		++online;
		if(online==6)
		{
			online=0;
			tprintf(bc, "\n");
		}
		++h;
	}
	if(online)
		tprintf(bc, "\n");
}


void dohelp(bc *bc, char *p)
{
help *h;
int len;
int i;

	if(!*p)
		listhelp(bc, p);
	else
	{
		h=helplist;
		len=strlen(p);
		while(h->name)
		{
			for(i=0;i<len;++i)
				if(tolower(h->name[i] != p[i]))
					break;
				else if(!p[i])
					break;
			if(!p[i])
				break;
			++h;
		}
		if(h->name)
			tprintf(bc, "\n%s:\n\n%s\n", h->name, h->info);
		else
			tprintf(bc, "No help available on '%s'\n", p);
	}

}
