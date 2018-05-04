#!/usr/bin/python
import datetime
from subprocess import call

datetime.now().strftime('%d-%m-%Y %H:%M:%S')

def play_sound(filename):
   call (["./pifm", filename])
   return
   