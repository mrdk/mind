CC=gcc
CFLAGS=-Wall -std=gnu99 -O3 -fno-strict-aliasing -fno-gcse

all: mind

mind.o: heads.c types.h

mind.s: mind.c heads.c types.h
	$(CC) -S $(CFLAGS) -fverbose-asm $<

test: mind
	@./mind -e '.( Finished.) cr'

stats:
	@wc *.mind *.c Makefile *.org

clean:
	rm -f mind mind.o

TAGS:
	etags *.c *.h start.mind

.PHONY: mind.s clean TAGS
