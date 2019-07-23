# pifunk makefile
# should run with sudo or root rights
USER=sudo
CC=gcc # use gnu c compiler
STD_CFLAGS=-Wall -std=c99 -g3 -ggdb -O3 -v -Iinclude -Llib -lsndfile -lm -shared -fPIC pifunk.c
LDFLAGS=-lpthread #-lgthread
#-std=gnu99 same as -std=iso9899:1999 alternative
PATH=/usr/local

# Determine the hardware platform.
# Enable ARM-specific options only on ARM, and compilation of the app only on ARM


UNAME := $(shell uname -m) #linux

ifeq ($(UNAME), armv6l)
	CFLAGS = -march=armv6 -mtune=arm1176jzf-s -mfloat-abi=hard -mfpu=vfp -ffast-math -DRASPI0
	TARGET = pi0
ifeq ($(UNAME), armv6l)
	CFLAGS = -march=armv6 -mtune=arm1176jzf-s -mfloat-abi=hard -mfpu=vfp -ffast-math -DRASPI1
	TARGET = pi1
ifeq ($(UNAME), armv7l)
	CFLAGS = -march=armv7-a -mtune=arm1176jzf-s -mfloat-abi=hard -mfpu=vfp -ffast-math -DRASPI2
	TARGET = pi2
ifeq ($(UNAME), armv8l)
	CFLAGS = -march=armv7-a -mtune=arm1176jzf-s -mfloat-abi=hard -mfpu=vfp -ffast-math -DRASPI3
	TARGET = pi3
ifeq ($(UNAME), armv8l)
	CFLAGS = -march=armv8-a -mtune=cortex-a53 -mfloat-abi=hard -mfpu=vfp -ffast-math -DRASPI4
	TARGET = pi4
ifeq ($(UNAME), armv7l)
	CFLAGS = -march=native -mtune=native -mfloat-abi=hard -mfpu=vfp -ffast-math -DRPI
	TARGET = rpi
ifeq ($(UNAME), armv7l)
	CFLAGS = -march=native -mtune=native -mfloat-abi=hard -mfpu=vfp -ffast-math -DRASPBERRY
	TARGET = raspberry
else
	CFLAGS = -march=native -mtune=native -mfloat-abi=hard -mfpu=vfp -ffast-math -DRASPI
	TARGET = other
endif

ifneq ($(TARGET), other)

pifunk: pifunk.c
					$(USER) $(CC)$(STD_CFLAGS) $(CFLAGS) $(LDFLAGS) -c -o bin/pifunk
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
						$(USER) $(CC)$(STD_CFLAGS) $(CFLAGS) $(LDFLAGS) -o bin/pifunk.bin

pifunk: 		pifunk.c
						$(USER) $(CC)$(STD_CFLAGS) $(CFLAGS) $(LDFLAGS) -o bin/pifunk

install: $(USER) cd $(PATH)/PiFunk
					install -m 0755 pifunk $(PATH)/bin
					install -m 0755 pifunk.bin $(PATH)/bin

uninstall: $(RM) $(PATH)/bin/pifunk $(PATH)/bin/pifunk.bin

clean: $(USER) rm -f bin/pifunk.out lib/pifunk.o
