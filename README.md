README

[logo]:https://raw.githubusercontent.com/silicator/PiFunk/master/docs/favicon.ico

# PiFunk Radio Transmitter in FM/AM for CB and PMR446 

**Early Experimental!**

- based on PiFM/AM-Script

___

## Preparation:

get this program via: 

`git clone https://github.com/silicator/PiFunk`

1-Wire by default BCM4 setting needs to be activated in boot-config for autostart

via command: `sudo modprobe w1-gpio,gpiopin=4`

(if you have problems deactivate 1-wire config!)

manually open with nano-editor: `sudo nano /boot/confiig.txt` (i provide one too)

add line: `dtoverlay=w1-gpio,gpiopin=4,pullup=0` (add pullup=1 if nedded)

Using w1-gpio needs a 4.7 kΩ pullup resistor connected between GPIO pin 

___

## Build:

You will need ALSA library for this:

`sudo apt-get install libsndfile-dev`

then go to directory:

`cd PiFunk`


compile with:

-lm flag for math lib obligatory

-g for debugger

-lsndfile for ALSA snd lib

sometimes -std=gnu99 

`gcc -lm -g -std=c99 -lsndfile pifunk.c -o pifunk pifunk.o pifunk.a`

`make clean`

`make`

`make install`

___

## Usage:

run with admin/root permissions:

Arguments: [filename (*.wav)] [freq (MHz)] [samplerate (kHz)] [modulation (fm/am)] [callsign (optional)] 

extra single Arguments:

[menu] as step-by-step assistent

[help] for more infos and arguments

Use '. dot' as decimal-comma separator! 

default: `sudo pifunk sound.wav 100.0000 22050 fm callsign`

Radio works with *.wav-file with 16-bit @ 22050.0 [Hz] mono / 1-700 MHz range.

___

## Warnings:

- Use power supply (~5 V via USB) with enough specifications only! 

- PWM on GPIO 4/Pin 7 @ ~500 mA max. (in example Pi B+ v1.2 @ 700 MHz ARM processor bcm2835-v1.55)

- Antenna should be grounded if possible!

- Use Low-/High-Band-Pass-Filters with ~10 uF caps  resistors/diodes 

- Pi operates with square-waves (²/^2) to prevent transmitting simultaneously on permitted frequencies!

- You can try to smooth it out with a 1:X-balloon.

- Dummy-load: 4-100 W @ 50 Ohm "cement" or similar with cooling-ribs with fan for testing.

- For transmission you should use tested Antennas! 

- Tip: can use just a copper wire i.e. a 2m/70 cm or other lambda(1/4)-antenna (17.5 cm/6.9 in for PMR)

___

## Disclaimer:

- Private Project! Work in Progress (WiP)

- I'm not a professional so **NO guarantees or warranty** for any damage or similar!!

- Usage at your **own risk** !!

- Check laws of your country! Some Frequencies are prohibited or need a Ham-Licence!

- Testers and feedback appreciated!

*Thank you and have fun!*

___

## Links:

[GitPage](https://silicator.github.io/PiFunk/)

[Readme guideline](README.md)

[Contribution guideline](docs/CONTRIBUTING.md)

[Code of Conduct guideline](docs/CODE_OF_CONDUCT.md)

[Copying guideline](docs/COPYING.md)

[License guideline](docs/LICENSE.md) under gpl
