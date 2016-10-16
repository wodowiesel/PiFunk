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
#import numpy
import socket
import subprocess
from subprocess import call
import io
import math
#import scipy.io.wavfile as wavfile
#import matplotlib.pyplot as plt

# RPi & GPIO lib bind
#sudo apt-get install python-rpi.gpio
#sudo python setup.py install
from RPi._GPIO import *
try: 
 import RPi.GPIO as GPIO

## def variables
#int = channels 
#float freq = Frequency 
#char *filename


## function Play file

#def play_file (filename, freq):
  #print (" testing play_file...")
  #filename = input(" Enter Filename: ")
  #freq = input(" Enter Frequency in MHz: ")
  #call ([" sudo ./pifunk ", filename, freq])
  #call ([" sudo ./pifm ", sound.wav, -freq])
  #print ( " Playing file (*.wav): " + filename + " on frequency (MHz):  " + freq)   
  #return 
   
#print (" testing script")


## basic operations
# continue
# break
# sleep(1)

   
## Commands:
# standard freq=100.0 or 103.3
# sudo ./pifunk sound.wav 100.0
# sudo ./pifm sound.wav 100.0
 
## play MP3
# ffmpeg -i sound.mp3 -f s16le -ar 22.05k -ac 1 | sudo ./pifunk -100.0

## Broadcast from a (usb) microphone, stereo
# arecord -d0 -c2 -f S16_LE -r 22050 -twav -D copy | sudo ./pifunk 100.0

## streams audio on network
#port=80
# microphpone device
#card=0
#subdevice=0
# arecord -D hw:${card},${subdevice} -f S16_LE -r 22050 -t wav | sudo nc -1./pifunk 100.0 $port

 
 
## run another py script from shell-terminal (holds main script i think?!
#subprocess.run(["sudo","python", "pifunk-pmr.py"])
#subprocess.run(["python", "pifunk-cb.py"])
#subprocess.run(["python", "pifunk-temp.py"])
#subprocess.run(["sudo","python", "pifm.py"]

