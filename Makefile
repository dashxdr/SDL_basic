DBG = # -g
CC	= gcc
CFLAGS = -O2 -Wall $(shell sdl-config --cflags) $(DBG)
LDFLAGS = $(shell sdl-config --libs) $(DBG)

all: basic
basic: main.o basic.o font.o render.o keyboard.o

main.o: main.c misc.h
basic.o: basic.c misc.h
font.o: font.c misc.h
render.o: render.c misc.h
keyboard.o: keyboard.c

clean:
	rm -f *.o basic
test:	all
	./basic
