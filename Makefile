CC=gcc
OBJ=main.o dns.o
DEPS=dns.h
CFLAGS=-g

default: all

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

all: $(OBJ)
	$(CC) $(CFLAGS) $^
