%{
#include <ctype.h>
#include "misc.h"

#define NAMELEN 16

typedef struct tokeninfo {
	char *at;
	union
	{
		double real;
		char *start;
		int integer;
		char name[NAMELEN];
		char string[256];
		int count;
		step *step;
		void *pointer;
	} value;
} tokeninfo;

typedef struct {
	int stepoff;
	char *at;
} lineref;


#define MAXSTEPS 100000
#define MAXLINEREFS 65536
typedef struct parse_state {
	bc *bc;
	int res;
	char *yypntr;
	char *yystart;
	char *yylast;
	char *linestart;
	step *nextstep;
	step steps[MAXSTEPS];
	int numlinerefs;
	lineref linerefs[MAXLINEREFS];
} ps;

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

static void disassemble(ps *ps)
{
step *s, *e;
bc *bc = ps->bc;
char name[NAMELEN];
linemap *lm = bc->lm, *elm = lm + bc->numlines;
	s = ps->steps;
	e = ps->nextstep;
	while(s<e)
	{
		while(lm < elm && lm->step == s-ps->steps)
		{
			char *p, save;
			p=lm->src;
			while(*p && *p != '\n') ++p;
			save = *p;
			*p = 0;
			tprintf(bc, ";-----%s\n", lm->src);
			*p = save;
			++lm;
		}
		tprintf(bc, "%4d: ", (int)(s-ps->steps));
		if(s->func == pushd)
			tprintf(bc, "pushd %.8f\n", (++s)->d);
		else if(s->func == datad)
			tprintf(bc, "datad %.8f\n", (++s)->d);
		else if(s->func == readd)
			tprintf(bc, "readd\n");
		else if(s->func == evald)
			tprintf(bc, "evald\n");
		else if(s->func == evals)
			tprintf(bc, "evals\n");
		else if(s->func == assignd)
			tprintf(bc, "assignd\n", name);
		else if(s->func == assigns)
			tprintf(bc, "assigns\n", name);
		else if(s->func == pushvd)
			tprintf(bc, "pushvd %s\n", bc->vvars[(++s)->i].name);
		else if(s->func == pushvs)
			tprintf(bc, "pushvs %s\n", bc->vvars[(++s)->i].name);
		else if(s->func == pushav)
			tprintf(bc, "pushav %s\n", bc->vvars[(++s)->i].name);
		else if(s->func == addd)
			tprintf(bc, "addd\n");
		else if(s->func == subd)
			tprintf(bc, "subd\n");
		else if(s->func == muld)
			tprintf(bc, "muld\n");
		else if(s->func == divd)
			tprintf(bc, "divd\n");
		else if(s->func == powerd)
			tprintf(bc, "powerd\n");
		else if(s->func == input)
			tprintf(bc, "input %d\n", (++s)->i);
		else if(s->func == arrayd)
			tprintf(bc, "arrayd %d\n", (++s)->i);
		else if(s->func == arrays)
			tprintf(bc, "arrays %d\n", (++s)->i);
		else if(s->func == pushi)
			tprintf(bc, "pushi %d\n", (++s)->i);
		else if(s->func == pushea)
		{
			tprintf(bc, "pushea %d (%d)\n", s[1].i,
				s-ps->steps + s[1].i);
			++s;
		}
		else if(s->func == rjmp || s->func == rcall)
		{
			tprintf(bc, "%s %d (%d)\n",
				s->func == rjmp ? "rjmp" : "rcall",
				s[1].i, s-ps->steps + s[1].i);
			++s;
		}
		else if(s->func == ongoto)
			tprintf(bc, "ongoto %d\n", (++s)->i);
		else if(s->func == pushs)
		{
			char temp[257];
			s += get_istring(temp, s+1);
			tprintf(bc, "pushs \"%s\"\n", temp);
		}
		else if(s->func == ongosub)
			tprintf(bc, "ongosub %d\n", (++s)->i);
		else if(s->func == ret)
			tprintf(bc, "ret\n");
		else if(s->func == skip2ne)
			tprintf(bc, "skip2ne\n");
		else if(s->func == eqs)
			tprintf(bc, "eqs\n");
		else if(s->func == nes)
			tprintf(bc, "nes\n");
		else if(s->func == adds)
			tprintf(bc, "adds\n");
		else if(s->func == eqd)
			tprintf(bc, "eqd\n");
		else if(s->func == ned)
			tprintf(bc, "ned\n");
		else if(s->func == ned)
			tprintf(bc, "ned\n");
		else if(s->func == led)
			tprintf(bc, "led\n");
		else if(s->func == ged)
			tprintf(bc, "ged\n");
		else if(s->func == ltd)
			tprintf(bc, "ltd\n");
		else if(s->func == gtd)
			tprintf(bc, "gtd\n");
		else if(s->func == sqrd)
			tprintf(bc, "sqrd\n");
		else if(s->func == fixd)
			tprintf(bc, "fixd\n");
		else if(s->func == intd)
			tprintf(bc, "intd\n");
		else if(s->func == sgnd)
			tprintf(bc, "sgnd\n");
		else if(s->func == sind)
			tprintf(bc, "sind\n");
		else if(s->func == cosd)
			tprintf(bc, "cosd\n");
		else if(s->func == powd)
			tprintf(bc, "powd\n");
		else if(s->func == logd)
			tprintf(bc, "logd\n");
		else if(s->func == expd)
			tprintf(bc, "expd\n");
		else if(s->func == tand)
			tprintf(bc, "tand\n");
		else if(s->func == atnd)
			tprintf(bc, "atnd\n");
		else if(s->func == atn2d)
			tprintf(bc, "atn2d\n");
		else if(s->func == absd)
			tprintf(bc, "absd\n");
		else if(s->func == rndd)
			tprintf(bc, "rndd\n");
		else if(s->func == performend)
			tprintf(bc, "end\n");
		else if(s->func == sleepd)
			tprintf(bc, "sleepd\n");
		else if(s->func == keyd)
			tprintf(bc, "keyd\n");
		else if(s->func == keycoded)
			tprintf(bc, "keycoded\n");
		else if(s->func == pop)
			tprintf(bc, "pop\n");
		else if(s->func == printd)
			tprintf(bc, "printd\n");
		else if(s->func == prints)
			tprintf(bc, "prints\n");
		else if(s->func == lf)
			tprintf(bc, "lf\n");
		else if(s->func == tab)
			tprintf(bc, "tab\n");
		else if(s->func == andandd)
			tprintf(bc, "andandd\n");
		else if(s->func == orord)
			tprintf(bc, "orord\n");
		else if(s->func == andd)
			tprintf(bc, "andd\n");
		else if(s->func == ord)
			tprintf(bc, "ord\n");
		else if(s->func == xord)
			tprintf(bc, "xord\n");
		else if(s->func == powerd)
			tprintf(bc, "powerd\n");
		else if(s->func == chs)
			tprintf(bc, "chs\n");
		else if(s->func == cls)
			tprintf(bc, "cls\n");
		else if(s->func == performfill)
			tprintf(bc, "fill\n");
		else if(s->func == home)
			tprintf(bc, "home\n");
		else if(s->func == color3)
			tprintf(bc, "color3\n");
		else if(s->func == color4)
			tprintf(bc, "color4\n");
		else if(s->func == box4)
			tprintf(bc, "box4\n");
		else if(s->func == rect4)
			tprintf(bc, "rect4\n");
		else if(s->func == performdisc)
			tprintf(bc, "disc\n");
		else if(s->func == performcircle)
			tprintf(bc, "circle\n");
		else if(s->func == performpen)
			tprintf(bc, "pen\n");
		else if(s->func == lend)
			tprintf(bc, "lend\n");
		else if(s->func == vald)
			tprintf(bc, "vald\n");
		else if(s->func == ascd)
			tprintf(bc, "ascd\n");
		else if(s->func == leftstr)
			tprintf(bc, "leftstr\n");
		else if(s->func == rightstr)
			tprintf(bc, "rightstr\n");
		else if(s->func == midstr)
			tprintf(bc, "midstr\n");
		else if(s->func == chrstr)
			tprintf(bc, "chrstr\n");
		else if(s->func == performstrstr)
			tprintf(bc, "strstr\n");
		else if(s->func == stringstr)
			tprintf(bc, "stringstr\n");
		else if(s->func == tabstr)
			tprintf(bc, "tabstr\n");
		else if(s->func == printat)
			tprintf(bc, "printat\n");
		else if(s->func == spot)
			tprintf(bc, "spot\n");
		else if(s->func == forceupdate)
			tprintf(bc, "forceupdate\n");
		else if(s->func == performmove)
			tprintf(bc, "move\n");
		else if(s->func == performline)
			tprintf(bc, "line\n");
		else if(s->func == performstop)
			tprintf(bc, "stop\n");
		else if(s->func == dimd)
			tprintf(bc, "dimd %d\n", (++s)->i);
		else if(s->func == dims)
			tprintf(bc, "dims %d\n", (++s)->i);
		else if(s->func == inkey)
			tprintf(bc, "inkey\n");
		else if(s->func == mousexd)
			tprintf(bc, "mousex\n");
		else if(s->func == mouseyd)
			tprintf(bc, "mousey\n");
		else if(s->func == mousebd)
			tprintf(bc, "mouseb\n");
		else if(s->func == ticksd)
			tprintf(bc, "ticks\n");
		else if(s->func == performfor)
			tprintf(bc, "for\n");
		else if(s->func == performnext)
			tprintf(bc, "next\n");
		else if(s->func == performnext1)
			tprintf(bc, "next1\n");
		else
			tprintf(bc, "??? %x\n", s->i);
		++s;
	}
}

static void addlineref(ps *ps, char *at)
{
	ps->linerefs[ps->numlinerefs].at = at;
	ps->linerefs[ps->numlinerefs++].stepoff = ps->nextstep - ps->steps;
}

static void addline(ps *ps, int number, int stepnum)
{
bc *bc = ps->bc;
	bc->lm[bc->numlines].step = stepnum;
	bc->lm[bc->numlines].src = ps->linestart;
	ps->linestart = ps->yypntr;
	bc->lm[bc->numlines++].linenumber = number;
}

static void adddata(ps *ps, double v)
{
	if(ps->bc->datanum < MAXDATA)
		ps->bc->data[ps->bc->datanum++] = v;
}

// returns linemap associated with this line
static linemap *findlinemap(ps *ps, int want)
{
int low, high, mid;
bc *bc=ps->bc;
	low = 0;
	high=bc->numlines;
	if(!high) return 0;
	for(;;)
	{
		mid = (low+high) >> 1;
		if(mid==low) break;
		if(want < bc->lm[mid].linenumber)
			high=mid;
		else
			low=mid;
	}
	if(want == bc->lm[mid].linenumber)
		return bc->lm + mid;
	else
		return 0;
}

// returns program step associated with this line
static int findline(ps *ps, int want)
{
linemap *lm;
	lm = findlinemap(ps, want);
	if(!lm)
		return -1;
	else
		return lm->step;
}


static int fixuplinerefs(ps *ps)
{
bc *bc=ps->bc;
int i;
int o;
int at;
int line;

	for(i=0;i<ps->numlinerefs;++i)
	{
		o=ps->linerefs[i].stepoff;
		at=findline(ps, ps->steps[o+1].i);
		if(at<0)
		{
			for(line=0;line<bc->numlines;++line)
				if(bc->lm[line].step > o)
					break;
			tprintf(ps->bc,
				"Unresolved line reference on line %d\n",
				bc->lm[line-1].linenumber);
			return -1;
		}
		ps->steps[o+1].i = at - o;
	}
	return 0;
}


static void emitfunc(ps *ps, void (*func)())
{
	ps->nextstep++ -> func = func;
}

static void emitint(ps *ps, int i)
{
	ps->nextstep++ -> i = i;
}

static void emitdouble(ps *ps, double val)
{
	ps->nextstep++ -> d = val;
}

static void emitpushd(ps *ps, double val)
{
	emitfunc(ps, pushd);
	emitdouble(ps, val);
}

static void emitpushvd(ps *ps, int num)
{
	emitfunc(ps, pushvd);
	emitint(ps, num);
}

static void emitinput(ps *ps, int num)
{
	emitfunc(ps, input);
	emitint(ps, num);
}

static void emitpushvs(ps *ps, int num)
{
	emitfunc(ps, pushvs);
	emitint(ps, num);
}

static void emitpushav(ps *ps, int num)
{
	emitfunc(ps, pushav);
	emitint(ps, num);
}

static void emitpushi(ps *ps, int v)
{
	emitfunc(ps, pushi);
	emitint(ps, v);
}

static void emitpushs(ps *ps, char *s)
{
int len=strlen(s);
step ts;
int i;
	emitfunc(ps, pushs);
	i=0;
	ts.str[i++]=len;
	do
	{
		if(*s)
			ts.str[i++] = *s++;
		if(i==STEPSIZE || !*s)
		{
			while(i<STEPSIZE)
				ts.str[i++] = 0;
			*ps->nextstep++ = ts;
			i=0;
		}
	} while(*s);
}

static void emitdims(ps *ps, int v)
{
	emitfunc(ps, dims);
	emitint(ps, v);
}

static void emitdimd(ps *ps, int v)
{
	emitfunc(ps, dimd);
	emitint(ps, v);
}

static void emitarrayd(ps *ps, int v)
{
	emitfunc(ps, arrayd);
	emitint(ps, v);
}

static void emitarrays(ps *ps, int v)
{
	emitfunc(ps, arrays);
	emitint(ps, v);
}

static void emitongoto(ps *ps, int v)
{
	emitfunc(ps, ongoto);
	emitint(ps, v);
}

static void emitongosub(ps *ps, int v)
{
	emitfunc(ps, ongosub);
	emitint(ps, v);
}

static void emitpushea(ps *ps, int v)
{
	emitfunc(ps, pushea);
	emitint(ps, v);
}

static void emitrjmp(ps *ps, int delta)
{
	emitfunc(ps, rjmp);
	emitint(ps, delta);
}

static void emitrcall(ps *ps, int delta)
{
	emitfunc(ps, rcall);
	emitint(ps, delta);
}


#define YYDEBUG 0
#define YYSTYPE tokeninfo

#define PS ((ps *)parm)


#define YYPARSE_PARAM parm
#define YYLEX_PARAM parm

int yylex(YYSTYPE *lvalp, ps *parm);
void yyerror(char *s);

%}
%pure_parser
%token IF THEN ELSE
%token ON GOTO GOSUB RETURN LET INPUT PRINT READ DATA DIM
%token FOR TO NEXT STEP END STOP REM
%token RESTORE
%token INT FIX SGN SIN COS RND POW LOG EXP TAN ATN2 ATN ABS SQR LEN
%token LEFTSTR MIDSTR RIGHTSTR CHRSTR STRSTR STRINGSTR VAL ASC TAB
%token MOUSEX MOUSEY MOUSEB XSIZE YSIZE TICKS KEY KEYCODE
%token INKEYSTR
%token MOVE PEN LINE COLOR CLEAR RANDOM CLS FILL HOME
%token CIRCLE DISC TEST BOX RECT SLEEP SPOT UPDATE
%token INTEGER REAL NUMSYMBOL STRINGSYMBOL STRING
%token LF
%left OROR
%left ANDAND
%left '=' NE LT GT LE GE
%left OR XOR
%left AND
%left RR LL
%left '+' '-'
%left '*' '/' MOD
%left POWER
%right UNARY
%expect 4
%%

program:
	/* nothing */
	| prog2
	| statements
	;

prog2:
	line 
	| prog2 line
	;

line:
	mark INTEGER statements LF {addline(PS, $2.value.integer,
					$1.value.step - PS->steps)}
	;

statements:
	statement
	| statements ':' statement ;

statement:
	IF numexpr optthen fixif stint mark
		{$4.value.step[1].i = $6.value.step - $4.value.step}
	| IF numexpr optthen fixif stint ELSE fixifelse stint mark
		{$4.value.step[1].i = $7.value.step - $4.value.step;
		$7.value.step[-1].i = $9.value.step - $7.value.step+2}
	| GOTO INTEGER {addlineref(PS, $2.at);emitrjmp(PS, $2.value.integer)}
	| LET assignexpr {/* implemented */}
	| assignexpr {/* implemented */}
	| print printlist {if($2.value.integer) emitfunc(PS, lf)}
	| DIM dimarraylist
	| END {emitfunc(PS, performend)}
	| STOP {emitfunc(PS, performstop)}
	| SLEEP num1 {emitfunc(PS, sleepd);emitfunc(PS, pop)}
	| PEN num1 {emitfunc(PS, performpen)}
	| COLOR num34 {if($2.value.count==3) emitfunc(PS, color3);
			else emitfunc(PS, color4)}
	| CLS {emitfunc(PS, cls)}
	| HOME {emitfunc(PS, home)}
	| CIRCLE num3 {emitfunc(PS, performcircle)}
	| DISC num3 {emitfunc(PS, performdisc)}
	| FILL {emitfunc(PS, performfill)}
	| MOVE num2 {emitfunc(PS, performmove)}
	| LINE num2 {emitfunc(PS, performline)}
	| BOX num4 {emitfunc(PS, box4)}
	| RECT num4 {emitfunc(PS, rect4)}
	| SPOT {emitfunc(PS, spot)}
	| UPDATE {emitfunc(PS, forceupdate)}
	| FOR forvar '=' numexpr TO numexpr optstep
		{emitpushav(PS, $2.value.integer);emitfunc(PS, performfor)}
	| NEXT optforvar
	| GOSUB INTEGER {addlineref(PS, $2.at);emitrcall(PS, $2.value.integer)}
	| RETURN {emitfunc(PS, ret)}
	| REM {/* do nothing */}
	| ON numexpr GOTO linelist {emitongoto(PS, $4.value.count)}
	| ON numexpr GOSUB linelist {emitongosub(PS, $4.value.count)}
	| INPUT inputlist {emitinput(PS, $2.value.count)}
	| READ readlist {/* implemented */}
	| DATA datalist {/* implemented */}
	| RANDOM
	| RESTORE
	| CLEAR num1
	| TEST
	;

print: PRINT
	| '?'
	;

fixif: /* nothing */ {emitfunc(PS, skip2ne);
		$$.value.step = PS->nextstep;
		emitrjmp(PS, 0)} // size of true side}
	;

fixifelse: /* nothing */ {emitrjmp(PS, 0); // true side to skip over false
		$$.value.step = PS->nextstep;}
	;

mark: /* nothing */ {$$.value.step = PS->nextstep}
	;

optstep: { emitpushd(PS, 1.0)}
	| STEP numexpr
	;

optforvar: {emitfunc(PS, performnext)}
	| forvar {emitpushav(PS, $1.value.integer);emitfunc(PS, performnext1)}
	;

forvar:
	NUMSYMBOL
	;	

stint:
	statements
	| INTEGER {addlineref(PS, $1.at);emitrjmp(PS, $1.value.integer)}
	;

optthen: /* nothing */
	| THEN
	;

num1:
	numexpr {$$.value.count=1}
	;

num2:
	numexpr ',' numexpr {$$.value.count = 2}
	;
num3:
	numexpr ',' numexpr ',' numexpr {$$.value.count = 3}
	;
num4:
	numexpr ',' numexpr ',' numexpr ',' numexpr {$$.value.count = 4}
	;

num34:
	num3
	| num4
	;


dimarraylist:
	dimarrayvar
	| dimarraylist ',' dimarrayvar
	;

dimarrayvar:
	NUMSYMBOL '(' dimlist ')' {emitpushav(PS, $1.value.integer);
				emitdimd(PS, $3.value.count)}
	| STRINGSYMBOL '(' dimlist ')' {emitpushav(PS, $1.value.integer);
				emitdims(PS, $3.value.count)}
	;

dimlist: INTEGER {$$.value.count = 1;emitpushi(PS, $1.value.integer)}
	| dimlist ',' INTEGER {$$.value.count = $1.value.count + 1;
				emitpushi(PS, $3.value.integer)}
	;

linelist: INTEGER {$$.value.count = 1;addlineref(PS, $1.at);
				emitpushea(PS, $1.value.integer)}
	| linelist ',' INTEGER {$$.value.count = $1.value.count + 1;
				addlineref(PS, $1.at);
				emitpushea(PS, $3.value.integer)}
	;

datalist:
	dataentry
	| datalist ',' dataentry
	;

dataentry:
	real {adddata(PS, $1.value.real)}
	| STRING
	;

real:
	INTEGER {$$.value.real = (double)$1.value.integer}
	| REAL
	;


readlist:
	readvar
	| readlist ',' readvar
	;

readvar:
	numvar {emitfunc(PS, readd)}
	| stringvar;

numvar:
	NUMSYMBOL {emitpushvd(PS, $1.value.integer)}
	| NUMSYMBOL '(' numlist ')' {emitpushav(PS, $1.value.integer);
					emitarrayd(PS, $3.value.count)}
	;

stringvar:
	STRINGSYMBOL {emitpushvs(PS, $1.value.integer)}
	| STRINGSYMBOL '(' numlist ')' {emitpushav(PS, $1.value.integer);
					emitarrays(PS, $3.value.count)}
	;

numlist:
	numexpr {$$.value.count = 1}
	| numlist ',' numexpr {$$.value.count = $1.value.count + 1}
	;

printlist: /* nothing */ {$$.value.integer = 1} // want newline
	| printitem {$$.value.integer = 1} // want newline
	| printlist printsep printitem {$$.value.integer = 1} // want newline
	| printlist printsep {$$.value.integer = 0} // no newline
	;

printsep: ';'
	| ',' {emitfunc(PS, tab)}
	;

printitem:
	'@' numexpr {emitfunc(PS, printat)}
	| numexpr {emitfunc(PS, printd)}
	| stringexpr {emitfunc(PS, prints)}
	;

singlenumpar:
	'(' numexpr ')'
	;

doublenumpar:
	'(' numexpr ',' numexpr ')'
	;

inputlist:
	inputlist2
	| STRING ';' inputlist2 {$$.value.count = $3.value.count;
				emitpushs(PS, $1.value.string);
				emitfunc(PS, prints)}
	;

inputlist2:
	inputvar {$$.value.count = 1}
	| inputlist2 ',' inputvar {$$.value.count = $1.value.count + 1}
	;

inputvar:
	numvar {emitpushi(PS, 0)}
	| stringvar {emitpushi(PS, 1)}
	;

assignexpr:
	numvar '=' numexpr {emitfunc(PS, assignd)}
	| stringvar '=' stringexpr {emitfunc(PS, assigns)}
	;

numexpr:
	'-' numexpr %prec UNARY {emitfunc(PS, chs)}
	| '(' numexpr ')'
	| numexpr '+' numexpr {emitfunc(PS, addd)}
	| numexpr '-' numexpr {emitfunc(PS, subd)}
	| numexpr '*' numexpr {emitfunc(PS, muld)}
	| numexpr '/' numexpr {emitfunc(PS, divd)}
	| numexpr POWER numexpr {emitfunc(PS, powerd)}
	| numexpr LL numexpr
	| numexpr RR numexpr
	| numexpr '=' numexpr {emitfunc(PS, eqd)}
	| numexpr NE numexpr {emitfunc(PS, ned)}
	| numexpr LT numexpr {emitfunc(PS, ltd)}
	| numexpr GT numexpr {emitfunc(PS, gtd)}
	| numexpr LE numexpr {emitfunc(PS, led)}
	| numexpr GE numexpr {emitfunc(PS, ged)}
	| numexpr AND numexpr {emitfunc(PS, andd)}
	| numexpr OR numexpr {emitfunc(PS, ord)}
	| numexpr XOR numexpr {emitfunc(PS, xord)}
	| numexpr ANDAND numexpr {emitfunc(PS, andandd)}
	| numexpr OROR numexpr {emitfunc(PS, orord)}
	| stringexpr '=' stringexpr {emitfunc(PS, eqs)}
	| stringexpr NE stringexpr {emitfunc(PS, nes)}
	| numfunc
	| special
	| numvar {emitfunc(PS, evald)}
	| real {emitpushd(PS, $1.value.real)}
	;

singlestringpar: '(' stringexpr ')'
	;

numfunc:
	INT singlenumpar {emitfunc(PS, intd)}
	| FIX singlenumpar {emitfunc(PS, fixd)}
	| SGN singlenumpar {emitfunc(PS, sgnd)}
	| SIN singlenumpar {emitfunc(PS, sind)}
	| COS singlenumpar {emitfunc(PS, cosd)}
	| RND singlenumpar {emitfunc(PS, rndd)}
	| POW doublenumpar {emitfunc(PS, powd)}
	| LOG singlenumpar {emitfunc(PS, logd)}
	| EXP singlenumpar {emitfunc(PS, expd)}
	| TAN singlenumpar {emitfunc(PS, tand)}
	| ATN singlenumpar {emitfunc(PS, atnd)}
	| ATN2 doublenumpar {emitfunc(PS, atn2d)}
	| ABS singlenumpar {emitfunc(PS, absd)}
	| SQR singlenumpar {emitfunc(PS, sqrd)}
	| SLEEP singlenumpar {emitfunc(PS, sleepd)}
	| KEY singlenumpar {emitfunc(PS, keyd)}
	| LEN singlestringpar {emitfunc(PS, lend)}
	| VAL singlestringpar {emitfunc(PS, vald)}
	| ASC singlestringpar {emitfunc(PS, ascd)}
	;

special:
	MOUSEX {emitfunc(PS, mousexd)}
	| MOUSEY {emitfunc(PS, mouseyd)}
	| MOUSEB {emitfunc(PS, mousebd)}
	| XSIZE {emitpushd(PS, PS->bc->xsize)}
	| YSIZE {emitpushd(PS, PS->bc->ysize)}
	| TICKS {emitfunc(PS, ticksd)}
	| KEYCODE {emitfunc(PS, keycoded)}
	;
specialstr:
	INKEYSTR {emitfunc(PS, inkey)}
	;

stringexpr:
	sitem
	| stringexpr '+' sitem {emitfunc(PS, adds)}
	| stringexpr sitem {emitfunc(PS, adds)}
	;

sitem:
	STRING {emitpushs(PS, $1.value.string)}
	| specialstr
	| stringfunc
	| stringvar {emitfunc(PS, evals)}
	| TAB singlenumpar {emitfunc(PS, tabstr)}
	;

stringfunc:
	LEFTSTR '(' stringexpr ',' numexpr ')' {emitfunc(PS, leftstr)}
	| RIGHTSTR '(' stringexpr ',' numexpr ')' {emitfunc(PS, rightstr)}
	| MIDSTR '(' stringexpr ',' numexpr ',' numexpr ')'
			 {emitfunc(PS, midstr)}
	| CHRSTR singlenumpar {emitfunc(PS, chrstr)}
	| STRSTR singlenumpar {emitfunc(PS, performstrstr)}
	| STRINGSTR '(' numexpr ',' stringexpr ')' {emitfunc(PS, stringstr)}
	;

%%

void yyerror(char *s)
{
}

int findvar(ps *ps, char *name)
{
int i;
variable *v;
bc *bc = ps->bc;
	v=bc->vvars;
	for(i=0;i<bc->numvars;++i, ++v)
		if(!strcmp(v->name, name))
			return i;
	if(bc->numvars < MAXVARIABLES)
	{
		memset(v, 0, sizeof(*v));
		strcpy(v->name, name);
		return bc->numvars++;
	}
//WARNING
	return -1; // ran out
}

static inline char at(ps *ps)
{
	return *ps->yypntr;
}
static inline char get(ps *ps)
{
	return *ps->yypntr++;
}
static inline char back(ps *ps)
{
	return *--ps->yypntr;
}

static int iskeyword(ps *ps, char *want)
{
char *p1=ps->yypntr;
char *p2=want;

	while(*p2)
	{
		if(tolower(*p1)!=*p2)
			break;
		++p1;
		++p2;
	}
	if(!*p2)
	{
		ps->yypntr += p2-want;
		return 1;
	}
	return 0;
}

int yylex(YYSTYPE *ti, ps *parm)
{
char ch;
ps *ps=parm;

	while(get(ps)==' ');
	back(ps);
	ps->yylast = ps->yypntr;
	ti->at = ps->yypntr;
if(0){char *p=ps->yypntr, csave;
while(*p && *p++ != '\n');
csave = *--p;
*p = 0;
printf("here:%s\n", ps->yypntr);
*p = csave;
}

// alphabetized for readability -- but atn2 must be before atn, keycode < key
	if(iskeyword(ps, "abs")) return ABS;
	if(iskeyword(ps, "and")) return ANDAND;
	if(iskeyword(ps, "asc")) return ASC;
	if(iskeyword(ps, "atn2")) return ATN2; // order critical!
	if(iskeyword(ps, "atn")) return ATN;
	if(iskeyword(ps, "box")) return BOX;
	if(iskeyword(ps, "chr$")) return CHRSTR;
	if(iskeyword(ps, "circle")) return CIRCLE;
	if(iskeyword(ps, "clear")) return CLEAR;
	if(iskeyword(ps, "cls")) return CLS;
	if(iskeyword(ps, "color")) return COLOR;
	if(iskeyword(ps, "cos")) return COS;
	if(iskeyword(ps, "data")) return DATA;
	if(iskeyword(ps, "dim")) return DIM;
	if(iskeyword(ps, "disc")) return DISC;
	if(iskeyword(ps, "else")) return ELSE;
	if(iskeyword(ps, "end")) return END;
	if(iskeyword(ps, "exp")) return EXP;
	if(iskeyword(ps, "fill")) return FILL;
	if(iskeyword(ps, "fix")) return FIX;
	if(iskeyword(ps, "for")) return FOR;
	if(iskeyword(ps, "gosub")) return GOSUB;
	if(iskeyword(ps, "goto")) return GOTO;
	if(iskeyword(ps, "home")) return HOME;
	if(iskeyword(ps, "if")) return IF;
	if(iskeyword(ps, "inkey$")) return INKEYSTR;
	if(iskeyword(ps, "input")) return INPUT;
	if(iskeyword(ps, "int")) return INT;
	if(iskeyword(ps, "keycode")) return KEYCODE;
	if(iskeyword(ps, "key")) return KEY;
	if(iskeyword(ps, "left$")) return LEFTSTR;
	if(iskeyword(ps, "len")) return LEN;
	if(iskeyword(ps, "let")) return LET;
	if(iskeyword(ps, "line")) return LINE;
	if(iskeyword(ps, "log")) return LOG;
	if(iskeyword(ps, "mid$")) return MIDSTR;
	if(iskeyword(ps, "mod")) return MOD;
	if(iskeyword(ps, "mouseb")) return MOUSEB;
	if(iskeyword(ps, "mousex")) return MOUSEX;
	if(iskeyword(ps, "mousey")) return MOUSEY;
	if(iskeyword(ps, "move")) return MOVE;
	if(iskeyword(ps, "next")) return NEXT;
	if(iskeyword(ps, "on")) return ON;
	if(iskeyword(ps, "or")) return OROR;
	if(iskeyword(ps, "pen")) return PEN;
	if(iskeyword(ps, "pow")) return POW;
	if(iskeyword(ps, "print")) return PRINT;
	if(iskeyword(ps, "random")) return RANDOM;
	if(iskeyword(ps, "read")) return READ;
	if(iskeyword(ps, "rect")) return RECT;
	if(iskeyword(ps, "rem") || at(ps) == '\'')
	{
		while((ch=get(ps)) && ch!='\n');
		back(ps);
		return REM;
	}
	if(iskeyword(ps, "restore")) return RESTORE;
	if(iskeyword(ps, "return")) return RETURN;
	if(iskeyword(ps, "right$")) return RIGHTSTR;
	if(iskeyword(ps, "rnd")) return RND;
	if(iskeyword(ps, "sgn")) return SGN;
	if(iskeyword(ps, "sin")) return SIN;
	if(iskeyword(ps, "sleep")) return SLEEP;
	if(iskeyword(ps, "spot")) return SPOT;
	if(iskeyword(ps, "sqr")) return SQR;
	if(iskeyword(ps, "step")) return STEP;
	if(iskeyword(ps, "stop")) return STOP;
	if(iskeyword(ps, "str$")) return STRSTR;
	if(iskeyword(ps, "string$")) return STRINGSTR;
	if(iskeyword(ps, "tab")) return TAB;
	if(iskeyword(ps, "tan")) return TAN;
	if(iskeyword(ps, "test")) return TEST;
	if(iskeyword(ps, "then")) return THEN;
	if(iskeyword(ps, "ticks")) return TICKS;
	if(iskeyword(ps, "to")) return TO;
	if(iskeyword(ps, "update")) return UPDATE;
	if(iskeyword(ps, "val")) return VAL;
	if(iskeyword(ps, "xsize")) return XSIZE;
	if(iskeyword(ps, "ysize")) return YSIZE;

	ch=get(ps);
	switch(ch)
	{
	case '?': return ch;
	case '@': return ch;
	case ',': return ch;
	case ';': return ch;
	case ':': return ch;
	case '(': return ch;
	case ')': return ch;
	case '+': return ch;
	case '-': return ch;
	case '*': return ch;
	case '/': return ch;
	case '&': return AND;
	case '|': return OR;
	case '^': return XOR;
	case '\n': return LF;
	case '=': return ch;
	case '~': return POWER;
	case 0: back(ps);
		return 0;
	case '>':
		ch=get(ps);
		if(ch=='>') return RR;
		if(ch=='=') return GE;
		back(ps);
		return GT;
	case '<':
		ch=get(ps);
		if(ch=='>') return NE;
		if(ch=='=') return LE;
		if(ch=='<') return LL;
		back(ps);
		return LT;
	}
	ch=back(ps);

	if(isdigit(ch) || ch=='.')
	{
		double intpart;
		double fracpart=0.0;
		int isreal = (ch=='.');

		intpart = 0.0;
		while(isdigit(ch=get(ps))) {intpart*=10;intpart+=ch-'0';}
		if(ch=='.')
		{
			double digit=0.1;
			isreal = 1;
			while(isdigit(ch=get(ps)))
			{
				fracpart += digit * (ch - '0');
				digit /= 10.0;
			}
		}
		back(ps);
		if(isreal)
		{
			ti->value.real = intpart + fracpart;
			return REAL;
		}
		ti->value.integer = intpart;
		return INTEGER;
	}
	if(isalpha(ch))
	{
		int t=0;
		char name[NAMELEN];
		for(;;)
		{
			ch=get(ps);
			if(!isalpha(ch) && !isdigit(ch))
				break;
			if(t<sizeof(name)-2)
				name[t++] = tolower(ch);
		}
		if(ch=='$')
		{
			name[t++] = ch;
			name[t] = 0;
			ti->value.integer = findvar(ps, name);
			return STRINGSYMBOL;
		}
		back(ps);
		name[t] = 0;
		ti->value.integer = findvar(ps, name);
		return NUMSYMBOL;
	}
	if(ch=='"')
	{
		get(ps);
		int t=0;
		for(;;)
		{
			ch=get(ps);
			if(!ch) {back(ps);return -1;} // nonterminated string
			if(ch=='\\')
			{
				ch=get(ps);
				if(!ch || ch=='\n')
					{back(ps);return -1;} // nonterminated
			} else if(ch=='"')
				break;
			if(t<sizeof(ti->value.string)-1)
				ti->value.string[t++] = ch;
		}
		ti->value.string[t] = 0;
		return STRING;
	}
	return -1;
}

void freeold(bc *bc)
{
int i;
int j,size;
bstring **s;
variable *v;

	for(i=0,v=bc->vvars;i<bc->numvars;++i,++v)
	{
		if(!v->pointer)
			continue;
		if(index(v->name, '$'))
		{
			if(v->rank)
			{
				s=v->pointer;
				size=v->dimensions[v->rank];
				for(j=0;j<size;++j)
					if(s[j])
						free_bstring(bc, s[j]);
			} else
				if(v->value.s)
					free_bstring(bc, v->value.s);
			v->value.s = 0;
		}
		free(v->pointer);
		v->pointer = 0;
	}

	bc->numvars = 0;
}

void pruninit(bc *bc)
{
	bc->numvars = 0;
	bc->datanum=0;
	bc->datapull=0;
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
	freeold(bc);
	bc->numstatements=0;
	bc->starttime = SDL_GetTicks();
	memset(bc->vvars, sizeof(bc->vvars), 0);
	reset_waitbase(bc);
}

void dump_data_init(ps *ps)
{
	emitrcall(ps, 0); // call to load up all the data...
}

void dump_data_finish(ps *ps)
{
bc *bc=ps->bc;
int i;
	bc->base[1].i = ps->nextstep - bc->base; // fixup initial rcall
	for(i=0;i<bc->datanum;++i)
	{
		emitfunc(ps, datad);
		emitdouble(ps, bc->data[i]);
	}

	emitfunc(ps, ret);
}

ps *newps(bc *bc, char *take)
{
struct parse_state *ps;
	ps = malloc(sizeof(struct parse_state));
	if(!ps)
	{
		tprintf(bc, "Out of memory.\n");
		return 0;
	}
	memset(ps, 0, sizeof(*ps));
	ps->bc = bc;
	ps->nextstep = ps->steps;
	bc->base = ps->steps;
	ps->yypntr = take;
	ps->yystart = take;
	ps->linestart = ps->yypntr;
	dump_data_init(ps);

	bc->numlines=0;


	ps->res=yyparse(ps);

	if(!ps->res)
	{
		emitfunc(ps, performend);
		dump_data_finish(ps);
		ps->res = fixuplinerefs(ps);
	} else
	{
		char linecopy[1024];
		int n;
		char *p;
		tprintf(bc, "Parse error\n");
		p = ps->yylast;
		while(p>ps->yystart && p[-1] != '\n') --p;
		n=0;
		for(n=0;p[n] && p[n]!='\n' && n<sizeof(linecopy)-1;++n)
			linecopy[n] = p[n];
		linecopy[n] = 0;
		tprintf(bc, "%s\n", linecopy);
		n=ps->yylast - p;
		if(n>sizeof(linecopy)-1)
			n=sizeof(linecopy)-1;
		memset(linecopy, ' ', n);
		linecopy[n]=0;
		tprintf(bc, "%s^\n", linecopy);
	}
	return ps;
}

void parseline(bc *bc, char *line)
{
ps *ps;
	ps = newps(bc, line);
	if(ps)
	{
		if(!ps->res)
		{
			bc->flags &= ~BF_RUNNING;
//			disassemble(ps);
			reset_waitbase(bc);
			vmachine(bc, ps->steps, bc->vstack);
		}
		free(ps);
	}
}

// I think it's an insertion sort, works great if they're already sorted
void sortlinerefs(ps *ps)
{
lineref *p1, *p2, *s, *e, t;
	s=ps->linerefs;
	e = s + ps->numlinerefs;
	for(p1 = s+1;p1<e;++p1)
	{
		t=*p1;
		p2=p1;
		while(p2>s && p2[-1].at > t.at)
		{
			*p2 = p2[-1];
			--p2;
		}
		*p2=t;
	}
}

void renumber(bc *bc, int delta, int start)
{
ps *ps;
int len;
char *put;
char *take, *end;
lineref *lr, *s;
int n;
int v;
char temp[32];
linemap *lm;

	ps = newps(bc, bc->program);
	if(!ps || ps->res)
		return;
// we know all the line references are good, the parser has to fix them
	sortlinerefs(ps); // just to be safe...we work 'em from back to front

// pass 1, replace all the references with the new values
	put = bc->program + sizeof(bc->program);
	*--put = 0;
	end = bc->program + strlen(bc->program);
	s = ps->linerefs;
	lr = ps->linerefs + ps->numlinerefs;
	while(lr > s)
	{
		--lr;
		take = lr->at;
		v=0;
		while(isdigit(*take))
			v=v*10 + *take++ - '0';
		len = end - take;
		put -= len;
		memmove(put, take, len);
		end = lr->at;
		lm=findlinemap(ps, v); // we know we find it...
		if(!lm)
		{
			tprintf(bc, "The world is insane...giving up.\n");
			free(ps);
			return;
		}
		n = lm - bc->lm;
		len = sprintf(temp, "%d", start+n*delta);
		put -= len;
		memcpy(put, temp, len);
	}
	len = end - bc->program;
	put -= len;
	memmove(put, bc->program, len);

// pass 2, fix up the line numbers themselves
	take = put;
	n = start;
	put = bc->program;
	while(*take)
	{
		while(isdigit(*take)) ++take;
		put += sprintf(put, "%d", n);
		n += delta;
		while((*put++ = *take) && *take++ != '\n');
	}
	*put = 0;
	free(ps);
}

void parse(bc *bc, int runit)
{
ps *ps;

	pruninit(bc);
	ps = newps(bc, bc->program);

	if(ps)
	{
		if(!ps->res)
		{
			if(!runit)
			{
				tprintf(bc, "Program parsed correctly\n");
				disassemble(ps);
			} else
			{
				bc->flags |= BF_RUNNING;
				vmachine(bc, ps->steps, bc->vstack);
			}
		}
		free(ps);
	}
}


