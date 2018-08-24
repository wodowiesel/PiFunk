README
# PiFunk Radio Transmitter in FM/AM for CB and PMR446 

Early Experimental! 
based on PiFM/AM-Script

get this programm via: 
git clone https://github.com/silicator/PiFunk

1-Wire by default BCM4 setting needs to be activated in boot-config for autostart
via command: sudo modprobe w1-gpio,gpiopin=4 

manually open with nano-editor: sudo nano /boot/confiig.txt
add line: dtoverlay=w1-gpio,gpiopin=4,pullup=0 (add pullup=1 if nedded)

Using w1-gpio needs a 4.7 kΩ pullup resistor connected between GPIO pin and
a 3.3 V supply header pin 1 or 17 is needed for more complex circuits or leds.

You will need ALSA libary for this:
sudo apt-get install libsndfile-dev

then go to directory:
cd PiFunk

compile with:
-lm flag for math lib obligatory,
-g for debugger
-lsndfile for ALSA snd lib
sometimes -std=gnu99 

gcc -lm -g -std=c99 -lsndfile pifunk.c -o pifunk pifunk.o pifunk.so 
make clean
make
make install

run with admin/root permissions:
Arguments: [filename (wav)] [freq (MHz)] [samplerate (kHz] [modulation (fm/am)] [callsign (optional)] 
extra single Arguments:
[menu] as step-by-step-asistent
[help] for more infos and arguments

default: sudo pifunk sound.wav 100.0000 22500 fm callsign

Radio works with *.wav-file with 16-bit @ 22500.0 [Hz] mono / 1-700 MHz range.
Use '. dot' as decimal-comma seperator! 

Pi oparates with square-waves (²/^2) PWM on GPIO 4/Pin 7 @ ~500 mA 
for example (Pi B+ v1.2 @ 700 MHz ARM processor bcm2835-v1.55)

Use power supply with enough specs only! 

Use Low-/Highbandpassfilters and/or ~10 uF-caps 
and resistors/diodes to prevent transmitting parallel on permitted frequencies (squarewave!).
You can try to smooth it out with a 1:1-baloon.

Dummyload: 4-100W @ 50 Ohm "cement" or with cooling-ribs with fan for testing.
Antanna should be grounded if possible!
For transmission you can use just a copper wire or example a 2m/70 cm or other lamda(1/4)-antenna
(depends on what band you wannt to transmit!)

Check laws of your country! 

Testers and feedback apreciated!

Thank you

[GitPage](https://silicator.github.io/PiFunk/)

[Readme guideline](README.md)

[Contribution guideline](docs/CONTRIBUTING.md)

[Code of Conduct guideline](docs/CODE_OF_CONDUCT.md)

[Copying guideline](docs/COPYING.md)

[License guideline](docs/LICENSE.md)
