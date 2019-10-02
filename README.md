# README

![alt text](https://raw.githubusercontent.com/silicator/PiFunk/master/docs/favicon.ico "Logo PiFunk")

## PiFunk Radio Transmitter - with FM/AM-Modulation for HAM-Bands

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

`dtoverlay=pps-gpio,gpiopin=18` for GPS-device pps(puls-pro-second)-support

Sync to GPS 1 PPS signal for Pi PCM-Clock (PIN 12 / GPIO 18 = PCM_CLK / PWM0) for accuracy

`sudo nano /etc/modules` opens modules.conf with text editor (provide one too)

`pps-gpio` Add this line

5. Save your edits with ctrl-o <return/enter> then exit with ctrl-x

___

### Installations

5. First update & upgrade system:

`sudo apt-get update` for system updates

`sudo apt-get upgrade` for system upgrades

6. You will need some libraries for this:

a) `sudo apt-get install libraspberrypi-dev raspberrypi-kernel-headers` for Kernel & Firmware

b) `sudo apt-get install libsndfile1-dev` or download it directly [SND](https://packages.debian.org/de/sid/libsndfile1-dev)

c) `sudo apt-get install python-dev python3-dev` for py3

d) [RPi-lib](https://pypi.org/project/RPi.GPIO/) (I use v0.6.5 from Nov 2018, also in repo)

`sudo wget https://pypi.python.org/packages/source/R/RPi.GPIO/RPi.GPIO-0.6.5.tar.gz`

then extract `tar -xvf RPi.GPIO-0.6.5.tar.gz`

and install it:

`sudo pip-3.7 install RPi.GPIO` for Py3 (easiest way)

or `sudo pip install RPi.GPIO` for Py2

or alternative way: `sudo apt-get -y install python3-rpi.gpio`

7. Compiler installation:

a) GNU C Compiler GCC `sudo apt-get install gcc` or g++

b) GNU Debugger GDB `sudo apt-get install gdbserver`

8. `sudo reboot` then reboot to apply the changes

___

### Build

9. Navigate to directory:

a) `cd PiFunk` with default path: `/home/pi/PiFunk`

b) GCC Compiler flags:

`-g3` for normal GNU compiler debug informations (0-3 level, 2 is default)

`-ggdb3` for GNU debugger informations level 3

`-Wall` for debug all warning informations

`-v` Print compilation verbose informations

`-std=c99` (as iso `-std=iso9899:1999` strict)

`-std=gnu99` with additional gnu extention to c99

(or `-std=gnu++17` or 11/14 as you like when using g++)

`-pedantic-errors` for error console messages if problem between c99 and gnu extentions

`-Iinclude ` for using include-directory with headerfiles

`-I/opt/vc/include/` for loading bcm-header folder

`-L/opt/vc/lib` for loading bcm folder

`-Llib` for using library-directory

`-lm` for math-lib is obligatory!

`-lbcm_host` for loading firmware v1.20190718

`-lpthread` lib for process threads

`-lgthread` lib for graphic threads

`-lsndfile` -l links library name for ALSA "snd"-lib

`-shared` for generating shared object libraries

`-c` for compiling without linking for making object

`-D_POSIX_C_SOURCE=199309L` for posix needed with bcm

`-D_USE_MATH_DEFINES` for math lib

`-DRASPI=1` defines the macro to be used by the preprocessor (here the PIX model or 0-4, else std-values 1-3)

 -> will be detected by the Makefile via the type of the ARM-Processor

 (other macros possible if in the C-code implemented)

`-fPIC` for generating position independent code (PIC) for bigger programs

`-O3` for Optimization Stage 1-3 (memory, speed etc.) via compiler

`-o` for output-filename flag

10. Generating libraries:

a) Image of the GCC Flow-diagram for generating [Libraries](docs/GCC_Schema.jpg)

*.c=C-code, *.h=headerfile, *.i=assembled preprocessor C-code, *.S=assembler-code, *.s=preprocessed assembler-code,

*.o=compiled object, *.lib=library object, *.a=archive object, *.so=shared dynamic library object,

*.out=default binary, pifunk(.bin)=executable binary (program)

b) manually compiling/linking libraries:

`sudo gcc -Wall -std=gnu99 -pedantic-errors -g3 -ggdb3 -Iinclude -Llib -I/opt/vc/include -lbcm_host -lm -lsndfile -lpthread -O3 -fPIC pifunk.c -shared -D_USE_MATH_DEFINES -D_GNU_SOURCE -D_POSIX_C_SOURCE=200809L -DRASPI=1 -o include/pifunk.i lib/pifunk.s lib/pifunk.o lib/pifunk.a lib/pifunk.lib lib/pifunk.so`

c) manually compiling/linking executable binary:

`sudo gcc -Wall -std=gnu99 -pedantic-errors -g3 -ggdb3 -Iinclude -Llib -I/opt/vc/include -lbcm_host -lm -lsndfile -lpthread -O3 -fPIC pifunk.c -shared -D_USE_MATH_DEFINES -D_GNU_SOURCE -D_POSIX_C_SOURCE=200809L -DRASPI=1 -o bin/pifunk.out bin/pifunk`

d) optional Pi-Flags:

 `-march=armv6l` architecture version of ARM ("native" is auto option)

 `-mtune=arm1176jzf-s` special architecture type tuning

 `-mfloat-abi=hard` floating-point ABI to use, Permissible values are: ‘soft’, ‘softfp’, ‘hard’

 `-mfpu=vfp` virtual floating point hardware module

 `-ffast-math` increase speed for float ops and outside the IEEE-standard and deactivates errno-functions

 `sudo piversion` for checking your piversion

e) Makefile commands:

 `sudo install` for installing pifunk files incl. build folder

 `sudo uninstall` for uninstalling pifunk files

 `sudo make` with pre-configured flags for compilation for all Pi's

 `sudo clean` for removing pifunk.out and pifunk.o files in bin folder if necessary

___

### Preparations

11. Hardware-Setup:

a) Use (original) power supply 10 W, 5 V @ ~2 A or ~5 V/500 mA via miniUSB 2.0 or 5 V Pins possible)

b) Check specifications: my Pi B+ v1.2 @ 700 MHz / 512 MB RAM on ARM processor with driver bcm2835-v1.55

-> SoC from Broadcom	depending on pi model: BCM2835,	BCM2836,	BCM2837,	BCM2837B0,	BCM2837,	BCM2837B0,	BCM2711

for more infos on other boards just visit [Adafruit](http://www.adafruit.com)

or [Wikipedia Spec Summary](https://de.wikipedia.org/wiki/Raspberry_Pi)

c) Antenna to GPCLK0 (GPIO 4, PIN 7) for PWM (Pulse with Modulation)

@ 2-4 mA (max. 50 mA on ALL PINs and 16 per bank!)

- Antenna should be grounded (see Pinout image) to prevent noise and other problems

- For transmission you should use tested/certified antennas with mounts (BNC/SDA/PL - m/f) if possible

- Tip: You could use just a copper wire for antenna:

 CB 11 m-Band (lambda/2, 5.5 m, 216.535" in) and 70 cm-Band (PMR) (lambda(1/4), 17.0 cm, 6.7" in)

![Pinout](docs/pinout-gpio-pib+.jpg)

d) You can try to smooth it out with a 1:X (3-9)-balun if using long HF antenna

- Dummy-load: 1-100 W @ 50 Ohm "cement" or similar (aluminium case) with cooler for testing

e) For handling overheating of the Pi's processor use cooling-ribs with fan (5 V DC/0.2 A - 20x20 mm)

you can overclock the Pi if you want to on own risk but it's not recommended

f) RTC: Module DS3231 uses 3.3 V (PIN 1), SDA0 (PIN 3, GPIO0 on I2C), SCL0 (PIN 5, GPIO1 on I2C) & GND (PIN 9)

-> need to activate I2C in pi config!

![RTC](docs/RTC-top.jpg)

g) GPS: Module Neo 7M uses

5 V (PIN 4), GND (PIN 6), RX to UART-TXD (GPIO 14 PIN 8), TX to UART-RXD (GPIO 15, PIN 10), PPS to PCM_CLK (GPIO 18, PIN 12)

it prints in NMEA format so change config `ttyAMA0` to `tty1`

`sudo cat /dev/ttyAMA0` or alternative `sudo cat /dev/ttyS0`

-> need to activate UART (serial 0) in pi config! Yes here crosswiring!! -> (RX of GPS receives what Pi TX'ed)

![GPS](docs/GPS-Neo7M.jpg)

h) Morsecode-table:

![Morsecode](docs/morsecodeCW.jpg)

___

### Run

12. Run with admin/root permissions:

Arguments: would be best to input in this specific order to prevent problems

Use '. dot' as decimal-comma separator!

`[-n <filename (.wav)>] [ -f <freq (MHz)>] [-s <samplerate (kHz)>] [-m <modulation (fm/am)>] [-c <callsign (optional)>] [-p <power 0-7)>]`

extra single menu-flags: -> no further argument needed

`[-a]` for assistant in step-by-step

`[-h]` for help with more infos and arguments

`[-u]` for extra menu (csv, commandline)

default: `sudo ./pifunk -n sound.wav -f 446.006250 -s 22050 -m fm -c callsign -p 7`

Radio works with .wav-file with 16-bit @ 22050.000 [Hz] mono / 0.1-700 to 1500 MHz range depending on the Pi

it's recommended not to transmit on frequencies higher than the processor speed (at the moment) but results would be interesting to know

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

14. Additional Guidelines

[GitPage](https://silicator.github.io/PiFunk/)

[Readme Guideline](README.md)

[Wiki](https://github.com/silicator/PiFunk/wiki)

[Contribution Guideline](docs/CONTRIBUTING.md)

[Code of Conduct Guideline](docs/CODE_OF_CONDUCT.md)

[Copying Guideline](docs/COPYING.md)

[License Guideline](LICENSE.md) under Open-Source GPLv3.0

Would appreciate being named in the source, Thank you.
