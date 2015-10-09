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

int randomblock[RANDOM_SIZE]={
0x76ca8ff1,
0x573d7ea4,
0x2ae6b258,
0x23140f07,
0x4e95c6da,
0x11551b85,
0x6988c599,
0x4ee418a4,
0x1907aca7,
0x70dd13f6,
0x65d89253,
0x401e07d7,
0x7cc12f8a,
0x742483de,
0x6f9464ad,
0x13a45c30,
0x53dde70d,
0x42d3a0de,
0x187e546f,
0x3069f430,
0x14ac1193,
0x5795ce93,
0x06089651,
0x14e771f5,
0x43876f29,
0x6427b411,
0x62277508,
0x680b4d6f,
0x38e14a23,
0x252f82be,
0x762ab4e6,
0x2fabda14,
0x7c6d0162,
0x2111673e,
0x52bfe91b,
0x4b02c83c,
0x326682c4,
0x3c48aeb4,
0x19e6e0e0,
0x4b6e2f6b,
0x2d25c2ab,
0x7fbf7334,
0x0b8c3742,
0x29e6f235,
0x73e3f712,
0x7b209bf0,
0x3d8b4e65,
0x47c1de1f,
0x3df43cce,
0x5609a2d4,
0x782bd24f,
0x52a04e61,
0x2d9f7167,
0x7e3468a1,
0x6787c057,
};

int randnum(bc *bc)
{
int p;
	p = bc->rpointer - RANDOM_TAP;
	if(p<0) p+=RANDOM_SIZE;
	p = bc->randomblock[bc->rpointer]^=bc->randomblock[p];
	++bc->rpointer;
	if(bc->rpointer == RANDOM_SIZE)
		bc->rpointer = 0;
	return p;
}

void initbc(bc *bc, SDL_Renderer *renderer, SDL_Window *window, int xsize, int ysize)
{
	memset(bc, 0, sizeof(mybc));
	memcpy(bc->randomblock, randomblock, sizeof(bc->randomblock));
	bc->lastcode = -1;
	bc->xsize = xsize;
	bc->ysize = ysize;
	bc->txpos = 0;
	bc->typos = 0;
	bc->cursorstate = 0;
	bc->renderer = renderer;
	bc->window = window; // actually serves no purpose...
//	bc->thescreen = surf;
//	bc->white = SDL_MapRGB(bc->thescreen->format, 255, 255, 255);
//	bc->black = SDL_MapRGB(bc->thescreen->format, 0, 0, 0);
//	bc->cursorcolor = SDL_MapRGB(bc->thescreen->format, 255, 0, 0);
	inittext(bc);
	pruninit(bc);
}


int main(int argc,char **argv)
{
	bc *bc;
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
	signal(SIGINT, SIG_DFL);
	signal(SIGTERM, SIG_DFL);


	SDL_Window *window = SDL_CreateWindow("SDL_basic", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
						xsize, ysize, SDL_WINDOW_HIDDEN);
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_ShowWindow(window);

	bc=&mybc;
	initbc(bc, renderer, window, xsize, ysize);

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
