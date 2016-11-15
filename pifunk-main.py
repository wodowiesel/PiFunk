## free Band combo (HAM): listener/transmitter as beacon, gps, internet, relais, aprs by server-client (all-in-one-version)
## supports UKW radio fm/am , ltp , 433, emg, cb, pmr, vhf, ts2/3, mp3/wave-Files , YT, RDS, microphone (usb&jack) etc.

## ARM -Structure on Pi's !!!

# !/usr/bin/python
## Imports
## py is function scope not lock scope

import os
import sys
import socket
import datetime
import time
import io
import math
from math import *
import threading
import subprocess
import random
import json
from RPi._GPIO import *
try: 
 import RPi.GPIO as GPIO
#import date
#import numpy

#import scipy -> evtl install
#import scipy.io.wavfile as wavfile

#import matplotlib.pyplot as plt

## RPi & GPIO lib bind
#sudo apt-get install python-rpi.gpio
#sudo python setup.py install



## def variables
#int (chan) = channels 
type(13.0)
<class 'int'> 

#float (freq) = frequency 
type(13.0)
<class 'float'> 
#alternative long or complex

#char (filename) = file
#string 

##hex-code
## 0x10A -->26


print("testing funk script")

## function Play file

#def play_file (self, filename, freq):
  #print (" Testing play_file ... ")
  #file = input(" Enter File-Name (*.wav): ")
  #freq = input(" Enter Frequency in MHz: ")
  #call ([" sudo ./pifunk ", filename, freq])
  #call ([" sudo ./pifm ", sound.wav, -freq])
  #print ( " Playing file (*.wav): " + filename + " on Frequency (MHz):  " + freq)   
  #return self , files , freq



## basic operations
# continue
# break
time.sleep(10)
return

## Commands:
## standard freq=100.0 or pifm original 103.3 
## -> sending on square-func means transmission on 3 other freqs: example:
# sudo ./pifunk sound.wav 100.0
# sudo ./pifm sound.wav 100.0
# sudo ./pifm wav/stereo.wav 100.0
# rpi3: sudo rmmod w1-gpio


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

## blinking function  
# def blink(pin):  
       # GPIO.output(pin,GPIO.HIGH)  
        #time.sleep(1)  
       # GPIO.output(pin,GPIO.LOW)  
       # time.sleep(1)  
      #  return  
## to use Raspberry Pi board pin numbers  
#GPIO.setmode(GPIO.BOARD)  
## set up GPIO output channel  
#GPIO.setup(11, GPIO.OUT)  
## blink GPIO17 50 times  
#for i in range(0,50):  
      #  blink(11)  
#GPIO.cleanup() 


