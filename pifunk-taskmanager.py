
#
## RPi Monitoring of CPU Temp Ram
#!/usr/bin/env python

## basic imports
import os 
import io
import sys
import glob
import socket
import datetime
import math
from math import *
import threading
from __future__ import division
import subprocess
from subprocess import PIPE, Popen
import psutil
import random

## extern or special imports
import json
from RPi._GPIO import *
try: 
 import RPi.GPIO as GPIO
 from RPi import GPIO
 
## Django incl most plugins
#

# Return CPU temperature as a character string                                      
def getCPUtemperature():
    res = os.popen('vcgencmd measure_temp').readline()
    return(res.replace("temp=","").replace("'C\n",""))

# Return RAM information (unit=kb) in a list                                        
# Index 0: total RAM                                                                
# Index 1: used RAM                                                                 
# Index 2: free RAM   
                                                              
def getRAMinfo():
    p = os.popen('free')
    i = 0
    while 1:
        i = i + 1
        line = p.readline()
        if i==2:
            return(line.split()[1:4])

# Return % of CPU used by user as a character string                                
def getCPUuse():
    return(str(os.popen("top -n1 | awk '/Cpu\(s\):/ {print $2}'").readline().strip(\
)))

# Return information about disk space as a list (unit included)                     
# Index 0: total disk space                                                         
# Index 1: used disk space                                                          
# Index 2: remaining disk space                                                     
# Index 3: percentage of disk used    
                                              
def getDiskSpace():
    p = os.popen("df -h /")
    i = 0
    while 1:
        i = i +1
        line = p.readline()
        if i==2:
            return(line.split()[1:5])
			

# CPU informatiom
CPU_temp = getCPUtemperature()
CPU_usage = getCPUuse()

# RAM information
# Output is in kb, here I convert it in Mb for readability
RAM_stats = getRAMinfo()
RAM_total = round(int(RAM_stats[0]) / 1000,1)
RAM_used = round(int(RAM_stats[1]) / 1000,1)
RAM_free = round(int(RAM_stats[2]) / 1000,1)

# Disk information
DISK_stats = getDiskSpace()
DISK_total = DISK_stats[0]
DISK_free = DISK_stats[1]
DISK_perc = DISK_stats[3]


def get_cpu_temperature():
    process = Popen(['vcgencmd', 'measure_temp'], stdout=PIPE)
    output, _error = process.communicate()
    return float(output[output.index('=') + 1:output.rindex("'")])


def main():
    cpu_temperature = get_cpu_temperature()
    cpu_usage = psutil.cpu_percent()
    
    ram = psutil.phymem_usage()
    ram_total = ram.total / 2**20       # MiB.
    ram_used = ram.used / 2**20
    ram_free = ram.free / 2**20
    ram_percent_used = ram.percent
    
    disk = psutil.disk_usage('/')
    disk_total = disk.total / 2**30     # GiB.
    disk_used = disk.used / 2**30
    disk_free = disk.free / 2**30
    disk_percent_used = disk.percent
    # 
    # Print top five processes in terms of virtual memory usage.
    # 
    processes = sorted(
        ((p.get_memory_info().vms, p) for p in psutil.process_iter()),
        reverse=True
    )
    for virtual_memory, process in processes[:5]:
        print virtual_memory // 2**20, process.pid, process.name

#show sys-temmp
def sys_temp(date, temp): 
		if len(sys.argv) == 3:
			date = sys.argv[1]
			temp = sys.argv[2]
			sys_temp(date, temp)
			update_temp(date,temp)
			time.sleep(1)
	else:
		print (' Usage: ' + sys.argv[0] + ' date temp ')
		
if __name__ == '__main__':

    main()
	
while not self.asleep():
    sheep += 1
#
