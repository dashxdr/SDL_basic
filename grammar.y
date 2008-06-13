%{
#include <ctype.h>
#include "misc.h"

typedef struct item item;
typedef struct list list;
typedef int step; // program element, needs to hold a void * or int

typedef struct tokeninfo {
	char *at;
	union
	{
		double real;
		char *start;
		int integer;
		char name[16];
		char string[256];
		int count;
		step *step;
	} value;
} tokeninfo;

#define MAXSTEPS 100000
typedef struct parse_state {
	bc *bc;
	char *yypntr;
	char *yylast;
	step *nextstep;
	step *firststep; // first step of the list currently being built up
	step steps[MAXSTEPS];
} ps;

#define DECLARE(name) void name(bc *bc){} extern void name(bc *bc);
DECLARE(pushd)
DECLARE(evald)
DECLARE(pushvd)
DECLARE(pushi)
DECLARE(pushvad)
DECLARE(arrayd)
DECLARE(assignd)
DECLARE(assigns)
DECLARE(addd)
DECLARE(subd)
DECLARE(muld)
DECLARE(divd)
DECLARE(powerd)
DECLARE(eqd)
DECLARE(ned)
DECLARE(ltd)
DECLARE(gtd)
DECLARE(led)
DECLARE(ged)
DECLARE(andd)
DECLARE(ord)
DECLARE(xord)
DECLARE(andandd)
DECLARE(orord)
DECLARE(eqs)
DECLARE(nes)
DECLARE(sqrd)
DECLARE(performif)
DECLARE(performend)
DECLARE(rjmp)

void getdouble(void *d, step *s)
{
void *p=d;
	((step*)p)[0] = s[0];
	((step*)p)[1] = s[1];
}

static void getname(char *p, step *s)
{
	*(int *)p = *(int *)s;
}

static void disassemble(ps *ps)
{
step *s, *e;
bc *bc = ps->bc;
double d;
char name[16];

	s = ps->steps;
	e = ps->nextstep;
	while(s<e)
	{
		tprintf(bc, "%4d: ", (int)(s-ps->steps));
		if(*s == (step)pushd)
		{
			getdouble(&d, s+1);
//printf("%x %x\n", s[1], s[2]);
			tprintf(bc, "pushd %.8f\n", d);
			s+=2;
		}
		else if(*s == (step)evald)
			tprintf(bc, "evald\n");
		else if(*s == (step)assignd)
			tprintf(bc, "assignd\n", name);
		else if(*s == (step)pushvd)
		{
			getname(name, s+1);
			tprintf(bc, "pushvd %s\n", name);
			++s;
		}
		else if(*s == (step)pushvad)
		{
			getname(name, s+1);
			tprintf(bc, "pushvad %s\n", name);
			++s;
		}
		else if(*s == (step)addd)
			tprintf(bc, "addd\n");
		else if(*s == (step)subd)
			tprintf(bc, "subd\n");
		else if(*s == (step)muld)
			tprintf(bc, "muld\n");
		else if(*s == (step)divd)
			tprintf(bc, "divd\n");
		else if(*s == (step)powerd)
			tprintf(bc, "powerd\n");
		else if(*s == (step)arrayd)
			tprintf(bc, "arrayd\n");
		else if(*s == (step)pushi)
			tprintf(bc, "pushi %d\n", (int)*++s);
		else if(*s == (step)rjmp)
			tprintf(bc, "rjmp %d\n", (int)*++s);
		else if(*s == (step)performif)
			tprintf(bc, "performif\n");
		else if(*s == (step)eqd)
			tprintf(bc, "eqd\n");
		else if(*s == (step)ned)
			tprintf(bc, "ned\n");
		else if(*s == (step)performend)
			tprintf(bc, "end\n");
		else
			tprintf(bc, "??? %x\n", (int)*s);
		++s;
	}
}

static step *closeoff(ps *ps)
{
/*
step *t;
	t=ps->firststep;
	*t = (step)(ps->nextstep - t);
	ps->firststep = ps->nextstep;
	*ps->nextstep++ = (step)0;
	return t;
*/
	return 0;
}

static void emitstep(ps *ps, step s)
{
	*ps->nextstep++ = s;
}

static void emitpushd(ps *ps, double val)
{
void *p=&val;
	emitstep(ps, (step)pushd);
	emitstep(ps, ((step *)p)[0]);
	emitstep(ps, ((step *)p)[1]);
}

static void emitname(ps *ps, char *name)
{
char temp[sizeof(int)] = {0};
	strcpy(temp, name);
	emitstep(ps, *(step *)temp);
}

static void emitpushvd(ps *ps, char *name)
{
	emitstep(ps, (step)pushvd);
	emitname(ps, name);
}

static void emitpushi(ps *ps, int v)
{
	emitstep(ps, (step)pushi);
	emitstep(ps, (step)v);
}

static void emitpushvad(ps *ps, char *name)
{
	emitstep(ps, (step)pushvad);
	emitname(ps, name);
}

static void emitrjmp(ps *ps, int delta)
{
	emitstep(ps, (step)rjmp);
	emitstep(ps, (step)delta);
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
%expect 6
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
	INTEGER statements LF
	;

statements:
	statement
	| statements ':' statement ;

statement:
	IF numexpr optthen fixif stint mark
		{$4.value.step[-1] = $6.value.step - $4.value.step}
	| IF numexpr optthen fixif stint ELSE fixifelse stint mark
		{$4.value.step[-1] = $7.value.step - $4.value.step;
		$7.value.step[-1] = $9.value.step - $7.value.step+2}
	| GOTO INTEGER
	| GOSUB INTEGER
	| ON numexpr GOTO intlist
	| ON numexpr GOSUB intlist
	| RETURN
	| RESTORE
	| LET assignexpr
	| assignexpr
	| INPUT inputlist
	| PRINT printlist
	| READ varlist
	| DATA datalist
	| DIM dimarraylist
	| FOR forvar '=' numexpr TO numexpr optstep
	| NEXT optforvar
	| RANDOM
	| END
	| STOP
	| SLEEP num1
	| MOVE num2
	| PEN num1
	| LINE num2
	| COLOR num34
	| CLEAR num1
	| CLS
	| FILL
	| HOME
	| CIRCLE num3
	| DISC num3
	| TEST
	| BOX num4
	| RECT num4
	| SPOT
	| UPDATE
	| REM
	;

fixif: /* nothing */ {emitpushi(PS, 0); // size of true side
		$$.value.step = PS->nextstep;
		emitstep(PS, (step)performif)}
	;

fixifelse: /* nothing */ {emitrjmp(PS, 0); // true side to skip over false
		$$.value.step = PS->nextstep;}
	;

mark: /* nothing */ {$$.value.step = PS->nextstep}
	;

optstep: { emitpushd(PS, 1.0)}
	| STEP numexpr
	;

optforvar:
	| forvar
	;

forvar:
	NUMSYMBOL
	;	

stint:
	statements
	| INTEGER
	;

optthen: /* nothing */
	| THEN
	;

num1:
	numexpr
	;

num2:
	numexpr ',' numexpr
	;
num3:
	numexpr ',' numexpr ',' numexpr
	;
num4:
	numexpr ',' numexpr ',' numexpr ',' numexpr
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
	sym '(' intlist ')'
	;

sym:
	NUMSYMBOL
	| STRINGSYMBOL
	;

intlist: INTEGER {$$.value.count = 1;emitpushi(PS, $1.value.integer)}
	| intlist ',' INTEGER {++$$.value.count;emitpushi(PS, $3.value.integer)}
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
	NUMSYMBOL {emitpushvd(PS, $1.value.name)}
	| NUMSYMBOL '(' numlist ')' {emitpushi(PS, $3.value.count);
					emitpushvad(PS, $1.value.name);
					emitstep(PS, (step)arrayd)}
	;

stringvar:
	STRINGSYMBOL
	| STRINGSYMBOL '(' numlist ')'
	;

numlist:
	numexpr {$$.value.count = 1}
	| numlist ',' numexpr {++$$.value.count}
	;

printlist: /* nothing */
	| printlist2
	;

printlist2:
	printitem
	| printlist2 printsep printitem
	| printlist2 printsep
	| printlist2 STRING
	;

printsep: ';'
	| ','
	;

printitem:
	'@' numexpr
	| expr
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
	numvar '=' ne {emitstep(PS, (step)assignd);
			$$.value.step = closeoff(PS)}
	| stringvar '=' se {emitstep(PS, (step)assigns);
			$$.value.step = closeoff(PS)}
	;

expr:
	numexpr
	| stringexpr
	;

numexpr:
	ne {$$.value.step = closeoff(PS)}
	;

ne:
	'-' ne %prec UNARY
	| '(' ne ')'
	| ne '+' ne {emitstep(PS, (step)addd)}
	| ne '-' ne {emitstep(PS, (step)subd)}
	| ne '*' ne {emitstep(PS, (step)muld)}
	| ne '/' ne {emitstep(PS, (step)divd)}
	| ne POWER ne {emitstep(PS, (step)powerd)}
	| ne LL ne
	| ne RR ne
	| ne '=' ne {emitstep(PS, (step)eqd)}
	| ne NE ne {emitstep(PS, (step)ned)}
	| ne LT ne {emitstep(PS, (step)ltd)}
	| ne GT ne {emitstep(PS, (step)gtd)}
	| ne LE ne {emitstep(PS, (step)led)}
	| ne GE ne {emitstep(PS, (step)ged)}
	| ne AND ne {emitstep(PS, (step)andd)}
	| ne OR ne {emitstep(PS, (step)ord)}
	| ne XOR ne {emitstep(PS, (step)xord)}
	| ne ANDAND ne {emitstep(PS, (step)andandd)}
	| ne OROR ne {emitstep(PS, (step)orord)}
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
	INT singlenumpar
	| FIX singlenumpar
	| SGN singlenumpar
	| SIN singlenumpar
	| COS singlenumpar
	| RND singlenumpar
	| POW doublenumpar
	| LOG singlenumpar
	| EXP singlenumpar
	| TAN singlenumpar
	| ATN singlenumpar
	| ATN2 doublenumpar
	| ABS singlenumpar
	| SQR singlenumpar {emitstep(PS, (step)sqrd)}
	| LEN singlestringpar
	| VAL singlestringpar
	| ASC singlestringpar
	;

special:
	MOUSEX
	| MOUSEY
	| MOUSEB
	| XSIZE
	| YSIZE
	| TICKS
	;
specialstr:
	INKEYSTR
	;

stringexpr:
	se { $$.value.step = closeoff(PS)}
	;

se:
	sitem
	| se '+' sitem
	| se sitem
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
		for(;;)
		{
			ch=get(ps);
			if(!isalpha(ch) && !isdigit(ch))
				break;
			if(t<sizeof(ti->value.name)-2)
				ti->value.name[t++] = ch;
		}
		if(ch=='$')
		{
			ti->value.name[t++] = ch;
			ti->value.name[t] = 0;
			return STRINGSYMBOL;
		}
		back(ps);
		ti->value.name[t] = 0;
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


void doparse(bc *bc)
{
struct parse_state *ps;
int res=0;
	ps = malloc(sizeof(struct parse_state));
	if(!ps)
	{
		tprintf(bc, "Out of memory.\n");
		return;
	}
	ps->bc = bc;
	ps->firststep = ps->steps;
	ps->nextstep = ps->steps;
	closeoff(ps);
	ps->yypntr = bc->program;

	res=yyparse(ps);

	if(!res)
	{
		tprintf(bc, "Program parsed correctly\n");
		emitstep(ps, (step)performend);
		disassemble(ps);
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


