%{
//#include "comp.h"
#define YYDEBUG 0
#ifdef _ACC_
#define YYSTYPE void *
#endif

%}
%token IF THEN ELSE
%token ON GOTO GOSUB RETURN LET INPUT PRINT READ DATA DIM
%token FOR TO NEXT STEP END STOP
%token INT FIX SGN SIN COS RND POW LOG EXP TAN ATN2 ATN ABS SQR LEN
%token LEFTSTR MIDSTR RIGHTSTR CHRSTR STRSTR STRINGSTR VAL ASC TAB
%token MOUSEX MOUSEY MOUSEB XSIZE YSIZE TICKS
%token MOVE PEN LINE COLOR CLEAR RANDOM CLS FILL HOME
%token CIRCLE DISC TEST BOX RECT SLEEP SPOT UPDATE
%token INTEGER REAL NUMSYMBOL STRINGSYMBOL STRING
%token LF
%left OROR
%left ANDAND
%left EQ NE LT GT LE GE
%left OR XOR
%left AND
%left RR LL
%left '+' '-'
%left '*' '/' MOD
%left POWER
%right UNARY
%expect 3
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
	IF expr THEN stint
	| IF expr THEN stint ELSE stint
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
	| printitem
	| printlist printsep printitem
	| printitem printsep
	;

singlenumpar:
	'(' numexpr ')'
	;

doublenumpar:
	'(' numexpr ',' numexpr ')'
	;


printsep: ';'
	| ','
	;

printitem:
	'@' numexpr
	| expr
	| TAB singlenumpar
	;

inputlist:
	inputlist2
	| STRING inputlist2
	;

inputlist2:
	lvalue
	| inputlist2 ',' lvalue
	;

lvalue:
	var
	;

assignexpr:
	numvar EQ numexpr
	| stringvar EQ stringexpr
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
	| numexpr EQ numexpr
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
	| stringexpr EQ stringexpr
	| stringexpr NE stringexpr
	| stringfunc
	| stringvar
	| STRING
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

int yylex(void)
{
	return -1;
}


