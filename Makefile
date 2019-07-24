# pifunk makefile
# should run with sudo or root rights
USER=sudo
CC=gcc# use gnu c compiler -std=gnu99 same as -std=iso9899:1999 alternative
STD_CFLAGS=-Wall -std=c99 -g3 -ggdb -v -O3 -Iinclude -fPIC pifunk.c
CXX=g++
CXXFLAGS=-Wall -std=c++14 -g3 -ggdb -v -O3 -Iinclude -fPIC pifunk.c
ASFLAGS=-s
LDFLAGS=-lm -lpthread#-lgthread
LDLIBS=-Llib -lsndfile -shared
PATH=/home/pi
MAKEINFO=makeinfo
#Determine the hardware platform.
#Enable ARM-specific options only on ARM, and compilation of the app only on ARM

UNAME := $(shell uname -m) #linux
CFLAGS =-march=armv6 -mtune=arm1176jzf-s -mfloat-abi=hard -mfpu=vfp -ffast-math -DRASPI1
TARGET = pi1

ifeq ($(UNAME), armv5l)
CFLAGS=-march=armv6 -mtune=arm1176jzf-s -mfloat-abi=hard -mfpu=vfp -ffast-math -DRASPI0
TARGET=pi0
endif

#ifeq ($(UNAME), armv6l)


#endif

ifeq ($(UNAME), armv7l)
CFLAGS =-march=armv7-a -mtune=arm1176jzf-s -mfloat-abi=hard -mfpu=vfp -ffast-math -DRASPI2
TARGET = pi2
endif

ifeq ($(UNAME), armv8l)
CFLAGS =-march=armv7-a -mtune=arm1176jzf-s -mfloat-abi=hard -mfpu=vfp -ffast-math -DRASPI3
TARGET = pi3
endif

ifeq ($(UNAME), armv8l)
CFLAGS =-march=armv8-a -mtune=cortex-a53 -mfloat-abi=hard -mfpu=vfp -ffast-math -DRASPI4
TARGET = pi4
endif

ifeq ($(UNAME), armv7l)
CFLAGS =-march=native -mtune=native -mfloat-abi=hard -mfpu=vfp -ffast-math -DRPI
TARGET = rpi
endif

ifeq ($(UNAME), armv7l)
CFLAGS =-march=native -mtune=native -mfloat-abi=hard -mfpu=vfp -ffast-math -DRASPBERRY
TARGET = raspberry
endif

ifeq ($(UNAME), armv7l)
CFLAGS =-march=native -mtune=native -mfloat-abi=hard -mfpu=vfp -ffast-math -DRASPI
TARGET = raspi
endif

#pifunk.info: pifunk.texi
#						 $(USER) $(MAKEINFO)

#@echo " Compiling PiFunk "

#pifunk.i: pifunk.c
#				  $(USER) $(CC) $(STD_CFLAGS) $(LDLIBS) $(LDFLAGS) $(CFLAGS) -E -C -o include/pifunk.i

pifunk.S: pifunk.c
					$(USER) $(CC) $(STD_CFLAGS) $(LDLIBS) $(LDFLAGS) $(CFLAGS) $(ASFLAGS) -c -o lib/pifunk.S

pifunk.s: pifunk.c
					$(USER) $(CC) $(STD_CFLAGS) $(LDLIBS) $(LDFLAGS) $(CFLAGS) $(ASFLAGS) -c -o lib/pifunk.s

pifunk.o: pifunk.c
					$(USER) $(CC)$(STD_CFLAGS) $(LDLIBS) $(LDFLAGS) $(CFLAGS) -o lib/pifunk.o

pifunk.a: pifunk.c
					$(USER) $(CC)$(STD_CFLAGS) $(LDLIBS) $(LDFLAGS) $(CFLAGS) -o lib/pifunk.a

pifunk.lib: pifunk.c
				  	$(USER) $(CC)$(STD_CFLAGS) $(LDLIBS) $(LDFLAGS) $(CFLAGS) -o lib/pifunk.lib

pifunk.so: pifunk.c
					 $(USER) $(CC)$(STD_CFLAGS) $(LDLIBS) $(LDFLAGS) $(CFLAGS)-o lib/pifunk.so

pifunk.out: pifunk.c
					  $(USER) $(CC)$(STD_CFLAGS) $(LDLIBS) $(LDFLAGS) $(CFLAGS) -o bin/pifunk.out

pifunk.bin: pifunk.c
						$(USER) $(CC)$(STD_CFLAGS) $(LDLIBS) $(LDFLAGS) $(CFLAGS) -o bin/pifunk.bin

pifunk: pifunk.c
				$(USER) $(CC)$(STD_CFLAGS) $(LDLIBS) $(LDFLAGS) $(CFLAGS) -o bin/pifunk

install: $(USER) cd $(PATH)/PiFunk
				 $(USER) install -m 0755 pifunk $(PATH)/bin

uninstall: $(USER) $(RM) $(PATH)/bin/pifunk $(PATH)/bin/pifunk.bin
#$(RM) = rm -f
.PHONY: clean
clean: $(USER) $(RM) bin/pifunk.out lib/pifunk.o
