## pifunk Makefile
## should run with sudo or root rights
USER=sudo
$(USER)
CC=gcc
$(CC)
CPP=gcc -E ## c-preproccessor
$(CPP)
CCN=gcc-9.2.0 ## newest version
$(CCN)
MAKEINFO=pifunk
$(MAKEINFO)
VERSION=0.1.7.8
$(VERSION)
STATUS=experimental
$(STATUS)

## use gnu c compiler, -std=gnu99 is c99 -std=iso9899:1999 with extra gnu extentions
CFLAGS=-std=gnu99 -Iinclude -I/opt/vc/include/ -D_USE_MATH_DEFINES -D_GNU_SOURCE -fPIC pifunk.c -O3
DEBUG=-Wall -Werror --print-directory -pedantic-errors -d -v -g3 -ggdb3
ASFLAGS=-s

LDFLAGS=-lgnu -lm -lpthread -lbcm_host -lsndfile -lpifunk -shared
LDLIBS=-Llib -L/opt/vc/lib/

## default paths
SHELL=/bin/sh/
$(SHELL)
HOME=/home/pi ## std-path
$(HOME)
RM=rm -f ## remove files or folder

## Determine the hardware platform
UNAME:=$(shell uname -m) ## linux
$(UNAME)
PCPUI:=$(shell cat /proc/cpuinfo | grep Revision | cut -c16-) ## my rev: 0010 -> 1.2 B+
$(PCPUI)
RPI_VERSION:=$(shell cat /proc/device-tree/model | grep -a -o "Raspberry\sPi\s[0-9]" | grep -o "[0-9]") ## grab revision
#$(RPI_VERSION)

## Enable ARM-specific options only
ifeq ($(UNAME), armv5l)
PFLAGS=-march=armv6 -mtune=arm1176jzf-s -mfloat-abi=hard -mfpu=vfp -ffast-math -DRASPI=0
TARGET=RASPI0
endif

ifeq ($(UNAME), armv6l)
PFLAGS=-march=armv6 -mtune=arm1176jzf-s -mfloat-abi=hard -mfpu=vfp -ffast-math -DRASPI=1
TARGET=RASPI1
endif

ifeq ($(UNAME), armv7l)
PFLAGS=-march=armv7-a -mtune=arm1176jzf-s -mfloat-abi=hard -mfpu=neon-vfpv4 -ffast-math -DRASPI=2
TARGET=RASPI2
endif

ifeq ($(UNAME), armv8l)
PFLAGS=-march=armv7-a -mtune=arm1176jzf-s -mfloat-abi=hard -mfpu=neon-vfpv4 -ffast-math -DRASPI=3
TARGET=RASPI3
endif

ifeq ($(UNAME), armv8l && $(shell expr $(RPI_VERSION) \>= 4), 1)
PFLAGS=-march=armv8-a -mtune=cortex-a53 -mfloat-abi=hard -mfpu=neon-fp-armv8 -ffast-math -DRASPI=4
TARGET=RASPI4
endif

## old/special pi versions
ifeq ($(UNAME), armv5l)
PFLAGS=-march=native -mtune=native -mfloat-abi=hard -mfpu=vfp -ffast-math -DRPI
TARGET=RPI
endif

ifeq ($(UNAME), armv5l)
PFLAGS=-march=native -mtune=native -mfloat-abi=hard -mfpu=vfp -ffast-math -DRASPBERRY
TARGET=RASPBERRY
endif

@echo "Compiling PiFunk"

## Generating objects in gcc specific order
## assembler code
pifunk.S:	pifunk.c
					$(USER) $(CC) $(DEBUG) $(CFLAGS) $(LDLIBS) $(LDFLAGS) $(PFLAGS) $(ASFLAGS)-c -o lib/pifunk.S
## precompiled c-code
pifunk.i:	pifunk.c
					$(USER) $(CC) $(DEBUG) $(CFLAGS) $(LDLIBS) $(LDFLAGS) $(PFLAGS)-E -C -o include/pifunk.i
## precompiled assemblercode
pifunk.s:	pifunk.c
					$(USER) $(CC) $(DEBUG) $(CFLAGS) $(LDLIBS) $(LDFLAGS) $(PFLAGS) $(ASFLAGS)-o lib/pifunk.s
## static object
pifunk.o:	pifunk.c
					$(USER) $(CC) $(DEBUG) $(CFLAGS) $(LDLIBS) $(LDFLAGS) $(PFLAGS)-o lib/pifunk.o
## archive
pifunk.a:	pifunk.c
					$(USER) $(CC)  $(DEBUG) $(CFLAGS) $(LDLIBS) $(LDFLAGS) $(PFLAGS)-o lib/pifunk.a
## library
pifunk.lib:	pifunk.c
						$(USER) $(CC)  $(DEBUG) $(CFLAGS) $(LDLIBS) $(LDFLAGS) $(PFLAGS)-o lib/pifunk.lib
## shared object
pifunk.so:	pifunk.c
						$(USER) $(CC)  $(DEBUG) $(CFLAGS) $(LDLIBS) $(LDFLAGS) $(PFLAGS)-o lib/pifunk.so

## lib list
OBJECTS	= pifunk.i pifunk.s pifunk.o pifunk.a pifunk.lib pifunk.so
$(OBJECTS)

## generating executable binaries
pifunk.out:	pifunk.c $(OBJECTS)
					$(USER) $(CC) $(DEBUG) $(CFLAGS) $(LDLIBS) $(LDFLAGS) $(PFLAGS)-o bin/pifunk.out

pifunk.bin: pifunk.c $(OBJECTS)
					$(USER) $(CC) $(DEBUG) $(CFLAGS) $(LDLIBS) $(LDFLAGS) $(PFLAGS)-o bin/pifunk.bin

pifunk:	pifunk.c $(OBJECTS)
					$(USER) $(CC) $(DEBUG) $(CFLAGS) $(LDLIBS) $(LDFLAGS) $(PFLAGS)-o bin/pifunk

all: pifunk

EXECUTABLES=pifunk pifunk.out pifunk.bin
$(EXECUTABLES)

## generate info file
.PHONY: 	info
info: pifunk.info
pifunk.info: pifunk.texi
						 $(MAKEINFO)

.PHONY: 	piversion
piversion:	$(USER) $(UNAME)
						$(USER) $(PCPUI)
						$(USER) $(RPI_VERSION)

.PHONY: 	install
install:	cd $(HOME)/PiFunk/
					$(USER) install -m 0755 pifunk $(HOME)/bin/

.PHONY: 	uninstall
uninstall:	$(USER) $(RM) $(HOME)/bin/pifunk $(HOME)/bin/pifunk

.PHONY:	clean
clean:	$(USER) $(RM) $(OBJECTS) $(EXECUTABLES)

.PHONY: 	help
help:	cd $(HOME)/PiFunk/bin/
			$(USER) ./pifunk -h

.PHONY: 	assistant
assistent:	cd $(HOME)/PiFunk/bin/
						$(USER) ./pifunk -a

.PHONY: 	menu
menu:	cd $(HOME)/PiFunk/bin/
			$(USER) ./pifunk -u

.PHONY: 	run
run:	cd $(HOME)/PiFunk/bin/
			$(USER) ./pifunk -n sound.wav -f 446.006250 -s 22050 -m fm -p 7 -c callsign
