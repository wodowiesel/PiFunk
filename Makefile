## PiFunk Makefile
## should run with sudo or root rights
USER:=sudo
#$(USER)
CC:=gcc ## gcc 8.3.0-6
#$(CC)
CXX:=g++
#$(CXX)

## general infos
MAKEINFO:=pifunk
#$(MAKEINFO)
VERSION:=0.1.7.7
#$(VERSION)
STATUS:=experimental
#$(STATUS)

SRC:=pifunk.c
#$(SRC)
SRCXX:=pifunk++.cpp
#$(SRCXX)

HC:=pifunk.h
#$(HC)
HXX:=pifunk++.hpp
#$(HXX)

## default paths
INIT:=/bin/sh ## init-shell
#$(INIT)
HOME:=/home/pi ## std-path
#$(HOME)
KERNEL_DIR:=/lib/modules/$(shell uname -r)/build/
#$(KERNEL_DIR)

RM:=rm -f ## remove files or folder
#$(RM)

## use gnu c compiler, -std=gnu99 is c99 -std=iso9899:1999 with extra gnu extentions, flags see below
## environment variable C_INCLUDE_PATH
## https://renenyffenegger.ch/notes/development/languages/C-C-plus-plus/GCC/options/index
CINC:=-Iinclude -I/opt/vc/include/ -I/usr/include/linux/ -I/usr/src/include/linux/ -I/usr/src/linux-headers-4.19.97+/include/ -I/usr/src/linux-headers-4.19.97+/include/linux/ ## kernel now 4.19.97+
#$(CINC)
CMA:=-D_USE_MATH_DEFINES -D_GNU_SOURCE
#$(CMA)

CFLAGS:=-std=gnu99 -O2 ## gnu extention & highest optimization level
#$(CFLAGS)
CXXFLAGS:=-std=gnu++17 -O3 ## for c++
#$(CXXFLAGS)

ASFLAGS:=-S -CC ## upper case assembler code without linker
#$(ASFLAGS)
PPFLAGS:=-E -CC ## c-preproccessor, -C or CC keeps the comments in preprocessor
#$(PPFLAGS)
LIFLAGS:=-c ## no linker used
#$(LIFLAGS)
SHFLAGS:=-shared -fPIC ## make shared big libraries
#$(SHFLAGS)

DEBUG:=-Wall -v -g3 -ggdb3 -pg ##-Q
#$(DEBUG)
## -pg makes profiles for object code for analysis with gprof
## -Wall shows all errors & warnings, -w inhibits warnings

LDLIBS:=-Llib -L/opt/vc/lib/ -L/usr/src/lib/
#$(LDLIBS)
PFLIBS:=-L$(HOME)/PiFunk/lib/
#$(PFLIBS)

LDFLAGS:=-lgnu -lpthread -lbcm_host -lbcm2835 -lsndfile -lm
#$(LDFLAGS)
PFFLAGS:=-llibpifunk ## own pifunk library, gcc assumes lib beginns with prefix "lib"
#$(PFFLAGS)
AOFLAGS:=-arm-none-eabi-objdump ## objdump gnu tools
#$(AOFLAGS)

## other optional macros if necessary
##-isystem $(KERNEL_DIR) : You must use the kernel headers of the kernel you're compiling against. Using the default /usr/include/linux won't work.
##-D__KERNEL__ : Defining this symbol tells the header files that the code will be run in kernel mode, not as a user process.
##-DMODULE: This symbol tells the header files to give the appropriate definitions for a kernel module.

## Determine the hardware/software platform
UNAME:=$(shell uname -m) ## processor: armv6l
#$(UNAME)
KERNEL:=$(shell uname -a) ## kernel: Linux raspberrypi 4.19.97+ #1294 Thu Jan 30 13:10:54 GMT 2020 armv6l GNU/Linux
#$(KERNEL)
RVERSION:=$(shell uname -r) ## vervion number: 4.19.97+
#$(RVERSION)
VCGVERSION:=$(shell vcgencmd version) ## vcg firmware: version 53a54c778c493957d99bf49762dfabc4eee80e45
#$(VCGVERSION)
OSVERSION:=$(shell cat /etc/rpi-issue) ## os Date, https://github.com/RPi-Distro/pi-gen, MD5, stage5
#$(OSVERSION)
RPIVERSION:=$(shell cat /proc/device-tree/model) ## grab revision: | grep -a -o "Raspberry\sPi\sModel\s[A-Z]\sPlus" | grep -a -o "Rev\s[0-9].[0-9]" : Raspberry Pi Model B Plus Rev 1.2
#$(RPIVERSION)
PCPUI:=$(shell cat /proc/cpuinfo) ## cpuinfos my rev: 0010 -> 1.2 B+: | grep Revision | cut -c16-
#$(PCPUI)

## Enable ARM-specific options only
## old/special pi versions
ifeq ($(UNAME), armv5)
TARGET:=RASPIH ## alternative1 historic
PFLAGS:=-march=native -mcpu=arm926ej-s -mtune=native -mfloat-abi=softfp -mfpu=vfp -ffast-math -DRASPIH
endif

ifeq ($(UNAME), armv5l || armv5b)
TARGET:=RASPIH2 ## alternative2
PFLAGS:=-march=armv5te -mcpu=arm926ej-s -mtune=arm926ej-s -mfloat-abi=hard -mfpu=vfp -ffast-math -DRASPIH2
endif

ifeq ($(UNAME), armv5l)
TARGET:=RASPIH3 ## alternative3
PFLAGS:=-march=armv5te -mcpu=arm1176jzf-s -mtune=arm1176jzf-s -mfloat-abi=hard -mfpu=vfp -ffast-math -DRASPIH3
endif

ifeq ($(UNAME), armv6)
TARGET:=RASPI0
PFLAGS:=-march=armv6t2e -mcpu=arm11 -mtune=arm1176jzf-s -mfloat-abi=softfp -mfpu=vfp -ffast-math -DRASPI0
endif

ifeq ($(UNAME), armv6l) # Thumb t + enhanced DSP active
TARGET:=RASPI1
PFLAGS:=-march=armv6t2e -mtune=arm1176jzf-s -mfloat-abi=hard -mfpu=vfp -ffast-math -DRASPI1
endif

ifeq ($(UNAME), armv7l)
TARGET:=RASPI2W
PFLAGS:=-march=armv8-a -mcpu=cortex-a53 -mtune=cortex-a53 -fstack-protector-strong -fno-plt -ffast-math -pipe -DRASPI2W
endif

ifeq ($(UNAME), armv7l)
TARGET:=RASPI2
PFLAGS:=-march=armv7-a -mcpu=cortex-a7 -mtune=arm1176jzf-s -mfloat-abi=hard -mfpu=neon-vfpv4 -ffast-math -DRASPI2
endif

ifeq ($(UNAME), armv7l)
TARGET:=RASPI2R
PFLAGS:=-march=armv7-a -mcpu=cortex-a53 -mtune=arm1176jzf-s -mfloat-abi=hard -mfpu=neon-vfpv4 -ffast-math -DRASPI2R
endif

ifeq ($(UNAME), armv8l && $(shell expr $(RPIVERSION) | grep -a -o "Raspberry\sPi\sModel\s[A-Z]" | grep -o "[0-9]" = 3), 1)
TARGET:=RASPI3
PFLAGS:=-march=armv7-a -mcpu=cortex-a53 -mtune=arm1176jzf-s -mfloat-abi=hard -mfpu=neon-vfpv4 -ffast-math -DRASPI3
endif

ifeq ($(UNAME), armv8l && $(shell expr $(RPIVERSION) | grep -a -o "Raspberry\sPi\sModel\s[A-Z]" | grep -o "[0-9]" = 4), 1)
TARGET:=RASPI4
PFLAGS:=-march=armv8-a -mcpu=cortex-a72 -mtune=cortex-a72 -mfloat-abi=hard -mfpu=neon-fp-armv8 -ffast-math -DRASPI4
endif

ifeq ($(UNAME), armv8l && $(shell expr $(RPIVERSION) | grep -a -o "Raspberry\sPi\sModel\s[A-Z]" | grep -o "[0-9]" = 4), 1)
TARGET:=RASPI400
PFLAGS:=-march=armv8-a -mcpu=cortex-a72 -mtune=cortex-a72 -mfloat-abi=hard -mfpu=neon-fp-armv8 -ffast-math -DRASPI400 #cortex-a53
endif

ifeq ($(UNAME), aarch64)
TARGET:=AARCH64
PFLAGS:=-march=armv8-a -mcpu=cortex-a53 -pipe -fstack-protector-strong -fno-plt -ffast-math -DAARCH64
endif

#$(TARGET)
#$(PFLAGS)

#@echo Compiling PiFunk

## Generating objects in gcc specific order, -save-temps
## translated assembler/c-code
src/pifunk.s:	$(SRC) $(HC)
					$(USER) $(CC) $(SRC) $(DEBUG) $(CFLAGS) $(CINC) $(LDLIBS) $(LDFLAGS) $(CMA) $(PFLAGS) $(ASFLAGS) $(LIFLAGS) -o src/pifunk.s ## for arm

src/pifunk.asm: $(SRC) $(HC)
					$(USER) $(CC) $(SRC) $(DEBUG) $(CFLAGS) $(CINC) $(LDLIBS) $(LDFLAGS) $(CMA) $(PFLAGS) $(ASFLAGS) $(LIFLAGS) -o src/pifunk.asm ## normal assembler suffix

## precompiled/processor c-code
src/pifunk.i:	$(SRC) $(HC)
					$(USER) $(CC) $(SRC) $(DEBUG) $(CFLAGS) $(CINC) $(LDLIBS) $(LDFLAGS) $(CMA) $(PFLAGS) $(PPFLAGS) -o src/pifunk.i

src/pifunk++.ii:	$(SRCXX) $(HXX)
					$(USER) $(CXX) $(SRCXX) $(DEBUG) $(CXXFLAGS) $(CINC) $(LDLIBS) $(LDFLAGS) $(CMA) $(PFLAGS) $(PFLIBS) $(PPFLAGS) -o src/pifunk++.ii 

## precompiled object/machine-code
src/pifunk.o:	$(SRC) $(HC)
					$(USER) $(CC) $(DEBUG) $(SRC) $(CFLAGS) $(CINC) $(LDLIBS) $(LDFLAGS) $(CMA) $(PFLAGS) $(LIFLAGS) -o src/pifunk.o

## static archive
lib/libpifunk.a:	libpifunk.o
					$(USER) $(CC) $(SRC) $(DEBUG) $(CFLAGS) $(CINC) $(LDLIBS) $(LDFLAGS) $(CMA) $(PFLAGS) $(LIFLAGS) -o lib/libpifunk.a
					$(USER) ar rcs $@ $^
					$(USER) ranlib libpifunk.a

## static library
lib/libpifunk.lib:	libpifunk.o
						$(USER) $(CC) $(SRC) $(DEBUG) $(CFLAGS) $(CINC) $(LDLIBS) $(LDFLAGS) $(CMA) $(PFLAGS) $(LIFLAGS) -o lib/libpifunk.lib
						$(USER) ar rcs $@ $^
						#$(USER) ranlib libpifunk.lib

## shared object
lib/libpifunk.so:	libpifunk.o
						$(USER) $(CC) $(DEBUG) $(CFLAGS) $(CINC) $(LDLIBS) $(LDFLAGS) $(CMA) $(PFLAGS) $(SHFLAGS) -o lib/libpifunk.so
						$(USER) ar rcs $@ $^
						#$(USER) ranlib libpifunk.so

## dynamic linked library
lib/libpifunk.dll:	libpifunk.o
						$(USER) $(CC) $(SRC) $(DEBUG) $(CFLAGS) $(CINC) $(LDLIBS) $(LDFLAGS) $(CMA) $(PFLAGS) $(LIFLAGS) -o lib/libpifunk.dll
						$(USER) ar rcs $@ $^
						#$(USER) ranlib libpifunk.dll

## lib object list
OBJECTS:=pifunk.s pifunk.asm pifunk.i pifunk.o libpifunk.a libpifunk.lib libpifunk.so libpifunk.dll
#$(OBJECTS)

OBJECTSXX:=pifunk++.s pifunk++.asm pifunk++.ii pifunk++.o libpifunk++.a libpifunk++.lib libpifunk++.so libpifunk++.dll
#$(OBJECTSXX)

## generating standard binaries
bin/pifunk.out:	$(SRC) $(HC)
						$(USER) $(CC) $(DEBUG) $(CFLAGS) $(CINC) $(LDLIBS) $(PFLIBS) $(LDFLAGS) $(CMA) $(PFLAGS) $(PFFLAGS) -save-temps -o bin/pifunk.out

## explicit binary
bin/pifunk.bin: $(SRC) $(HC)
						$(USER) $(CC) $(DEBUG) $(CFLAGS) $(CINC) $(LDLIBS) $(PFLIBS) $(LDFLAGS) $(CMA) $(PFLAGS) -save-temps -o bin/pifunk.bin

## normal binary
bin/pifunk:	$(SRC) $(HC)
						$(USER) $(CC) $(DEBUG) $(CFLAGS) $(CINC) $(LDLIBS) $(PFLIBS) $(LDFLAGS) $(PFFLAGS) $(CMA) $(PFLAGS) -save-temps -o bin/pifunk

## executable list
EXECUTABLES:=pifunk.out pifunk.bin pifunk
#$(EXECUTABLES)

## executable list g++
EXECUTABLESXX:=pifunk++.out pifunk++.bin pifunk++
#$(EXECUTABLESXX)

.PHONY:		libpifunk
lib/pifunk.so:	$(SRC)
						$(USER) $(CC) $(DEBUG) $(CFLAGS) $(CINC) $(LDLIBS) $(LDFLAGS) $(PFLAGS) $(SHFLAGS) $(CMA) -o lib/libpifunk.so

## your own cpp&hpp file (not provided)
.PHONY:		pifunk++
bin/pifunk++:	$(SRCXX) $(HXX)
						 $(USER) $(CXX) $(DEBUG) $(CXXFLAGS) $(CINC) $(LDLIBS) $(LDFLAGS) $(PFLIBS) $(PFFLAGS) $(PFLAGS) $(CMA) -o bin/pifunk++

## generate info file
.PHONY: 	info
info: 		pifunk.info
pifunk.info: pifunk.texi
						 #$(MAKEINFO)

.PHONY: 	piversion
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

.PHONY: 	uninstall
uninstall:	$(USER) $(RM) $(HOME)/PiFunk/bin/pifunk.out
						$(USER) $(RM) $(HOME)/PiFunk/bin/pifunk.bin
						$(USER) $(RM) $(HOME)/PiFunk/bin/pifunk

.PHONY:	clean
clean:	cd $(HOME)/PiFunk/
				$(USER) $(RM) $(OBJECTS)
				$(USER) $(RM) $(EXECUTABLES)

.PHONY: 		assistant
assistent:	cd $(HOME)/PiFunk/bin/
						$(USER) ./pifunk -a

.PHONY: 	menu
menu:			cd $(HOME)/PiFunk/bin/
					$(USER) ./pifunk -u

.PHONY: 	help
help:			cd $(HOME)/PiFunk/bin/
					$(USER) ./pifunk -h

.PHONY: 	run
run:			cd $(HOME)/PiFunk/bin/
					$(USER) ./pifunk -n sound.wav -f 26.9650 -s 22050 -m fm -p 7 -c callsign -g 7 -d 14 -b 15.0 -t 1 -x off -l 0

.PHONY: 	run+
run++:			cd $(HOME)/PiFunk/bin/
					$(USER) ./pifunk++ -n sound.wav -f 26.9650 -s 22050 -m fm -p 7 -c callsign -g 7 -d 14 -b 15.0 -t 1 -x off -l 0
