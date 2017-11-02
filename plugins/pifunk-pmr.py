
#
## Band: PMR analog & digital 
## normal spiral 4cmbuilt-in and 17 cm selfmade liniar-Cu-Antenna ontop 
## @ 446.0 - .1 and .2 MHz in 6.25/12.5 kHz steps with max. 500 mW EIRP. 
## pmr: stereo-jack (mine: ALDI tevion set - V/04/2005, ean: 23139695, CE 0359, sn: 503076883)
## -> connection: upper= earplug sound mono, lower= mic mono 
##(later with BNC-adapter/ hardware-module)
##---------------------------------------------------------------------------------------------------

## basic Imports
# !/usr/bin/python
import StringIO
import os
import sys
import glob
import datetime
# import date
import time
import io
import socket
import subprocess
from subprocess import call #, pipe
import math
from math import *
import threading
import random
import logging
import array
import wave

#import asyncio
##external or special imports
import json
import numpy

#import scipy.io.wavfile as wavfile
#import matplotlib.pyplot as plt

## django imports with most plugins
#

##------------------------------------------------------
import csv
with open("ctsspmr.csv") as fh:
    rows = []
    for row in csv.DictReader(fh):
        rows.append(row)
# or, if you like:
rows = [row for row in csv.DictReader(open("ctsspmr.csv"))]
print rows

## def variables
#int channels
#int chfrq
#int subchannels

#float freq 
# pmr_base = '446.'
# float Frequency
# =  pmr_base+chfrq if needed for splitting
##------------------------------------------------------------------------------------

#total range from 446. (00625 - 19375)
## analog main: (1-8) 12,5 Hz steps
## 1 is default chan. with sub 0/1, unless any func does different things

# case [1]: freq=446.00625 #Standard
# case [2]: freq=446.01875 #Geocaching
# case [3]: freq=446.03125 #random
# case [4]: freq=446.04375 #at 3-chan-PMR-devices its ch. 2
# case [5]: freq=446.05625 #Contest
# case [6]: freq=446.06875 #Events
# case [7]: freq=446.08125 #at 3-chanl-PMR-devices it's ch. 3
# case [8]: freq=446.09375 #random talk stuff

## dpmr digital new since 28.09.2016
# erweitert um extra 8 chan
## 12.5 kHz steps too
# case [9]:  freq=446.10625
# case [10]: freq=446.11875
# case [11]: freq=446.13125
# case [12]: freq=446.14375
# case [13]: freq=446.15625
# case [14]: freq=446.16875
# case [15]: freq=446.18125
# case [16]: freq=446.19375
#------------------------------------------#

#weitere option in halben schritten
# * bedeutet kommt in 12,5er schritten bereits vor
## 6.25 kHz steps
# case [a]: freq=446.10312
# case [b]: freq=446.11875*
# case [c]: freq=446.13125*
# case [d]: freq=446.14375*
# case [f]: freq=446.15625*
# case [g]: freq=446.16875*
# case [h]: freq=446.18125*
# case [i]: freq=446.19375*

# case [j]: freq=446.10625*
# case [k]: freq=446.11875
# case [l]: freq=446.13125
# case [m]: freq=446.14375
# case [n]: freq=446.15625
# case [o]: freq=446.16875
# case [p]: freq=446.18125
# case [q]: freq=446.19375*

##-------------------------------------------------------------------------
## subchannels: (0/1-38) CTSS/ DCS tones !!!

## 0 = all subch. -> base/default
## if subchannels is 0 = all ch. ??? check special stuff

# case [0]:  ctss=0

# case [1]:  ctss=1
# case [2]:  ctss=2
# case [3]:  ctss=3
# case [4]:  ctss=4
# case [5]:  ctss=5
# case [6]:  ctss=6

# case [7]:  freq=446.1
# case [8]:  freq=446.1
# case [9]:  freq=446.1
# case [10]: freq=446.1
# case [11]: freq=446.1
# case [12]: freq=446.1
# case [13]: freq=446.1
# case [14]: freq=446.1
# case [15]: freq=446.1
# case [16]: freq=446.1
# case [17]: freq=446.1
# case [18]: freq=446.1
# case [19]: freq=446.1
# case [20]: freq=446.1
# case [21]: freq=446.1
# case [22]: freq=446.1
# case [23]: freq=446.1
# case [24]: freq=446.1
# case [25]: freq=446.1
# case [26]: freq=446.1
# case [27]: freq=446.1
# case [28]: freq=446.1
# case [29]: freq=446.1
# case [30]: freq=446.1
# case [31]: freq=446.1
# case [32]: freq=446.1
# case [33]: freq=446.1
# case [34]: freq=446.1
# case [35]: freq=446.1
# case [36]: freq=446.1
# case [37]: freq=446.1
# case [38]: freq=446.1

## more functions here
#
print('pmrtest')
#



