#include <stdarg.h>
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


typedef struct {
	void (*func)(struct basic_context *);
	char *name;
	int type;
} disentry;

disentry dlist[]={
{pushs, "pushs", D_STRING},
{pushd, "pushd", D_DOUBLE},
{datad, "datad", D_DOUBLE},
{readd, "readd", D_NONE},
{evald, "evald", D_NONE},
{evals, "evals", D_NONE},
{assignd, "assignd", D_NONE},
{assigns, "assigns", D_NONE},
{pushvd, "pushvd", D_VARIABLE},
{pushvs, "pushvs", D_VARIABLE},
{pushav, "pushav", D_VARIABLE},
{addd, "addd", D_NONE},
{subd, "subd", D_NONE},
{muld, "muld", D_NONE},
{divd, "divd", D_NONE},
{powerd, "powerd", D_NONE},
{input, "input", D_INTEGER},
{arrayd, "arrayd", D_VARIABLE},
{arrays, "arrays", D_VARIABLE},
{pushi, "pushi", D_INTEGER},
{pushea, "pushea", D_OFFSET},
{rjmp, "rjmp", D_OFFSET},
{rcall, "rcall", D_OFFSET},
{ongosub, "ongosub", D_INTEGER},
{ret, "ret", D_NONE},
{skip2ne, "skip2ne", D_NONE},
{eqs, "eqs", D_NONE},
{nes, "nes", D_NONE},
{adds, "adds", D_NONE},
{eqd, "eqd", D_NONE},
{ned, "ned", D_NONE},
{led, "led", D_NONE},
{ged, "ged", D_NONE},
{ltd, "ltd", D_NONE},
{gtd, "gtd", D_NONE},
{sqrd, "sqrd", D_NONE},
{fixd, "fixd", D_NONE},
{intd, "intd", D_NONE},
{sgnd, "sgnd", D_NONE},
{sind, "sind", D_NONE},
{cosd, "cosd", D_NONE},
{powd, "powd", D_NONE},
{logd, "logd", D_NONE},
{expd, "expd", D_NONE},
{tand, "tand", D_NONE},
{atnd, "atnd", D_NONE},
{atn2d, "atn2d", D_NONE},
{absd, "absd", D_NONE},
{rndd, "rndd", D_NONE},
{performend, "end", D_NONE},
{sleepd, "sleepd", D_NONE},
{keyd, "keyd", D_NONE},
{keycoded, "keycoded", D_NONE},
{pop, "pop", D_NONE},
{printd, "printd", D_NONE},
{prints, "prints", D_NONE},
{lf, "lf", D_NONE},
{tab, "tab", D_NONE},
{andandd, "andandd", D_NONE},
{orord, "orord", D_NONE},
{andd, "andd", D_NONE},
{ord, "ord", D_NONE},
{xord, "xord", D_NONE},
{chs, "chs", D_NONE},
{cls, "cls", D_NONE},
{performfill, "fill", D_NONE},
{home, "home", D_NONE},
{color3, "color3", D_NONE},
{color4, "color4", D_NONE},
{box4, "box4", D_NONE},
{rect4, "rect4", D_NONE},
{performdisc, "disc", D_NONE},
{performcircle, "circle", D_NONE},
{performpen, "pen", D_NONE},
{lend, "lend", D_NONE},
{vald, "vald", D_NONE},
{ascd, "ascd", D_NONE},
{leftstr, "leftstr", D_NONE},
{rightstr, "rightstr", D_NONE},
{midstr, "midstr", D_NONE},
{chrstr, "chrstr", D_NONE},
{performstrstr, "strstr", D_NONE},
{stringstr, "stringstr", D_NONE},
{tabstr, "tabstr", D_NONE},
{printat, "printat", D_NONE},
{spot, "spot", D_NONE},
{forceupdate, "forceupdate", D_NONE},
{performmove, "move", D_NONE},
{performline, "line ", D_NONE},
{performstop, "stop", D_NONE},
{dimd, "dimd", D_INTEGER},
{dims, "dims", D_INTEGER},
{inkey, "inkey", D_NONE},
{mousexd, "mousexd", D_NONE},
{mouseyd, "mouseyd", D_NONE},
{mousebd, "mousebd", D_NONE},
{ticksd, "ticksd", D_NONE},
{performfor, "for", D_NONE},
{performnext, "next", D_NONE},
{performnext1, "next1", D_NONE},
{quiet, "quiet", D_NONE},
{setsound, "setsound", D_NONE},
{freq, "freq", D_NONE},
{vol, "vol", D_NONE},
{dur, "dur", D_NONE},
{fmul, "fmul", D_NONE},
{wsin, "wsin", D_NONE},
{wtri, "wtri", D_NONE},
{wsaw, "wsaw", D_NONE},
{wsqr, "wsqr", D_NONE},
{soundgo, "soundgo", D_NONE},
{note, "note", D_NONE},
{0,0,0}
};


void xpr(bc *bc, char *format, ...)
{
va_list ap;

	va_start(ap, format);
	vprintf(format, ap);
	va_end(ap);
}



void disassemble(bc *bc, step *s, int num)
{
step *e, *first;
linemap *lm = bc->lm, *elm = lm + bc->numlines;
disentry *de;
char temp[257];
void (*pr)(struct basic_context *bc, char *format, ...);

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
				pr(bc, " %d\n", s->i);
				break;
			}
		}
		else
			pr(bc, "??? %x\n", s->i);
		++s;
	}
}
