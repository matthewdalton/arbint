CC=gcc
CFLAGS=-g #-pg
#LDFLAGS=-pg

all: build test

build: arbint-test tags

arbint: AI_mem.o AI_new.o

arbint-test: arbint-test.o arbint-unittest.o AI_mem.o AI_new.o AI_maths.o AI_str.o AI_pow.o AI_iterator.o

clean:
	rm *.o arbint-test

tags:
	etags *.[ch]

test: build
	./arbint-test