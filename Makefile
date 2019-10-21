# pifunk Makefile
# should run with sudo or root rights
USER=sudo
CXX=g++
CCN=gcc-9.2.0

# use gnu c compiler, -std=gnu99 is c99 -std=iso9899:1999 with extra gnu extentions
CXXFLAGS=-Wall -Werror -std=gnu++99 -pedantic-errors -g3 -ggdb3 -v -Iinclude -I/opt/vc/include -O3 -fPIC pifunk.c -D_USE_MATH_DEFINES -D_GNU_SOURCE
ASFLAGS=-s
LDFLAGS=-lm -lpthread -lbcm_host -lsndfile -shared
LDLIBS=-Llib -L/opt/vc/lib

PATH=/home/pi #std path
RM=rm -f #remove file or folder

MAKEINFO=makeinfo
EXECUTABLE=pifunk++
VERSION=0.1.7.7e

#Determine the hardware platform
UNAME:=$(shell uname -m) #linux
PCPUI:=$(shell cat /proc/cpuinfo | grep Revision | cut -c16-) #my rev: 0010 -> 1.2 B+
RPI_VERSION:=$(shell cat /proc/device-tree/model | grep -a -o "Raspberry\sPi\s[0-9]" | grep -o "[0-9]") #

$(UNAME)
$(PCPUI)
#$(RPI_VERSION)

#Enable ARM-specific options only
ifeq ($(UNAME), armv5l)
PFLAGS=-march=armv6 -mtune=arm1176jzf-s -mfloat-abi=hard -mfpu=vfp -ffast-math -DRASPI=0
TARGET=pi0
endif

ifeq ($(UNAME), armv6l)
PFLAGS=-march=armv6 -mtune=arm1176jzf-s -mfloat-abi=hard -mfpu=vfp -ffast-math -DRASPI=1
TARGET=pi1
endif

ifeq ($(UNAME), armv7l)
PFLAGS=-march=armv7-a -mtune=arm1176jzf-s -mfloat-abi=hard -mfpu=neon-vfpv4 -ffast-math -DRASPI=2
TARGET=pi2
endif

ifeq ($(UNAME), armv8l)
PFLAGS=-march=armv7-a -mtune=arm1176jzf-s -mfloat-abi=hard -mfpu=neon-vfpv4 -ffast-math -DRASPI=3
TARGET=pi3
endif

ifeq ($(UNAME), armv8l && $(shell expr $(RPI_VERSION) \>= 4), 1)
PFLAGS=-march=armv8-a -mtune=cortex-a53 -mfloat-abi=hard -mfpu=neon-fp-armv8 -ffast-math -DRASPI=4
TARGET=pi4
endif

#old/special pi versions
ifeq ($(UNAME), armv6l)
PFLAGS=-march=native -mtune=native -mfloat-abi=hard -mfpu=vfp -ffast-math -DRPI
TARGET=rpi
endif

ifeq ($(UNAME), armv6l)
PFLAGS=-march=native -mtune=native -mfloat-abi=hard -mfpu=vfp -ffast-math -DRASPBERRY
TARGET=raspberry
endif

#@echo Compiling PiFunk++

#pifunk++.info: pifunk++.texi
#						  $(USER) $(MAKEINFO)
all: pifunk++

pifunk++.i:	pifunk++.cpp
					$(USER) $(CC) $(CFLAGS) $(LDLIBS) $(LDFLAGS) $(PFLAGS)-E -C -o include/pifunk.i

pifunk++.S:	pifunk++.cpp
					$(USER) $(CC) $(CFLAGS) $(LDLIBS) $(LDFLAGS) $(PFLAGS) $(ASFLAGS)-c -o lib/pifunk.S

pifunk++.s:	pifunk++.cpp
					$(USER) $(CC) $(CFLAGS) $(LDLIBS) $(LDFLAGS) $(PFLAGS) $(ASFLAGS)-o lib/pifunk.s

pifunk++.o:	pifunk++.cpp
					$(USER) $(CC)$(CFLAGS) $(LDLIBS) $(LDFLAGS) $(PFLAGS)-o lib/pifunk.o

pifunk++.a:	pifunk++.cpp
					$(USER) $(CC) $(CFLAGS) $(LDLIBS) $(LDFLAGS) $(PFLAGS)-o lib/pifunk.a

pifunk++.lib:	pifunk++.cpp
					$(USER) $(CC) $(CFLAGS) $(LDLIBS) $(LDFLAGS) $(PFLAGS)-o lib/pifunk.lib

pifunk++.so:	pifunk++.cpp
					$(USER) $(CC) $(CFLAGS) $(LDLIBS) $(LDFLAGS) $(PFLAGS)-o lib/pifunk.so

pifunk++.out:	pifunk++.cpp
					$(USER) $(CC) $(CFLAGS) $(LDLIBS) $(LDFLAGS) $(PFLAGS)-o bin/pifunk.out

pifunk++.bin: pifunk++.cpp
					$(USER) $(CC) $(CFLAGS) $(LDLIBS) $(LDFLAGS) $(PFLAGS)-o bin/pifunk.bin

pifunk++:	pifunk++.cpp pifunk.hpp
	$(USER) $(CXX) $(CXXFLAGS) $(LDLIBS) $(LDFLAGS) $(PFLAGS)-o bin/pifunk

.PHONY: 	piversion
piversion:	$(USER) $(UNAME)
		$(USER) $(PCPUI)
		$(USER) $(RPI_VERSION)

.PHONY: 	pifunk++
pifunk++:	pifunk++.cpp pifunk.hpp pifunk++.so
				$(USER) $(CXX) $(CXXFLAGS) $(LDLIBS) $(LDFLAGS) $(PFLAGS)-o bin/pifunk++

.PHONY: 	install
install:	cd $(PATH)/PiFunk
		$(USER) install -m 0755 pifunk++ $(PATH)/bin/

.PHONY: 	uninstall
uninstall:	$(USER) $(RM) $(PATH)/bin/pifunk++ $(PATH)/bin/pifunk++

.PHONY: 	help
help:		cd $(PATH)/PiFunk/bin/
		$(USER) ./pifunk++ -h

.PHONY: 	assistant
assistent:	cd $(PATH)/PiFunk/bin/
		$(USER) ./pifunk++ -a

.PHONY: 	run
run:		cd $(PATH)/PiFunk/bin/
		$(USER) ./pifunk++ -n sound.wav -f 446.006250 -s 22050 -m fm -c callsign -p 7

.PHONY:	clean
clean:	$(USER) $(RM) bin/pifunk++.out lib/pifunk++.so bin/pifunk++
