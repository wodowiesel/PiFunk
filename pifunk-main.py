# free Band combo (HAM): listener/transmitter as beacon, gps, internet, relais, aprs by server-client (all-in-one-version)
# supports UKW radio fm/am , ltp , 433, emg, cb, pmr, vhf, ts2/3, mp3/wave-Files , YT, RDS, microphone (usb&jack) etc.

# !/usr/bin/python
# Imports


import os
import sys
# import datetime
import date
import time
import RPi.GPIO as GPIO
import json


# Process
from subprocess import call


def play_sound( filename, freq):
   call(["./pifunk ", filename, freq])
   printf( " Playing File: " + filename + " on Frequency: (MHz) " + freq)
   return filename, freq
   #continue
   
# Commands:
#filename ="sound.wav"
# freq=Frequency  
# sudo ./pifunk sound.wav 100.0 
#22050 stereo 

# play MP3
# ffmpeg -i sound.mp3 -f s16le -ar 22.05k -ac 1 | sudo ./pifunk -(freq and file input)

# Broadcast from a (usb) microphone
# arecord -d0 -c2 -f S16_LE -r 22050 -twav -D copy | sudo ./pifunk 


