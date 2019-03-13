# README

![alt text](https://raw.githubusercontent.com/silicator/PiFunk/master/docs/favicon.ico "Logo PiFunk")

## PiFunk Radio Transmitter in FM/AM for CB and PMR446 etc.

**Early Experimental!**

based on PiFM/AM-Scripts
___

### Preparations:

get this program via: 

`git clone https://github.com/silicator/PiFunk`

1-Wire by default BCM4 setting needs to be activated in boot-config for autostart

via command: `sudo modprobe w1-gpio,gpiopin=4`

Using w1-gpio sometimes needs a 4.7 kΩ pullup resistor connected on GPIO pin 

(if you have problems deactivate 1-wire config!)

manually open with nano-editor: `sudo nano /boot/config.txt` (i provide one too)

add line: `dtoverlay=w1-gpio,gpiopin=4,pullup=0` (add pullup=1 if needed)
___

### Build:

You will need some libraries for this:

`sudo apt update` for system updates

`sudo apt upgrade` for system upgrades

`sudo apt-get install libsndfile-dev`

`sudo apt-get install python-dev python3-dev` for py3

[RPi-lib](https://pypi.org/project/RPi.GPIO/) (i use v0.6.5 from Nov 2018)

`sudo pip-3.2 install RPi.GPIO` for Py3 (easiest way)

`sudo pip install RPi.GPIO` for Py2

or alternative ways: `sudo apt-get -y install python3-rpi.gpio`

`wget https://pypi.python.org/packages/source/R/RPi.GPIO/RPi.GPIO-0.6.5.tar.gz`

then extract `tar -xvf RPi.GPIO-0.6.5.tar.gz` and install

then go to directory:

`cd PiFunk`

compile with:

GNU installer `sudo apt-get install gcc`

`-lm` flag for math lib is obligatory!

`-g` flag for debugger (optional)

`-lsndfile` for ALSA snd lib

`-std=c99` (sometimes gnu99) for C-standard 

command:

`gcc -g -std=c99 -lm -lsndfile pifunk.c -o pifunk pifunk.o pifunk.a pifunk.out`

`make`

`make install`
___

### Usage:

run with admin/root permissions:

Arguments: `[filename (.wav)] [freq (MHz)] [samplerate (kHz)] [modulation (fm/am)] [callsign (optional)] `

extra single Arguments:

[`menu`] as step-by-step assistent

[`help`] for more infos and arguments

Use '. dot' as decimal-comma separator! 

default: `sudo pifunk sound.wav 100.000 22050 fm callsign`

Radio works with *.wav-file with 16-bit @ 22050.000 [Hz] mono / 1-700+ MHz range.

CTSS-Tones for PMR can be found here [CTSS](ctsspmr.csv) 

___

### Warnings:

- Use (original) power supply 10 W, 5V @ ~2 A or ~5 V/500 mA via miniUSB 2.0 or 5.5 V Pins possible)

- PWM on GPIO 4/Pin 7 @ 4 mA (50 mA max. on ALL pins or 16 per bank!!!)

(in example: Pi B+ v1.2 @ 700 MHz/512 MB RAM on ARM processor bcm2835-v1.55)

for more Specifications just visit [Adafruit](http://www.adafruit.com)

- Antenna should be grounded if possible (PIN 9 right one next to GPIO4)![Pinout](docs/pinout-gpio-pib+.jpg)

- You can try to smooth it out with a 1:X-balloon if using long HF antenna

- Dummy-load: 1-100 W @ 50 Ohm "cement" or similar with cooling-ribs with fan for testing.

- For transmission you should use tested Antennas! 

- Tip: You could use just a copper wire for 2m/70cm-band or 
other lambda(1/4)-antennas (17.5cm/6.9in for PMR)
___

### Disclaimer:

- Private Project! Work in Progress (WIP)

- I'm not a professional so **NO guarantees or warranty** for any damage or similar!!

- Usage at your **own risk** !!

- Check laws of your country first! Some Frequencies are prohibited or need a Ham-License!

- Pi operates with square-waves (²/^2)!!- Use Low-/High-Band-Pass-Filters with ~10 uF caps 
with solenoids or resistors/diodes to prevent transmitting (TX) simultaneously on permitted frequencies!

* Help / Testers and Feedback always appreciated!*

* Thank you and have fun!*
___

### Links:

[GitPage](https://silicator.github.io/PiFunk/)

[Readme Guideline](README.md)

[Contribution Guideline](docs/CONTRIBUTING.md)

[Code of Conduct Guideline](docs/CODE_OF_CONDUCT.md)

[Copying Guideline](docs/COPYING.md)

[License Guideline](LICENSE.md) under Open-Source GPLv3.0

Would appreciate beeing Named in the Source
