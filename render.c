#include <SDL.h>
#include "misc.h"


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


void circle(bc *bc, int cx,int cy,int radius,int c)
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
