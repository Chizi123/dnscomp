CC=gcc
_OBJ=main.o dns.o
_DEPS=dns.h
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
