DBG = # -g
CC	= gcc
CFLAGS = -O2 -Wall $(shell sdl-config --cflags) $(DBG)
LDFLAGS = $(shell sdl-config --libs) $(DBG) -lm

# ftgrays standalone
CFLAGS += -D_STANDALONE_


all: basic
basic: main.o basic.o font.o render.o keyboard.o ftgrays.o parse.o \
	vmachine.o sound.o vdis.o

main.o: main.c misc.h
basic.o: basic.c misc.h
font.o: font.c misc.h
render.o: render.c misc.h
keyboard.o: keyboard.c misc.h
parse.o: parse.c
parse.c: grammar.y misc.h
	bison -o $@ $< -v
vmachine.o: vmachine.c misc.h
sound.o: sound.c misc.h
vdis.o: vdis.c misc.h

ftgrays.o: ftgrays.c ftgrays.h ftimage.h


clean:
	rm -f *.o basic
test:	all
	./basic
