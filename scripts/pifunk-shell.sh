
#pifunk++ shell-terminal script addinonaly
echo "installing dependencies: "
sudo pip install RPi.GPIO
sudo apt-get install python-rpi.gpio
sudo python setup.py install
# sudo ./pifunk++ -n sound.wav -f 446.006250 -s 22050 -m fm -c callsign -p 7
sudo python pifunk-main.py
# sudo python pifunk-temp.py
# sudo python pifunk-cb.py
# sudo python pifunk-pmr.py
# sudo python pifunk++.py
