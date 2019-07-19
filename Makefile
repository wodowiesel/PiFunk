# pifunk makefile
# should be run with sudo or root rights
USER=sudo
CC=gcc # use gnu c compiler
STD_CFLAGS=-Wall -std=c99 -g3 -ggdb -O3 -v -Iinclude -Llib -lsndfile -lm -shared -fPIC pifunk.c
LDFLAGS=-lpthread#-lgthread
#-std=gnu99 same as -std=iso9899:1999 as alternative

# Enable ARM-specific options only on ARM, and compilation of the app only on ARM
# Determine the hardware platform. Below, pi1 stands for the RaspberryPi 1 (the original one),
# and pi2 stands for both the RaspberryPi 2 and 3.

UNAME := $(shell uname -m) #linux

ifeq ($(UNAME), armv6l)
	CFLAGS = -march=armv6 -mtune=arm1176jzf-s -mfloat-abi=hard -mfpu=vfp -ffast-math -DRASPI0
	TARGET = pi0
else ifeq ($(UNAME), armv6l)
	CFLAGS = -march=armv6 -mtune=arm1176jzf-s -mfloat-abi=hard -mfpu=vfp -ffast-math -DRASPI1
	TARGET = pi1
else ifeq ($(UNAME), armv7l)
	CFLAGS = -march=armv7-a -mtune=arm1176jzf-s -mfloat-abi=hard -mfpu=vfp -ffast-math -DRASPI2
	TARGET = pi2
else ifeq ($(UNAME), armv8l)
	CFLAGS = -march=armv7-a -mtune=arm1176jzf-s -mfloat-abi=hard -mfpu=vfp -ffast-math -DRASPI3
	TARGET = pi3
else ifeq ($(UNAME), armv8l)
	CFLAGS = -march=armv8-a -mtune=cortex-a53 -mfloat-abi=hard -mfpu=vfp -ffast-math -DRASPI4
	TARGET = pi4
else ifeq ($(UNAME), armv7l)
	CFLAGS = -march=native -mtune=native -mfloat-abi=hard -mfpu=vfp -ffast-math -DRPI
	TARGET = rpi
else ifeq ($(UNAME), armv7l)
	CFLAGS = -march=native -mtune=native -mfloat-abi=hard -mfpu=vfp -ffast-math -DRASPBERRY
	TARGET = raspberry
else
	CFLAGS = -march=native -mtune=native -mfloat-abi=hard -mfpu=vfp -ffast-math -DRASPI1
	TARGET = other
endif

ifneq ($(TARGET), other)

#pifunk: pifunk.c
#					$(USER) $(CC)$(STD_CFLAGS) $(CFLAGS) $(LDFLAGS) -c -o bin/pifunk
endif

pifunk.i: pifunk.c
				$(USER) $(CC)$(STD_CFLAGS) $(CFLAGS) $(LDFLAGS) -E -C -o include/pifunk.i

pifunk.s: pifunk.c
					$(USER) $(CC)$(STD_CFLAGS) $(CFLAGS) $(LDFLAGS) -s -o lib/pifunk.s

pifunk.o: pifunk.c
					$(USER) $(CC)$(STD_CFLAGS) $(CFLAGS) $(LDFLAGS) -o lib/pifunk.o

pifunk.a: pifunk.c
					$(USER) $(CC)$(STD_CFLAGS) $(CFLAGS) $(LDFLAGS) -o lib/pifunk.a

pifunk.lib: pifunk.c
				  	$(USER) $(CC)$(STD_CFLAGS) $(CFLAGS) $(LDFLAGS) -o lib/pifunk.lib

pifunk.so: pifunk.c
					 $(USER) $(CC)$(STD_CFLAGS) $(CFLAGS) $(LDFLAGS) -o lib/pifunk.so

pifunk.out: pifunk.c
					  $(USER) $(CC)$(STD_CFLAGS) $(CFLAGS) $(LDFLAGS) -o bin/pifunk.out

pifunk.bin: pifunk.c
						$(USER) $(CC)$(STD_CFLAGS) $(CFLAGS) $(LDFLAGS)-o bin/pifunk.bin

pifunk: pifunk.c
				$(USER) $(CC)$(STD_CFLAGS) $(CFLAGS) $(LDFLAGS) -o bin/pifunk

install: $(USER) cd PiFunk

clean: $(USER) rm -f bin/pifunk.out
