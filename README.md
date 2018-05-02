# PiFunk Radio Transceiver for CB and PMR446 
Early Experimental! 

(based on PiFM/AM-Script)

get it with : 
gitclone https://github.com/silicator/pifunk

cd pifunk
make clean
make

compile with:
gcc -lm -std=c99 pifunk.c -o pifunk 

run with admin/root permissions:
sudo pifunk sound.wav 100.0000 22500 fm callsign

Use '. dot' as decimal-comma seperator! 

Arguments: [filename] [freq] [samplerate] [mod (fm/am)] [callsign (optional)] 

or [help] for more infos 

Pi oparates with square-waves (²/^2) PWM on GPIO 4 (Pin 7 @ ~500 mA)
Use power supply with enough specs only!
Check laws of your country! Do not produce transsions on permitted frequencies.
You can try to smooth it out with 1:1 baloon or dummyload 50W @ 50 Ohm for testing.



