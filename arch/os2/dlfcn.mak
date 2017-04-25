CC= gcc
CFLAGS= -Zmtd -O2 -Wall
AR= ar rc

.SUFFIXES: .c .o .a

default: dlfcn.a

all: default ../../src/dlfcn.h ../../src/dlfcn.a

dlfcn.a: dlfcn.o
	$(AR) $@ $<

dlfcn.o: dlfcn.c

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

../../src/dlfcn.h: dlfcn.h
	cp $< $@

../../src/dlfcn.a: dlfcn.a
	cp $< $@
