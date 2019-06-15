# pifunk makefile
# should be run with sudo or root rights
CC = gcc # use gnu compiler
STD_CFLAGS =-g -Wall -std=c99 -Iinclude -Llib -lsndfile -shared -lm -fPIC pifunk.c #-std=gnu99 as alternative

# Enable ARM-specific options only on ARM, and compilation of the app only on ARM
# Determine the hardware platform. Below, pi1 stands for the RaspberryPi 1 (the original one),
# and pi2 stands for both the RaspberryPi 2 and 3.

UNAME := $(shell uname -m) #linux

ifeq ($(UNAME), armv6l) #my version
	CFLAGS = $(STD_CFLAGS) -march=armv6 -mtune=arm1176jzf-s -mfloat-abi=hard -mfpu=vfp -ffast-math -DRASPI=1
	TARGET = pi1

else ifeq ($(UNAME), armv7l)
	CFLAGS = $(STD_CFLAGS) -march=armv7-a -mtune=arm1176jzf-s -mfloat-abi=hard -mfpu=vfp -ffast-math -DRASPI=2
	TARGET = pi2

else
	CFLAGS = $(STD_CFLAGS)
	TARGET = other
endif

ifneq ($(TARGET), other)

app: pifunk.c
					$(CC)$(CFLAGS)-c -o bin/pifunk
endif

pifunk.i: pifunk.c
					$(CC) $(CFLAGS) -o include/pifunk.i

pifunk.o: pifunk.c
					$(CC)$(CFLAGS)-o lib/pifunk.o

pifunk.s: pifunk.c
					$(CC)$(CFLAGS)-o lib/pifunk.s

pifunk.so: pifunk.c
					$(CC)$(CFLAGS)-o lib/pifunk.so

pifunk.lib: pifunk.c
					$(CC)$(CFLAGS)-o lib/pifunk.lib

pifunk.a: pifunk.c
					$(CC)$(CFLAGS)-o lib/pifunk.a

pifunk.out: pifunk.c
					$(CC)$(CFLAGS)-o bin/pifunk.out
					
pifunk: pifunk.c
					$(CC)$(CFLAGS)-o bin/pifunk
install:

clean: rm -f *.o
