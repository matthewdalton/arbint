CC=gcc
CFLAGS=-g

all: arbint-test tags test

arbint: AI_mem.o AI_new.o

arbint-test: arbint-test.o arbint-unittest.o AI_mem.o AI_new.o AI_maths.o AI_str.o

clean:
	rm *.o arbint-test

tags:
	etags *.[ch]

test:
	./arbint-test