
#pifunk shell-terminal script addinonaly
echo " installing dependencies: "
sudo pip install RPi.GPIO
sudo apt-get install python-rpi.gpio
sudo python setup.py install
# sudo ./pifunk sound.wav 100.0
# sudo ./pifm sound.wav 100.0
sudo python pifunk-main.py
# sudo python pifunk-temp.py
# sudo python pifunk-cb.py
# sudo python pifunk-pmr.py
# sudo python pifm.py



