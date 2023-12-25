#!/usr/bin/python
## PiFunk Radio Transmitter for PMR446/CB on FM & AM also for ltp, 433
## and maybe someday, beacon, gps, internet, relais, aprs, vhf, ts2/3, RDS, morse, echolink
## microphone (usb & jack) + playlist, mp3/wav-Files. (WIP)
## dependency python 3.7.x needeed to run script
## pifunk GPIO's: 4 (pin 7 GP-CLK0) and GND (pin 9 = GND) or 14 (pin 8 TXD) & GND (pin 6) & 15 (pin 10 RDX) & 21 (pin 40 SCLK) --> 39 GND
## ARM - Structure on Pi's !!! (can only be emulated on PC, so no real GPIO access!!) my Pi: 1.2 rev. 2 B+
##------------------------------------------------------------------------------------------------------------------------------------
## Avoid transmitting on 26.995, 27.045, 27.095, 27.145 and 27.195 MHz, as these are Class C radio-control channels,
## and the FCC takes a dim view of voice broadcasts on these frequencies. For that matter,
## re-broadcast of copyrighted material (sports, news and weather programming) is a ##violation of the law,
## and could result in fines, jail time, and confiscation of all radio equipment on your premises.
## UK law is 4 W (4000 mW) / GER 100 mW ERP for PMR 0.5 and 4 W for CB
## ->sending on square-func means transmission on 3 other freqs so please use a low-pass-filter!
##-------------------------------------------------------------------------------------------------------------------------------------
## py is function-scope not lock-scope!!

## Imports
import io
import os
import sys
import gc
import glob
import string
import StringIO
import struct
import types
import decimal
import array
import argparse
import readline
import rlcomplete
import atexit
import calendar
import time ## from time import time
import timeit
import datetime
import site
import re
import math ## from math import *
import statistics
import locale
import distutils
import serial
import signal
import asyncio
import smbus
import tracemalloc
import threading
import builtins
import collections
import queue
import subprocess ## from subprocess import run, call, pipe
import multiprocessing
import wave
import random
import logging
import weakref
import socket
import ipaddress
import mimetypes
import urllib.request

## some other plugins
import tar
import json
import csv
#import numpy as np
#import scipy -> evtl install
#import scipy.io.wavfile as wavfile
#import matplotlib.pyplot as plt

## RPi & GPIO lib obligatory for controlling gpios
#sudo apt-get install python-rpi.gpio
try:
 import RPi.GPIO as GPIOLIB
 import RPI.GPIO as GPIO
except:
 from RPi._GPIO import GPIOLIB
else:
 from RPi import GPIOLIB

## load navigations stuff
import gps
import pynmea2 ## for gps devices e.g. Neo-7M
#sudo python setup.py install

## hex-code: 0x10A --> dec: 26
##------------------------------------------------------------------------------
## loading hardware on startup

def __init__ (self):
  print ("\nWelcome to PiFunk!\n")
  self.data = []
  print (sys.argv)

  try:
      os.system ("sudo apt-get install i2c-tools")
      os.system ("sudo apt-get install python-smbus")

      os.system ("sudo adduser pi i2c")
      os.system ("sudo i2cdetect -y 0")

  except:
          os.system ("sudo i2cdetect -y 1")

  try:
          os.system ("sudo modprobe w1-gpio") ## rpi 1-2

  except:
          os.system ("sudo rmmod w1-gpio") ## rpi 3/4

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
  gpio_pin = 4
  GPIO.initialize ()
  GPIO.setwarnings (False)
  GPIO.setmode (GPIO.BCM)
  GPIO.setmode (GPIO.BOARD)
  GPIO.setup (4, GPIO.OUT) ## or 11 if used
  output = GPIO.output (4, TRUE)
  sensor_data = (gpio_pin, GPIO.PINS.GND, GPIO.PINS.RXD, GPIO.PINS.TXD)
  pi_pwm = GPIO.PWM (4, freq)
  pi_pwm.start (0)

  while True:
      for Duty in range (0, 60, 1)
      pi_pwm.ChangeDutyCycle (Duty)
##------------------------------------------------------------------------------

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
  freq = float (input ("\nEnter frequency (1 kHz-1000 MHz) with . as decimal (5 digit accuracy):  "))
  for samplerate in range (0.000001, 1000)
  if freq > 0:
      print ("\nFrequency is: " + freq " MHz \n")
      return freq
  else:
      freq = float (446.00000)
      print ("\nFrequency must be > 0 !! Using 446.00000 MHz instead! \n")
      return freq

def sampler (samplerate):
  samplerate = int (input ("\nEnter samplerate (22050/44100/48000 kHz): "))
  if samplerate > 0:
      print ("\nSamplerate is: " + samplerate + "\n")
      return samplerate
  else:
      samplerate = int (22050)
      print ("\nNo custom samplerate specified or negative! Using standard 22050 kHz!! \n")
      return samplerate

def channels (channels):
  channels = int (input ("\nEnter number of channels (1 mono or 2 stereo): "))
  if channels 1 || 2:
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

def c_arg_parser (filename, freq, samplerate, mod, callsign, power):
  print ("\nParsing args to C ... \n")

  return filename, freq, samplerate, mod, callsign, power

def play_wav (filename, freq, samplerate, mod, callsign, power):
  print ("\nPlaying wav ... \n")
  if (filename && freq && samplerate && mod != None):
      call (["sudo ./pifunk ", filename, freq, samplerate, mod, callsign, power])
  else:
      call (["sudo ./pifunk sound.wav 100.00000 22050 fm callsign 7"])

def play_mp3 ():
  print ("\nPiping mp3 to ffmpeg ... \n")
  ffmpeg -i sounds/sound.mp3 -f s16le -ar 22050 -ac 1 | sudo ./pifunk -f 100.00000 fm

def play_mic ():
  ## Broadcast from a (usb) microphone, stereo
  print ("\nUsing mic and piping through aracord ... \n")
  arecord -d0 -c2 -f S16_LE -r 22050 -twav -D copy | sudo ./pifunk 100.00000 fm
  arecord -D plughw:1,0 -c1 -d 0 -r 22050 -f S16_LE | sudo ./pifunk -f 100.00000 fm

def play_net ():
  $port = 80 ## http web
  card = 0 ## microphone devices
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
  for i in range (0, 61, 1): led_blink (11)
  GPIO.cleanup ()

def csv_reader ():
  csv_file_object = csv.reader (open ("docs/ctsspmr.csv", "rb"))
  print ("\nImporting CTSS-Table \n")
  with open ("docs/ctsspmr.csv", "rb") as f:
  reader = csv.reader (f, delimiter = ",", quotechar = "|")
  for row in reader: print (",".join (row))
  ## reading single infos of ctss tones later (wip)
  print ("\nImporting completed \n")
  return reader

def logger ():
  with open ("logs/log.txt", "w") as f:
  call (["sudo python ", "./pifunk-log.py"], stdout = f)
  print ("\nLogging ... \n")

##------------------------------------------------------------------------------
if __name__ == "__main__":
 print ("\nStarting Program!\n")
 datetime.now().strftime("\n%d-%m-%Y, %H:%M:%S \n")
 (int(sys.argv[1])

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
## main program
try:
 __init__ ()

#print (datetime.datetime.now ())
#current_time = datetime.datetime.now ()
#str (datetime.now ())
#current_time.isoformat ()

## here a menu with switchcase
 logger ()
 csv_reader ()

## if args not specified, else so to play
 soundfile ()
 frequency ()
 sampler ()
 channels ()
 modulation ()
 callsign ()

 c_arg_parser ()

try:
## if all args are parsed so to transmission mode
 led_on ()
 led_blinking ()
 led_off ()

## here a switchcase later
 play_wav ()
 play_mp3 ()
 play_net ()
 play_mic ()

except KeyboardInterrupt:
 GPIO.cleanup ()       ## clean up GPIO on CTRL+C exit

except:
 print ("\nERROR\n")
 pass

GPIO.cleanup ()        ## clean up GPIO on normal exit

##------------------------------------------------------------------------------

## test-area

print ("\nEnd of script!\n")
