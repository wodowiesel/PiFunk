# README

![alt text](https://raw.githubusercontent.com/silicator/PiFunk/master/docs/favicon.ico "Logo PiFunk")

## PiFunk Radio Transmitter - in FM/AM for HAM-Bands

**Early Experimental!**

### Acknowledgements

based on PiFM/AM-Scripts
___

### Configurations

1. Get this program via git or download it:

`git clone https://github.com/silicator/PiFunk`

2. To configure the Pi for modules via menu (I2C, UART etc.): `sudo raspi-config`

or manually via command: `sudo modprobe w1-gpio,gpiopin=4`

Using w1-gpio sometimes needs a 4.7 - 10 kΩ pullup resistor connected on GPIO Pin

(if you have problems deactivate 1-wire config!)

1-Wire by default BCM4 setting needs to be activated in boot-config for autostart

3. Manually open with nano-editor: `sudo nano /boot/config.txt` (i provide one too)

check/add lines:

`dtoverlay=w1-gpio,gpiopin=4,pullup=0` add pullup=1 if needed

`dtoverlay=audio=on` for bcm audio

optional:

`dtoverlay=i2c1-bcm2708` for I2C Bus

`enable_uart=1` for UART RX & TX

`init_uart_baud=9600`

`dtparam=spi=on` for SPI support

`dtoverlay=pps-gpio,gpiopin=18` for GPS-device

Listen to GPS 1 PPS signal for Pi Clock (PIN 18) sync

`sudo nano /etc/modules`

`pps-gpio` Add this line to the modulefile

5. Save your edits with ctrl-o <return/enter> then exit with <ctrl-x>
___

### Installations

5. First update & upgrade system:

`sudo apt update` for system updates

`sudo apt upgrade` for system upgrades

6. You will need some libraries for this:

a) `sudo apt-get install libsndfile1-dev` or download it directly [SND](https://packages.debian.org/de/sid/libsndfile1-dev)

b) `sudo apt-get install libraspberrypi-dev raspberrypi-kernel-headers` for Kernel & Firmware

c) `sudo apt-get install python-dev python3-dev` for py3

d) [RPi-lib](https://pypi.org/project/RPi.GPIO/) (I use v0.6.5 from Nov 2018, also in repo)

`sudo wget https://pypi.python.org/packages/source/R/RPi.GPIO/RPi.GPIO-0.6.5.tar.gz`

then extract `tar -xvf RPi.GPIO-0.6.5.tar.gz`

and install it:

`sudo pip-3.7 install RPi.GPIO` for Py3 (easiest way)

or `sudo pip install RPi.GPIO` for Py2

or alternative ways: `sudo apt-get -y install python3-rpi.gpio`

7. Compiler installation:

a) GNU C Compiler  `sudo apt-get install gcc`

b) GNU GDB Debugger `sudo apt-get install gdbserver`

8. `sudo reboot` then reboot to apply the changes

___

### Build
9. Navigate to directory:

a) `cd PiFunk` with default path: `/home/pi/PiFunk`

b) GCC Compiler flags:

`-g3` for debugger informations (0-3 level, 2 is default)

`-Wall` for debug warning informations

`-v`	Print compilation verbose infos

`-DRASPI1` defines the macro to be used by the preprocessor (here the PI model 0-4)

 -> will be detected my the make-file via the type of the ARM-Processor

  (other macros possible if in the C-code implemented)

`-std=c99` (sometimes gnu99 or as iso -std=iso9899:1999) for C99-standard or -std=c++11 or 14

`-lm` for math-lib is obligatory!

`-Iinclude ` for using include-directory with headerfiles

`-lsndfile` -l links libname for ALSA "snd"-lib

`-Llib` for using library-directory

`-c` for compiling without linking for making object

`-shared` for generating shared object libraries

`-fPIC` for generating position independent code (PIC) for bigger programs

`-O3` for Optimization Stage 1-3 (memory, speed etc.) via compiler

`-o` for output-filename flag

`-D_USE_MATH_DEFINES` for math lib

`-lbcm_host` for loading firmware v1.20190718 located in /opt/vc/include/

10. Generating libraries:

a) Image of the GCC Flow-diagram for generating [Libraries](docs/GCC_Schema.jpg)

*.c=C-SourcCcode, *.h=headerfile, *.i=assembled preprocessor-C-Code, *.s= preprocessed assembler code,

*.o= compiled objcet, *.lib=library object, *.a=archive object, *.so=shared dynamic library object,

*.out=default binary, pifunk(.bin)= executable binary (program)

b) manually compiling/linking libraries:

`sudo gcc -g3 -Wall -std=c99 -O3 -lm -Iinclude -Llib -lsndfile -fPIC pifunk.c -shared -o include/pifunk.i lib/pifunk.s lib/pifunk.o lib/pifunk.a lib/pifunk.lib lib/pifunk.so`

c) manually compiling/linking executable binary:

`sudo gcc -g3 -Wall -std=c99 -O3 -lm -Iinclude -Llib -lsndfile -fPIC pifunk.c -shared -o bin/pifunk.out bin/pifunk`

 d) optional:
 -march=armv6l architecture version of ARM, -mtune=arm1176jzf-s architecture type tuning ("march=native" is auto option)

 -mfloat-abi=hard floating-point ABI to use, Permissible values are: ‘soft’, ‘softfp’, ‘hard’

 -mfpu=vfp floating point hardware module

 -ffast-math increase speed for float ops and outside the IEEE-standard and deactivates errno-functions

`sudo make` with pre-configured flags for compilation for all Pi's

`sudo clean` for removing *.out files if necessary

___

### Preparations

11. Hardware-Setup:

a) - Use (original) power supply 10 W, 5 V @ ~2 A or ~5 V/500 mA via miniUSB 2.0 or 5 V Pins possible)

b) - Check Specifications: my Pi B+ v1.2 @ 700 MHz/ 512 MB RAM on ARM processor with driver bcm2835-v1.55

 -> SoC from Broadcom	depending on pi model: BCM2835	BCM2836	BCM2837	BCM2837B0	BCM2837	BCM2837B0	BCM2711

  for more infos on other boards just visit [Adafruit](http://www.adafruit.com)

  or [Wikipedia Spec Summary](https://de.wikipedia.org/wiki/Raspberry_Pi)

c) - Antenna to GPCLK0 (GPIO 4, PIN 7) for PWM (Pulse with Modulation)

  @ 2-4 mA (max. 50 mA on ALL PINs and 16 per bank!)

- Antenna should be grounded (see Pinout image) to prevent noise and other problems

- For transmission you should use tested/certified antennas with mounts (BNC/SDA/PL - m/f) if possible

- Tip: You could use just a copper wire for antenna:

 CB 11 m-Band (lambda/2, 5.5 m, 216.535" in") and 70 cm-Band (PMR) (lambda(1/4), 17.0 cm, 6.7" in)

![Pinout](docs/pinout-gpio-pib+.jpg)

d) - You can try to smooth it out with a 1:X (3-9)-balun if using long HF antenna

- Dummy-load: 1-100 W @ 50 Ohm "cement" or similar (aluminium case) with cooler for testing

e) - For handling overheating of the Pi's processor use cooling-ribs with fan (5 V DC/0.2 A - 20x20 mm)

  you can overclock the Pi if you want to on own risk but it's not recommended

f) - RTC: Module DS3231 uses 3.3 V (PIN 1), SDA0 (PIN 3, GPIO0 on I2C), SCL0 (PIN 5, GPIO1 on I2C) & GND (PIN 9)

-> need to activate I2C in pi config!

![RTC](docs/RTC-top.jpg)

g) - GPS: Module Neo 7M uses

  5 V (PIN 4), GND (PIN 6), RX to UART-TXD (GPIO 14 PIN 8), TX to UART-RXD (GPIO 15, PIN 10), PPS to PCM_CLK (GPIO 18, PIN 12)

  it prints in NMEA format so change config `ttyAMA0` to `tty1`

  `sudo cat /dev/ttyAMA0` or alternative `sudo cat /dev/ttyS0`

-> need to activate UART (serial0) in pi config! Yes here crosswiring!! -> (RX of GPS receives what Pi TX'ed)

![GPS](docs/GPS-Neo7M.jpg)

h) - Morsecode-table:

![Morsecode](docs/morsecodeCW.jpg)

___

### Run

12.Run with admin/root permissions:

Arguments: would be best to input in this specific order to prevent problems

Use '. dot' as decimal-comma separator!

`[-n <filename (.wav)>] [ -f <freq (MHz)>] [-s <samplerate (kHz)>] [-m <modulation (fm/am)>] [-c <callsign (optional)>] [-p <power 0-7)>]`

extra single Arguments: -> no further argument needed

`[-a]` for assistant in step-by-step

`[-h]` for help with more infos and arguments

`[-u]` for extra menu (csv, commandline)

default: `sudo ./pifunk -n sound.wav -f 446.006250 -s 22050 -m fm -c callsign -p 7`

Radio works with .wav-file with 16-bit @ 22050.000 [Hz] mono / 0.1-700 to 1500 MHz range depending on the Pi

it's recommended not to transmit on frequencies higher than the processor speed (at the moment)

but results would be interesting to know

explicit CTSS-Tones (38 included) for PMR can be found here: [CTSS](ctsspmr.csv)

___

### Disclaimer

13. Warnings/Caution:

- Private Project! Work in Progress (WIP)

- I'm not a professional so **NO guarantees or warranty** for any damage etc.!!

- Usage at your **own risk** !!

- Check laws of your country first! Some Frequencies are prohibited or need a HAM-License!

- Pi operates with square-waves (²/^2)!! Use Low-/High-Band-Pass-Filters with dry (not electrolytic) capacitors

  (C=10-100 pF) with solenoid (ring) toroid (ferrit) chokes (B=10-50 uH like the FT37-43)

  or resistors (R=10 kOhm), diodes to prevent backflow

  transmission (TX) simultaneously on permitted frequencies! -> [Bandpass-Diagram](docs/filter_600.jpg)


* Help / Testers and Feedback always appreciated! :)

* Thank you and have fun 73!

___

### Links:

14.) additional Guidelines

[GitPage](https://silicator.github.io/PiFunk/)

[Readme Guideline](README.md)

[Contribution Guideline](docs/CONTRIBUTING.md)

[Code of Conduct Guideline](docs/CODE_OF_CONDUCT.md)

[Copying Guideline](docs/COPYING.md)

[License Guideline](LICENSE.md) under Open-Source GPLv3.0

Would appreciate being named in the source.
