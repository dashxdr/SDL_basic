#include <SDL.h>

extern SDL_Surface *thescreen;
extern int xsize,ysize;
extern int txsize, tysize;

#define KEYMAX 32
#define KEYHISTSIZE 16

#define HISTSIZE 512
#define LINESIZE 128

typedef struct basic_context {
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
	int fheight;
	int lastupdate;
	int tainted;
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



// basic.c
void colordot(bc *bc, unsigned int x,unsigned int y,int c);
void update(bc *bc);


// font.c

void inittext(bc *bc);
void drawtext(bc *bc, int x, int y, Uint32 fgcolor, Uint32 bgcolor, char *str);
void termtext(bc *bc, char *str, ...);
void cursor(bc *bc, int onoff);

// render.c

// keyboard.c

void markkey(bc *bc, int code, int mod, int downup);
void typeline(bc *bc, char *prompt,int echocr);
