#include <SDL.h>
#include <SDL_image.h>
#include <math.h>
#include "ftgrays.h"

#define M_PI2 (M_PI*2.0)

#define RANDOM_SIZE 55
#define RANDOM_TAP  31

#define KEYMAX 32
#define KEYHISTSIZE 16

#define SCROLLHISTORYSIZE 8192

#define MAX_PROGRAM_LINES 100000

#define HISTSIZE 512
#define LINESIZE 256

#define BF_QUIT         1 // we can quit
#define BF_NOPROMPT     2 // don't put up "Ready" prompt.
#define BF_EDIT         4 // preload a line, for editing
#define BF_RUNERROR     8 // Error on a line
#define BF_CCHIT       16 // Hit control-c
#define BF_ENDHIT      32 // End
#define BF_STOPHIT     64 // Stop
#define BF_RUNNING    128 // program is running
#define BF_INPUT      256 // we're in input

struct linepointer {
int linenum;
char *line;
};

#define GOSUBMAX   10000

typedef struct bstring {
	int length;
	char string[];
} bstring;

#define MAX_FORS 100


struct basic_context;
#define STEPSIZE sizeof(double)
typedef union {
	int i;
	double d;
	bstring *bs;
	void (*func)(struct basic_context *);
	void *p;
	char str[STEPSIZE];
	unsigned char ustr[STEPSIZE];
} step; // program element, needs to hold a void * or int or double

typedef struct {
	int linenumber;
	int step;
	char *src;
} linemap;

#define MAXDATA 100000

#define MAXVARIABLES 1024
#define NAMELEN 16
#define MAXDIMENSIONS 16
// array and variable of the same name share structure
typedef struct {
	char name[NAMELEN];
	int rank;
	int dimensions[MAXDIMENSIONS+1];
	void *pointer;
	union {
		double d;
		bstring *s;
	} value;
} variable;

typedef struct {
	variable *v;
	double delta, end;
	step *start;
} forstate;

#define SND_ACTIVE         1
#define SND_QUIET          2

typedef struct {
	int flags;
	double start;
	double frequency;
	double fmul;
	double volume;
	double duration;
	double time;
	short *wave;
	int index;
	int count;
} sound;

#define MAX_SHAPE_POINTS 100000
#define MAX_SHAPE_CONTOURS 64
typedef struct {
	int numpoints;
	int numcontours;
	int pathstops[MAX_SHAPE_CONTOURS];
	FT_Vector  points[MAX_SHAPE_POINTS];
	char tags[MAX_SHAPE_POINTS];

} shape;

#define MAX_SOUNDS 32

typedef struct mytexture {
	SDL_Texture *texture;
	int w, h;
} mytexture;

typedef struct basic_context {
	int flags;
	SDL_Renderer *renderer;
	SDL_Window *window;
	SDL_Texture *font;
	double time;
	int mousex, mousey, mouseb;
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
	int nextupdate;
	int tainted;
	int textsize;
	char *textstate, *textbak;
	char filename[128];
	char workspace[1024];
	int scrollhistoryin;
	char *scrollhistory;
	char program[1000000]; // 1M ought to be big enough...
	int numlines;
	struct linepointer lps[MAX_PROGRAM_LINES]; // 100K lines

	int numfors;
	forstate forstates[MAX_FORS];
	int gosubsp;
	step *gosubs[GOSUBMAX];
	Uint32 temp;
// graphics rendering state
	double gx, gy; // position
	int gred, ggreen, gblue, galpha;
	double pen; // pen size
	unsigned char pool[0x100000];
// shape state
	shape shape;
	shape pshape;
//
	int datanum;
	int datapull;
	double data[MAXDATA];
	int starttime;
	double waitbase;
	step *base;
	step *lastip;
	step *vip;
	step *vsp;
	step vstack[512];
	int numvars;
	variable vvars[MAXVARIABLES];
	linemap lm[MAX_PROGRAM_LINES];
	int takeaction;
// sound
	int leftover;
	int samples;
	int soundworking;
	sound sounds[MAX_SOUNDS];
	sound isounds[MAX_SOUNDS];
	sound *csound;
	double soundtime;
	int soundticks;
	short wsqr[8192], wtri[8192], wsin[8192], wsaw[8192];
// random numbers
	int randomblock[RANDOM_SIZE];
	int rpointer;
// Loaded sdl textures
#define MAXTEXTURES 256
	mytexture textures[MAXTEXTURES];
} bc;

// main.c

extern int globaltime;
int randnum(bc *bc);

// font.c

void inittext(bc *bc);
void drawtext(bc *bc, int x, int y, int colormode, char *str);
void cursor(bc *bc, int onoff);
int tprintf(bc *bc, char *s, ...);
void drawcharxy(bc *bc, unsigned int x, unsigned int y, char c);
void cleartext(bc *bc);

// render.c

#define TAG_ONPATH    1 // on the path
#define TAG_CONTROL2  0 // quadratic bezier control point
#define TAG_CONTROL3  2 // cubic bezier control point

void update(bc *bc);
void forceupdate(bc *bc);
void resetupdate(bc *bc);
void drawchar(bc *bc, int x, int y, int c, int colormode);
void taint(bc *bc);
void stroke(bc *bc, double x, double y);
void fillscreen(bc *bc, int r, int g, int b, int a);
void circle(bc *bc, double cx, double cy, double radius);
void disc(bc *bc, double cx, double cy, double radius);
void rendertest(bc *bc);
void shape_init(shape *shape);
void shape_add(shape *shape, double x, double y, int tag);
void shape_end(shape *shape);
void shape_done(bc *bc, shape *shape);
void spot(bc *bc);
void lock(bc *bc);
void unlock(bc *bc);
void arc_piece(shape *shape, double xc, double yc, double r, double a, double da);
void drawtexture(bc *bc, int num, double x, double y);

// keyboard.c

void markkey(bc *bc, int code, int mod, int downup);
void typeline(bc *bc, char *prompt,int echocr);
void scaninput(bc *bc);
void flushinput(bc *bc);
int takedown(bc *bc);
int checkpressed(bc *bc, int code);
int showhistory(bc *bc, int back);

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

// basic.c

void processline(bc *bc, char *line);

// parse.c, generated by bison from grammar.y

void parse(bc *bc, int runit);
void parseline(bc *bc, char *line);
void renumber(bc *bc, int delta, int start);
void pruninit(bc *bc);

// vmachine.c

#define MODIFIER_BITS       4 // enough for 15 modifiers
#define RENDER_ROUND        1 // figure has a rounding value
#define RENDER_ROTATE       2 // figure is rotated

#define DECLARE(name) extern void name(bc *bc);
DECLARE(pushd)
DECLARE(evald)
DECLARE(pushvd)
DECLARE(pushav)
DECLARE(pushi)
DECLARE(arrayd)
DECLARE(arrays)
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
DECLARE(rndd)
DECLARE(skip2ne) // skip next 2 steps if TOS != 0
DECLARE(performend)
DECLARE(sleepd)
DECLARE(rjmp)
DECLARE(printd)
DECLARE(lf)
DECLARE(tab)
DECLARE(chs)
DECLARE(cls)
DECLARE(home)
DECLARE(color3)
DECLARE(color4)
DECLARE(box)
DECLARE(rect)
DECLARE(performdisc)
DECLARE(dimd)
DECLARE(dims)
DECLARE(performcircle)
DECLARE(performpen)
DECLARE(performstop)
DECLARE(performfill)
DECLARE(performmove)
DECLARE(performline)
DECLARE(intd)
DECLARE(fixd)
DECLARE(sgnd)
DECLARE(sind)
DECLARE(cosd)
DECLARE(powd)
DECLARE(logd)
DECLARE(expd)
DECLARE(tand)
DECLARE(atnd)
DECLARE(atn2d)
DECLARE(absd)
DECLARE(mousexd)
DECLARE(mouseyd)
DECLARE(mousebd)
DECLARE(ticksd)
DECLARE(performfor)
DECLARE(performnext)
DECLARE(performnext1)
DECLARE(rcall)
DECLARE(ret)
DECLARE(ongoto)
DECLARE(ongosub)
DECLARE(pushea)
DECLARE(datad)
DECLARE(readd)
DECLARE(pushs)
DECLARE(adds)
DECLARE(evals)
DECLARE(prints)
DECLARE(pushvs)
DECLARE(input)
DECLARE(lend)
DECLARE(vald)
DECLARE(ascd)
DECLARE(leftstr)
DECLARE(rightstr)
DECLARE(midstr)
DECLARE(chrstr)
DECLARE(performstrstr)
DECLARE(stringstr)
DECLARE(tabstr)
DECLARE(printat)
DECLARE(inkey)
DECLARE(pop)
DECLARE(keyd)
DECLARE(keycoded)
DECLARE(quiet)
DECLARE(setsound)
DECLARE(freq)
DECLARE(vol)
DECLARE(dur)
DECLARE(soundgo)
DECLARE(note)
DECLARE(fmul)
DECLARE(wsin)
DECLARE(wsqr)
DECLARE(wtri)
DECLARE(wsaw)
DECLARE(arc)
DECLARE(wedge)
DECLARE(shinit)
DECLARE(shend)
DECLARE(shdone)
DECLARE(shmove)
DECLARE(shline)
DECLARE(shcurve)
DECLARE(shcubic)
DECLARE(not)
DECLARE(modd)
DECLARE(loadtexture)
DECLARE(performdrawtexture)

void vmachine(bc *bc, step *program, step *stack);
void reset_waitbase(bc *bc);
void free_bstring(bc *bc, bstring *bs);
bstring *make_bstring(bc *bc, char *string, int length);
bstring *dup_bstring(bc *bc, bstring *bs);
bstring *make_raw_bstring(bc *bc, int length);

// sound.c

void soundopen(bc *bc);

// vdis.c

void disassemble(bc *bc, step *p, int num);

// help.c

void dohelp(bc *bc, char *p);
