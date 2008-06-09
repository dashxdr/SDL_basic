#include <SDL.h>
#include "ftgrays.h"

#define KEYMAX 32
#define KEYHISTSIZE 16

#define SCROLLHISTORYSIZE 8192

#define HISTSIZE 512
#define LINESIZE 128

#define BF_QUIT         1 // we can quit
#define BF_NOPROMPT     2 // don't put up "Ready" prompt.
#define BF_EDIT         4 // preload a line, for editing
#define BF_RUNERROR     8 // Error on a line
#define BF_CCHIT       16 // Hit control-c
#define BF_ENDHIT      32 // End
#define BF_STOPHIT     64 // Stop
#define BF_RUNNING    128 // program is running

struct linepointer {
int linenum;
char *line;
};

#define GOSUBMAX   10000

#define MAX_VARIABLES 2048 // a, b, c, i, j, a(10,20), etc max
#define RANK_VARIABLE  0
#define RANK_ARRAY     1 // and 2, 3, 4, etc.
#define RANK_INVALID   -50
#define RANK_MASK     0xff

#define RANK_STRING   0x100 // mask

typedef struct bstring {
	int length;
	char string[];
} bstring;

struct forinfo {
	char name[16];
	double step;
	double end;
	int nextline;
};
#define MAX_FORS 100

struct variable {
	char name[16];
	int rank; // RANK_*
	int dimensions[20];
	void *array; // pointer to doubles or bstrings
	double value;
	bstring *string;
};

struct varname {
	char name[16];
	struct variable *var;
};



#define MAX_SHAPE_POINTS 1024
#define MAX_SHAPE_CONTOURS 64

typedef struct basic_context {
	int flags;
	SDL_Surface *thescreen;
	int xsize, ysize;
	int txsize, tysize; // screen dimensions in characters
	Uint32 fgcolor, bgcolor, cursorcolor, black, white;
	int cursorstate;
	float posx, posy;
	int txpos, typos;
	int pressedcodes[KEYMAX],downcodes[KEYMAX],numpressed,numdown;
	int keytake, keyput, keysin;
	int keyhist[KEYHISTSIZE];
	int nexttime, lastcode;
	char debline[LINESIZE];
	char *dp;
	char *debhist;
	int hcount;
	int fwidth, fheight;
	int lastupdate;
	int tainted;
	int textsize;
	char *textstate, *textbak;
	char filename[128];
	char workspace[1024];
	int scrollhistoryin;
	char *scrollhistory;
	int let_code;
	char lineerror[256];
	char program[1000000]; // 1M ought to be big enough...
	char runnable[1000000];
	int numlines;
	struct linepointer lps[100000]; // 100K lines
	int online, nextline;
	int execute_count;
	int numvariables;
	struct variable vars[MAX_VARIABLES];
	struct varname varnames[MAX_VARIABLES];

	int tokenmap[256];
	int numfors;
	struct forinfo fors[MAX_FORS];
	int dataline;
	char *datatake;
	int gosubsp;
	int gosubstack[GOSUBMAX];
	int numstatements;
	Uint32 temp;
// graphics rendering state
	double gx, gy; // position
	int gred, ggreen, gblue, galpha;
	double pen; // pen size
	unsigned char pool[65536];
// shape state
	int shape_numpoints;
	int shape_numcontours;
	short shape_pathstops[MAX_SHAPE_CONTOURS];
	FT_Vector  shape_points[MAX_SHAPE_POINTS];
	char shape_tags[MAX_SHAPE_POINTS];
	int starttime;
} bc;

#define MYF1 0x180
#define MYF2 0x181
#define MYF3 0x182
#define MYF4 0x183
#define MYF5 0x184
#define MYF6 0x185
#define MYF7 0x186
#define MYF8 0x187
#define MYF9 0x188
#define MYF10 0x189
#define MYLEFT 0x190
#define MYRIGHT 0x191
#define MYUP 0x192
#define MYDOWN 0x193
#define MYPAGEUP 0x194
#define MYPAGEDOWN 0x195
#define MYHOME 0x196
#define MYEND 0x197
#define MYALTL 0x198
#define MYALTR 0x199
#define MYCTRLL 0x19a
#define MYCTRLR 0x19b
#define MYSHIFTL 0x19c
#define MYSHIFTR 0x19d

#define MYDELETE 0x7f
#define MYSHIFTED 0x40
#define MYALTED 0x200



// main.c


// font.c

void inittext(bc *bc);
void drawtext(bc *bc, int x, int y, Uint32 fgcolor, Uint32 bgcolor, char *str);
void cursor(bc *bc, int onoff);
void tprintf(bc *bc, char *s, ...);
void drawcharxy(bc *bc, unsigned int x, unsigned int y, char c);
void cleartext(bc *bc);

// render.c

#define TAG_ONPATH    1 // on the path
#define TAG_CONTROL2  0 // quadratic bezier control point
#define TAG_CONTROL3  2 // cubic bezier control point

void update(bc *bc);
void drawchar(bc *bc, int x, int y, unsigned char *p, Uint32 fg, Uint32 bg);
void taint(bc *bc);
void stroke(bc *bc, double x, double y);
void fillscreen(bc *bc, int r, int g, int b, int a);
void circle(bc *bc, double cx, double cy, double radius);
void disc(bc *bc, double cx, double cy, double radius);
void rendertest(bc *bc);
void shape_init(bc *bc);
void shape_add(bc *bc, double x, double y, int tag);
void shape_end(bc *bc);
void shape_done(bc *bc);
void spot(bc *bc);

// keyboard.c

void markkey(bc *bc, int code, int mod, int downup);
void typeline(bc *bc, char *prompt,int echocr);
void scaninput(bc *bc);
void flushinput(bc *bc);

// basic.c

void runinit(bc *bc);
void processline(bc *bc, char *line);
void run_error(bc *bc, char *s, ...);
int token_flags(bc *bc, unsigned char val);
void execute(bc *bc, char **p);
int is_numeric_function(bc *bc, int token); // all parameters #'s, returns #
int is_general_function(bc *bc, int token); // anything goes
int function_parameter_count(bc *bc, int token);
void (*statement_handler(bc *bc, int token))();


extern int token_then;
extern int token_to;
extern int token_else;
extern int token_if;
extern int token_to;
extern int token_step;
extern int token_data;

// expr.c

#define EXPR_IF            1 // it's an IF expression
#define EXPR_ERROR         2 // an error occured
#define EXPR_NOSTRING      4 // expression can't be a string
#define EXPR_LET           8 // it's part of an assignment
#define EXPR_LVALUE       16 // no operations, just an lvalue
#define EXPR_DIDLET       32 // actually did a let
#define EXPR_NUMERIC      64 // need numeric expression
#define EXPR_STRING      128 // expression must be a string

#define OT_DOUBLE   1
#define OT_BSTRING  2
#define OT_PDOUBLE  3
#define OT_PBSTRING 4

#define EXPR_ERR_MISMATCH   "Type mismatch -- can't mix strings and numbers"
#define EXPR_ERR_INVALID    "Illegal operation"
#define EXPR_ERR_BAD_LVALUE "Illegal item on left side of '='"
#define EXPR_ERR_BAD_INDEX  "Illegal array index"
#define EXPR_ERR_RANGE_ERROR "Array index out of range"
#define EXPR_ERR_MISCOUNT   "Incorrect number of indexes on array"
#define EXPR_ERR_NO_ARRAY   "Array must be declared first"
#define EXPR_ERR_MISSING    "Missing operand in expression"
#define EXPR_ERR_NOOP       "Expression has no effect"
#define EXPR_ERR_OPENPAR    "Missing '('"
#define EXPR_ERR_CLOSEPAR   "Missing ')'"
#define EXPR_ERR_PAR_COUNT  "Incorrect number of parameters to function"
#define EXPR_ERR_NUMERIC    "String expression not allowed"
#define EXPR_ERR_STRING     "String expression required"

typedef struct expr_info {
	int flags_in;
	int flags_out;
// results
	bstring *string;
	void *indirect;
	double value;
	int type;
	char *error;
} einfo;

struct gen_func_ret {
	char **take;
	bstring *string;
	double value;
	int type;
	char error[128];
};


int expr(bc *bc, char **take, einfo *ei);
int gather_variable_name(bc *bc, char *put, char **take);
struct variable *find_variable(bc *bc, char *name);
struct variable *add_variable(bc *bc, char *name);
void free_bstring(bstring *bs);
bstring *make_bstring(char *string, int length);
bstring *dup_bstring(bstring *bs);
bstring *make_raw_bstring(int length);
void set_variable(bc *bc, char *name, double value);

