
##PiFunk Radio transmitter for PMR446/CB on FM & AM also for ltp, 433, cb, pmr
##and maybe someday, beacon, gps, internet, relais, aprs, vhf, ts2/3, RDS, morse, echolink
##microphone (usb & jack) + playlist, mp3/wav-Files. (WIP)
## dependency python 3.7.x needeed to run script
##pifm GPIO's: 4 (pin 7 GP-CLK0) and GND (pin 9 = GND) or 14 (pin 8 TXD) & GND (pin 6) & 15 (pin 10 RDX) & 21 (pin 40 SCLK) --> 39 GND
##ARM - Structure on Pi's !!! (can only be emulated on PC, so no real GPIO access!!) my Pi : rev. 2 B+
##------------------------------------------------------------------------------
#Avoid transmitting on 26.995, 27.045, 27.095, 27.145 and 27.195 MHz, as these are Class C radio-control channels,
#and the FCC takes a dim view of voice broadcasts on these frequencies. For that matter,
#re-broadcast of copyrighted material (sports, news and weather programming) is a ##violation of the law,
#and could result in fines, jail time, and confiscation of all radio equipment on your premises.
#UK law is 4 W (4000 mW) / GER 100 mW ERP for PMR and 4 W for CB
#-> sending on square-func means transmission on 3 other freqs so please use a low-pass-filter!
#-------------------------------------------------------------------------------
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
import signal
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
import csv
#import numpy as np
#import scipy -> evtl install
#import scipy.io.wavfile as wavfile
#import matplotlib.pyplot as plt

try:
 import RPi.GPIO as GPIO1
 import RPI.GPIO as GPIO2
except:
 from RPi._GPIO import GPIO3
 from RPi import GPIO4

## RPi & GPIO lib
#sudo apt-get install python-rpi.gpio
#sudo python setup.py install

##------------------------------------------------------------------------------
#loading hardware on startup
def _init_ ():
  print ("\nWelcome to PiFunk!\n")
  try:
          os.system ("sudo modprobe w1-gpio") #rpi 1-2
  except: os.system ("sudo rmmod w1-gpio") # rpi3:

  base_dir = "/sys/bus/w1/devices/"
  device_folder = glob.glob (base_dir + "28*") [0]
  device_file = device_folder + "/w1_slave"

  cpid = os.fork ()
  if not cpid:
      os._exit (0)
      os.waitpid (cpid, 0)

  pullup = 0
  DEBUG = 1
  LOGGER = 1
  GPIO.initialize ()
  GPIO.setwarnings (False)
  GPIO.setmode (GPIO.BCM)
  GPIO.setmode (GPIO.BOARD)
  GPIO.setup (4, GPIO.OUT) #or 11 if used
  output = GPIO.output (4, TRUE)
  GPIO.output (4, TRUE)
  sensor_pin = 4
  sensor_data = (sensor_pin, GPIO.PINS.GND, GPIO.PINS.RXD, GPIO.PINS.TXD)
#hex-code: 0x10A --> dec:26
##------------------------------------------------------------------------------

#definitions
def soundfile (filename):
  filename = char (input ("\nEnter filename (*.wav): "))
  if filename != 0:
      print ("\nFilename is: " + filename + " \n")
      return filename
  else:
      print ("\nNo custom filename specified! using standard file sound.wav !! \n")
      filename = char ("sound.wav")
      return filename

def frequency (freq):
  freq = float (input ("\nEnter frequency (MHz) with . as decimal (5 digit accuracy):  "))
  if freq > 0:
      print ("\nFrequency is: " + freq " (MHz) " + " \n")
      return freq
  else:
      freq = float (446.00000)
      print ("\nFrequency must be > 0 !! Using 446.00000 MHz instead! \n")
      return freq

def sampler (samplerate):
  samplerate = int (input ("\nEnter samplerate (22050/44100/48000 kHz): "))
  if samplerate > 0:
      print ("\nSamplerate is: " + samplerate + " \n")
      return samplerate
  else:
      samplerate = int (22050)
      print ("\nNo custom samplerate specified or negative! Using standard 22050 kHz!! \n")
      return samplerate

def channels (channels):
  channels = int (input ("\nEnter number of channels (1 mono or 2 stereo): "))
  if channels > 0:
      print ("\nChannels: " + channels + " \n")
      return channels
  else:
      channels = 1
      print ("\nNo channels specified! Using standard: 1 for mono \n")
      return channels


def modulation (mod):
  mod = char (input ("\nEnter modulation type (fm/am): ")).lower()
  if mod == "fm" || "am":
      print ("\nModulation is: " + mod + " \n")
      return mod
  else:
      mod = char ("fm")
      print ("\nNo mode specified! Using standard fm \n")
      return mod

def callname (callsign):

  callsign = char (input ("\nEnter callsign: "))
  if callsign != 0:
      print ("\nCallsign is: " + callsign + " \n")
      return callsign
  else:
      callsign = char ("callsign")
      print ("\nNo callsign specified! Using standard *callsign* \n")
      return callsign

def c_arg_parser ():
  print ("\nParsing args to C ... \n")
  return filename, freq, samplerate, mod, callsign, channels

def play_wav ():
  print ("\nPlaying wav ... \n")
  if (filename && freq && samplerate && mod != 0):
      call (["sudo ./pifunk ", filename, freq, samplerate, mod, callsign])
  else:
      call (["sudo ./pifunk sound.wav 100.00000 22050 fm callsign"])

def play_mp3 ():
  print ("\nPiping mp3 to ffmpeg ... \n")
  ffmpeg -i sounds/sound.mp3 -f s16le -ar 22.05k -ac 1 | sudo ./pifunk -100.00000 fm

def play_mic ():
  ## Broadcast from a (usb) microphone, stereo
  print ("\nUsing mic and piping through aracord ... \n")
  arecord -d0 -c2 -f S16_LE -r 22050 -twav -D copy | sudo ./pifunk 100.00000 fm
  arecord -D plughw:1,0 -c1 -d 0 -r 22050 -f S16_LE | sudo ./pifunk -f 100.00000 fm

def play_net ():
  $port = 80 #http web
  card = 0 # microphone devices
  subdevice = 0
  print ("\nUsing network stream and piping through arecord ... \n")
  arecord -D hw:${card},${subdevice} -f S16_LE -r 22050 -t wav | sudo nc -1 ./pifunk 100.0000 $port
  arecord -D hw:${0},${0} -f S16_LE -r 22050 -t wav | sudo nc -1 ./pifunk 100.00000 $port

def led_on (pin):
  GPIO.output (pin, GPIO.HIGH)
  #time.sleep(1)
  print ("\nLED on \n")
  return pin

def led_off (pin):
  GPIO.output (pin, GPIO.LOW)
  #time.sleep(1)
  print ("\nLED off \n")
  return pin

def led_blink (pin):
  led_on (pin)
  time.sleep (1)
  led_off (pin)
  time.sleep (1)

def led_blinking (pin):
  print ("\nBlinking 1/sec \n")
  for i in range (0, 60): led_blink (11)
  GPIO.cleanup ()

def csv_reader ():
  csv_file_object = csv.reader (open ("docs/ctsspmr.csv", "rb"))
  print ("\nImporting CTSS-Table \n")
  with open ("docs/ctsspmr.csv", "rb") as f:
  reader = csv.reader (f, delimiter = ",", quotechar = "|")
  for row in reader: print (",".join (row))
  # reading single infos of ctss tones later (wip)
  print ("\nImporting completed \n")
  return reader

def logger ():
  with open ("logs/log.txt", "w") as f:
  call (["sudo python ", "./pifunk-log.py"], stdout = f)
  print ("\nLogging ... \n")

##------------------------------------------------------------------------------
## run another py-script from shell-terminal (holds main script)
## selecting a individual band:

#subprocess.run (["sudo", "python", "pi-gpio.py"])

#subprocess.run (["sudo", "python", "pifunk-pmr.py"])

#subprocess.run (['sudo', 'python', 'pifunk-cb.py'])

#subprocess.run (["sudo", "python", "pifunk-temp.py"])

#subprocess.run (["sudo", "python", "pifunk-cpu-ram-disk.py"])

#subprocess.run (["sudo", "python", "pi-minidisplay.py"])

## run shell/bat-script if neccessary

##------------------------------------------------------------------------------
# main program
_init_ ()
datetime.now().strftime("%d-%m-%Y, %H:%M:%S \n")
#print (datetime.datetime.now ())
#current_time = datetime.datetime.now ()
#str (datetime.now ())
#current_time.isoformat ()

#here a menu with switchcase
c_arg_parser ()
logger ()
csv_reader ()

#if args not specified, else so to play
soundfile ()
frequency ()
sampler ()
channels ()
modulation ()
callsign ()

try:
#if all args are parsed so to transmission mode
 led_on ()
 led_blinking ()
 led_off ()

#here a switchcase later
 play_wav ()
 play_mp3 ()
 play_net ()
 play_mic ()

except KeyboardInterrupt:
  GPIO.cleanup()       # clean up GPIO on CTRL+C exit

GPIO.cleanup()           # clean up GPIO on normal exit
##------------------------------------------------------------------------------
##test-area
#nosetests
print ("\nEnd of script!\n")
