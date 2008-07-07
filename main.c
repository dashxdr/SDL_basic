/*
   SDL_basic written by David Ashley, released 20080621 under the GPL
   http://www.linuxmotors.com/SDL_basic
   dashxdr@gmail.com
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <SDL.h>
#include "misc.h"

#define IXSIZE 1024
#define IYSIZE 768

#define MINXSIZE 256
#define MINYSIZE 256
#define MAXXSIZE 2048
#define MAXYSIZE 2048


bc mybc;
int globaltime=0;

Uint32 mytimer(Uint32 interval, void *param)
{
int nt;
	nt=globaltime+1;
	if(!nt) ++nt; // my god how anal this is...
	globaltime=nt;
	return 10;
}

void initbc(bc *bc, SDL_Surface *surf, int xsize, int ysize)
{
	memset(bc, 0, sizeof(mybc));
	bc->lastcode = -1;
	bc->xsize = xsize;
	bc->ysize = ysize;
	bc->txpos = 0;
	bc->typos = 0;
	bc->cursorstate = 0;
	bc->thescreen = surf;
	bc->white = SDL_MapRGB(bc->thescreen->format, 255, 255, 255);
	bc->black = SDL_MapRGB(bc->thescreen->format, 0, 0, 0);
	bc->cursorcolor = SDL_MapRGB(bc->thescreen->format, 255, 0, 0);
	inittext(bc);
	pruninit(bc);
}


#ifdef WIN32_BUILD
#undef main
#endif
int main(int argc,char **argv)
{
int videoflags;
bc *bc;
SDL_Surface *thescreen;
int xsize, ysize;
int i;
int v1,v2;
	xsize=IXSIZE;
	ysize=IYSIZE;

	for(i=1;i<argc;++i)
	{
		if(sscanf(argv[i], "%dx%d", &v1, &v2) == 2)
		{
			if(v1<MINXSIZE || v1>MAXXSIZE || v2<MINYSIZE || v2>MAXYSIZE)
			{
				fprintf(stderr, "Dimensions are out of range. Must be (%d,%d) to (%d,%d)\n",
					MINXSIZE, MINYSIZE, MAXXSIZE, MAXYSIZE);
				exit(0);
			}
			if(v1&7 || v2&7)
			{
				fprintf(stderr, "Dimensions must be a multiple of 8\n");
				exit(0);
			}
			xsize = v1;
			ysize = v2;
			continue;
		}
	}

	if ( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) < 0 )
	{
		fprintf(stderr, "Couldn't initialize SDL: %s\n",SDL_GetError());
		exit(1);
	}
	videoflags = 0; // SDL_RESIZABLE
	thescreen = SDL_SetVideoMode(xsize, ysize, 32, videoflags);
	if ( thescreen == NULL )
	{
		fprintf(stderr, "Couldn't set display mode: %s\n",
							SDL_GetError());
		exit(5);
	}

	bc=&mybc;
	initbc(bc, thescreen, xsize, ysize);

	SDL_AddTimer(10, mytimer, bc);
	soundopen(bc);

	for(;;)
	{
		if(!(bc->flags & BF_NOPROMPT))
			tprintf(bc, "Ready\n\n");
		else
			bc->flags &= ~BF_NOPROMPT;
		typeline(bc, (bc->flags & BF_EDIT) ? bc->workspace : "", 1);
		bc->flags &= ~BF_EDIT;
		if(bc->flags & BF_QUIT) break;
		processline(bc, bc->debline);
		if(bc->flags & BF_QUIT) break;
//		printf("%s\n", bc->debline);
	}
	SDL_Quit();
	return 0;
}

#undef main
