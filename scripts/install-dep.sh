#dependencies
sudo apt-get update
sudo apt-get upgrade

sudo apt-get install gdbserver
sudo apt-get install libraspberrypi-dev raspberrypi-kernel-headers i2c-tools
sudo apt-get install libsndfile1-dev

sudo apt-get install git
sudo apt-get install git-core

sudo apt-get install python3-dev
#sudo apt-get install python-rpi.gpio
sudo apt-get -y install python3-rpi.gpio
#sudo pip-3.7 install RPi.GPIO

sudo git clone git://git.drogon.net/wiringPi
sudo apt-get install wiringpi
