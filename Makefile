CFLAGS=-Wall -std=gnu99 -O3 -fno-strict-aliasing -fno-gcse

all: mind

mind.o: heads.c

mind.s: mind.c heads.c
	$(CC) -S $(CFLAGS) $<

test: mind
	@./mind

stats:
	@wc *.mind *.c Makefile *.txt

TAGS:
	etags *.c start.mind

.PHONY: mind.s TAGS
