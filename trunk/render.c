#include <SDL.h>
#include <math.h>

#include "misc.h"
#include "ftgrays.h"


void vector(bc *bc, int sx,int sy,int dx,int dy,int c)
{
int diffx,diffy;
int stepx,stepy;
float val,delta;

	stepx=stepy=0;
	diffx=dx-sx;
	if(diffx<0) {diffx=-diffx;stepx=-1;}
	else if(diffx>0) stepx=1;
	diffy=dy-sy;
	if(diffy<0) {diffy=-diffy;stepy=-1;}
	else if(diffy>0) stepy=1;

	colordot(bc, sx,sy,c);

	if(diffx>diffy)
	{
		val=sy;
		delta=(dy-val)/diffx;
		val+=0.5;
		while(diffx--)
		{
			sx+=stepx;
			val+=delta;
			colordot(bc, sx,(int)val,c);
		}
	} else if(diffx<diffy)
	{
		val=sx;
		delta=(dx-val)/diffy;
		val+=0.5;
		while(diffy--)
		{
			sy+=stepy;
			val+=delta;
			colordot(bc, (int)val,sy,c);
		}
	} else
		while(diffx--)
		{
			sx+=stepx;
			sy+=stepy;
			colordot(bc, sx,sy,c);
		}
}


void drawcircle(bc *bc, int cx,int cy,int radius,int c)
{
int x,y,e;

	x=0;
	y=radius;
	e=3-(radius<<1);
	while(x<=y)
	{
		colordot(bc, cx+x,cy+y,c);
		colordot(bc, cx-x,cy+y,c);
		colordot(bc, cx+x,cy-y,c);
		colordot(bc, cx-x,cy-y,c);
		colordot(bc, cx+y,cy+x,c);
		colordot(bc, cx-y,cy+x,c);
		colordot(bc, cx+y,cy-x,c);
		colordot(bc, cx-y,cy-x,c);
		if(e<0)
			e+=(x<<2)+6;
		else
		{
			e+=((x-y)<<2)+10;
			--y;
		}
		++x;
	}
}


void stroke(bc *bc, double x, double y)
{
Uint32 color;
	bc->tainted = 1;
	color = maprgb(bc, bc->gred, bc->ggreen, bc->gblue);
	vector(bc, bc->gx, bc->gy, x, y, color);

	bc->gx = x;
	bc->gy = y;
}

void fillscreen(bc *bc, int r, int g, int b)
{
int i;
Uint32 color;
int w;

	bc->tainted = 1;
	color = maprgb(bc, r, g, b);
	for(i=0;i<bc->xsize;++i)
		colordot(bc, i, 0, color);
	w=bc->xsize * 4;
	for(i=1;i<bc->ysize;++i)
	{
		memcpy(thescreen->pixels + i*thescreen->pitch,
				thescreen->pixels, w);
	}

}

void circle(bc *bc, double cx, double cy, double radius)
{
Uint32 color;
	bc->tainted = 1;
	color = maprgb(bc, bc->gred, bc->ggreen, bc->gblue);
	drawcircle(bc, cx, cy, radius, color);

}

static void colordot_32(SDL_Surface *surf, unsigned int x, unsigned int y, Uint32 c, int f2)
{
Uint32 a;
//printf("%d, %d\n", x, y);
if(x>=1024 || y>=768) return;
	a=((f2+1) * (c>>24))>>8;
	if(a==0xff)
		*((Uint32 *)(surf->pixels)+y * surf->pitch/4+x)=c;
	else
	{
		Uint32 *p, t;
		Uint32 ai;
		
		p=(Uint32 *)(surf->pixels)+y * (surf->pitch>>2)+x;

		ai=a^255;
		t=*p;

		*p = ((a*(c&0xff) + ai*(t&0xff))>>8) |
			(((a*(c&0xff00) + ai*(t&0xff00))&0xff0000)>>8) |
			(((a*(c&0xff0000) + ai*(t&0xff0000))&0xff000000)>>8);
	}
}


static void solidstrip(bc *bc, void *_span, int y)
{
int x, w, coverage;
SDL_Surface *s = bc->thescreen;

	x=((FT_Span *)_span)->x;
	w=((FT_Span *)_span)->len;
	coverage=((FT_Span *)_span)->coverage;
	while(w--)
		colordot_32(s, x++, y, ~0, coverage); //c->solidcolor, coverage);
}

void myspanner(int y, int count, FT_Span *spans, void *user)
{
bc *bc=user;

// renders spans in reverse order, but why not? They don't overlap
	while(count--)
	{
		solidstrip(bc, spans+count, y);
		
	}
}


#define TAG_ONPATH    1 // on the path
#define TAG_CONTROL2  0 // quadratic bezier control point
#define TAG_CONTROL3  2 // cubic bezier control point

#define IFACTOR 64  // used to fix coords to the grays rendering engine 

void rendertest(bc *bc)
{
	printf("render test!\n");
FT_Vector points[1024];
char tags[1024];
short pathstops[100];
int res;
FT_Raster myraster;
FT_Raster_Params myparams;
FT_Outline myoutline;
int i;
float a;

	for(i=0;i<100;++i)
	{
		a=i*3.1415928*2/100;
		points[i].x = IFACTOR*(512+200*cos(a));
		points[i].y = IFACTOR*(384+200*sin(a));
		tags[i] = TAG_ONPATH;
	}
	pathstops[0] = i-1;

	myoutline.n_contours = 1;
	myoutline.n_points = 100;
	myoutline.points = points;
	myoutline.tags = tags;
	myoutline.contours = pathstops;
	myoutline.flags = FT_OUTLINE_IGNORE_DROPOUTS |
			FT_OUTLINE_EVEN_ODD_FILL;

	myparams.target = 0;
	myparams.source = &myoutline;
	myparams.flags = FT_RASTER_FLAG_DIRECT | FT_RASTER_FLAG_AA |
		FT_RASTER_FLAG_CLIP;
	myparams.gray_spans = myspanner;
	myparams.user = bc;
	myparams.clip_box.xMin = 0;
	myparams.clip_box.xMax = bc->xsize-1;
	myparams.clip_box.yMin = 0;
	myparams.clip_box.yMax = bc->ysize-1;
	res=SDL_basic_ft_grays_raster.raster_new(0, &myraster);

	SDL_basic_ft_grays_raster.raster_reset(myraster, bc->pool, sizeof(bc->pool));

	res=SDL_basic_ft_grays_raster.raster_render(myraster, &myparams);

	SDL_basic_ft_grays_raster.raster_done(myraster);



	bc->tainted = 1;
}
