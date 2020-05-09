
#pifunk  additional shell-terminal script
echo "installing dependencies & packages: "

sudo apt-get update
sudo apt-get upgrade

sudo apt-get install libraspberrypi-dev raspberrypi-kernel-headers i2c-tools
sudo apt-get install libsndfile1-dev

sudo apt-get install git
sudo apt-get install git-core
sudo git clone https://www.github.com/wodowiesel/PiFunk

sudo apt-get install gdbserver
sudo apt-get install python3-dev

sudo gpio -v
sudo gpio readall
sudo apt-get install wiringpi
sudo git clone git://git.drogon.net/wiringPi

#sudo apt-get install python-rpi.gpio
sudo apt-get -y install python3-rpi.gpio
#sudo pip-3.7 install RPi.GPIO

sudo raspi-config
sudo i2cdetect -y 0 # on Pi Rev. 1
sudo i2cdetect -y 1 # and Rev. 2
sudo cd PiFunk

sudo make

#sudo gcc pifunk.c -w -Q -std=gnu99 -g3 -ggdb3 -pg -Iinclude -I/opt/vc/include -Llib -L/opt/vc/lib/ -lbcm_host -lbcm2838 -lm -lpthread -lgnu -lsndfile -O3 -fPIC -D_USE_MATH_DEFINES -D_GNU_SOURCE -DRASPI1 -o lib/pifunk.o
#sudo gcc pifunk.o -w -Q -std=gnu99 -g3 -ggdb3 -pg -Iinclude -I/opt/vc/include -Llib -L/opt/vc/lib/ -lbcm_host -lbcm2838 -lm -lpthread -lgnu -lsndfile -O3 -fPIC -D_USE_MATH_DEFINES -D_GNU_SOURCE -DRASPI1 -shared -o lib/pifunk.so
#sudo gcc pifunk.c -Wall -Q -std=gnu99 -g3 -ggdb3 -pg -Iinclude -I/opt/vc/include -Llib -L/opt/vc/lib/ -lbcm_host -bcm2835 -lm -lpthread -lgnu -lsndfile -O3 -D_USE_MATH_DEFINES -D_GNU_SOURCE -DRASPI1 -o bin/pifunk
#sudo reboot

# run py code
sudo ./pifunk -n sound.wav -f 446.006250 -s 22050 -m fm -p 7 -c callsign -g 7 -d 14 -b 12.50 -t 1 -x on

#sudo python pifunk.py
