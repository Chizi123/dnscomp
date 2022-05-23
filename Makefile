CC=gcc
_OBJ=main.o dns.o slist.o
_DEPS=dns.h servers.h slist.h
CFLAGS=-Wall -lpthread

ifeq ($(DEV),1)
	CFLAGS += -g
else
	CFLAGS += -O2 -flto
endif

default: dnscomp

%.o: %c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

dnscomp: $(_OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -rf *.o *~
