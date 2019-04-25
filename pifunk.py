
## additionally installed py 2.7.x & 3.6.x
## free Band combo (HAM): transmitter for PMR466 /CB  and maybe someday, beacon, gps, internet, relais, aprs
## supports UKW radio fm/am, ltp, 433, emg, cb, pmr, vhf, ts2/3, RDS, morse, echolink,
## microphone (usb & jack) + player & list , mp3/wav-Files.
## pifm GPIO's: 4 (pin 7 gp-clk0) and GND (pin 9 = Ground) or 14 ( pin 8 TXD) & gnd (pin 6) & 15(pin 10 rdx)
## 21 (pin 40 sclk) --> 39 gnd(pin)
## ARM - Structure on Pis !!! (can only be emulated !!) my Pi : rev.2 B+
##-----------------------------------------------------------------------------------------------------------------------------
##Avoid transmitting on 26.995, 27.045, 27.095, 27.145 and 27.195 MHz, as these are Class C radio-control channels,
#and the FCC takes a dim view ##of voice broadcasts on these frequencies. For that matter,
#re-broadcast of copyrighted material (sports, news and weather programming) is a ##violation of the law,
#and could result in fines, jail time, and confiscation of all radio equipment on your premises.
#UK law is 4 W (4000 mW) / GER 100 mW ERP for PMR and 4 W for CB
## -> sending on square-func means transmission on 3 other freqs
#-------------------------------------------------------------------------------------------
## py is function-scope not lock-scope!!

#!/usr/bin/python
## Imports

import StringIO
import io
import os
import sys
import glob
import socket
import datetime
import time #from time import time
import threading
import subprocess #from subprocess import run, call, pipe
import math #from math import *
import array
import wave
import random
import logging
#import asyncio #yield from *

##---------------------------------------------------------
## some other plugins
import json
import csv as csv
#import numpy as np
#import scipy -> evtl install
#import scipy.io.wavfile as wavfile
#import matplotlib.pyplot as plt

try:
 import RPi.GPIO as GPIO1
 import RPI.GPIO as GPIO2
 from RPi._GPIO import GPIO3
 from RPi import GPIO4

## RPi & GPIO lib
#sudo apt-get install python-rpi.gpio
#sudo python setup.py install

##------------------------------------------------------------------------------
#loading hardware on startup
def initialisation ():
  os.system("sudo modprobe w1-gpio")
  # rpi3:
  #os.system("sudo rmmod w1-gpio")

  cpid = os.fork ()
  if not cpid:
  os._exit (0)
  os.waitpid (cpid, 0)

  base_dir = "/sys/bus/w1/devices/"
  device_folder = glob.glob (base_dir + "28*") [0]
  device_file = device_folder + "/w1_slave"

  pullup = 0
  GPIO.initialize ()
  GPIO.setwarnings (False)
  sensor_pin = 4
  GPIO.setmode (GPIO.BCM)
  DEBUG = 1
  LOGGER = 1
  GPIO.setmode (GPIO.BOARD)
  GPIO.setup (4, GPIO.OUT)
  GPIO.setup (11, GPIO.OUT)
  output = GPIO.output (4)
  GPIO.output (11, TRUE)
  sensor_data = (sensor_pin, GPIO.PINS.GND, GPIO.PINS.RXD, GPIO.PINS.TXD)

#hex-code: 0x10A --> dec:26

##------------------------------------------------------------------------------

def soundfile (self, filename):
  filename = input ("Enter filename (*.wav): ")
  print ("Filename is: " + filename + " \n")
  return self, filename

def frequency (self, freq):
  float (freq) = input ("Enter frequency (MHz) with . dot as decimal (5 digit accuracy):  ")
  print ("\nFrequency is: " + freq " (MHz) " + " \n")
  return self, freq

def sampler (self, samplerate):
  int(samplerate_std) = 22050
  int(samplerate) = input ("Enter samplerate (kHz): ") || samplerate_std
  print ("Samplerate is: " + samplerate + " \n")
  return self, samplerate, samplerate_std

def modulation (self, mod):
  char (mode) = input ("Enter modulation type (FM/AM): ")
  print ("\nModulation is: " + mod + " \n")
  return self, mod

def channel (self, channels):
  int (channels) = input ("Enter number of channels (1 or 2): ")
  print ("\nChannels: " + channels + " \n")
  return self, channels

def callname (self, callsign):
  callsign = input ("Enter callsign: ")
  print ("Callsign is: " + callsigne + " \n")
  return self, callsign

def play_wav (self):
  #print ("Playing soundfile " + filename + " on frequency: " + freq " (MHz) and samplerate " + samplerate " (kHz) and modulation")
  print ("Playing wav ... \n")
  if (filename && freq && samplerate && mod != 0): call (["sudo ./pifunk ", filename, freq, samplerate, mod, callsign])
  else: call (["sudo ./pifunk sound.wav 100.00000 22050 fm callsign"])

def play_mp3 (self):
  print ("Playing mp3 ... \n")
  ffmpeg -i sounds/sound.mp3 -f s16le -ar 22.05k -ac 1 | sudo ./pifunk -100.00000 fm

def microfone (self):
  ## Broadcast from a (usb) microphone, stereo
  print ("Using mic ... \n")
  arecord -d0 -c2 -f S16_LE -r 22050 -twav -D copy | sudo ./pifunk 100.00000 fm
  arecord -D plughw:1,0 -c1 -d 0 -r 22050 -f S16_LE | sudo ./pifunk -f 100.00000 fm

def stream_net (self):
  $port = 80
  card = 0 # microphone devices
  subdevice = 0
  print ("Using net stream ... \n")
  arecord -D hw:${card},${subdevice} -f S16_LE -r 22050 -t wav | sudo nc -1 ./pifunk 100.0000 $port
  arecord -D hw:${0},${0} -f S16_LE -r 22050 -t wav | sudo nc -1 ./pifunk 100.00000 $port

def led_on (self, pin):
  GPIO.output(pin, GPIO.HIGH)
  #time.sleep(1)
  print("LED on \n")

def led_off (self, pin):
  GPIO.output (pin, GPIO.LOW)
  #time.sleep(1)
  print ("LED off \n")
  return pin

def blink (self):
  led_on ()
  time.sleep (1)
  led_off ()
  time.sleep (1)

def blinking (self):
  print ("Blinking 1/sec \n")
  for i in range (0, 60): blink (11)
  GPIO.cleanup ()

def csv_reader (self):
  csv_file_object = csv.reader (open("docs/ctsspmr.csv", 'rb'))
  print ("Importing CTSS-Table \n")
  with open("docs/ctsspmr.csv", "rb") as f:
  reader = csv.reader(f, delimiter = "," , quotechar = "|")
  for row in reader: print(",".join(row))
  # reading single infos of ctss tones later (wip)
  print ("Importing completed \n")
  return reader

def logger (self):
  with open ("logs/log.txt", "w") as f:
  #call(["python ", "./pifunk-main.py"], stdout = f)
  print ("Logging... \n")

def c_arg_parser (self):
  pass


##------------------------------------------------------------------------------------------------------

## run another py-script from shell-terminal (holds main script, i think?!)
##selecting a individual band:

#subprocess.run(["sudo", "python", "pi-gpio.py"])

#subprocess.run(["sudo", "python", "pifunk-pmr.py"])

#subprocess.run(['sudo', 'python', 'pifunk-cb.py'])

#subprocess.call('sudo', 'python', 'pifunk-cb.py')

#subprocess.run(["sudo", "python", "pifunk-temp.py"])

#subprocess.run(["sudo", "python", "pifunk-cpu-ram-disk.py"])

#subprocess.run(["sudo", "python", "pi-minidisplay.py"])

## run shell/bat-script if neccessary

##------------------------------------------------------------------------------
# main programm

print("pifunk py-script \n")
datetime.now().strftime("%d-%m-%Y, %H:%M:%S \n")
#print(datetime.datetime.now())
#current_time = datetime.datetime.now()
#.strftime('%Y-%m-%d %H:%M:%S')
#str(datetime.now())
#current_time.isoformat()


##------------------------------------------------------------------------------
##test-area
#nosetests
#print('end of scriipt')
