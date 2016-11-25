
#
## Band: CB - AM /FM type (70 cm) 2 m  
## channels: 1-20/40/80 in 10 kHz steps 
## 2 devices i use: *a) Albrecht GmbH A4400 - 4W max. 
## (~10-16V / 2-3A) 40 chan. (only FM +scan)
## SN: 94062452, Deutsche Post Z GW00 310V, CEPT-PR27D, uses ~13,7V !!
## b) XXX ~12V/ 2-3A 80 chan. (FM & AM) -added later
## 2 magnet-feet (threaded-screw) with antennas (PL-plug and BNC if needed): 
## *a) liniar 33+28=61 cm (later with BNC-adapter, maybe modul)
## b) spiral (4 of 19 cm at 8 turns) 9+19=28 cm 
## powered by regulator (220V - AC @50Hz): output at 12-13,8V DC at 2-3A / 
## and/or portable: 12V-car-bat./cigarette-plug  with a DC-balun
## devices incl. dig. volt-meters & passive cooling-pads 5W/mK each)
## need PWR-SR/RF-meter for calibration & testing! Team Electronic SWR-1180W
## sending/transmitting should be used WITH a low-pass-filter !!!!
## receiving should be fine...
## using muli-meter for checking :) but no oscillator 
##-------------------------------------------------------------------------------------

## !/usr/bin/python
## Imports
## py is function scope not lock scope
## maybe put imports in external file (category sys / rpi&gpio/ radio-ham)
import os
import sys
import glob
import socket
import datetime
#from datetime import datetime
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

## django imports with most plugins
#
## MUST BE ASCII !
#-------------------------------------------------------------------------------------------------------------------------
## Auf den CEPT-konformen Kanaelen 1 bis 40 sind die Modulationsarten FM, AM und SSB erlaubt.
## Auf einigen Kanaelen ist zusaetzlich Datenuebertragung erlaubt.
## Des Weiteren sind national weitere Kanaele fuer den CB-Funk verfuegbar.

# maybe switch-modul/class/object?! helpful +input
# case [1]:   freq=26.965 #empfohlener Anrufkanal (FM)	
# case [2]:   freq=26.975 #inoffizieller Berg-DX-Kanal (FM)
# case [3]:   freq=26.985 
# case [4]:   freq=27.005 #empfohlener Anrufkanal (AM)/Anrufkanal Feststationen (AM)
# case [5]:   freq=27.015 #Kanal wird von italienischen Fernfahrern in Deutschland und Italien benutzt.
# case [6]:   freq=27.025 #Datenkanal (D)
# case [7]:   freq=27.035 #Datenkanal (D)
# case [8]:   freq=27.055
# case [9]:   freq=27.065 #Fernfahrerkanal (AM)/weltweiter Notrufkanal	EMG
# case [10]:  freq=27.075 #antennen-abgleich

# Bei genauerer Betrachtung obiger Tabelle fallen einige Stellen auf, an denen sich Nachbarkanaele nicht um 10 kHz, sondern um 20 kHz unterscheiden. 
# Die dazwischen versteckten Kanaele werden ueblicherweise folgenderweise bezeichnet:
# Diese Kanaele sind in den meisten Laendern nicht fuer CB-Funk zugelassen. 
# Allerdings werden sie in einigen Laendern, darunter auch Deutschland[3], fuer andere Zwecke wie z. B. Funkfernsteuerungen, Babyphones, kabellose Tastaturen und Maeuse u. a. verwendet.

# case [7A]:  freq=27.045 
# case [11A]: freq=27.095 #Eurobalise-Energieversorgung
# case [15A]: freq=27.145 
# case [19A]: freq=27.195 


# case [11]:  freq=27.085 #freigegeben zur Zusammenschaltung mehrerer CB-Funkgeraete über eine Internetverbindung in Deutschland
# case [12]:  freq=27.105 
# case [13]:  freq=27.115 
# case [14]:  freq=27.125 #oft verwendet fuer Spielzeug-Fernsteuerungen (mittels Selektivton)
# case [15]:  freq=27.135 #inoffizieller Anrufkanal SSB (USB)	
# case [16]:  freq=27.155 #Funkverkehr mit und zwischen Wasserfahrzeugen
# case [17]:  freq=27.165 #Kanal wird von dänischen Schwertransportfahrern in Deutschland und Daenemark benutzt.
# case [18]:  freq=27.175
# case [19]:  freq=27.185 #empfohlener Fernfahrerkanal (FM)/oft von Walkie-Talkies genutzt/teilweise auch als Notrufkanal angegeben/auch von Babyfonen genutzt	
# case [20]:  freq=27.205 #zum Antennenabgleich genutzte Mitte bei 40-Kanal-Geraeten, wird in oesterreich sehr oft für Schwertransportfahrten benutzt

##40ch devices
# case [21]:  freq=27.215 #türkischer Anrufkanal in Deutschland und Europa (FM)	
# case [22]:  freq=27.225 #oft von Walkie-Talkies genutzt, auch von Babyfonen genutzt, wird auch als Anrufkanal für rumaenische Fernlastfahrer verwendet
# case [23]:  freq=27.255 #Die Kanaele 23, 24, 25 sind sog. Dreher, sie folgen nicht dem aufsteigenden 10-kHz-Raster	
# case [24]:  freq=27.235 #Datenkanal (D)
# case [25]:  freq=27.245 #Datenkanal (D), USB ROS Intern.	
# case [26]:  freq=27.265 
# case [27]:  freq=27.275 
# case [28]:  freq=27.285 #Kanal wird von polnischen Fernfahrern in Deutschland benutzt, Anrufkanal in Polen, 
# wobei allgemein die CB-Kanalfrequenz in Polen um 5 kHz niedriger ist.
# case [29]:  freq=27.295 #Freigegeben zur Zusammenschaltung mehrerer CB-Funkgeraete über eine Internetverbindung in Deutschland	
# case [30]:  freq=27.305 #inoffizieller DX-Kanal (FM), Anrufkanal für Funker aus dem ehemaligen Jugoslawien
# case [31]:  freq=27.315 #inoffizieller DX-Kanal (FM)
# case [32]:  freq=27.325 
# case [33]:  freq=27.335 
# case [34]:  freq=27.345 #freigegeben zur Zusammenschaltung mehrerer CB-Funkgeraete ueber eine Internetverbindung in Deutschland
# case [35]:  freq=27.355 #öffentlicher Kanal
# case [36]:  freq=27.365 #Datenkanal USB ROS international
# case [37]:  freq=27.375 #Gateway-Kanal oesterreich, FM	
# case [38]:  freq=27.385 #inoffizieller internationaler DX-Kanal (LSB)
# case [39]:  freq=27.395 #Freigegeben zur Zusammenschaltung mehrerer CB-Funkgeraete ueber eine Internetverbindung in Deutschland	
# case [40]:  freq=27.405 #ab Maerz 2016 freigegeben zur Zusammenschaltung mehrerer CB-Funkgeraete ueber eine Internetverbindung in Deutschland (FM/AM/SSB in D)


## 80 chan devices
# Auf den nationalen Zusatzkanaelen 41 bis 80 ist nur die Modulationsart FM erlaubt. 
# Nachfolgend sind die Frequenzen der nationalen Zusatzkanaele, die im CB-Funk benutzt werden duerfen, aufgelistet:

# case [41]:  freq=27.565 #Ab Maerz 2016 Freigegeben zur Zusammenschaltung mehrerer CB-Funkgeraete 
# ueber eine Internetverbindung in Deutschland (FM),inoffizieller DX-Kanal (FM)
# case [42]:  freq=27.575 #inoffizieller DX-Kanal (FM)
# case [43]:  freq=27.585 
# case [44]:  freq=27.595 
# case [45]:  freq=27.605 
# case [46]:  freq=27.615
# case [47]:  freq=27.625 
# case [48]:  freq=27.635
# case [49]:  freq=27.645
# case [50]:  freq=27.655 
# case [51]:  freq=27.665 #
# case [52]:  freq=27.675 #Datenkanal (D)(FM)
# case [53]:  freq=27.685 #Datenkanal (D)(FM)	
# case [54]:  freq=27.695 
# case [55]:  freq=27.705 
# case [56]:  freq=27.715 
# case [57]:  freq=27.725 
# case [58]:  freq=27.735 
# case [59]:  freq=27.745 
# case [60]:  freq=27.755 

# case [61]:  freq=26.765 #Freigegeben zur „Zusammenschaltung mehrerer CB-Funkgeraete ueber eine Internetverbindung“ in Deutschland	
# case [62]:  freq=26.775
# case [63]:  freq=26.785	
# case [64]:  freq=26.795
# case [65]:  freq=26.805
# case [66]:  freq=26.815
# case [67]:  freq=26.825	
# case [68]:  freq=26.835
# case [69]:  freq=26.845
# case [70]:  freq=26.855
# case [71]:  freq=26.865 #Freigegeben zur „Zusammenschaltung mehrerer CB-Funkgeraete ueber eine Internetverbindung“ in Deutschland	
# case [72]:  freq=26.875
# case [73]:  freq=26.885
# case [74]:  freq=26.895
# case [75]:  freq=26.905
# case [76]:  freq=26.915 #Datenkanal (D)(FM)
# case [77]:  freq=26.925 #Datenkanal (D)(FM)
# case [78]:  freq=26.935
# case [79]:  freq=26.945
# case [80]:  freq=26.955 #Freigegeben zur „Zusammenschaltung mehrerer CB-Funkgeraete über eine Internetverbindung“ in Deutschland
#
#nonsensetest
print ('cb')
#pass
#



