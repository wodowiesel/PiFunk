# PiFunk Radio Transceiver for CB and PMR446  
Early Experimental! 

(based on PiFM/AM-Script)

get it with : 
gitclone https://github.com/silicator/pifunk
cd pifunk
make

compile with:
gcc -lm -std=c99 pifunk.c -o pifunk 

run with:
sudo pifunk sound.wav 100.0000 22500 fm callsign
