CFLAGS=-Wall -std=gnu99 -O3 -fno-strict-aliasing -fno-gcse

all: mind

mind.o: heads.c types.h

mind.s: mind.c heads.c
	$(CC) -S $(CFLAGS) $<

test: mind
	@./mind

stats:
	@wc *.mind *.c Makefile *.org

clean:
	rm -f mind mind.o

TAGS:
	etags *.c *.h start.mind

.PHONY: mind.s clean TAGS
