#include "misc.h"
#include <math.h>

#define SAMPLING_RATE 48000
#define FRAGSIZE 1024
#define PIECES 16

#define FFIX (4294967296.0/SAMPLING_RATE)
#define TIME_PER_PIECE ((double)FRAGSIZE / (SAMPLING_RATE*PIECES))

void fillaudio(void *data, Uint8 *buffer, int len)
{
bc *bc=data;
int i,j, sp;
Sint16 *p;
sound *s;
int accum[FRAGSIZE*2/PIECES], *ap;
Uint32 v, dv;
int vol;

	p = (void *)buffer;
	len>>=2; // stereo, 16 bit signed samples = 4 bytes/sample

	len /= PIECES;
	for(j=0;j<PIECES;++j)
	{
		memset(accum, 0, sizeof(accum));
		for(sp = 0;sp < MAX_SOUNDS;++sp)
		{
			s=bc->sounds + sp;
			if((s->flags & (SND_ACTIVE | SND_QUIET)) &&
				bc->soundtime >= s->start)
			{
				bc->isounds[sp] = *s;
				if(bc->isounds[sp].flags & SND_QUIET)
					bc->isounds[sp].flags &=~SND_ACTIVE;
				bc->isounds[sp].flags &= ~SND_QUIET;
				s->flags &= ~(SND_ACTIVE | SND_QUIET);
			}
			s = bc->isounds + sp;
			if(~s->flags & SND_ACTIVE)
				continue;
			ap = accum;
			v = s->index;
			dv = FFIX * s->frequency;
			vol = s->volume * 16384;
			if(sp < MAX_SOUNDS-4) // last 4 are noise
			{
				for(i=0;i<len;++i)
				{
					v += dv;
					ap[i*2+0] += (s->wave[(v>>19) & 0x1fff] * vol) >> 17;
					ap[i*2+1] += (s->wave[(v>>19) & 0x1fff] * vol) >> 17;
				}
			} else
			{
				int t = ~0, t2, t3=0;
				for(i=0;i<len;++i)
				{
					v += dv;
					t2 = v>>31;
					if(t != t2)
					{
						t = t2;
						t3 = (rand() & 0x1fff) - 4096;
					}
					ap[i*2+0] += t3*vol >> 14;
					ap[i*2+1] += t3*vol >> 14;
				}
			}
			s->index = v;
			s->time += TIME_PER_PIECE;
			if(s->time >= s->duration)
				s->flags &= ~SND_ACTIVE;
			s->frequency *= s->fmul;
			if(s->frequency > SAMPLING_RATE/2 || s->frequency < 4)
				s->flags &= ~SND_ACTIVE;
		}
		ap = accum;
		for(i=0;i<len;++i)
		{
			*p++ = *ap++;
			*p++ = *ap++;
		}
		bc->soundtime += TIME_PER_PIECE;
	}
}

void soundopen(bc *bc)
{
SDL_AudioSpec wanted;
int i;
sound *s;

	for(i=0;i<MAX_SOUNDS;++i)
	{
		s=bc->sounds + i;
		s->flags = 0;
		s->frequency = 440.0;
		s->volume = 1.0;
		s->duration = 0.0;
		s->fmul = 1.0;
		s->time = 0.0;
	}

	for(i=0;i<8192;++i)
	{
		bc->wsqr[i] = (i<4096) ? -0x8000 : 0x7fff;
		bc->wsin[i] = 0x7fff*sin(i*3.1415927/4096.0);
		bc->wtri[i] = (i<4096) ? -0x8000 + i*8 : 0xffff - i*8;
		bc->wsaw[i] = -0x8000 + i*8;
	}

	memset(&wanted,0,sizeof(wanted));
	wanted.freq=SAMPLING_RATE;
	wanted.channels=2;
	wanted.format=AUDIO_S16;
	wanted.samples=FRAGSIZE;
	wanted.callback=fillaudio;
	wanted.userdata=bc;

	if(SDL_OpenAudio(&wanted,0)<0)
	{
		fprintf(stderr,"Couldn't open audio: %s\n",SDL_GetError());
		return;
	}
	bc->soundworking = 1;

	SDL_PauseAudio(0);
}
