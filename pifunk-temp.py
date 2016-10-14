
import os
import glob
import datetime
#import time
#import date
import sys
import json


# Process
from subprocess import call

import RPi.GPIO as GPIO

# -> waterproof 3-wire temp-sensor (model ) needs 100kOhm resistor on pin 4
# binding hardware to system
os.system('modprobe w1-gpio')
os.system('modprobe w1-therm')

# autostart + init
pullup=1
 
GPIO.initialize()
sensor_pin = 4
sensor_data=(sensor_pin, GPIO.PINS.GND, GPIO.PINS.RXD, GPIO.PINS.TXD)
#device dir path
base_dir = '/sys/bus/w1/devices/'
device_folder = glob.glob(base_dir + '28*')[0]
device_file = device_folder + '/w1_slave'
 
#gathering analog temp
def read_temp_raw():
    f = open(device_file, 'r')
    lines = f.readlines()
    f.close()
    return lines
 
def read_temp():
    lines = read_temp_raw()
    while lines[0].strip()[-3:] != 'YES':
        time.sleep(0.1)
        lines = read_temp_raw()
        equals_pos = lines[1].find('t=')
		
     if equals_pos != -1:
        temp_string = lines[1][equals_pos+2:]
		# in Celsius 0-100°
        temp_c = float(temp_string) / 1000.0 
		# in Fahrenheit in 32°
        temp_f = temp_c * 9.0 / 5.0 + 32.0
        return temp_c, temp_f
 
 while True:
  temp_c = read_temp()
  print(' DateTime: ': datetime.datetime.now(), ' Temperature (°C/°F): ': temp_c,temp_f})
  time.sleep(1)

# show sys-temmp
def sys_temp(date, temp): 
	if __name__ == '__main__':
		if len(sys.argv) == 3:
			date = sys.argv[1]
			temp = sys.argv[2]
			sys_temp(date, temp)
			time.sleep(0.1)
	else:
		print ' Usage: ' + sys.argv[0] + ' date temp '
		
		
		