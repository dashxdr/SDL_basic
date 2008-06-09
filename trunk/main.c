#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <SDL.h>
#include "misc.h"

#define IXSIZE 1024
#define IYSIZE 768


int main(int argc,char **argv)
{
int videoflags;
bc mybc, *bc;
SDL_Surface *thescreen;
int xsize, ysize;

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

	runinit(bc);

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
	return 0;
}
