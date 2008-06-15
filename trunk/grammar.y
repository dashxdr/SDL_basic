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



#define MAXSTEPS 100000
#define MAXLINEREFS 65536
typedef struct parse_state {
	bc *bc;
	char *yypntr;
	char *yylast;
	step *nextstep;
	step *startstep;
	step steps[MAXSTEPS];
	int numlinerefs, linerefs[MAXLINEREFS];
} ps;

static void disassemble(ps *ps)
{
step *s, *e;
bc *bc = ps->bc;
char name[NAMELEN];

	s = ps->steps;
	e = ps->nextstep;
	while(s<e)
	{
		tprintf(bc, "%4d: ", (int)(s-ps->steps));
		if(s->func == pushd)
		{
//printf("%x %x\n", s[1], s[2]);
			tprintf(bc, "pushd %.8f\n", s[1].d);
			++s;
		}
		else if(s->func == evald)
			tprintf(bc, "evald\n");
		else if(s->func == assignd)
			tprintf(bc, "assignd\n", name);
		else if(s->func == assigns)
			tprintf(bc, "assigns\n", name);
		else if(s->func == pushv)
			tprintf(bc, "pushv %s\n", bc->vvars[(++s)->i].name);
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
		else if(s->func == arrayd)
			tprintf(bc, "arrayd\n");
		else if(s->func == arrays)
			tprintf(bc, "arrays\n");
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
			tprintf(bc, "ongoto\n");
		else if(s->func == ongosub)
			tprintf(bc, "ongosub\n");
		else if(s->func == ret)
			tprintf(bc, "ret\n");
		else if(s->func == skip2ne)
			tprintf(bc, "skip2ne\n");
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
		else if(s->func == printd)
			tprintf(bc, "printd\n");
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
			tprintf(bc, "dimd\n");
		else if(s->func == dims)
			tprintf(bc, "dims\n");
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

static void lineref(ps *ps)
{
	ps->linerefs[ps->numlinerefs++] = ps->nextstep - ps->steps;
}

static void addline(ps *ps, int number)
{
bc *bc = ps->bc;
int t1,t2;
	t1=bc->lm[bc->numlines].step = ps->startstep - ps->steps;
	t2=bc->lm[bc->numlines++].linenumber = number;
	ps->startstep = ps->nextstep;
}

static int findline(ps *ps, int want)
{
int low, high, mid;
bc *bc=ps->bc;
	low = 0;
	high=bc->numlines;
	if(!high) return -1;
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
		return bc->lm[mid].step;
	else
		return -1;

}

static int fixuplinerefs(ps *ps)
{
int i;
int o;
int at;
	for(i=0;i<ps->numlinerefs;++i)
	{
		o=ps->linerefs[i];
		at=findline(ps, ps->steps[o+1].i);
		if(at<0)
		{
#warning fix me up
			tprintf(ps->bc, "Bad line reference!!!!\n");
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


static void emitstep(ps *ps, step s)
{
	*ps->nextstep++ = s;
}

static void emitpushd(ps *ps, double val)
{
	emitstep(ps, (step)pushd);
	*(double *)ps->nextstep = val;
	++ps->nextstep;
}

static void emitpushv(ps *ps, int num)
{
	emitfunc(ps, pushv);
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
%token MOUSEX MOUSEY MOUSEB XSIZE YSIZE TICKS
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
	;

prog2:
	line 
	| prog2 line
	;

line:
	INTEGER statements LF {addline(PS, $1.value.integer)}
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
	| GOTO INTEGER {lineref(PS);emitrjmp(PS, $2.value.integer)}
	| LET assignexpr
	| assignexpr
	| PRINT printlist {if($2.value.integer) emitfunc(PS, lf)}
	| DIM dimarraylist
	| END {emitfunc(PS, performend)}
	| STOP {emitfunc(PS, performstop)}
	| SLEEP num1 {emitstep(PS, (step)sleepd)}
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
	| GOSUB INTEGER {lineref(PS);emitrcall(PS, $2.value.integer)}
	| RETURN {emitfunc(PS, ret)}
	| REM {/* do nothing */}
	| RANDOM
	| ON numexpr GOTO linelist {emitpushi(PS, $4.value.count);
			emitfunc(PS, ongoto)}
	| ON numexpr GOSUB linelist {emitpushi(PS, $4.value.count);
			emitfunc(PS, ongosub)}
	| RESTORE
	| INPUT inputlist
	| READ varlist
	| DATA datalist
	| CLEAR num1
	| TEST
	;

fixif: /* nothing */ {emitstep(PS, (step)skip2ne);
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
	| INTEGER {lineref(PS);emitrjmp(PS, $1.value.integer)}
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
	NUMSYMBOL '(' intlist ')' {emitpushi(PS, $3.value.count);
				emitpushav(PS, $1.value.integer);
				emitfunc(PS, dimd)}
	| STRINGSYMBOL '(' intlist ')' {emitpushi(PS, $3.value.count);
				emitpushav(PS, $1.value.integer);
				emitfunc(PS, dims)}
	;

intlist: INTEGER {$$.value.count = 1;emitpushi(PS, $1.value.integer)}
	| intlist ',' INTEGER {++$$.value.count;emitpushi(PS, $3.value.integer)}
	;

linelist: INTEGER {$$.value.count = 1;lineref(PS);
				emitpushea(PS, $1.value.integer)}
	| linelist ',' INTEGER {++$$.value.count;lineref(PS);
				emitpushea(PS, $3.value.integer)}
	;

datalist:
	dataentry
	| datalist ',' dataentry
	;

dataentry:
	real
	| STRING
	;

real:
	INTEGER {$$.value.real = (double)$1.value.integer}
	| REAL
	;


varlist:
	var
	| varlist ',' var
	;

var:
	numvar
	| stringvar;
	;

numvar:
	NUMSYMBOL {emitpushv(PS, $1.value.integer)}
	| NUMSYMBOL '(' numlist ')' {emitpushi(PS, $3.value.count);
					emitpushav(PS, $1.value.integer);
					emitfunc(PS, arrayd)}
	;

stringvar:
	STRINGSYMBOL
	| STRINGSYMBOL '(' numlist ')' {emitpushi(PS, $3.value.count);
					emitpushav(PS, $1.value.integer);
					emitfunc(PS, arrays)}
	;

numlist:
	numexpr {$$.value.count = 1}
	| numlist ',' numexpr {++$$.value.count}
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
	'@' numexpr
	| expr {emitfunc(PS, printd);}
	| TAB singlenumpar
	;

singlenumpar:
	'(' numexpr ')'
	;

doublenumpar:
	'(' numexpr ',' numexpr ')'
	;

inputlist:
	inputlist2
	| STRING ';' inputlist2
	;

inputlist2:
	lvalue
	| inputlist2 ',' lvalue
	;

lvalue:
	var
	;

assignexpr:
	numvar '=' numexpr {emitstep(PS, (step)assignd)}
	| stringvar '=' stringexpr {emitstep(PS, (step)assigns)}
	;

expr:
	numexpr
	| stringexpr
	;

numexpr:
	'-' numexpr %prec UNARY {emitfunc(PS, chs)}
	| '(' numexpr ')'
	| numexpr '+' numexpr {emitstep(PS, (step)addd)}
	| numexpr '-' numexpr {emitstep(PS, (step)subd)}
	| numexpr '*' numexpr {emitstep(PS, (step)muld)}
	| numexpr '/' numexpr {emitstep(PS, (step)divd)}
	| numexpr POWER numexpr {emitstep(PS, (step)powerd)}
	| numexpr LL numexpr
	| numexpr RR numexpr
	| numexpr '=' numexpr {emitstep(PS, (step)eqd)}
	| numexpr NE numexpr {emitstep(PS, (step)ned)}
	| numexpr LT numexpr {emitstep(PS, (step)ltd)}
	| numexpr GT numexpr {emitstep(PS, (step)gtd)}
	| numexpr LE numexpr {emitstep(PS, (step)led)}
	| numexpr GE numexpr {emitstep(PS, (step)ged)}
	| numexpr AND numexpr {emitstep(PS, (step)andd)}
	| numexpr OR numexpr {emitstep(PS, (step)ord)}
	| numexpr XOR numexpr {emitstep(PS, (step)xord)}
	| numexpr ANDAND numexpr {emitstep(PS, (step)andandd)}
	| numexpr OROR numexpr {emitstep(PS, (step)orord)}
	| stringexpr '=' stringexpr {emitstep(PS, (step)eqs)}
	| stringexpr NE stringexpr {emitstep(PS, (step)nes)}
	| numfunc
	| special
	| numvar {emitstep(PS, (step)evald)}
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
	| LEN singlestringpar
	| VAL singlestringpar
	| ASC singlestringpar
	;

special:
	MOUSEX {emitfunc(PS, mousexd)}
	| MOUSEY {emitfunc(PS, mouseyd)}
	| MOUSEB {emitfunc(PS, mousebd)}
	| XSIZE {emitpushd(PS, PS->bc->xsize)}
	| YSIZE {emitpushd(PS, PS->bc->ysize)}
	| TICKS {emitfunc(PS, ticksd)}
	;
specialstr:
	INKEYSTR
	;

stringexpr: sss
	;

sss:
	sitem
	| sss '+' sitem
	| sss sitem
	;

sitem:
	STRING
	| specialstr
	| stringfunc
	| stringvar
	;

stringfunc:
	LEFTSTR '(' stringexpr ',' numexpr ')'
	| RIGHTSTR '(' stringexpr ',' numexpr ')'
	| MIDSTR '(' stringexpr ',' numexpr ',' numexpr ')'
	| CHRSTR singlenumpar
	| STRSTR singlenumpar
	| STRINGSTR '(' numexpr ',' stringexpr ')'
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

// alphabetized for readability -- but atn2 must be before atn
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
				name[t++] = ch;
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
				ti->value.name[t++] = ch;
		}
		ti->value.name[t] = 0;
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
			s=v->pointer;
			size=v->dimensions[v->rank];
			for(j=0;j<size;++j)
				if(s[j])
					free_bstring(s[j]);
		}
		free(v->pointer);
		v->pointer = 0;
	}

	bc->numvars = 0;
}

void pruninit(bc *bc)
{
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
	freeold(bc);
	bc->numstatements=0;
	bc->starttime = SDL_GetTicks();
	reset_waitbase(bc);
}



void parse(bc *bc, int runit)
{
struct parse_state *ps;
int res=0;
	ps = malloc(sizeof(struct parse_state));
	if(!ps)
	{
		tprintf(bc, "Out of memory.\n");
		return;
	}
	memset(ps, 0, sizeof(*ps));
	ps->bc = bc;
	ps->startstep = ps->steps;
	ps->nextstep = ps->steps;
	ps->yypntr = bc->program;
	bc->numvars = 0;

	pruninit(bc);
	res=yyparse(ps);

//printf("numvars = %d\n", bc->numvars);
	if(!res)
	{
		tprintf(bc, "Program parsed correctly\n");
		emitstep(ps, (step)performend);
		res = fixuplinerefs(ps);
		if(res) return;
		if(!runit)
			disassemble(ps);
		else 
			vmachine(bc, ps->steps, bc->vstack);
		free(ps);
		return;
	}
	tprintf(bc, "yyparse returned %d\n", res);
	if(res)
	{
		char linecopy[1024];
		int n;
		char *p;
		p = ps->yylast;
		while(p>bc->program && p[-1] != '\n') --p;
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

	free(ps);
}


