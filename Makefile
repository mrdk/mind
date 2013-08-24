all: mind
clean: docclean srcclean

## Program

CC=gcc
CFLAGS=-MMD -W -Wall -std=gnu99 -O3 -fno-strict-aliasing -fno-gcse

mind: mind.o io.o

-include *.d

test: mind
	@./mind -e '.( Finished.) cr'

TAGS:
	etags *.c *.h

%.s: %.c
	$(CC) -S $(CFLAGS) -fverbose-asm $<

srcclean:
	rm -f mind *.o *.d *.s

.PHONY: srcclean TAGS


## Documentation

html latexpdf linkcheck:
	make -C doc/ $@

docclean:
	make -C doc/ clean

.PHONY: docclean html latexpdf linkcheck
