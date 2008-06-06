#include <SDL.h>

extern SDL_Surface *thescreen;
extern int xsize,ysize;
extern int txsize, tysize;

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

struct linepointer {
int linenum;
char *line;
};

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
void colordot(bc *bc, unsigned int x,unsigned int y,int c);
void update(bc *bc);


// font.c

void inittext(bc *bc);
void drawtext(bc *bc, int x, int y, Uint32 fgcolor, Uint32 bgcolor, char *str);
void cursor(bc *bc, int onoff);
void tprintf(bc *bc, char *s, ...);
void drawcharxy(bc *bc, unsigned int x, unsigned int y, char c);

// render.c

// keyboard.c

void markkey(bc *bc, int code, int mod, int downup);
void typeline(bc *bc, char *prompt,int echocr);
void updatef(bc *bc);
void scaninput(bc *bc);

// basic.c

void processline(bc *bc, char *line);

// expr.c

double expr(bc *bc, char **take);
