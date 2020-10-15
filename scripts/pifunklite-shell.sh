
#pifunklite additional shell-terminal script
echo "PiFunklite Shell script "
echo "installing dependencies & packages: "

sudo git clone https://github.com/wodowiesel/PiFunk --branch lite --single-branch lite

sudo gpio -v
sudo gpio readall

sudo raspi-config
sudo i2cdetect -y 0 # on Pi Rev. 1
sudo i2cdetect -y 1 # and Rev. 2
sudo cd PiFunklite

sudo make clean
sudo gcc -std=gnu99 -v -ggdb3 -g3 -O3 -pg -Wall -Iinclude -I/opt/vc/include -Llib -L/opt/vc/lib/ -L/usr/src/lib/ -lgcc -lbcm_host -lbcm2835 -lsndfile -lm -lpthread -D_USE_MATH_DEFINES -D_GNU_SOURCE -march=armv6 -mtune=arm1176jzf-s -mfloat-abi=hard -mfpu=vfp -ffast-math -DRASPI1 --save-temps -o bin/pifunklite
sudo make
sudo make install
#sudo reboot

# run directly
sudo ./pifunklite -n sound.wav -f 26.9650 -s 22050 -m fm -t a -b 12.5 -p 7 -g 7 -d 14 -l 0

#sudo python pifunklite.py
