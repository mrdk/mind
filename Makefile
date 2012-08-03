CC=gcc
CFLAGS=-Wall -std=gnu99 -O3 -fno-strict-aliasing -fno-gcse

all: mind

mind.o: heads.c types.h

mind.s: mind.c heads.c types.h
	$(CC) -S $(CFLAGS) -fverbose-asm $<

test: mind
	@./mind -e '.( Finished.) cr'

TAGS:
	etags *.c *.h start.mind

clean: docclean srclean

srcclean:
	rm -f mind mind.o

docclean:
	make -C doc/ clean

html latexpdf linkcheck:
	make -C doc/ $@

.PHONY: mind.s srcclean docclean html latexpdf linkcheck TAGS
