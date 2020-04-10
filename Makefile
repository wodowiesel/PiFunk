## pifunk Makefile
## should run with sudo or root rights
USER=sudo
$(USER)
CC=gcc # gcc 8.3.0-6
$(CC)
CXX=g++
$(CXX)
MAKEINFO=pifunk
$(MAKEINFO)
VERSION=0.1.7.8
$(VERSION)
STATUS=experimental
$(STATUS)

## default paths
INIT=/bin/sh ## init-shell
$(INIT)
HOME=/home/pi ## std-path
$(HOME)
KERNEL_DIR:=/lib/modules/$(shell uname -r)/build/
$(KERNEL_DIR)

RM=rm -f ## remove files or folder
#$(RM)

## use gnu c compiler, -std=gnu99 is c99 -std=iso9899:1999 with extra gnu extentions, flags see below
## https://renenyffenegger.ch/notes/development/languages/C-C-plus-plus/GCC/options/index
CINC:=-Iinclude -I/opt/vc/include/ -I/usr/include/linux/ -I/usr/src/include/linux/ -I/usr/src/linux-headers-$(shell uname -r)/include/linux/ ## kernel now 4.19.97+
$(CINC)
CMA=-D_USE_MATH_DEFINES -D_GNU_SOURCE
$(CMA)

CFLAGS=-std=gnu99 -fPIC pifunk.c -O3
$(CFLAGS)
CXXFLAGS=-std=gnu++17 -fPIC pifunk.cpp -O3
$(CXXFLAGS)

ASFLAGS=-S -CC ## upper case assembler code without linker
$(ASFLAGS)
PPFLAGS=-E -CC ## c-preproccessor
$(PPFLAGS)
LIFLAGS=-c ## no linker
$(LIFLAGS)

DEBUG=-Wall -Werror -v -g3 -ggdb3
## --working-directory
## -pedantic-errors, -dM (dumps preprocessor Macros)
$(DEBUG)

LDLIBS=-Llib -L/opt/vc/lib/ -L/usr/src/lib/
$(LDLIBS)
PFLIBS=-L$(HOME)/PiFunk/lib/
$(PFLIBS)

LDFLAGS=-lgnu -lm -lpthread -lbcm_host -lbcm2835 -lsndfile -shared
$(LDFLAGS)
PFFLAGGS=-lpifunk
$(PFFLAGS)

## other optional macros if necessary
##-isystem $(KERNEL_DIR) : You must use the kernel headers of the kernel you're compiling against. Using the default /usr/include/linux won't work.
##-D__KERNEL__ : Defining this symbol tells the header files that the code will be run in kernel mode, not as a user process.
##-DMODULE: This symbol tells the header files to give the appropriate definitions for a kernel module.

## Determine the hardware/software platform
UNAME:=$(shell uname -m) ## processor: armv6l
$(UNAME)
KERNEL:=$(shell uname -a) ## kernel: Linux raspberrypi 4.19.97+ #1294 Thu Jan 30 13:10:54 GMT 2020 armv6l GNU/Linux
$(KERNEL)
RVERSION:=$(shell uname -r) ## vervion number: 4.19.97+
$(RVERSION)
VCGVERSION:=$(shell vcgencmd version) ## vcg firmware: version 53a54c778c493957d99bf49762dfabc4eee80e45
$(VCGVERSION)
OSVERSION:=$(shell cat /etc/rpi-issue) ## os Date, http://github.com/RPi-Distro/pi-gen, MD5, stage5
$(OSVERSION)
RPIVERSION:=$(shell cat /proc/device-tree/model) ## grab revision: | grep -a -o "Raspberry\sPi\sModel\s[A-Z]\sPlus" | grep -a -o "Rev\s[0-9].[0-9]" : Raspberry Pi Model B Plus Rev 1.2
$(RPIVERSION)
PCPUI:=$(shell cat /proc/cpuinfo) ## cpuinfos my rev: 0010 -> 1.2 B+: | grep Revision | cut -c16-
$(PCPUI)

## Enable ARM-specific options only
## old/special pi versions
ifeq ($(UNAME), armv5)
PFLAGS=-march=native -mtune=native -mfloat-abi=soft -mfpu=vfp -ffast-math -DRPI
TARGET=RPI # alternative1
endif

ifeq ($(UNAME), armv5l)
PFLAGS=-march=native -mtune=native -mfloat-abi=softfp -mfpu=vfp -ffast-math -DRASPBERRY
TARGET=RASPBERRY # alternative2
endif

ifeq ($(UNAME), armv6)
PFLAGS=-march=armv6 -mtune=arm1176jzf-s -mfloat-abi=softfp -mfpu=vfp -ffast-math -DRASPI0
TARGET=RASPI0 # & Pi W
endif

ifeq ($(UNAME), armv6l)
PFLAGS=-march=armv6 -mtune=arm1176jzf-s -mfloat-abi=hard -mfpu=vfp -ffast-math -DRASPI1
TARGET=RASPI1
endif

ifeq ($(UNAME), armv7l)
PFLAGS=-march=armv7-a -mtune=arm1176jzf-s -mfloat-abi=hard -mfpu=neon-vfpv4 -ffast-math -DRASPI2
TARGET=RASPI2
endif

ifeq ($(UNAME), armv8l)
PFLAGS=-march=armv7-a -mtune=arm1176jzf-s -mfloat-abi=hard -mfpu=neon-vfpv4 -ffast-math -DRASPI3
TARGET=RASPI3
endif

ifeq ($(UNAME), armv8l && $(shell expr $(RPIVERSION) | grep -a -o "Raspberry\sPi\sModel\s[A-Z]" | grep -o "[0-9]" = 4), 1)
PFLAGS=-march=armv8-a -mtune=cortex-a53 -mfloat-abi=hard -mfpu=neon-fp-armv8 -ffast-math -DRASPI4
TARGET=RASPI4
endif

$(PFLAGS)
$(TARGET)

#@echo "Compiling PiFunk"

## Generating objects in gcc specific order
## assembler code
pifunk.S:	pifunk.c
					$(USER) $(CC) $(DEBUG) $(CFLAGS) $(CINC) $(LDLIBS) $(LDFLAGS) $(CMA) $(PFLAGS) $(ASFLAGS) $(LIFLAGS) -o lib/pifunk.S
## precompiled/processor c-code
pifunk.i:	pifunk.c
					$(USER) $(CC) $(DEBUG) $(CFLAGS) $(CINC) $(LDLIBS) $(LDFLAGS) $(CMA) $(PFLAGS) $(PPFLAGS) -CC -o lib/pifunk.i # -C or CC keeps the comments in preprocessor
## precompiled assemblercode
pifunk.s:	pifunk.c
					$(USER) $(CC) $(DEBUG) $(CFLAGS) $(CINC) $(LDLIBS) $(LDFLAGS) $(CMA) $(PFLAGS) $(ASFLAGS) -o lib/pifunk.s
## static object
pifunk.o:	pifunk.c
					$(USER) $(CC) $(DEBUG) $(CFLAGS) $(CINC) $(LDLIBS) $(LDFLAGS) $(CMA) $(PFLAGS) -o lib/pifunk.o
## archive
pifunk.a:	pifunk.c
					$(USER) $(CC) $(DEBUG) $(CFLAGS) $(CINC) $(LDLIBS) $(LDFLAGS) $(CMA) $(PFLAGS) -o lib/pifunk.a
## library
pifunk.lib:	pifunk.c
						$(USER) $(CC) $(DEBUG) $(CFLAGS) $(CINC) $(LDLIBS) $(LDFLAGS) $(CMA) $(PFLAGS) -o lib/pifunk.lib
## shared object
pifunk.so:	pifunk.c
						$(USER) $(CC) $(DEBUG) $(CFLAGS) $(CINC) $(LDLIBS) $(LDFLAGS) $(CMA) $(PFLAGS) -o lib/pifunk.so

## lib object list
OBJECTS=pifunk.i pifunk.s pifunk.o pifunk.a pifunk.lib pifunk.so
$(OBJECTS)

## generating executable binaries
pifunk.out:	pifunk.c $(OBJECTS)
						$(USER) $(CC) $(DEBUG) $(CFLAGS) $(CINC) $(LDLIBS) $(PFLIBS) $(LDFLAGS) $(CMA) $(PFLAGS) -o bin/pifunk.out

pifunk.bin: pifunk.c $(OBJECTS)
						$(USER) $(CC) $(DEBUG) $(CFLAGS) $(CINC) $(LDLIBS) $(PFLIBS) $(LDFLAGS) $(CMA) $(PFLAGS) -o bin/pifunk.bin

pifunk:			pifunk.c $(OBJECTS)
						$(USER) $(CC) $(DEBUG) $(CFLAGS) $(CINC) $(LDLIBS) $(PFLIBS) $(LDFLAGS) $(PFFLAGS) $(CMA) $(PFLAGS) -o bin/pifunk

#all: pifunk.out pifunk.bin pifunk

EXECUTABLES=pifunk.out pifunk.bin pifunk
$(EXECUTABLES)

.PHONY:		pifunklib
pifunk.lib:	pifunk.c
						$(USER) $(CC) $(DEBUG) $(CFLAGS) $(CINC) $(LDLIBS) $(LDFLAGS) $(CMA) $(PFLAGS) -o lib/pifunk.lib

.PHONY:		pifunk+
pifunk+:	pifunk.cpp $(OBJECTS)
					$(USER) $(CXX) $(DEBUG) $(CXXFLAGS) $(CINC) $(LDLIBS) $(PFLIBS) $(LDFLAGS) $(PFFLAGS) $(CMA) $(PFLAGS) -o bin/pifunk+

## generate info file
.PHONY: 	info
info: 		pifunk.info
pifunk.info: pifunk.texi
						 $(MAKEINFO)

.PHONY: 		piversion
piversion:	$(USER) $(UNAME)
						$(USER) $(KERNEL)
						$(USER) $(RVERSION)
						$(USER) $(VCGVERSION)
						$(USER) $(OSVERSION)
						$(USER) $(RPIVERSION)
						$(USER) $(PCPUI)

.PHONY: 	install
install:	cd $(HOME)/PiFunk/
					$(USER) install -m 0755 pifunk $(HOME)/bin/

.PHONY: 		uninstall
uninstall:	$(USER) $(RM) $(HOME)/PiFunk/bin/pifunk.out
						$(USER) $(RM) $(HOME)/PiFunk/bin/pifunk.bin
						$(USER) $(RM) $(HOME)/PiFunk/bin/pifunk

.PHONY:	clean
clean:	cd $(HOME)/PiFunk/
				$(USER) $(RM) $(OBJECTS)
				$(USER) $(RM) $(EXECUTABLES)

.PHONY: 	help
help:			cd $(HOME)/PiFunk/bin/
					$(USER) ./pifunk -h

.PHONY: 		assistant
assistent:	cd $(HOME)/PiFunk/bin/
						$(USER) ./pifunk -a

.PHONY: 	menu
menu:			cd $(HOME)/PiFunk/bin/
					$(USER) ./pifunk -u

.PHONY: 	run
run:			cd $(HOME)/PiFunk/bin/
					$(USER) ./pifunk -n sound.wav -f 446.006250 -s 22050 -m fm -p 7 -c callsign

.PHONY: 	run+
run+:			cd $(HOME)/PiFunk/bin/
					$(USER) ./pifunk+ -n sound.wav -f 446.006250 -s 22050 -m fm -p 7 -c callsign
