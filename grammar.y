%{
#include <ctype.h>
#include "misc.h"
#define YYDEBUG 0
#define YYSTYPE void *

#define BC ((bc *)parm)


#define YYPARSE_PARAM parm
#define YYLEX_PARAM parm

int yylex(YYSTYPE *lvalp, bc *parm);
void yyerror(char *s);

%}
%pure_parser
%token IF THEN ELSE
%token ON GOTO GOSUB RETURN LET INPUT PRINT READ DATA DIM
%token FOR TO NEXT STEP END STOP REM
%token INT FIX SGN SIN COS RND POW LOG EXP TAN ATN2 ATN ABS SQR LEN
%token LEFTSTR MIDSTR RIGHTSTR CHRSTR STRSTR STRINGSTR VAL ASC TAB
%token MOUSEX MOUSEY MOUSEB XSIZE YSIZE TICKS
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
%expect 5
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
	INTEGER statements LF ;

statements:
	statement
	| statements ':' statement ;

optstep:
	| STEP numexpr
	;

optforvar:
	| forvar
	;

forvar:
	NUMSYMBOL
	;	

stint:
	statement
	| INTEGER
	;

statement:
	IF expr optthen stint
	| IF expr optthen stint ELSE stint
	| GOTO INTEGER
	| GOSUB INTEGER
	| ON numexpr GOTO intlist
	| RETURN
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

intlist: INTEGER
	| intlist ',' INTEGER
	;

datalist:
	dataentry
	| datalist ',' dataentry
	;

dataentry:
	INTEGER
	| STRING
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
	NUMSYMBOL
	| NUMSYMBOL '(' numlist ')'
	;

stringvar:
	STRINGSYMBOL
	| STRINGSYMBOL '(' numlist ')'
	;

numlist:
	numexpr
	| numlist ',' numexpr
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
	numvar '=' numexpr
	| stringvar '=' stringexpr
	;

expr:
	numexpr
	| stringexpr
	;

numexpr:
	'-' numexpr %prec UNARY
	| '(' numexpr ')'
	| numexpr '+' numexpr
	| numexpr '-' numexpr
	| numexpr '*' numexpr
	| numexpr '/' numexpr
	| numexpr LL numexpr
	| numexpr RR numexpr
	| numexpr '=' numexpr
	| numexpr NE numexpr
	| numexpr LT numexpr
	| numexpr GT numexpr
	| numexpr LE numexpr
	| numexpr GE numexpr
	| numexpr AND numexpr
	| numexpr OR numexpr
	| numexpr XOR numexpr
	| numexpr ANDAND numexpr
	| numexpr OROR numexpr
	| numexpr POWER numexpr
	| stringexpr '=' stringexpr
	| stringexpr NE stringexpr
	| numfunc
	| special
	| numvar
	| INTEGER
	| REAL
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
	| SQR singlenumpar
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


stringexpr:
	stringexpr '+' stringexpr
	| STRING
	| stringfunc
	| stringvar
	| STRING stringfunc
	| STRING stringvar
	| stringfunc STRING
	| stringvar STRING
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

static inline char at(bc *bc)
{
	return *bc->yypntr;
}
static inline char get(bc *bc)
{
	return *bc->yypntr++;
}
static inline char back(bc *bc)
{
	return *--bc->yypntr;
}

static int iskeyword(bc *bc, char *want)
{
char *p1=bc->yypntr;
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
		bc->yypntr += p2-want;
printf("matched %s\n", want);
		return 1;
	}
	return 0;
}

int yylex(YYSTYPE *lvalp, bc *parm)
{
bc *bc=parm;
char ch;

	while(get(bc)==' ');
	back(bc);
{char *p=bc->yypntr, csave;
while(*p && *p++ != '\n');
csave = *--p;
*p = 0;
printf("here:%s\n", bc->yypntr);
*p = csave;
}

// alphabetized for readability -- but atn2 must be before atn
	if(iskeyword(bc, "abs")) return ABS;
	if(iskeyword(bc, "and")) return ANDAND;
	if(iskeyword(bc, "asc")) return ASC;
	if(iskeyword(bc, "atn2")) return ATN2; // order critical!
	if(iskeyword(bc, "atn")) return ATN;
	if(iskeyword(bc, "box")) return BOX;
	if(iskeyword(bc, "chr$")) return CHRSTR;
	if(iskeyword(bc, "circle")) return CIRCLE;
	if(iskeyword(bc, "clear")) return CLEAR;
	if(iskeyword(bc, "cls")) return CLS;
	if(iskeyword(bc, "color")) return COLOR;
	if(iskeyword(bc, "cos")) return COS;
	if(iskeyword(bc, "data")) return DATA;
	if(iskeyword(bc, "dim")) return DIM;
	if(iskeyword(bc, "disc")) return DISC;
	if(iskeyword(bc, "else")) return ELSE;
	if(iskeyword(bc, "end")) return END;
	if(iskeyword(bc, "exp")) return EXP;
	if(iskeyword(bc, "fill")) return FILL;
	if(iskeyword(bc, "fix")) return FIX;
	if(iskeyword(bc, "for")) return FOR;
	if(iskeyword(bc, "gosub")) return GOSUB;
	if(iskeyword(bc, "goto")) return GOTO;
	if(iskeyword(bc, "home")) return HOME;
	if(iskeyword(bc, "if")) return IF;
	if(iskeyword(bc, "input")) return INPUT;
	if(iskeyword(bc, "int")) return INT;
	if(iskeyword(bc, "left$")) return LEFTSTR;
	if(iskeyword(bc, "len")) return LEN;
	if(iskeyword(bc, "let")) return LET;
	if(iskeyword(bc, "line")) return LINE;
	if(iskeyword(bc, "log")) return LOG;
	if(iskeyword(bc, "mid$")) return MIDSTR;
	if(iskeyword(bc, "mod")) return MOD;
	if(iskeyword(bc, "mouseb")) return MOUSEB;
	if(iskeyword(bc, "mousex")) return MOUSEX;
	if(iskeyword(bc, "mousey")) return MOUSEY;
	if(iskeyword(bc, "move")) return MOVE;
	if(iskeyword(bc, "next")) return NEXT;
	if(iskeyword(bc, "on")) return ON;
	if(iskeyword(bc, "or")) return OROR;
	if(iskeyword(bc, "pen")) return PEN;
	if(iskeyword(bc, "pow")) return POW;
	if(iskeyword(bc, "print")) return PRINT;
	if(iskeyword(bc, "random")) return RANDOM;
	if(iskeyword(bc, "read")) return READ;
	if(iskeyword(bc, "rect")) return RECT;
	if(iskeyword(bc, "rem") || at(bc) == '\'')
	{
		while((ch=get(bc)) && ch!='\n');
		back(bc);
		return REM;
	}
	if(iskeyword(bc, "return")) return RETURN;
	if(iskeyword(bc, "right$")) return RIGHTSTR;
	if(iskeyword(bc, "rnd")) return RND;
	if(iskeyword(bc, "sgn")) return SGN;
	if(iskeyword(bc, "sin")) return SIN;
	if(iskeyword(bc, "sleep")) return SLEEP;
	if(iskeyword(bc, "spot")) return SPOT;
	if(iskeyword(bc, " sqr")) return SQR;
	if(iskeyword(bc, "step")) return STEP;
	if(iskeyword(bc, "stop")) return STOP;
	if(iskeyword(bc, "string$")) return STRINGSTR;
	if(iskeyword(bc, "tab")) return TAB;
	if(iskeyword(bc, "tan")) return TAN;
	if(iskeyword(bc, "test")) return TEST;
	if(iskeyword(bc, "then")) return THEN;
	if(iskeyword(bc, "ticks")) return TICKS;
	if(iskeyword(bc, "to")) return TO;
	if(iskeyword(bc, "update")) return UPDATE;
	if(iskeyword(bc, "val")) return VAL;
	if(iskeyword(bc, "xsize")) return XSIZE;
	if(iskeyword(bc, "ysize")) return YSIZE;

	ch=get(bc);
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
	case 0: back(bc);
		return 0;
	case '>':
		ch=get(bc);
		if(ch=='>') return RR;
		if(ch=='=') return GE;
		back(bc);
		return GT;
	case '<':
		ch=get(bc);
		if(ch=='>') return NE;
		if(ch=='=') return LE;
		if(ch=='<') return LL;
		back(bc);
		return LT;
	}
	ch=back(bc);

	if(isdigit(ch) || ch=='.')
	{
		double intpart;
		int isreal = (ch=='.');

		intpart = 0.0;
		while(isdigit(ch=get(bc))) {intpart*=10;intpart+=ch-'0';}
		if(ch=='.')
		{
			double digit=0.1;
			double fracpart=0.0;
			isreal = 1;
			while(isdigit(ch=get(bc)))
			{
				fracpart += digit * (ch - '0');
				digit /= 10.0;
			}
		}
		back(bc);
// must set lvalp
		if(isreal) return REAL;
		return INTEGER;
	}
	if(isalpha(ch))
	{
		for(;;)
		{
			ch=get(bc);
			if(!isalpha(ch) && !isdigit(ch))
				break;
		}
		if(ch=='$')
			return STRINGSYMBOL;
		back(bc);
		return NUMSYMBOL;
	}
	if(ch=='"')
	{
		get(bc);
		for(;;)
		{
			ch=get(bc);
			if(!ch) {back(bc);return -1;} // nonterminated string
			if(ch=='\\')
			{
				ch=get(bc);
				if(!ch || ch=='\n')
					{back(bc);return -1;} // nonterminated
			} else if(ch=='"')
				break;
// stuff a char in a string I'm building up
		}
		return STRING;
	}
	return -1;
}


