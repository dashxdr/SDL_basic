#include <SDL.h>
#include "misc.h"

#define ENDMARK 0xaabacada


int sdlinout[]={
SDLK_0,'0',')',
SDLK_1,'1','!',
SDLK_2,'2','@',
SDLK_3,'3','#',
SDLK_4,'4','$',
SDLK_5,'5','%',
SDLK_6,'6','^',
SDLK_7,'7','&',
SDLK_8,'8','*',
SDLK_9,'9','(',
SDLK_a,'a','A',
SDLK_b,'b','B',
SDLK_c,'c','C',
SDLK_d,'d','D',
SDLK_e,'e','E',
SDLK_f,'f','F',
SDLK_g,'g','G',
SDLK_h,'h','H',
SDLK_i,'i','I',
SDLK_j,'j','J',
SDLK_k,'k','K',
SDLK_l,'l','L',
SDLK_m,'m','M',
SDLK_n,'n','N',
SDLK_o,'o','O',
SDLK_p,'p','P',
SDLK_q,'q','Q',
SDLK_r,'r','R',
SDLK_s,'s','S',
SDLK_t,'t','T',
SDLK_u,'u','U',
SDLK_v,'v','V',
SDLK_w,'w','W',
SDLK_x,'x','X',
SDLK_y,'y','Y',
SDLK_z,'z','Z',
SDLK_MINUS,'-','_',
SDLK_EQUALS,'=','+',
SDLK_LEFTBRACKET,'[','{',
SDLK_RIGHTBRACKET,']','}',
SDLK_SEMICOLON,';',':',
SDLK_QUOTE,'\'','"',
SDLK_BACKSLASH,'\\','|',
SDLK_SLASH,'/','?',
SDLK_PERIOD,'.','>',
SDLK_COMMA,',','<',
SDLK_BACKQUOTE,'`','~',
SDLK_BACKSPACE,8,8,
SDLK_TAB,9,9,
SDLK_DELETE,MYDELETE,MYDELETE,
SDLK_RETURN,13,13,
SDLK_F1,MYF1,MYF1,
SDLK_F2,MYF2,MYF2,
SDLK_F3,MYF3,MYF3,
SDLK_F4,MYF4,MYF4,
SDLK_F5,MYF5,MYF5,
SDLK_F6,MYF6,MYF6,
SDLK_F7,MYF7,MYF7,
SDLK_F8,MYF8,MYF8,
SDLK_F9,MYF9,MYF9,
SDLK_F10,MYF10,MYF10,
SDLK_ESCAPE,0x1b,0x1b,
SDLK_LEFT,MYLEFT,MYLEFT,
SDLK_RIGHT,MYRIGHT,MYRIGHT,
SDLK_UP,MYUP,MYUP,
SDLK_DOWN,MYDOWN,MYDOWN,
SDLK_PAGEUP,MYPAGEUP,MYPAGEUP|MYSHIFTED,
SDLK_PAGEDOWN,MYPAGEDOWN,MYPAGEDOWN|MYSHIFTED,
SDLK_SPACE,' ',' ',
SDLK_HOME,MYHOME,MYHOME,
SDLK_END,MYEND,MYEND,
SDLK_LALT,MYALTL,MYALTL,
SDLK_RALT,MYALTR,MYALTR,
SDLK_LCTRL,MYCTRLL,MYCTRLL,
SDLK_RCTRL,MYCTRLR,MYCTRLR,
SDLK_LSHIFT,MYSHIFTL,MYSHIFTL,
SDLK_RSHIFT,MYSHIFTR,MYSHIFTR,
ENDMARK
};

void mapkey(int code,int qual,int *mapped)
{
int *list;
	list=sdlinout;
	while(*list!=ENDMARK)
	{
		if(*list==code) break;
		list+=3;
	}
	if(*list==ENDMARK)
	{
		*mapped=mapped[1]=-1;
		return;
	}
	*mapped=(qual&KMOD_SHIFT) ? list[2] : list[1];
	if(qual & KMOD_ALT)
		*mapped|=MYALTED;
	if(qual & KMOD_CTRL)
		*mapped&=0x1f;
	mapped[1]=list[1];
}


void markkey(bc *bc, int code,int mod,int status)
{
int i;
int *ip;
int mapped[2];

	mapkey(code,mod,mapped);
	code=mapped[1];
	if(code<0) return;
	if(status)
	{
		if(bc->numdown<KEYMAX)
			bc->downcodes[bc->numdown++]=code;
		ip=bc->pressedcodes;
		i=bc->numpressed;
		while(i)
			if(*ip++==code) break;
			else i--;
		if(!i && bc->numpressed<KEYMAX)
			bc->pressedcodes[bc->numpressed++]=code;
//		if(*mapped==(MYALTED | 'x')) {writebbram();exit(0);}
		if(bc->keysin<KEYHISTSIZE)
		{
			bc->keyhist[bc->keyput++ & (KEYHISTSIZE-1)]=*mapped;
			++bc->keysin;
		}
	} else
	{
		i=bc->numpressed;
		ip=bc->pressedcodes;
		while(i)
			if(*ip++==code)
			{
				*--ip=bc->pressedcodes[--bc->numpressed];
				break;
			} else i--;
	}
}

int ignorable(int code)
{
	return code==MYALTL || code==MYALTR || 
		code==MYCTRLL || code==MYCTRLR ||
		code==MYSHIFTL || code==MYSHIFTR;
}

#define REPEATDELAY 200
#define REPEATRATE 20
int takedown(bc *bc)
{
//static int nexttime=0;
//static int lastcode=-1;
int time;
int i,j;

	if(bc->keysin)
	{
		--bc->keysin;
		bc->lastcode=bc->keyhist[bc->keytake++ & (KEYHISTSIZE-1)];
		bc->nexttime=SDL_GetTicks()+REPEATDELAY;
		return bc->lastcode;
	}
	time=SDL_GetTicks();
	j=0;
	for(i=0;i<bc->numpressed;++i)
		if(!ignorable(bc->pressedcodes[i]))
			++j;

	if(j)
	{
		if(time>=bc->nexttime)
		{
			bc->nexttime+=REPEATRATE;
			return bc->lastcode;
		}
	} else
		bc->lastcode=-1;
	return -1;
}

int checkdown(bc *bc, int code)
{
int *p,i;
	i=bc->numdown;
	p=bc->downcodes;
	while(i--)
		if(*p++==code) return 1;
	return 0;
}

int checkpressed(bc *bc, int code)
{
int *p,i;
	i=bc->numpressed;
	p=bc->pressedcodes;
	while(i--)
		if(*p++==code) return 1;
	return 0;
}

void updatef(bc *bc)
{
int new;

	if(!bc->tainted) return;
	new=SDL_GetTicks();
	if(new-bc->lastupdate<20) return;
	bc->lastupdate=new;
	bc->tainted=0;
	update(bc);
}


void scaninput(bc *bc)
{
SDL_Event event;
int key,mod;

	updatef(bc);
	bc->numdown=0;
	while(SDL_PollEvent(&event))
	{
		switch(event.type)
		{
		case SDL_KEYDOWN:
			key=event.key.keysym.sym;
			mod=event.key.keysym.mod;
			markkey(bc, key,mod,1);
			break;
		case SDL_KEYUP:
			key=event.key.keysym.sym;
			mod=event.key.keysym.mod;
			markkey(bc, key,mod,0);
			break;
/*
		case SDL_MOUSEBUTTONUP:
			bs&=~(1<<event.button.button-1);
			mousex=event.button.x>>1;
			mousey=event.button.y>>1;
			mouseb=bs;
			break;
		case SDL_MOUSEBUTTONDOWN:
			bs|=1<<event.button.button-1;
			mousex=event.button.x>>1;
			mousey=event.button.y>>1;
			mouseb=bs;
			break;
		case SDL_MOUSEMOTION:
			mousex=event.motion.x>>1;
			mousey=event.motion.y>>1;
			break;
*/
		}
	}
}

int showhistory(bc *bc, int back)
{
	return 0;
}

char *complete(char *base)
{
	return "";
}

void listall(char *base)
{
}

#define CR 13

void cr(bc *bc)
{
	termtext(bc, "\n");
}

void typeline(bc *bc, char *prompt,int echocr)
{
int i=0,j;
int code;
int backcount=0;
char linesave[LINESIZE],ch;
int linesin;
char ref;
int xdelta;
int plen;
char *p1,*p2;
char token[128];
char *fake;
int scrollback;

	plen=strlen(prompt);
	xdelta=0;
	linesin=bc->hcount>HISTSIZE ? HISTSIZE : bc->hcount;
	*bc->debline=0;
	termtext(bc, prompt);
	ref=0;
	i=0;
	fake=0;
	scrollback=0;
	for(;;)
	{
		if(!fake)
		{
			SDL_Delay(10);
			scaninput(bc);
			code=takedown(bc);
printf("code=%d\n", code);
		} else
		{
			code=*fake++;
			if(!code) {fake=0;continue;}
		}
		if(code==-1) continue;
		if(code==MYPAGEUP || code==(MYPAGEUP|MYSHIFTED))
		{
			if(code==MYPAGEUP) ++scrollback;
			else scrollback+=bc->fheight;
			scrollback=showhistory(bc, scrollback);
			continue;
		} else if(code==MYPAGEDOWN || code==(MYPAGEDOWN|MYSHIFTED))
		{
			if(code==MYPAGEDOWN) --scrollback;
			else scrollback-=bc->fheight;
			if(scrollback<0) scrollback=0;
			scrollback=showhistory(bc, scrollback);
			continue;
		}
		if(code==9 || code==4)
		{
			j=0;
			while(i+xdelta-j>0)
			{
				ch=bc->debline[i+xdelta-j-1];
				if(!isalpha(ch) && !isdigit(ch) && ch!='_' && ch!='.') break;
				++j;
			}
			p1=token;
			while(j)
				*p1++=bc->debline[i+xdelta-j--];
			*p1=0;
			if(*token)
			{
				if(code==9)
					fake=complete(token);
				else
				{
					cr(bc);
					listall(token);
					ref=1;
				}
			}
		} else if(code==0x7f)
		{
			if(!xdelta) continue;
			p1=bc->debline+i+xdelta;
			p2=p1+1;
			while((*p1++=*p2++));
			--i;
			++xdelta;
			++ref;
		} else if(code==MYLEFT)
		{
			if(i+xdelta>0) {--xdelta;++ref;}
		} else if(code==MYRIGHT)
		{
			if(xdelta<0) {++xdelta;++ref;}
		} else if(code==MYUP)
		{
			if(backcount>=linesin) continue;
			if(!backcount)
				memcpy(linesave,bc->debline,LINESIZE);
			++backcount;
			memcpy(bc->debline,
				bc->debhist+LINESIZE*((bc->hcount-backcount) & (HISTSIZE-1)),
				LINESIZE);
			xdelta=0;
			++ref;
		} else if(code==MYDOWN)
		{
			if(!backcount) continue;
			--backcount;
			if(!backcount) memcpy(bc->debline,linesave,LINESIZE);
			else
				memcpy(bc->debline,bc->debhist+LINESIZE*((bc->hcount-backcount) & (HISTSIZE-1)),LINESIZE);
			xdelta=0;
			++ref;
		} else if(code>=0 && code<128)
		{
			if(code==8)
			{
				if(i+xdelta)
				{
					--i;
					p1=bc->debline+i+xdelta;
					p2=p1+1;
					while((*p1++=*p2++));
					++ref;
				} else continue;
			} else if(code==CR)
			{
				if(echocr) cr(bc);
				break;
			}
			else if(code>=0x20 && i<sizeof(bc->debline)-1)
			{
				p2=bc->debline+i;
				p1=p2+1;
				j=1-xdelta;
				while(j--) *p1--=*p2--;
				*p1=code;
				++ref;
			} else continue;
		}
		if(ref)
		{
			if(scrollback)
				scrollback=showhistory(bc, 0);
			i=strlen(bc->debline);
			termtext(bc, "\r%s%s\033k\033%dx",prompt,bc->debline,plen+i+xdelta);
			ref=0;
		}

	}
	if(i)
	{
		memcpy(bc->debhist+LINESIZE*(bc->hcount & (HISTSIZE-1)),bc->debline,LINESIZE);
		++bc->hcount;
	}
}
