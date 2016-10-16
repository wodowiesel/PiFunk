## free Band combo (HAM): listener/transmitter as beacon, gps, internet, relais, aprs by server-client (all-in-one-version)
## supports UKW radio fm/am , ltp , 433, emg, cb, pmr, vhf, ts2/3, mp3/wave-Files , YT, RDS, microphone (usb&jack) etc.

# !/usr/bin/python
## Imports


import os
import sys
import datetime
#import date
import time
import random
import json
import numpy
import socket
import subprocess
import call
import io

import scipy.io.wavfile as wavfile
#import matplotlib.pyplot as plt

## def variables
int channels 
float freq = Frequency 
char *filename


## function Play file
def play_file( filename, freq):
   
  call(["./pifunk ", filename, freq])
 # call(["./pifm ", filename, -freq])
   
   return filename, freq
   
print ( " Playing file (wave): " + filename + " on frequency (MHz):  " + freq)   
return

# basic operations
# continue
# break
# sleep(1)

   
## Commands:

# sudo ./pifunk sound.wav 100.0 
 
## play MP3
# ffmpeg -i sound.mp3 -f s16le -ar 22.05k -ac 1 | sudo ./pifunk -100.0

## Broadcast from a (usb) microphone, stereo
# arecord -d0 -c2 -f S16_LE -r 22050 -twav -D copy | sudo ./pifunk 100.0

## streams audio on network
# port=80
 # microphpone device
# card=0
# subdevice=0
# arecord -D hw:${card},${subdevice} -f S16_LE -r 22050 -t wav | sudo nc -1./pifunk 100.0 $port

 
 
## run another py script from shell-terminal (holds main script i think?!
# or from subprocess import bla
# subprocess.run(["python", "pifunk-pmr.py"])


