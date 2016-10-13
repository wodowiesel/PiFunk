# free Band combo (HAM): listener/transmitter as beacon, gps, internet, relais, aprs by server-client (all-in-one-version)
# supports UKW radio fm/am , ltp , 433, emg, cb, pmr, vhf, ts2/3, mp3/wave-Files , YT, RDS, microphone (usb&jack) etc.

# Imports

# !/usr/bin/python

import time
import os
import date

# int freq=frequency # in MHz

# Process
from subprocess import call

#call play_sound
#def play_sound( filename ):
 #  call(["./pifunk ", filename, freq])
   #return
   
# Commands:
   
printf(" PiFunk ")
  
# sudo ./pifunk sound.wav 100.0 22050 stereo

# play MP3
#ffmpeg -i sound.wav -f s16le -ar 22.05k -ac 1 | sudo ./pifunk -100.0 # (freq and file input)

# Broadcast from a usb microphone
# arecord -d0 -c2 -f S16_LE -r 22050 -twav -D copy | sudo ./pifunk -100.0