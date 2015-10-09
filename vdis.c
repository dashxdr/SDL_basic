/*
   SDL_basic written by David Ashley, released 20080621 under the GPL
   http://www.linuxmotors.com/SDL_basic
   dashxdr@gmail.com
*/
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "misc.h"

static int get_istring(char *put, step *s)
{
int len;
int i;
int need=1;
	len = s->str[0];
	i=1;
	while(len--)
	{
		if(i==STEPSIZE)
		{
			++s;
			i=0;
			++need;
		}
		*put++ = s->str[i++];
	}
	*put = 0;
	return need;
}

#define D_NONE       0
#define D_DOUBLE     1
#define D_VARIABLE   2
#define D_STRING     3
#define D_OFFSET     4
#define D_INTEGER    5
#define D_MODIFIERS  6

typedef struct {
	void (*func)(struct basic_context *);
	char *name;
	int type;
} disentry;

disentry dlist[]={
{absd, "absd", D_NONE},
{addd, "addd", D_NONE},
{adds, "adds", D_NONE},
{andandd, "andandd", D_NONE},
{andd, "andd", D_NONE},
{arc, "arc", D_NONE},
{arrayd, "arrayd", D_VARIABLE},
{arrays, "arrays", D_VARIABLE},
{ascd, "ascd", D_NONE},
{assignd, "assignd", D_NONE},
{assigns, "assigns", D_NONE},
{atn2d, "atn2d", D_NONE},
{atnd, "atnd", D_NONE},
{box, "box", D_MODIFIERS},
{chrstr, "chrstr", D_NONE},
{chs, "chs", D_NONE},
{cls, "cls", D_NONE},
{color3, "color3", D_NONE},
{color4, "color4", D_NONE},
{cosd, "cosd", D_NONE},
{datad, "datad", D_DOUBLE},
{dimd, "dimd", D_INTEGER},
{dims, "dims", D_INTEGER},
{divd, "divd", D_NONE},
{performdrawtexture, "drawtexture", D_NONE},
{dur, "dur", D_NONE},
{eqd, "eqd", D_NONE},
{eqs, "eqs", D_NONE},
{evald, "evald", D_NONE},
{evals, "evals", D_NONE},
{expd, "expd", D_NONE},
{fixd, "fixd", D_NONE},
{fmul, "fmul", D_NONE},
{forceupdate, "forceupdate", D_NONE},
{freq, "freq", D_NONE},
{ged, "ged", D_NONE},
{gtd, "gtd", D_NONE},
{home, "home", D_NONE},
{inkey, "inkey", D_NONE},
{input, "input", D_INTEGER},
{intd, "intd", D_NONE},
{keycoded, "keycoded", D_NONE},
{keyd, "keyd", D_NONE},
{led, "led", D_NONE},
{leftstr, "leftstr", D_NONE},
{lend, "lend", D_NONE},
{lf, "lf", D_NONE},
{loadtexture, "loadtexture", D_NONE},
{logd, "logd", D_NONE},
{ltd, "ltd", D_NONE},
{midstr, "midstr", D_NONE},
{modd, "modd", D_NONE},
{mousebd, "mousebd", D_NONE},
{mousexd, "mousexd", D_NONE},
{mouseyd, "mouseyd", D_NONE},
{muld, "muld", D_NONE},
{ned, "ned", D_NONE},
{nes, "nes", D_NONE},
{note, "note", D_NONE},
{ongosub, "ongosub", D_INTEGER},
{ord, "ord", D_NONE},
{orord, "orord", D_NONE},
{performcircle, "circle", D_NONE},
{performdisc, "disc", D_NONE},
{performend, "end", D_NONE},
{performfill, "fill", D_NONE},
{performfor, "for", D_NONE},
{performline, "line ", D_NONE},
{performmove, "move", D_NONE},
{performnext, "next", D_NONE},
{performnext1, "next1", D_NONE},
{performpen, "pen", D_NONE},
{performstop, "stop", D_NONE},
{performstrstr, "strstr", D_NONE},
{pop, "pop", D_NONE},
{powd, "powd", D_NONE},
{powerd, "powerd", D_NONE},
{printat, "printat", D_NONE},
{printd, "printd", D_NONE},
{prints, "prints", D_NONE},
{pushav, "pushav", D_VARIABLE},
{pushd, "pushd", D_DOUBLE},
{pushea, "pushea", D_OFFSET},
{pushi, "pushi", D_INTEGER},
{pushs, "pushs", D_STRING},
{pushvd, "pushvd", D_VARIABLE},
{pushvs, "pushvs", D_VARIABLE},
{quiet, "quiet", D_NONE},
{rcall, "rcall", D_OFFSET},
{readd, "readd", D_NONE},
{rect, "rect", D_MODIFIERS},
{ret, "ret", D_NONE},
{rightstr, "rightstr", D_NONE},
{rjmp, "rjmp", D_OFFSET},
{rndd, "rndd", D_NONE},
{setsound, "setsound", D_NONE},
{sgnd, "sgnd", D_NONE},
{shcubic, "shcubic", D_NONE},
{shcurve, "shcurve", D_NONE},
{shdone, "shdone", D_NONE},
{shend, "shend", D_NONE},
{shinit, "shinit", D_NONE},
{shline, "shline", D_NONE},
{sind, "sind", D_NONE},
{skip2ne, "skip2ne", D_NONE},
{sleepd, "sleepd", D_NONE},
{soundgo, "soundgo", D_NONE},
{spot, "spot", D_NONE},
{sqrd, "sqrd", D_NONE},
{stringstr, "stringstr", D_NONE},
{subd, "subd", D_NONE},
{tab, "tab", D_NONE},
{tabstr, "tabstr", D_NONE},
{tand, "tand", D_NONE},
{ticksd, "ticksd", D_NONE},
{vald, "vald", D_NONE},
{vol, "vol", D_NONE},
{wedge, "wedge", D_NONE},
{wsaw, "wsaw", D_NONE},
{wsin, "wsin", D_NONE},
{wsqr, "wsqr", D_NONE},
{wtri, "wtri", D_NONE},
{xord, "xord", D_NONE},
{0,0,0}
};


void xpr(bc *bc, char *format, ...)
{
va_list ap;

	va_start(ap, format);
	vprintf(format, ap);
	va_end(ap);
}

void dump_modifiers(bc *bc,
		int (*pr)(struct basic_context *bc, char *format, ...),
		unsigned int v)
{
int t;
char *p;
	if(!v)
		pr(bc, " -NONE-");
	while(v)
	{
		t=v & ((1<<MODIFIER_BITS)-1);
		if(t==RENDER_ROUND)
			p="ROUND";
		else if(t==RENDER_ROTATE)
			p="ROTATE";
		else p="???";
		pr(bc, " %s", p);

		v>>=MODIFIER_BITS;
	}
	pr(bc, "\n");

}


void disassemble(bc *bc, step *s, int num)
{
step *e, *first;
linemap *lm = bc->lm, *elm = lm + bc->numlines;
disentry *de;
char temp[257];
int (*pr)(struct basic_context *bc, char *format, ...);

//	pr=xpr;
	pr=tprintf;

	first = s;
	e = s+num;
	while(s<e)
	{
		while(lm < elm && lm->step == s-first)
		{
			char *p, save;
			p=lm->src;
			while(*p && *p != '\n') ++p;
			save = *p;
			*p = 0;
			pr(bc, ";-----%s\n", lm->src);
			*p = save;
			++lm;
		}
		pr(bc, "%4d: ", (int)(s-first));
		de=dlist;
		while(de->func)
		{
			if(de->func == s->func)
				break;
			++de;
		}
		if(de->func)
		{
			pr(bc, "%s", de->name);
			switch(de->type)
			{
			case D_NONE:
				pr(bc, "\n");
				break;
			case D_DOUBLE:
				pr(bc, " %.8lf\n", (++s)->d);
				break;
			case D_VARIABLE:
				pr(bc, " %s\n", bc->vvars[(++s)->i].name);
				break;
			case D_STRING:
				s += get_istring(temp, s+1);
				pr(bc, " \"%s\"\n", temp);
				break;
			case D_OFFSET:
				pr(bc, " %d (%d)\n",
				s[1].i, s-first + s[1].i);
				++s;
				break;
			case D_INTEGER:
				pr(bc, " %d\n", s[1].i);
				++s;
				break;
			case D_MODIFIERS:
				++s;
				dump_modifiers(bc, pr, s->i);
				break;
			}
		}
		else
			pr(bc, "??? %x\n", s->i);
		++s;
	}
}
