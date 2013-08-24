CC=gcc
CFLAGS=-W -Wall -std=gnu99 -O3 -fno-strict-aliasing -fno-gcse

all: mind
clean: docclean srcclean

## Executable

mind.o: mind.c heads.c types.h

test: mind
	@./mind -e '.( Finished.) cr'

TAGS:
	etags *.c *.h

%.s: %.c
	$(CC) -S $(CFLAGS) -fverbose-asm $<

srcclean:
	rm -f mind *.o *.s

.PHONY: srcclean TAGS


## Documentation

html latexpdf linkcheck:
	make -C doc/ $@

docclean:
	make -C doc/ clean

.PHONY: docclean html latexpdf linkcheck
