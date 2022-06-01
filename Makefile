CC=gcc
_OBJ=dns.o slist.o
_DEPS=dns.h servers.h slist.h
CFLAGS=-Wall -lpthread

ifeq ($(DEV),1)
	CFLAGS += -g
else
	#CFLAGS += -O2 -flto
endif

default: dnscomp

%.o: %c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

dnscomp: $(_OBJ) main.o
	$(CC) -o $@ $^ $(CFLAGS)

test: $(_OBJ) test.o
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -rf *.o *~
