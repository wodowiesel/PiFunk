## free Band combo (HAM): listener/transmitter as beacon, gps, internet, relais, aprs by server-client (all-in-one-version)
## supports UKW radio fm/am , ltp , 433, emg, cb, pmr, vhf, ts2/3, mp3/wave-Files , YT, RDS, microphone (usb&jack) etc.
## pifm GPIO's: 4(pin 7 gp-clk0) and GND(pin 9 = Ground) or  14 ( pin 8 TXD) & gnd (pin 6) & 15(pin 10 rdx)
#21 (pin 40 sclk) --> 39 gnd(pin)
## ARM -Structure on Pi's !!!

# !/usr/bin/python
## Imports
## py is function scope not lock scope

import os
import sys
import glob
import socket
import datetime
from datetime import datetime
import time
from time import time
#import date
import io
import math
from math import *
import threading
import subprocess
from subprocess import *
import random

import json

from RPi._GPIO import *
try: 
 import RPi.GPIO as GPIO
 from RPi import GPIO
## RPi & GPIO lib bind
#sudo apt-get install python-rpi.gpio
#sudo python setup.py install
##loading hardware on startup
#os.system('sudo modprobe w1-gpio')
#pullup=1
#GPIO.initialize()
#GPIO.setwarnings(False)
#sensor_pin = 4
#sensor_data=(sensor_pin, GPIO.PINS.GND, GPIO.PINS.RXD, GPIO.PINS.TXD)
#device dir path
#base_dir = '/sys/bus/w1/devices/'
#device_folder = glob.glob(base_dir + '28*')[0]
#device_file = device_folder + '/w1_slave'!

#import numpy

#import scipy -> evtl install
#import scipy.io.wavfile as wavfile

#import matplotlib.pyplot as plt



## def variables
#channels = chan
#int(80)
#class 'int'

#frequency = freq 
#float(26.965) 
#class 'float' 
#alternative long or complex

#string str
#files = filename 
print str("files")


##hex-code
## 0x10A -->26

##testing
#time.sleep(1)
#print("testing funk script")
#print(datetime.datetime.now())
#current_time = datetime.datetime.now()
#.strftime('%Y-%m-%d %H:%M:%S')
#str(datetime.now())
#current_time.isoformat()

## function Play file

#def play_file (self, filename, freq):
  #print (" Testing play_file ... ")
  #file = input(" Enter File-Name (*.wav): ")
  #freq = input(" Enter Frequency in MHz: ")
  #call ([" sudo ./pifunk ", filename, freq])
  #call ([" sudo ./pifm ", sound.wav, -freq])
  #print ( " Playing file (*.wav): " + filename + " on Frequency (MHz):  " + freq)   
  #return self , files , freq

## basic behavior
# continue
# break
# return
#time.sleep(1)

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
#arecord -D plughw:1,0 -c1 -d 0 -r 22050 -f S16_LE | sudo ./fm_transmitter -f 100.0 -

## streams audio on network
#$port = 80
## microphone devices
#card = 0
#subdevice = 0
# arecord -D hw:${card},${subdevice} -f S16_LE -r 22050 -t wav | sudo nc -1 ./pifunk 100.0 $port
# arecord -D hw:${0},${0} -f S16_LE -r 22050 -t wav | sudo nc -1 ./pifunk 100.0 $port

## run another py-script from shell-terminal (holds main script, i think?!)
##selecting a individual band:
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
#GPIO.output(11, True) 
## blink GPIO17 50 times  
#for i in range(0,60):
      #  blink(11)  
#GPIO.cleanup() 

