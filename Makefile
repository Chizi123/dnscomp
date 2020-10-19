CC=gcc
_OBJ=main.o dns.o servers.o
_DEPS=dns.h servers.h
IDIR=include
CFLAGS=-I$(IDIR) -Wall -g
ODIR=obj
SRCDIR=src

DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

default: a.out

%.o: %c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

a.out: $(_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean
clean:
	rm -rf *.o *~
