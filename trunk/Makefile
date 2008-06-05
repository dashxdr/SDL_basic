CC	= gcc
CFLAGS = -O2 -Wall $(shell sdl-config --cflags)
LDFLAGS = $(shell sdl-config --libs)

all: basic
basic: basic.c
clean:
	rm -f *.o basic
test:	all
	./basic
