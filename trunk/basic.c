#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <SDL.h>
#include "misc.h"


#define IXSIZE 1024
#define IYSIZE 768

int xsize,ysize;


int mousex,mousey;

SDL_Surface *thescreen;
void clear(void)
{
int i;
	for(i=0;i<ysize;++i)
		memset(thescreen->pixels + i*thescreen->pitch, 0, xsize*4);
}

// 32bpp
void colordot(bc *bc, unsigned int x,unsigned int y,int c)
{
	if(x<xsize && y<ysize)
		*((Uint32 *)(bc->thescreen->pixels+y*bc->thescreen->pitch)+x)=c;
}
void colorrect(bc *bc, int x, int y, int w, int h, int c)
{
int i,j;
	for(j=0;j<h;++j)
		for(i=0;i<w;++i)
			colordot(bc, x+i, y+j, c);
}
void scrlock(void)
{
	if(SDL_MUSTLOCK(thescreen))
	{
		if ( SDL_LockSurface(thescreen) < 0 )
		{
			fprintf(stderr, "Couldn't lock display surface: %s\n",
								SDL_GetError());
			return;
		}
	}
}
void scrunlock(void)
{
	if(SDL_MUSTLOCK(thescreen))
		SDL_UnlockSurface(thescreen);
	SDL_UpdateRect(thescreen, 0, 0, 0, 0);
}

Uint32 maprgb(int r,int g,int b)
{
	return SDL_MapRGB(thescreen->format,r,g,b);
}



void iterate(void)
{
	scrlock();
	scrunlock();
}

void lock(bc *bc)
{
	if(SDL_MUSTLOCK(bc->thescreen))
	{
		if ( SDL_LockSurface(bc->thescreen) < 0 )
		{
			fprintf(stderr, "Couldn't lock display surface: %s\n",
								SDL_GetError());
		}
	}
}
void unlock(bc *bc)
{
	if(SDL_MUSTLOCK(bc->thescreen))
		SDL_UnlockSurface(bc->thescreen);
}

void update(bc *bc)
{
	SDL_UpdateRect(bc->thescreen, 0, 0, 0, 0);
}



void doexit(int n)
{
	exit(0);
}





int main(int argc,char **argv)
{
int code, mod;
char moved;
SDL_Event event;
int videoflags;
int doneflag;
bc mybc;


	xsize=IXSIZE;
	ysize=IYSIZE;

	if ( SDL_Init(SDL_INIT_VIDEO) < 0 )
	{
		fprintf(stderr, "Couldn't initialize SDL: %s\n",SDL_GetError());
		exit(1);
	}
	videoflags = 0;
	thescreen = SDL_SetVideoMode(xsize, ysize, 32, videoflags);
	if ( thescreen == NULL )
	{
		fprintf(stderr, "Couldn't set display mode: %s\n",
							SDL_GetError());
		exit(5);
	}

	memset(&mybc, 0, sizeof(mybc));
	mybc.lastcode = -1;
	mybc.xsize = xsize;
	mybc.ysize = ysize;
	mybc.txpos = 0;
	mybc.typos = 0;
	mybc.cursorstate = 0;
	mybc.thescreen = thescreen;
	mybc.white = SDL_MapRGB(mybc.thescreen->format, 255, 255, 255);
	mybc.black = SDL_MapRGB(mybc.thescreen->format, 0, 0, 0);
	mybc.cursorcolor = SDL_MapRGB(mybc.thescreen->format, 255, 0, 0);
	inittext(&mybc);


	lock(&mybc);
//	drawtext(&mybc, 20,20,mybc.white, mybc.black, "This is a test!");
	termtext(&mybc, "Ready\nFOOOOO");
int i;for(i=0;i<2192;++i) {char ttt[128];sprintf(ttt, " %d", i);termtext(&mybc, ttt);}
	cursor(&mybc, 1);
	unlock(&mybc);
	update(&mybc);

	typeline(&mybc, "Ready", 1);

	doneflag = 0;
	while(!doneflag)
	{
		iterate();
		usleep(50000);
		moved=0;
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
			case SDL_MOUSEMOTION:
				mousex=event.motion.x;
				mousey=event.motion.y;
				break;
			case SDL_MOUSEBUTTONDOWN:
//				moved=processdown();
				break;
			case SDL_KEYDOWN:
				code=event.key.keysym.sym;
				mod=event.key.keysym.mod;
				if(code==SDLK_ESCAPE) doneflag = 1;
				markkey(&mybc, code, mod, 1);
				break;
			case SDL_KEYUP:
				code=event.key.keysym.sym;
				mod=event.key.keysym.mod;
				markkey(&mybc, code,mod,0);
				break;
			case SDL_QUIT:
				doneflag = 1;
				break;
			}
		}
	}
	return 0;
}
