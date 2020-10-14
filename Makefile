## pifunklite Makefile
## should run with sudo or root rights
USER=sudo
$(USER)
CC=gcc ## gcc 8.3.0-6
$(CC)
CXX=g++
$(CXX)

## general infos
MAKEINFO=pifunklite
$(MAKEINFO)
VERSION=0.1.7.6
$(VERSION)
STATUS=lite
$(STATUS)

SOURCE=pifunklite.c
$(SOURCE)
SOURCECXX=pifunklite.cpp
$(SOURCECXX)

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
## environment variable C_INCLUDE_PATH
## https://renenyffenegger.ch/notes/development/languages/C-C-plus-plus/GCC/options/index
CINC:=-Iinclude -I/opt/vc/include/ ## kernel
# -I/usr/include/linux/ -I/usr/src/linux-headers-4.19.97+/include/linux/
$(CINC) #
CMA=-D_USE_MATH_DEFINES -D_GNU_SOURCE
$(CMA)

CFLAGS=-std=gnu99 -O3 ## gnu extention & highest optimization level
$(CFLAGS)
CXXFLAGS=-std=gnu++17 -O3 ## for c++
$(CXXFLAGS)

ASFLAGS=-S -CC ## upper case assembler code without linker
$(ASFLAGS)
PPFLAGS=-E -CC ## c-preproccessor, -C or CC keeps the comments in preprocessor
$(PPFLAGS)
LIFLAGS=-c ## no linker used
$(LIFLAGS)
SHFLAGS=-shared -fPIC ## make shared big libraries
$(SHFLAGS)

DEBUG=-Wall -v -g3 -ggdb3 -pg # -Q
$(DEBUG)
## -pg makes profiles for object code for analysis with gprof
## -Wall shows all errors & warnings, -w inhibits warnings

LDLIBS=-Llib -L/opt/vc/lib/ -L/usr/src/lib/
$(LDLIBS)
PFLIBS=-L$(HOME)/PiFunklite/lib/
$(PFLIBS)

LDFLAGS=-lbcm_host -lbcm2835 -lgcc -lsndfile -lm -pthread ## -lm after -lsnd, -lgnu
$(LDFLAGS)
PFFLAGGS=-lpifunklite ## own pifunk library, gcc assumes lib beginns with prefix "lib"
$(PFFLAGS)
#AOFLAGGS=-arm-none-eabi-objdump ## objdump gnu tools
#$(AOFLAGS)

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
TARGET=RPI ## alternative1
endif

ifeq ($(UNAME), armv5l)
PFLAGS=-march=native -mtune=native -mfloat-abi=softfp -mfpu=vfp -ffast-math -DRASPBERRY
TARGET=RASPBERRY ## alternative2
endif

ifeq ($(UNAME), armv6)
PFLAGS=-march=armv6 -mtune=arm1176jzf-s -mfloat-abi=softfp -mfpu=vfp -ffast-math -DRASPI0
TARGET=RASPI0 ## & Pi W
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

$(TARGET)
$(PFLAGS)

#@echo Compiling PiFunk

## Generating objects in gcc specific order, -save-temps
## translated assembler/c-code
pifunklite.s:	$(SOURCE) pifunk.h
					$(USER) $(CC) $(SOURCE) $(DEBUG) $(CFLAGS) $(CINC) $(LDLIBS) $(LDFLAGS) $(CMA) $(PFLAGS) $(ASFLAGS) $(LIFLAGS) -o src/pifunklite.s ## for arm
					$(USER) $(CC) $(SOURCE) $(DEBUG) $(CFLAGS) $(CINC) $(LDLIBS) $(LDFLAGS) $(CMA) $(PFLAGS) $(ASFLAGS) $(LIFLAGS) -o src/pifunklite.asm ## normal assebler suffix
## precompiled/processor c-code
pifunklite.i:	$(SOURCE) pifunk.h
					$(USER) $(SOURCE) $(CC) $(DEBUG) $(CFLAGS) $(CINC) $(LDLIBS) $(LDFLAGS) $(CMA) $(PFLAGS) $(PPFLAGS) -o lib/pifunklite.i
## precompiled object/machine-code
pifunklite.o:	$(SOURCE) pifunk.h
					$(USER) $(CC) $(DEBUG) $(SOURCE) $(CFLAGS) $(CINC) $(LDLIBS) $(LDFLAGS) $(CMA) $(PFLAGS) $(LIFLAGS) -o lib/pifunklite.o
## static archive
pifunklite.a:	pifunklite.o
					$(USER) $(CC) $(SOURCE) $(DEBUG) $(CFLAGS) $(CINC) $(LDLIBS) $(LDFLAGS) $(CMA) $(PFLAGS) $(LIFLAGS) -o lib/pifunklite.a
					$(USER) ar rcs -t $@ $^
					$(USER) ranlib pifunklite.a
## static library
pifunklite.lib:	pifunklite.o pifunklite.a
						$(USER) $(CC) $(SOURCE) $(DEBUG) $(CFLAGS) $(CINC) $(LDLIBS) $(LDFLAGS) $(CMA) $(PFLAGS) $(LIFLAGS) -o lib/pifunklite.lib
						$(USER) ar rcs -t $@ $^
						$(USER) ranlib pifunklite.lib
## shared object
pifunklite.so:	pifunklite.o
						$(USER) $(CC) $(DEBUG) $(CFLAGS) $(CINC) $(LDLIBS) $(LDFLAGS) $(CMA) $(PFLAGS) $(SHFLAGS) -o lib/pifunklite.so lib/pifunklite.o
						$(USER) ar rcs -t $@ $^
						$(USER) ranlib pifunklite.so
## dynamic linked library
pifunklite.dll:	pifunklite.o
						$(USER) $(CC) $(SOURCE) $(DEBUG) $(CFLAGS) $(CINC) $(LDLIBS) $(LDFLAGS) $(CMA) $(PFLAGS) $(LIFLAGS) -o lib/pifunklite.dll
						$(USER) ar rcs -t $@ $^
						$(USER) ranlib pifunklite.dll
## lib object list
OBJECTS=pifunklite.s pifunklite.i pifunklite.o pifunklite.a pifunklite.lib pifunklite.so pifunklite.dll
$(OBJECTS)

## generating standard binary
pifunklite.out:	$(SOURCE) pifunk.h
						$(USER) $(CC) $(DEBUG) $(CFLAGS) $(CINC) $(LDLIBS) $(PFLIBS) $(LDFLAGS) $(CMA) $(PFLAGS) $(PFFLAGS) -save-temps -o bin/pifunklite.out
## explicit binary
pifunklite.bin: $(SOURCE) pifunk.h
						$(USER) $(CC) $(DEBUG) $(CFLAGS) $(CINC) $(LDLIBS) $(PFLIBS) $(LDFLAGS) $(CMA) $(PFLAGS) -save-temps -o bin/pifunklite.bin
## normal binary
pifunklite:			$(SOURCE) pifunk.h
						$(USER) $(CC) $(DEBUG) $(CFLAGS) $(CINC) $(LDLIBS) $(PFLIBS) $(LDFLAGS) $(PFFLAGS) $(CMA) $(PFLAGS) -save-temps -o bin/pifunklite
## executable list
#allbin: pifunklite.out pifunklite.bin pifunklite
EXECUTABLES=pifunklite.out pifunklite.bin pifunklite
$(EXECUTABLES)

.PHONY:		pifunklite+
pifunk+:	$(SOURCECXX) $(OBJECTS)
					$(USER) $(CXX) $(DEBUG) $(CXXFLAGS) $(CINC) $(LDLIBS) $(PFLIBS) $(LDFLAGS) $(PFFLAGS) $(CMA) $(PFLAGS) -o bin/pifunklite+

## generate info file
.PHONY: 	info
info: 		pifunklite.info
pifunk.info: pifunklite.texi
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
install:	cd $(HOME)/PiFunklite/
					$(USER) install -m 0777 pifunklite $(HOME)/bin/
					# https://askubuntu.com/questions/638796/what-is-meaning-of-755-permissions-in-samba-share
					# file permission mode to so that the user ID, group &other bit for the owner is setuid
					# -rwsr-xr-x (4755), -rwxr-xr-x (0755), read+write+execute (0777)

.PHONY: 		uninstall
uninstall:	$(USER) $(RM) $(HOME)/PiFunklite/bin/pifunklite.out
						$(USER) $(RM) $(HOME)/PiFunklite/bin/pifunklite.bin
						$(USER) $(RM) $(HOME)/PiFunklite/bin/pifunklite

.PHONY:	clean
clean:	cd $(HOME)/PiFunklite/
				$(USER) $(RM) $(OBJECTS)
				$(USER) $(RM) $(EXECUTABLES)

.PHONY: 		assistant
assistent:	cd $(HOME)/PiFunklite/bin/
						$(USER) ./pifunklite -a

.PHONY: 	menu
menu:			cd $(HOME)/PiFunklite/bin/
					$(USER) ./pifunklite -u

.PHONY: 	help
help:			cd $(HOME)/PiFunklite/bin/
					$(USER) ./pifunklite -h

.PHONY: 	run
run:			cd $(HOME)/PiFunklite/bin/
					$(USER) ./pifunklite -n sound.wav -f 26.9650 -s 22050 -m fm -t a -b 12.5 -p 7 -g 7 -d 14 -l 0

.PHONY: 	run+
run+:			cd $(HOME)/PiFunklite/bin
					$(USER) ./pifunklite+ -n sound.wav -f 26.9650 -s 22050 -m fm -t a -b 12.5 -p 7 -g 7 -d 14 -l 0
