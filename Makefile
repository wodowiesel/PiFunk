# pifunk makefile
# should be run with sudo or root rights
USER=sudo
CC=gcc # use gnu c compiler
STD_CFLAGS=-Wall -std=c99 -g3 -O3 -v -Iinclude -Llib -lsndfile -lm -shared -fPIC pifunk.c
#-std=gnu99 same as -std=iso9899:1999 as alternative

# Enable ARM-specific options only on ARM, and compilation of the app only on ARM
# Determine the hardware platform. Below, pi1 stands for the RaspberryPi 1 (the original one),
# and pi2 stands for both the RaspberryPi 2 and 3.

UNAME := $(shell uname -m) #linux

ifeq ($(UNAME), armv6l) #my version
	CFLAGS = $(STD_CFLAGS) -march=armv6 -mtune=arm1176jzf-s -mfloat-abi=hard -mfpu=vfp -ffast-math -DRASPI1
	TARGET = pi1
else ifeq ($(UNAME), armv7l)
	CFLAGS = $(STD_CFLAGS) -march=armv7-a -mtune=arm1176jzf-s -mfloat-abi=hard -mfpu=vfp -ffast-math -DRASPI2
	TARGET = pi2
else ifeq ($(UNAME), armv8l)
		CFLAGS = $(STD_CFLAGS) -march=armv7-a -mtune=arm1176jzf-s -mfloat-abi=hard -mfpu=vfp -ffast-math -DRASPI3
		TARGET = pi3
else ifeq ($(UNAME), armv8l)
			CFLAGS = $(STD_CFLAGS) -march=armv7-a -mtune=arm1176jzf-s -mfloat-abi=hard -mfpu=vfp -ffast-math -DRASPI4
			TARGET = pi4
else
	CFLAGS = $(STD_CFLAGS) -c -march=armv7-a -mtune=arm1176jzf-s -mfloat-abi=hard -mfpu=vfp -ffast-math
	TARGET = other
endif

ifneq ($(TARGET), other)

app: pifunk.c
					$(USER)$(CC)$(CFLAGS)-o bin/pifunk
endif

pifunk.i: pifunk.c
					$(CC)$(CFLAGS)-o include/pifunk.i

pifunk.s: pifunk.c
					$(CC)$(CFLAGS)-o lib/pifunk.s

pifunk.o: pifunk.c
					$(CC)$(CFLAGS)-o lib/pifunk.o

pifunk.a: pifunk.c
					$(CC)$(CFLAGS)-o lib/pifunk.a

pifunk.lib: pifunk.c
					$(CC)$(CFLAGS)-o lib/pifunk.lib

pifunk.so: pifunk.c
					$(CC)$(CFLAGS)-o lib/pifunk.so

pifunk.out: pifunk.c
					$(CC)$(CFLAGS)-o bin/pifunk.out

pifunk: pifunk.c
					$(USER)$(CC)$(CFLAGS)-o bin/pifunk

install:

clean: rm -f bin/*.o
