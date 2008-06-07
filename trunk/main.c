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

Uint32 maprgb(bc *bc, int r,int g,int b)
{
	return SDL_MapRGB(bc->thescreen->format,r,g,b);
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






int main(int argc,char **argv)
{
int code, mod;
char moved;
SDL_Event event;
int videoflags;
int doneflag;
bc mybc, *bc;


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

	bc=&mybc;
	memset(bc, 0, sizeof(mybc));
	bc->lastcode = -1;
	bc->xsize = xsize;
	bc->ysize = ysize;
	bc->txpos = 0;
	bc->typos = 0;
	bc->cursorstate = 0;
	bc->thescreen = thescreen;
	bc->white = SDL_MapRGB(bc->thescreen->format, 255, 255, 255);
	bc->black = SDL_MapRGB(bc->thescreen->format, 0, 0, 0);
	bc->cursorcolor = SDL_MapRGB(bc->thescreen->format, 255, 0, 0);
	inittext(bc);


	lock(bc);
//	drawtext(bc, 20,20,bc->white, bc->black, "This is a test!");
//int i;for(i=0;i<2192;++i) tprintf(bc, "%d ", i);
	cursor(bc, 1);
	unlock(bc);
	update(bc);

	for(;;)
	{
		if(!(bc->flags & BF_NOPROMPT))
			tprintf(bc, "Ready\n\n");
		else
			bc->flags &= ~BF_NOPROMPT;
		typeline(bc, (bc->flags & BF_EDIT) ? bc->workspace : "", 1);
		bc->flags &= ~BF_EDIT;
		bc->flags &= BF_CCHIT;
		processline(bc, bc->debline);
		if(bc->flags & BF_QUIT) break;
//		printf("%s\n", bc->debline);
	}
return 0;

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
				markkey(bc, code, mod, 1);
				break;
			case SDL_KEYUP:
				code=event.key.keysym.sym;
				mod=event.key.keysym.mod;
				markkey(bc, code,mod,0);
				break;
			case SDL_QUIT:
				doneflag = 1;
				break;
			}
		}
	}
	return 0;
}
