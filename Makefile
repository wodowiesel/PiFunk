# pifunk makefile
# should run with sudo or root rights
USER=sudo
CC=gcc
# use gnu c compiler -std=gnu99 same as -std=iso9899:1999 alternative
STD_CFLAGS=-Wall -std=c99 -g3 -ggdb -v -Iinclude -I/opt/vc/include -fPIC pifunk.c -O3
CXX=g++
CXXFLAGS=-Wall -std=c++17 -g3 -ggdb -v -Iinclude -I/opt/vc/include -fPIC pifunk.c -O3
ASFLAGS=-s
LDFLAGS=-lm -lpthread -lgthread -lsndfile -D_USE_MATH_DEFINES -D_GNU_SOURCE -D_POSIX_C_SOURCE=200809L -L/opt/vc/lib -lbcm_host

LDLIBS=-Llib -shared
PATH=/home/pi
MAKEINFO=makeinfo
EXECUTABLE=pifunk
VERSION=0.1.7.6
#Determine the hardware platform.
#Enable ARM-specific options only on ARM, and compilation of the app only on ARM
RM=rm -f
PCPUI:=$(shell cat /proc/cpuinfo | grep Revision | cut -c16-) #my rev: 0010
UNAME:=$(shell uname -m) #linux

ifeq ($(UNAME), armv5l)
CFLAGS=-march=armv6 -mtune=arm1176jzf-s -mfloat-abi=hard -mfpu=vfp -ffast-math -DRASPI=0
TARGET=pi0
endif

ifeq ($(UNAME), armv6l)
CFLAGS=-march=armv6 -mtune=arm1176jzf-s -mfloat-abi=hard -mfpu=vfp -ffast-math -DRASPI=1
TARGET=pi1
endif

ifeq ($(UNAME), armv7l)
CFLAGS=-march=armv7-a -mtune=arm1176jzf-s -mfloat-abi=hard -mfpu=vfp -ffast-math -DRASPI=2
TARGET=pi2
endif

ifeq ($(UNAME), armv8l)
CFLAGS=-march=armv7-a -mtune=arm1176jzf-s -mfloat-abi=hard -mfpu=vfp -ffast-math -DRASPI=3
TARGET=pi3
endif

ifeq ($(UNAME), armv8l)
CFLAGS=-march=armv8-a -mtune=cortex-a53 -mfloat-abi=hard -mfpu=vfp -ffast-math -DRASPI=4
TARGET=pi4
endif

ifeq ($(UNAME), armv7l)
CFLAGS=-march=native -mtune=native -mfloat-abi=hard -mfpu=vfp -ffast-math -DRPI
TARGET=rpi
endif

ifeq ($(UNAME), armv7l)
CFLAGS=-march=native -mtune=native -mfloat-abi=hard -mfpu=vfp -ffast-math -DRASPBERRY
TARGET= raspberry
endif

ifeq ($(UNAME), NULL)
CFLAGS=-march=native -mtune=native -mfloat-abi=hard -mfpu=vfp -ffast-math -DRASPI
TARGET=raspi
endif

#@echo Compiling PiFunk

#pifunk.info: pifunk.texi
#						 $(USER) $(MAKEINFO)

pifunk.i:	pifunk.c
					$(USER) $(CC) $(STD_CFLAGS) $(LDLIBS) $(LDFLAGS) $(CFLAGS)-E -C -o include/pifunk.i

pifunk.S:	pifunk.c
					$(USER) $(CC) $(STD_CFLAGS) $(LDLIBS) $(LDFLAGS) $(CFLAGS) $(ASFLAGS)-c -o lib/pifunk.S

pifunk.s:	pifunk.c
					$(USER) $(CC) $(STD_CFLAGS) $(LDLIBS) $(LDFLAGS) $(CFLAGS) $(ASFLAGS)-o lib/pifunk.s

pifunk.o:	pifunk.c
					$(USER) $(CC)$(STD_CFLAGS) $(LDLIBS) $(LDFLAGS) $(CFLAGS)-o lib/pifunk.o

pifunk.a:	pifunk.c
					$(USER) $(CC) $(STD_CFLAGS) $(LDLIBS) $(LDFLAGS) $(CFLAGS)-o lib/pifunk.a

pifunk.lib:	pifunk.c
						$(USER) $(CC) $(STD_CFLAGS) $(LDLIBS) $(LDFLAGS) $(CFLAGS)-o lib/pifunk.lib

pifunk.so:	pifunk.c
						$(USER) $(CC) $(STD_CFLAGS) $(LDLIBS) $(LDFLAGS) $(CFLAGS)-o lib/pifunk.so

pifunk.out:	pifunk.c
						$(USER) $(CC) $(STD_CFLAGS) $(LDLIBS) $(LDFLAGS) $(CFLAGS)-o bin/pifunk.out

pifunk.bin: pifunk.c
						$(USER) $(CC) $(STD_CFLAGS) $(LDLIBS) $(LDFLAGS) $(CFLAGS)-o bin/pifunk.bin

pifunk:	pifunk.c
				$(USER) $(CC) $(STD_CFLAGS) $(LDLIBS) $(LDFLAGS) $(CFLAGS)-o bin/pifunk

install:	$(USER) cd $(PATH)/PiFunk
					$(USER) install -m 0755 pifunk $(PATH)/bin

uninstall:	$(USER) $(RM) $(PATH)/bin/pifunk $(PATH)/bin/pifunk.bin

.PHONY:	clean
clean:	$(USER) $(RM) bin/pifunk.out lib/pifunk.o
