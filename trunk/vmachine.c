#include <math.h>
#include "misc.h"

#define TD(name) void name(bc *bc){}
TD(arrayd)
TD(arrays)
TD(assigns)
TD(eqs)
TD(nes)


/**************************************************************************
          low level generic functions
**************************************************************************/

void checkdone(bc *bc)
{
	if(bc->flags & (BF_CCHIT | BF_RUNERROR | BF_ENDHIT |
		 BF_STOPHIT | BF_QUIT))
		bc->vdone = 1;
}

void pushd(bc *bc){bc->vsp++->d = bc->vip++->d;}
void pushi(bc *bc){bc->vsp++->i = bc->vip++->i;}
void performend(bc *bc){bc->vdone = 1;}
void rjmp(bc *bc)
{
	bc->vip += bc->vip->i - 1;
	if(!(++bc->xcount&0xff))
	{
		update(bc);
		checkdone(bc);
		scaninput(bc);
	}
}

void addd(bc *bc){--bc->vsp;bc->vsp[-1].d += bc->vsp[0].d;}
void muld(bc *bc){--bc->vsp;bc->vsp[-1].d *= bc->vsp[0].d;}
void subd(bc *bc){--bc->vsp;bc->vsp[-1].d -= bc->vsp[0].d;}
void divd(bc *bc){--bc->vsp;if(bc->vsp[0].d!=0.0) bc->vsp[-1].d /= bc->vsp[0].d;}
void powerd(bc *bc){--bc->vsp;bc->vsp[-1].d = pow(bc->vsp[-1].d, bc->vsp[0].d);}

void andd(bc *bc){--bc->vsp;bc->vsp[-1].d = (int)bc->vsp[-1].d & (int)bc->vsp[0].d;}
void ord(bc *bc){--bc->vsp;bc->vsp[-1].d = (int)bc->vsp[-1].d | (int)bc->vsp[0].d;}
void xord(bc *bc){--bc->vsp;bc->vsp[-1].d = (int)bc->vsp[-1].d ^ (int)bc->vsp[0].d;}
void andandd(bc *bc){--bc->vsp;bc->vsp[-1].d = bc->vsp[-1].d && bc->vsp[0].d;}
void orord(bc *bc){--bc->vsp;bc->vsp[-1].d = bc->vsp[-1].d || bc->vsp[0].d;}

void pushv(bc *bc){bc->vsp++ -> p = &bc->vvars[bc->vip++ -> i].value.d;}
void pushav(bc *bc){bc->vsp++->i = bc->vip++->i;}
void evald(bc *bc){	bc->vsp[-1].d = *(double *)bc->vsp[-1].p;}
 // skip next 2 steps if TOS != 0
void skip2ne(bc *bc){if((--bc->vsp)->d!=0.0) bc->vip+=2;}
void eqd(bc *bc){--bc->vsp;bc->vsp[-1].d = bc->vsp[-1].d == bc->vsp[0].d;}
void ned(bc *bc){--bc->vsp;bc->vsp[-1].d = bc->vsp[-1].d != bc->vsp[0].d;}
void led(bc *bc){--bc->vsp;bc->vsp[-1].d = bc->vsp[-1].d <= bc->vsp[0].d;}
void ged(bc *bc){--bc->vsp;bc->vsp[-1].d = bc->vsp[-1].d >= bc->vsp[0].d;}
void ltd(bc *bc){--bc->vsp;bc->vsp[-1].d = bc->vsp[-1].d < bc->vsp[0].d;}
void gtd(bc *bc){--bc->vsp;bc->vsp[-1].d = bc->vsp[-1].d > bc->vsp[0].d;}

void intd(bc *bc)
{
double *d=&bc->vsp[-1].d;
	if(*d<0.0)
		*d=(int)*d - 1;
	else
		*d=(int)*d;
}
void fixd(bc *bc){double *d=&bc->vsp[-1].d;*d = (int)*d;}
void sgnd(bc *bc){double *d=&bc->vsp[-1].d;*d = *d<0.0 ? -1 : (*d>0.0 ? 1 : 0.0);}
void sind(bc *bc){double *d=&bc->vsp[-1].d;*d = sin(*d);}
void cosd(bc *bc){double *d=&bc->vsp[-1].d;*d = cos(*d);}
void powd(bc *bc){bc->vsp[-1].d=pow(bc->vsp[-2].d, bc->vsp[-1].d);--bc->vsp;}
void logd(bc *bc){double *d=&bc->vsp[-1].d;*d = log(*d);}
void expd(bc *bc){double *d=&bc->vsp[-1].d;*d = exp(*d);}
void tand(bc *bc){double *d=&bc->vsp[-1].d;*d = tan(*d);}
void atnd(bc *bc){double *d=&bc->vsp[-1].d;*d = atan(*d);}
void atn2d(bc *bc){bc->vsp[-2].d=atan2(bc->vsp[-2].d, bc->vsp[-1].d);--bc->vsp;}
void absd(bc *bc){double *d=&bc->vsp[-1].d;if(*d<0.0) *d = -*d;}

void sqrd(bc *bc){double *d=&bc->vsp[-1].d;if(*d>=0.0) *d = sqrt(*d);}
void chs(bc *bc){bc->vsp[-1].d = -bc->vsp[-1].d;}

// take both off the stack...
void assignd(bc *bc){*(double *)bc->vsp[-2].p = bc->vsp[-1].d;bc->vsp-=2;}

static void dim(bc *bc, int size)
{
variable *v;
int rank;
int i;
	v=bc->vvars+bc->vsp[-1].i;
	rank = bc->vsp[-2].i;
	bc->vsp -= rank+2;
	v->dimensions[0]=1;
	v->rank = rank;
#warning do some sanity checks...
	for(i=0;i<rank;++i)
		v->dimensions[i+1] = v->dimensions[i] * bc->vsp[i].i;
	v->pointer = calloc(v->dimensions[rank], size);
#warning check for allocation success
}

void dimd(bc *bc)
{
	dim(bc, sizeof(double));
}

void dims(bc *bc)
{
	dim(bc, sizeof(bstring *));
}



/**************************************************************************
          BASIC high level commands, for the most part...
**************************************************************************/

void printd(bc *bc)
{
double d;
	d=(--bc->vsp)->d;
	if(d==(int)d)
		tprintf(bc, " %d ", (int)d);
	else
		tprintf(bc, " %.2f ", d);
}

void lf(bc *bc){tprintf(bc, "\n");}
void tab(bc *bc)
{
int t;
	t=(bc->txpos&~15)+16;
	if(t>=bc->txsize)
		tprintf(bc, "\n");
	else
	{
		char spaces[32];
		t-=bc->txpos;
		memset(spaces, ' ', t);
		spaces[t] = 0;
		tprintf(bc, "%s", spaces);
	}
}

void home(bc *bc)
{
	tprintf(bc, "\0330x\0330y");
}

void cls(bc *bc)
{
	cleartext(bc);
	home(bc);
	fillscreen(bc, 0, 0, 0, 255);
}

void rndd(bc *bc)
{
int r;
	r=bc->vsp[-1].d;
	if(r)
		bc->vsp[-1].d = 1 + rand()%r;
	else
		bc->vsp[-1].d = (rand()&0x7fffffff)/(double)0x7fffffff;
}
void sleepd(bc *bc)
{
int until, diff;
double d;
	d=(--bc->vsp)->d;

	if(d<0) return;
	if(d>2.0) d=2.0;

	until=bc->waitbase + d*1000;

	for(;;)
	{
		diff = until - SDL_GetTicks();
		if(diff<0 || diff>2000)
		{
//printf("%s: diff out of range! %d\n", __FUNCTION__, diff);
			reset_waitbase(bc);
			break;
		}
		SDL_Delay(1);
		scaninput(bc);
		checkdone(bc);
	}
}

void mousexd(bc *bc){(bc->vsp++)->d = bc->mousex;}
void mouseyd(bc *bc){(bc->vsp++)->d = bc->mousey;}
void mousebd(bc *bc){(bc->vsp++)->d = bc->mouseb;}
void ticksd(bc *bc){(bc->vsp++)->d = SDL_GetTicks() - bc->starttime;}

void color3(bc *bc)
{
	bc->gred = bc->vsp[-3].d;
	bc->ggreen = bc->vsp[-2].d;
	bc->gblue = bc->vsp[-1].d;
	bc->galpha = 255;
	bc->vsp-=3;
}

void color4(bc *bc)
{
	bc->gred = bc->vsp[-4].d;
	bc->ggreen = bc->vsp[-3].d;
	bc->gblue = bc->vsp[-2].d;
	bc->galpha = bc->vsp[-1].d;
	bc->vsp-=4;
}

// x,y,  width, height
void box4(bc *bc)
{
double list[4];

	list[0]=bc->vsp[-4].d;
	list[1]=bc->vsp[-3].d;
	list[2]=bc->vsp[-2].d;
	list[3]=bc->vsp[-1].d;
	bc->vsp-=4;
	shape_init(bc);
	shape_add(bc, list[0]-list[2], list[1]-list[3], TAG_ONPATH);
	shape_add(bc, list[0]+list[2], list[1]-list[3], TAG_ONPATH);
	shape_add(bc, list[0]+list[2], list[1]+list[3], TAG_ONPATH);
	shape_add(bc, list[0]-list[2], list[1]+list[3], TAG_ONPATH);
	shape_done(bc);
}

void rect4(bc *bc)
{
double list[4];
double pen2=bc->pen/2.0;

	list[0]=bc->vsp[-4].d;
	list[1]=bc->vsp[-3].d;
	list[2]=bc->vsp[-2].d;
	list[3]=bc->vsp[-1].d;
	bc->vsp-=4;
	shape_init(bc);
	shape_add(bc, list[0]-list[2]-pen2, list[1]-list[3]-pen2, TAG_ONPATH);
	shape_add(bc, list[0]+list[2]+pen2, list[1]-list[3]-pen2, TAG_ONPATH);
	shape_add(bc, list[0]+list[2]+pen2, list[1]+list[3]+pen2, TAG_ONPATH);
	shape_add(bc, list[0]-list[2]-pen2, list[1]+list[3]+pen2, TAG_ONPATH);
	shape_end(bc);
	shape_add(bc, list[0]-list[2]+pen2, list[1]-list[3]+pen2, TAG_ONPATH);
	shape_add(bc, list[0]+list[2]-pen2, list[1]-list[3]+pen2, TAG_ONPATH);
	shape_add(bc, list[0]+list[2]-pen2, list[1]+list[3]-pen2, TAG_ONPATH);
	shape_add(bc, list[0]-list[2]+pen2, list[1]+list[3]-pen2, TAG_ONPATH);
	shape_done(bc);

}

void performdisc(bc *bc)
{
	disc(bc, bc->vsp[-3].d, bc->vsp[-2].d, bc->vsp[-1].d);
	bc->vsp -= 3;
}

void performcircle(bc *bc)
{
	circle(bc, bc->vsp[-3].d, bc->vsp[-2].d, bc->vsp[-1].d);
	bc->vsp -= 3;
}

void performpen(bc *bc)
{
	bc->pen = (--bc->vsp)->d;
}

void performstop(bc *bc)
{
	bc->flags |= BF_STOPHIT;
}

void performfill(bc *bc)
{
	cleartext(bc);
	fillscreen(bc, bc->gred, bc->ggreen, bc->gblue, bc->galpha);
}

void performmove(bc *bc)
{
	bc->gx = bc->vsp[-2].d;
	bc->gy = bc->vsp[-1].d;
	bc->vsp -= 2;
}

void performline(bc *bc)
{
	stroke(bc, bc->vsp[-2].d, bc->vsp[-1].d);
	bc->vsp -= 2;
}

void vmachine(bc *bc, step *program, step *stack)
{
	bc->vdone = 0;
	bc->vip = program;
	bc->vsp = stack;

	while(!bc->vdone)
		(bc->vip++ -> func)(bc);
}
