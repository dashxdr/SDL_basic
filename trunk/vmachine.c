#include <math.h>
#include "misc.h"

#define TD(name) void name(bc *bc){}
TD(pushvad)
TD(arrayd)
TD(assigns)
TD(eqs)
TD(nes)


/**************************************************************************
          low level generic functions
**************************************************************************/

void pushd(bc *bc){bc->vsp++->d = bc->vip++->d;}
void pushi(bc *bc){bc->vsp++->i = bc->vip++->i;}
void performend(bc *bc){bc->vdone = 1;}
void rjmp(bc *bc){bc->vip += bc->vip->i - 1;}

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

void pushvd(bc *bc){bc->vsp++ -> p = &bc->vvars[bc->vip++ -> i].value.d;}
void evald(bc *bc){	bc->vsp[-1].d = *(double *)bc->vsp[-1].p;}
 // skip next 2 steps if TOS != 0
void skip2ne(bc *bc){if((--bc->vsp)->d!=0.0) bc->vip+=2;}
void eqd(bc *bc){--bc->vsp;bc->vsp[-1].d = bc->vsp[-1].d == bc->vsp[0].d;}
void ned(bc *bc){--bc->vsp;bc->vsp[-1].d = bc->vsp[-1].d != bc->vsp[0].d;}
void led(bc *bc){--bc->vsp;bc->vsp[-1].d = bc->vsp[-1].d <= bc->vsp[0].d;}
void ged(bc *bc){--bc->vsp;bc->vsp[-1].d = bc->vsp[-1].d >= bc->vsp[0].d;}
void ltd(bc *bc){--bc->vsp;bc->vsp[-1].d = bc->vsp[-1].d < bc->vsp[0].d;}
void gtd(bc *bc){--bc->vsp;bc->vsp[-1].d = bc->vsp[-1].d > bc->vsp[0].d;}

void sqrd(bc *bc){if(bc->vsp[-1].d>=0.0) bc->vsp[-1].d = sqrt(bc->vsp[-1].d);}
void chs(bc *bc){bc->vsp[-1].d = -bc->vsp[-1].d;}

// take both off the stack...
void assignd(bc *bc){*(double *)bc->vsp[-2].p = bc->vsp[-1].d;bc->vsp-=2;}


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
	}
}

void color3(bc *bc)
{
	bc->gred = bc->vsp[-3].d;
	bc->ggreen = bc->vsp[-2].d;
	bc->gblue = bc->vsp[-1].d;
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

void performdisc(bc *bc)
{
	disc(bc, bc->vsp[-3].d, bc->vsp[-2].d, bc->vsp[-1].d);
	bc->vsp -= 3;
}




void vmachine(bc *bc, step *program, step *stack)
{
	bc->vdone = 0;
	bc->vip = program;
	bc->vsp = stack;

	while(!bc->vdone)
		(bc->vip++ -> func)(bc);
}
