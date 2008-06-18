#include "misc.h"
#include <math.h>

#define SAMPLING_RATE 48000
#define FRAGSIZE 2048
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
Uint32 v, dv, vol;

	p = (void *)buffer;
	len>>=2; // stereo, 16 bit signed samples = 4 bytes/sample

	len /= PIECES;
	for(j=0;j<PIECES;++j)
	{
		memset(accum, 0, sizeof(accum));
		for(sp = 0;sp < MAX_SOUNDS;++sp)
		{
			s=bc->sounds + sp;
			if(~s->flags & SND_ACTIVE)
				continue;
			ap = accum;
			v = s->index;
			dv = FFIX * s->frequency;
			vol = s->volume * 16384;
			for(i=0;i<len;++i)
			{
				v += dv;
				ap[i*2+0] += ((((v>>19) & 0x1fff) - 4096) * vol) >> 14;
				ap[i*2+1] += ((((v>>19) & 0x1fff) - 4096) * vol) >> 14;
			}
			s->index = v;
			s->time += TIME_PER_PIECE;
			if(s->time >= s->duration)
				s->flags &= ~SND_ACTIVE;
//			s->frequency *= 1.0001;
		}
		ap = accum;
		for(i=0;i<len;++i)
		{
			*p++ = *ap++;
			*p++ = *ap++;
		}
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
		s->time = 0.0;
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
#if 0
	bc->sounds[0].flags = SND_ACTIVE;
	bc->sounds[0].frequency = 110.0;// * pow(2.0, 5.0/12.0);;
	bc->sounds[1].flags = SND_ACTIVE;
	bc->sounds[1].frequency = bc->sounds[0].frequency * pow(2.0, 4.0/12.0);
	bc->sounds[2].flags = SND_ACTIVE;
	bc->sounds[2].frequency = bc->sounds[0].frequency * pow(2.0, 7.0/12.0);
	bc->sounds[3].flags = SND_ACTIVE;
	bc->sounds[3].frequency = bc->sounds[0].frequency * pow(2.0, 12.0/12.0);
#endif

	SDL_PauseAudio(0);
}
