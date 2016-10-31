## free Band combo (HAM): listener/transmitter as beacon, gps, internet, relais, aprs by server-client (all-in-one-version)
## supports UKW radio fm/am , ltp , 433, emg, cb, pmr, vhf, ts2/3, mp3/wave-Files , YT, RDS, microphone (usb&jack) etc.

## ARM -Structure on Pi's !!!

# !/usr/bin/python
## Imports


import os
import sys
import socket
import datetime
import time
import io
import math
import threading
import subprocess
import random
import json
from RPi._GPIO import *
try: 
 import RPi.GPIO as GPIO
#import numpy
#import scipy
#import date

#import scipy.io.wavfile as wavfile
#import matplotlib.pyplot as plt

# RPi & GPIO lib bind
#sudo apt-get install python-rpi.gpio
	#sudo python setup.py install



## def variables
int (chan) = channels 
float (freq) = frequency 
char (filename) = file

print("testing funk script")

## function Play file

#def play_files (self, files, freq):
  #print (" Testing play_file... ")
  #file = input(" Enter File-Name (*.wav): ")
  #freq = input(" Enter Frequency in MHz: ")
  #call ([" sudo ./pifunk ", filename, freq])
  #call ([" sudo ./pifm ", sound.wav, -freq])
  #print ( " Playing file (*.wav): " + filename + " on Frequency (MHz):  " + freq)   
  #return self , files , freq


return
## basic operations
# continue
# break
#time.sleep(1)


## Commands:
# standard freq=100.0 or 103.3
# sudo ./pifunk sound.wav 100.0
# sudo ./pifm sound.wav 100.0
# sudo ./pifm wav/stereo.wav 100.0

## play MP3
# ffmpeg -i mp3/sound.mp3 -f s16le -ar 22.05k -ac 1 | sudo ./pifunk -100.0

## Broadcast from a (usb) microphone, stereo
# arecord -d0 -c2 -f S16_LE -r 22050 -twav -D copy | sudo ./pifunk 100.0

## streams audio on network
# $port = 80
# microphone devices
#card = 0
#subdevice = 0
# arecord -D hw:${card},${subdevice} -f S16_LE -r 22050 -t wav | sudo nc -1 ./pifunk 100.0 $port
# arecord -D hw:${0},${0} -f S16_LE -r 22050 -t wav | sudo nc -1 ./pifunk 100.0 $port

## run another py-script from shell-terminal (holds main script i think?!
#subprocess.run(["sudo", "python", "pifunk-pmr.py"])
#subprocess.run(["sudo", "python", "pifunk-cb.py"])
#subprocess.run(["sudo", "python", "pifunk-temp.py"])
#subprocess.run(["sudo", "python", "pifm.py"])
#subprocess.run(["sudo", "python", "pi-minidisplay.py"])


