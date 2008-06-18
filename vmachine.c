#include <math.h>
#include <stdarg.h>
#include "misc.h"

#define TD(name) void name(bc *bc){}

void reset_waitbase(bc *bc)
{
	bc->waitbase = SDL_GetTicks();
}

bstring *make_raw_bstring(bc *bc, int length)
{
bstring *bs;
	bs=malloc(length + sizeof(bstring) + 1);
	if(bs)
	{
		bs->length = length;
		bs->string[length]=0;
	}
	return bs;
}

bstring *make_bstring(bc *bc, char *string, int length)
{
bstring *bs;
	bs=make_raw_bstring(bc, length);
#warning check for allocation failure
	if(bs)
	{
		memcpy(bs->string, string, length);
	}
	return bs;
}

void free_bstring(bc *bc, bstring *bs)
{
	if(bs) free(bs);
}

bstring *dup_bstring(bc *bc, bstring *bs)
{
	return make_bstring(bc, bs->string, bs->length);
}


void takeaction(bc *bc)
{
	bc->takeaction = 0;
}

static int online(bc *bc)
{
int n, line;
	n=bc->lastip - bc->base;
	for(line=0;line<bc->numlines;++line)
		if(bc->lm[line].step > n)
			break;
	if(line)
		return bc->lm[line-1].linenumber;
	else return 0;
}

void verror(bc *bc, char *s, ...)
{
va_list ap;
char temp[1024];
int line = online(bc);
	va_start(ap, s);
	vsnprintf(temp, sizeof(temp), s, ap);
	va_end(ap);
	tprintf(bc, "\nLine %d: %s\n", line, temp);
	bc->flags |= BF_RUNERROR;
	takeaction(bc);
}

void vardump(bc *bc)
{
variable *v;
int i;
	for(i=0;i<bc->numvars;++i)
	{
		v=bc->vvars + i;
		tprintf(bc, "---- Variable %s\n", v->name);
	}
}

void nomem(bc *bc)
{
	verror(bc, "Out of memory");
}

int needstop(bc *bc)
{
	return !!(bc->flags & (BF_CCHIT | BF_RUNERROR | BF_ENDHIT |
				 BF_STOPHIT | BF_QUIT));
}



/**************************************************************************
          low level generic functions
**************************************************************************/

void pop(bc *bc){--bc->vsp;}
void pushd(bc *bc){bc->vsp++->d = bc->vip++->d;}
void pushi(bc *bc){bc->vsp++->i = bc->vip++->i;}
void pushea(bc *bc){bc->vsp++->p = bc->vip -1 + bc->vip->i;++bc->vip;}
void performend(bc *bc){bc->flags|=BF_ENDHIT;takeaction(bc);}
void rjmp(bc *bc)
{
	bc->vip += bc->vip->i - 1;
}

void rcall(bc *bc)
{
	if(bc->gosubsp == GOSUBMAX)
	{
		verror(bc, "Gosub stack overflow"); // ipfix = -1
		++bc->vip;
	} else
	{
		bc->gosubs[bc->gosubsp++] = bc->vip+1;
		bc->vip += bc->vip->i -1;
	}
}

void ret(bc *bc)
{
	if(!bc->gosubsp)
		verror(bc, "Return outside of subroutine"); // ipfix = -1
	else
		bc->vip = bc->gosubs[--bc->gosubsp];
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

void pushvd(bc *bc){bc->vsp++ -> p = &bc->vvars[bc->vip++ -> i].value.d;}
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
void powd(bc *bc){bc->vsp[-2].d=pow(bc->vsp[-2].d, bc->vsp[-1].d);--bc->vsp;}
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
int t;
	v=bc->vvars+bc->vsp[-1].i;
	rank = (bc->vip++)->i;
	bc->vsp -= rank+1;
	if(v->rank)
	{
		verror(bc, "Duplicate dimension variable '%s'", v->name); // ipfix=-1
		return;
	}
	v->dimensions[0]=1;
	v->rank = rank;
	for(i=0;i<rank;++i)
	{
		t=bc->vsp[i].i;
		if(t<1)
		{
			verror(bc, "Invalid dimension size on '%s'", v->name);//ipfix=-1
			return;
		}
		v->dimensions[i+1] = v->dimensions[i] * t;
	}
	v->pointer = calloc(v->dimensions[rank], size);
	if(!v->pointer)
		nomem(bc); // ipfix=-1
}

void dimd(bc *bc)
{
	dim(bc, sizeof(double));
}

void dims(bc *bc)
{
	dim(bc, sizeof(bstring *));
}

static void *array(bc *bc, int size)
{
variable *v;
int rank;
int i;
int j;
int t;

	v=bc->vvars + (bc->vip++)->i;
	rank = v->rank;
	if(!v->rank)
	{
		verror(bc, "Must use DIM before referencing array '%s'\n", // ipfx=-1
			v->name);
		return 0;
	}
	bc->vsp -= rank;
	j=0;
	for(i=0;i<rank;++i)
	{
		t=bc->vsp[i].d - 1;
		j += t*v->dimensions[i];
		if(t<0 || j>=v->dimensions[i+1])
		{
			verror(bc, "Subscript out of range");//ipfix=-1
			return 0;
		}
	}
	return (char *)v->pointer + j * size;
}

void arrayd(bc *bc)
{
double *d;

	d = array(bc, sizeof(double *));
	if(d)
		(bc->vsp++)->p = d;
}

static forstate *getfor(bc *bc, variable *v)
{
int i;
forstate *fs;
	for(i=bc->numfors, fs=bc->forstates+i-1;i;--i, --fs)
		if(fs->v == v)
			return fs;
	return 0;	
}

void performfor(bc *bc)
{
forstate *fs;
variable *v;
int i;
	v=bc->vvars + bc->vsp[-1].i;
	fs = getfor(bc, v);
	if(!fs)
	{
		if(bc->numfors == MAX_FORS)
		{
			verror(bc,"Too many for statements. Limit %d.\n", MAX_FORS);//-1
			return;
		}
		fs = bc->forstates + bc->numfors++;
		fs->v = v;
	} else if((i = bc->numfors - 1 - (fs-bc->forstates)))
	{
		forstate t;
		t=*fs;
		while(i--)
			*fs = fs[1], ++fs;
		*fs = t;
	}
	fs->delta = bc->vsp[-2].d;
	fs->end = bc->vsp[-3].d;
	fs->start = bc->vip;
	v->value.d = bc->vsp[-4].d;
	bc->vsp -= 4;
}

void lownext(bc *bc, forstate *fs)
{
variable *v=fs->v;
	if((fs->delta > 0.0 && v->value.d < fs->end) ||
		(fs->delta < 0.0 && v->value.d > fs->end) ||
		(fs->delta == 0.0 && v->value.d != fs->end))
	{
		v->value.d += fs->delta;
		bc->vip = fs->start;
	} else
	{
		--bc->numfors;
		if(fs - bc->forstates < bc->numfors)
		{
			memmove(fs, fs+1, (bc->numfors - (fs-bc->forstates)) *
				sizeof(*fs));
		}
	}
}

void performnext(bc *bc)
{
	if(bc->numfors)
		lownext(bc, bc->forstates + bc->numfors - 1);
	else
		verror(bc, "NEXT without FOR");//ipfix= -1
}

void performnext1(bc *bc)
{
variable *v = bc->vvars + (--bc->vsp)->i;
forstate *fs;
	fs = getfor(bc, v);
	if(fs)
		lownext(bc, fs);
	else
		verror(bc, "NEXT without FOR, '%s'", v->name); // ipfix=-1
}

/**************************************************************************
          string functions
**************************************************************************/

void tabstr(bc *bc)
{
char temp[32];
	sprintf(temp, "\033%dx", (int)bc->vsp[-1].d);
	bc->vsp[-1].bs = make_bstring(bc, temp, strlen(temp));
}

void stringstr(bc *bc)
{
bstring *bs;
int len;
	len = bc->vsp[-2].d;
	if(len<0) len=0;
	bs = make_raw_bstring(bc, len);
	memset(bs->string, bc->vsp[-1].bs->string[0], len);
	free_bstring(bc, bc->vsp[-1].bs);
	--bc->vsp;
	bc->vsp[-1].bs = bs;
}


#define TLEN 0
#define TASC 1
#define TVAL 2
static void lenascval(bc *bc, int type)
{
bstring *bs;
	bs = bc->vsp[-1].bs;
	if(type == TLEN)
		bc->vsp[-1].d = bs->length;
	else if(type == TASC)
	{
		if(bs->length>0)
			bc->vsp[-1].d = bs->string[0] & 255;
		else
			bc->vsp[-1].d = 0.0;
	} else
	{
		bc->vsp[-1].d = 0.0;
		sscanf(bs->string, "%lf", &bc->vsp[-1].d);
	}
	free_bstring(bc, bs);
}

void lend(bc *bc){lenascval(bc, TLEN);}
void ascd(bc *bc){lenascval(bc, TASC);}
void vald(bc *bc){lenascval(bc, TVAL);}


#define SLEFT 0
#define SMID 1
#define SRIGHT 2
static void leftmidrightstr(bc *bc, int type)
{
bstring *s1=0, *s2;
int start=0, len=0;
int v1, v2;

	if(type == SMID)
	{
		s1 = bc->vsp[-3].bs;
		v1 = bc->vsp[-2].d;
		v2 = bc->vsp[-1].d;
		len = v2;
	} else
	{
		s1 = bc->vsp[-2].bs;
		v1 = bc->vsp[-1].d;
		len = v1;
	}

	if(len > s1->length)
		len = s1->length;
	if(len<0)
		len = 0;

	if(type==SLEFT)
			start = 0;
	else if(type==SRIGHT)
		start = s1->length - len;
	else
	{
		start = v1-1;
		if(start<0)
			start=0;
		if(start+len > s1->length)
			start = s1->length - len;
	}

	s2 = make_bstring(bc, s1->string + start, len);
	free_bstring(bc, s1);
	if(type == SMID)
		bc->vsp -= 2;
	else
		--bc->vsp;
	bc->vsp[-1].bs = s2;
}

void leftstr(bc *bc){leftmidrightstr(bc, SLEFT);}
void midstr(bc *bc){leftmidrightstr(bc, SMID);}
void rightstr(bc *bc){leftmidrightstr(bc, SRIGHT);}

#define TYPE_CHR 0
#define TYPE_STR 1
void chrstrstring(bc *bc, int type)
{
char t[128];
double d=bc->vsp[-1].d;
	if(type == TYPE_CHR)
	{
		t[0] = d;
		t[1]=0;
	} else
	{
		if(d != (int)d)
			snprintf(t, sizeof(t), "%.2lf", d);
		else
			snprintf(t, sizeof(t), "%d", (int)d);
	}
	bc->vsp[-1].bs = make_bstring(bc, t, strlen(t));
}

void chrstr(bc *bc){chrstrstring(bc,TYPE_CHR);}
void performstrstr(bc *bc){chrstrstring(bc, TYPE_STR);}




void arrays(bc *bc)
{
bstring *s;

	s = array(bc, sizeof(bstring **));
	if(s)
		(bc->vsp++)->p = s;
}

void assigns(bc *bc)
{
	if(*(bstring **)bc->vsp[-2].p)
		free_bstring(bc, *(bstring **)bc->vsp[-2].p);
	*(bstring **)bc->vsp[-2].p = bc->vsp[-1].bs;
	bc->vsp -= 2;
}

void pushvs(bc *bc)
{
	bc->vsp++ -> p = &bc->vvars[bc->vip++ -> i].value.s;
}

void evals(bc *bc)
{
bstring *bs;
	bs=*(bstring **)bc->vsp[-1].p;
	if(bs)
		bc->vsp[-1].bs = dup_bstring(bc, bs);
	else
		bc->vsp[-1].bs = make_bstring(bc, "", 0);
}

void pushs(bc *bc)
{
int len;
bstring *bs;
char *p;
int i;

	len = bc->vip->ustr[0];
	bs=make_raw_bstring(bc, len);
	i=1;
	p=bs->string;
	while(len--)
	{
		*p++ = bc->vip->str[i++];
		if(i==STEPSIZE)
		{
			i=0;
			++bc->vip;
		}
	}
	if(i)
		++bc->vip;
	(bc->vsp++)->bs = bs;
}

void prints(bc *bc)
{
bstring *bs;
	bs=(--bc->vsp)->bs;
	tprintf(bc, "%s", bs->string);
	free_bstring(bc, bs);
}

void adds(bc *bc)
{
bstring *left, *right, *new;
	left = bc->vsp[-2].bs;
	right = bc->vsp[-1].bs;
	new = make_raw_bstring(bc, left->length + right->length);
	memcpy(new->string, left->string, left->length);
	memcpy(new->string + left->length, right->string, right->length);
	free_bstring(bc, left);
	free_bstring(bc, right);
	bc->vsp[-2].bs = new;
	--bc->vsp;
}

static int bstring_comp(bc *bc)
{
int min,max;
int res;
bstring *left, *right;
	left = bc->vsp[-2].bs;
	right = bc->vsp[-1].bs;
	--bc->vsp;

	min=left->length;
	max=right->length;
	if(max<min)
		res=min,min=max,max=res;

	res=memcmp(left->string, right->string, min);
	if(!res)
	{
		if(min!=max)
			res = left->length < right->length ? -1 : 1;
	}
	free_bstring(bc, left);
	free_bstring(bc, right);

	return res;

}

void eqs(bc *bc)
{
int res;
	res = bstring_comp(bc);
	bc->vsp[-1].d = !res;
}

void nes(bc *bc)
{
int res;
	res = bstring_comp(bc);
	bc->vsp[-1].d = !!res;
}



/**************************************************************************
          BASIC high level commands, for the most part...
**************************************************************************/

void inkey(bc *bc)
{
int t;
char s[2];
	t=takedown(bc);
	if(t<0)
		(bc->vsp++)->bs = make_raw_bstring(bc, 0);
	else
	{
		s[0]=t;
		s[1]=0;
		(bc->vsp++)->bs = make_bstring(bc, s, 1);
	}
}

void keyd(bc *bc)
{
	bc->vsp[-1].d = checkpressed(bc, (int)bc->vsp[-1].d);
}

void keycoded(bc *bc)
{
	(bc->vsp++)->d = takedown(bc);
}

// each input has on the stack 2 steps:
// 1) the pointer to its value
// 2) the type (0 = double, 1 = string)
void input(bc *bc)
{
int n;
int i;
double *d;
bstring **bs;

	bc->flags |= BF_INPUT;
	n=(bc->vip++)->i;
	bc->vsp -= n*2;

	for(i=0;i<n;++i)
	{
		tprintf(bc, "? ");
		typeline(bc, "", 1);
		if(bc->vsp[i*2+1].i == 0) // double
		{
			d = (double *)bc->vsp[i*2].p;
			if(sscanf(bc->debline, "%lf", d) != 1)
				*d = 0.0;
		} else
		{
			bs = (bstring **)bc->vsp[i*2].p;
			if(*bs)
				free_bstring(bc, *bs);
			*bs=make_bstring(bc, bc->debline, strlen(bc->debline));
		}
	}
	bc->flags &= ~BF_INPUT;

	reset_waitbase(bc);

}

void printat(bc *bc)
{
int v=(--bc->vsp)->d;
	tprintf(bc, "\033%dx\033%dy", (v&63), (v>>6)&15);
}

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
int now;

	now = SDL_GetTicks();
	d=(--bc->vsp)->d;

	if(d<0) d = 0.0;
	if(d>10.0) d=10.0;

	until=bc->waitbase + d*1000;


	while(!needstop(bc))
	{
		diff = until - SDL_GetTicks();
		if(diff<0 || diff>10000)
		{
//printf("%s: diff out of range! %d\n", __FUNCTION__, diff);
			reset_waitbase(bc);
			break;
		}
		scaninput(bc);
		SDL_Delay(1);
	}
	(bc->vsp++) -> d = (SDL_GetTicks() - now)*.001;
}

/**************************************************************************
          sound functions
**************************************************************************/

static int vcheck(bc *bc, int v)
{
	if(v<0 || v>MAX_SOUNDS)
	{
		verror(bc, "Voice index '%d' out of range", v);
		return -1;
	}
	return 0;
}

void silence(bc *bc)
{
int v;
int i;
	v=(--bc->vsp)->d;
	if(vcheck(bc, v)) return;
	if(!v)
	{
		for(i=0;i<MAX_SOUNDS;++i)
			bc->sounds[i].flags &= ~SND_ACTIVE;
	} else
		bc->sounds[v-1].flags &= ~SND_ACTIVE;
}

void setsound(bc *bc)
{
int v;
	v=(--bc->vsp)->d;
	if(vcheck(bc, v)) return;
	{
		bc->csound = bc->sounds + v-1;
		bc->csound -> duration = 1000000.0; // forever!
	}
}

void freq(bc *bc)
{
	bc->csound->frequency = (--bc->vsp)->d;
}

void vol(bc *bc)
{
double v;
	v = (--bc->vsp)->d;
	if(v<0.0 || v>100.0)
	{
		verror(bc, "Volume must be between 0.0 and 100.0");
		return;
	}
	bc->csound->volume = v/100.0;
}

void dur(bc *bc)
{
double v;
	v = (--bc->vsp)->d;
	if(v<0.0)
	{
		verror(bc, "Duration cannot be negative");
		return;
	}
	bc->csound->duration = v;
}

void soundgo(bc *bc)
{
	bc->csound->time = 0.0;
	bc->csound->flags |= SND_ACTIVE;
}

void note(bc *bc)
{
	bc->vsp[-1].d = 261.625565 * pow(1.0594630943592953, bc->vsp[-1].d - 60.0);
}


/**************************************************************************
          graphics/rendering functions
**************************************************************************/

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
	verror(bc, "STOP"); // ipfix=-1
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

void ongoto(bc *bc)
{
int num;
int want;
	num = (bc->vip++)->i;
	bc->vsp -= num+1;
	want = bc->vsp->d;
	if(want<1 || want>num)
		verror(bc, "ON index out of range: %d, must be 1-%d", want, num);//-1
	else
		bc->vip = bc->vsp[want].p;
}

void ongosub(bc *bc)
{
	if(bc->gosubsp == GOSUBMAX)
		verror(bc, "Gosub stack overflow"); // -1
	else
		bc->gosubs[bc->gosubsp++] = bc->vip+1;
	ongoto(bc);
}

void datad(bc *bc)
{
	if(bc->datanum < MAXDATA)
		bc->data[bc->datanum++] = (bc->vip++)->d;
	else
	{
		verror(bc, "Too much data in program"); // ipfix=-1
		++bc->vip;
	}
}

void readd(bc *bc)
{
	--bc->vsp;
	if(bc->datapull < bc->datanum)
		*(double *)bc->vsp->p = bc->data[bc->datapull++];
	else
		verror(bc, "Ran out of data"); // ipfix=-1
}


void vmachine(bc *bc, step *program, step *stack)
{
	bc->vip = program;
	bc->vsp = stack;
	bc->base = program;
	bc->datanum = 0;
	bc->datapull = 0;
	for(;;)
	{
//printf("%d\n", bc->vip - bc->base);
		bc->lastip = bc->vip;
		(bc->vip++ -> func)(bc);
		if(bc->takeaction != globaltime)
		{
			bc->takeaction = globaltime;
			update(bc);
			scaninput(bc);
			if(needstop(bc))
				break;
		}
	}
	resetupdate(bc);
	if(bc->flags & BF_RUNNING)
	{
		tprintf(bc, "Elapsed time %.3f seconds.\n",
			(SDL_GetTicks()-bc->starttime)/1000.0);
		bc->flags &= ~BF_RUNNING;
	}
	if(bc->flags & BF_CCHIT)
	{
		tprintf(bc, "\nControl-C stopped on line %d\n", online(bc));
		bc->flags &= ~BF_CCHIT;
		flushinput(bc);
	}
	bc->flags &= ~(BF_RUNERROR | BF_ENDHIT);

}
