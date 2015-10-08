#DBG += -g
#DBG += -pg
CC	= gcc
MAXE = -fmax-errors=8
CFLAGS = -O2 -Wall $(shell sdl2-config --cflags) $(DBG) $(MAXE)
LDFLAGS = $(DBG)
LDLIBS = $(shell sdl2-config --libs) -lm -lSDL2_image

# ftgrays standalone
CFLAGS += -D_STANDALONE_


all: basic
basic: main.o basic.o font.o render.o keyboard.o ftgrays.o parse.o \
	vmachine.o sound.o vdis.o help.o

main.o: main.c misc.h
basic.o: basic.c misc.h
font.o: font.c misc.h
render.o: render.c misc.h
keyboard.o: keyboard.c misc.h
parse.o: parse.c
parse.c: grammar.y misc.h
	bison -o $@ $<
vmachine.o: vmachine.c misc.h
sound.o: sound.c misc.h
vdis.o: vdis.c misc.h
help.o: help.c misc.h

ftgrays.o: ftgrays.c ftgrays.h ftimage.h


clean:
	rm -f *.o basic parse.c

test:	all
	./basic

WORK = /ram
VER = 1.0.4
DDIR = SDL_basic-$(VER)

package:  clean
	rm -rf $(WORK)/$(DDIR)
	mkdir $(WORK)/$(DDIR)
	cp *.c *.h *.y Makefile* README COPYING AUTHORS TODO $(WORK)/$(DDIR)
	cp cool*.bas cubic.bas startrek1.bas wumpus.bas $(WORK)/$(DDIR)
	cp sprite.png $(WORK)/$(DDIR)
	cp INSTALL ChangeLog $(WORK)/$(DDIR)
	cd $(WORK) && tar czf $(DDIR).tgz $(DDIR)
