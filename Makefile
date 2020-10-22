CC=gcc
_OBJ=main.o dns.o slist.o
_DEPS=dns.h servers.h slist.h
IDIR=include
CFLAGS=-Wall -g -lpthread
ODIR=obj
SRCDIR=src

DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

default: dnscomp

%.o: %c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

dnscomp: $(_OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -rf *.o *~
