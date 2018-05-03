# PiFunk Radio Transceiver for CB and PMR446 
Early Experimental! 

(based on PiFM/AM-Script)

get this programm via: 
gitclone https://github.com/silicator/pifunk


1-Wire(by default BCM4) setting needs to be activated in boot/config.txt
via command: sudo modprobe w1-gpio,gpiopin=4 
manually: dtoverlay=w1-gpio,gpiopin=4  (add  pullup=1 if nedded)

Using w1-gpio needs a 4.7 kΩ pullup resistor connected between GPIO pin and
a 3.3 V supply (header pin 1 or 17) is needed for more complex circuits.

then go to directory:
cd pifunk
make clean
make

compile with:
gcc -lm -std=c99 pifunk.c -o pifunk 

run with admin/root permissions:
sudo pifunk sound.wav 100.0000 22500 fm callsign

Radio works with *.wav-file with 16-bit @ 22500.0 [Hz] mono / 1-700.00000 MHz frequency-range. 
Use '. dot' as decimal-comma seperator! 

Arguments: [filename] [freq] [samplerate] [modulation (fm/am)] [callsign (optional)] 

[menu] as step-by-step-asistent
or [help] for more infos and arguments

Pi oparates with square-waves (²/^2) PWM on GPIO 4/Pin 7 @ ~500 mA (Pi 2B @ 700 MHz ARM processor)
Use power supply with enough specs only! 
Use Low-/Highbandpassfilters and/or ~10 uF-caps and resistors/diodes 
to prevent transmitting on permitted frequencies.



You can try to smooth it out with 1:1 baloon or dummyload 50W @ 50 Ohm for testing.
For transmission you can use just a wire or a 2m/70 cm antenna.

Check laws of your country! 

Thank you


