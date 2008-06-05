#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include "SDL.h"


#define XSIZE 640
#define YSIZE 480

long gridcolor,xcolor,ocolor,white;

int mousex,mousey;

SDL_Surface *thescreen;
void clear(void)
{
	memset(thescreen->pixels,0,XSIZE*YSIZE*2);
}
void colordot(int x,int y,int c)
{
	*((unsigned short *)thescreen->pixels+y*thescreen->pitch/2+x)=c;
}
void colorrect(int x, int y, int w, int h, int c)
{
int i,j;
	for(j=0;j<h;++j)
		for(i=0;i<w;++i)
			colordot(x+i, y+j, c);
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
	else
		SDL_UpdateRect(thescreen, 0, 0, 0, 0);
}

unsigned long maprgb(int r,int g,int b)
{
	return SDL_MapRGB(thescreen->format,r,g,b);
}



void iterate(void)
{
	scrlock();
	scrunlock();
}



void doexit(int n)
{
	exit(0);
}

int main(int argc,char **argv)
{
int first;
int code;
char moved;
SDL_Event event;
unsigned long videoflags;

	if ( SDL_Init(SDL_INIT_VIDEO) < 0 )
	{
		fprintf(stderr, "Couldn't initialize SDL: %s\n",SDL_GetError());
		exit(1);
	}
	videoflags = 0;
	thescreen = SDL_SetVideoMode(XSIZE, YSIZE, 16, videoflags);
	if ( thescreen == NULL )
	{
		fprintf(stderr, "Couldn't set display mode: %s\n",
							SDL_GetError());
		exit(5);
	}


	first=0;

	signal(SIGINT, doexit);
	signal(SIGTERM, doexit);
	signal(SIGKILL, doexit);


	for(;;)
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
				if(code==SDLK_ESCAPE) exit(0);
				break;
			}
		}
	}
	return 0;
}
