/*
   SDL_basic written by David Ashley, released 20080621 under the GPL
   http://www.linuxmotors.com/SDL_basic
   dashxdr@gmail.com
*/
#include <string.h>
#include <ctype.h>

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
SDLK_KP_ENTER,13,13,
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
		if(*mapped==3 || *mapped == 0x1b) bc->flags |= BF_CCHIT;
		if(bc->numdown<KEYMAX)
			bc->downcodes[bc->numdown++]=code;
		ip=bc->pressedcodes;
		i=bc->numpressed;
		while(i)
			if(*ip++==code) break;
			else i--;
		if(!i && bc->numpressed<KEYMAX)
			bc->pressedcodes[bc->numpressed++]=code;
		if(*mapped==(MYALTED | 'q')) bc->flags |= BF_QUIT; // alt-q exits
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
			if((bc->flags & BF_INPUT) || (~bc->flags & BF_RUNNING))
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

void flushinput(bc *bc)
{
	while(takedown(bc)>=0);
}

void fakekey(bc *bc, int key, int mod)
{
	markkey(bc, key, mod, 1);
	markkey(bc, key, mod, 0);
}

void process_wheel(bc *bc, int delta)
{
	if(!delta) return;
	int key;
	if(delta<0)
	{
		key = SDLK_PAGEDOWN;
		delta = -delta;
	} else
		key = SDLK_PAGEUP;
	delta *= 3;
	while(delta-- > 0)
		fakekey(bc, key, 0);
}

void scaninput(bc *bc)
{

	update(bc);
	bc->numdown=0;
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		int mod, key;
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
		case SDL_MOUSEBUTTONUP:
			bc->mouseb&=~(1<<(event.button.button-1));
			bc->mousex=event.button.x;
			bc->mousey=event.button.y;
			break;
		case SDL_MOUSEBUTTONDOWN:
			bc->mouseb|=1<<(event.button.button-1);
			bc->mousex=event.button.x;
			bc->mousey=event.button.y;
			break;
		case SDL_MOUSEWHEEL:
			process_wheel(bc, event.wheel.y);
			break;
		case SDL_MOUSEMOTION:
			bc->mousex=event.motion.x;
			bc->mousey=event.motion.y;
			break;
		case SDL_QUIT:
			bc->flags |= BF_QUIT;
			break;
		}
	}
}

int showhistory(bc *bc, int back)
{
char *p;
int i,j,k;

	if(back>bc->scrollhistoryin) back=bc->scrollhistoryin;
	memcpy(bc->textbak,bc->textstate,bc->textsize);
	for(j=0;j<bc->tysize;++j)
	{
		k=bc->scrollhistoryin-back+j;
		if(k<bc->scrollhistoryin)
			p=bc->scrollhistory+bc->txsize*(k&(SCROLLHISTORYSIZE-1));
		else
			p=bc->textbak+bc->txsize*(k-bc->scrollhistoryin);
		for(i=0;i<bc->txsize;++i)
			drawcharxy(bc, i,j,*p++);
	}

	update(bc);
	memcpy(bc->textstate, bc->textbak, bc->textsize);
	return back;

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
	tprintf(bc, "\n");
}

void typeline(bc *bc, char *preload,int echocr)
{
int i=0,j;
int code;
int backcount=0;
char linesave[LINESIZE],ch;
int linesin;
char ref;
int xdelta;
char *p1,*p2;
char token[128];
char *fake;
int scrollback;
int startx, starty;
char spaces[256], *zerospaces;
char *tail;
int oldlen;

	memset(spaces, ' ', sizeof(spaces));
	zerospaces = spaces + sizeof(spaces)-1;
	*zerospaces = 0;
top:
	tail="";
	startx = bc->txpos;
	starty = bc->typos;
	xdelta=0;
	linesin=bc->hcount>HISTSIZE ? HISTSIZE : bc->hcount;
	strcpy(bc->debline, preload);
	i=strlen(bc->debline);
	tprintf(bc, preload);
	ref=0;
	fake=0;
	scrollback=0;
	while(!(bc->flags & (BF_CCHIT | BF_QUIT)))
	{
		if(!fake)
		{
			SDL_Delay(10);
			scaninput(bc);
			code=takedown(bc);
//if(code=='q' + MYALTED) exit(0); // alt-q
		} else
		{
			code=*fake++;
			if(!code) {fake=0;continue;}
		}
		if(code==-1) continue;
		if(code==MYPAGEUP || code==(MYPAGEUP|MYSHIFTED))
		{
			if(code==MYPAGEUP) ++scrollback;
			else scrollback+=bc->tysize;
			scrollback=showhistory(bc, scrollback);
			continue;
		} else if(code==MYPAGEDOWN || code==(MYPAGEDOWN|MYSHIFTED))
		{
			if(code==MYPAGEDOWN) --scrollback;
			else scrollback-=bc->tysize;
			if(scrollback<0) scrollback=0;
			scrollback=showhistory(bc, scrollback);
			if(scrollback)
				continue;
			else
				++ref;
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
			if(backcount<linesin)
			{
				if(!backcount)
					memcpy(linesave,bc->debline,LINESIZE);
				++backcount;
				oldlen = strlen(bc->debline);
				memcpy(bc->debline,
					bc->debhist+LINESIZE*((bc->hcount-backcount) &
						(HISTSIZE-1)), LINESIZE);
				oldlen -= strlen(bc->debline);
				if(oldlen>0) tail=zerospaces-oldlen;
				xdelta=0;
			}
			++ref;
		} else if(code==MYDOWN)
		{
			if(backcount)
			{
				--backcount;
				oldlen = strlen(bc->debline);
				if(!backcount) memcpy(bc->debline,linesave,LINESIZE);
				else
					memcpy(bc->debline,bc->debhist+LINESIZE*((bc->hcount-backcount) & (HISTSIZE-1)),LINESIZE);
				oldlen -= strlen(bc->debline);
				if(oldlen>0) tail=zerospaces-oldlen;
				xdelta=0;
			}
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
			else if(code>=0x20 && i<bc->txsize-1 && i<sizeof(bc->debline)-1)
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
			tprintf(bc, "\033%dx\033%dy%s %s\033%dx\033%dy",
				startx, starty, bc->debline, tail, i+xdelta+startx, starty);
			tail="";
			ref=0;
		}

	}
	if(bc->flags & BF_CCHIT)
	{
		flushinput(bc);
		if(!(bc->flags & BF_RUNNING))
		{
			bc->flags &= ~BF_CCHIT;
			preload="";
			tprintf(bc, "\n");
			goto top;
		}
	}
	if(i)
	{
		memcpy(bc->debhist+LINESIZE*(bc->hcount & (HISTSIZE-1)),bc->debline,LINESIZE);
		++bc->hcount;
	}
}
