CC = gcc
STD_CFLAGS = -Wall -std=c99 -c -g
#-std=gnu99 as alternative
# Enable ARM-specific options only on ARM, and compilation of the app only on ARM
UNAME := $(shell uname -m)

# Determine the hardware platform. Below, pi1 stands for the RaspberryPi 1 (the original one),
# and pi2 stands for both the RaspberryPi 2 and 3.
ifeq ($(UNAME), armv6l)
	CFLAGS = $(STD_CFLAGS) -march=armv6 -mtune=arm1176jzf-s -mfloat-abi=hard -mfpu=vfp -ffast-math -DRASPI=1
	TARGET = pi1
else ifeq ($(UNAME), armv7l)
	CFLAGS = $(STD_CFLAGS) -march=armv7-a -mtune=arm1176jzf-s -mfloat-abi=hard -mfpu=vfp -ffast-math -DRASPI=2
	TARGET = pi2
else
	CFLAGS = $(STD_CFLAGS)
	TARGET = other
endif

ifneq ($(TARGET), other)

app: pifunk.o
	$(CC) -o pifunk pifunk.o -lm -lsndfile

endif

pifunk: pifunk2.o 
	$(CC) -o pifunk pifunk.o -lm -lsndfile

pifunk.o: pifunk.c
	$(CC) $(CFLAGS) pifunk.c

clean:
