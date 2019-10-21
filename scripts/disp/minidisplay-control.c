
/* mini-i2C-display
#vcc an 3,3v
#gnd an gnd
#sda an pin 3
#scl an pin 5
#(rpi b+)
#http://m.instructables.com/id/How-to-use-OLED-display-arduino-module/

modprobe i2c_bcm2708
modprobe i2c_dev
chmod 666 /dev/i2c-1
adduser pi i2c
i2cdetect -l
#zeigt er liste der busse an
i2cdetect -y 1
#(wenn bus 1 sonst bus 0) listet er ger�te auf dem bus auf
#adresse ist bei mir 0x3c (ich nutze wiringPi)

#kompilieren mit:
g++ wpi-i2c.c -o wpi-i2c -lwiringPi
#zuerst wird das display initialisiert, dann gecleared.
#mit reset_pos() wird die schreibeposition wieder auf anfang gesetzt auf dem display,
#dazu bitte die dokumentation lesen, insbesondere das kapitel �ber die verschiedenen schreibmodi (vertikal/horizontal) etc.
#Mit render() werden dann 8 Byte daten geschrieben, mit denen sich ein 8x8 px gro�es zeichen darstellen l�sst,
#im beispiel ein kleines haus-symbol. im internet gibt es etliche 8x8 header only fonts, mit denen sich dann einfach texte darstellen lassen.
*/

include <wiringPiI2C.h>
include <string.h>

void init(int display)
{
wiringPiI2CWriteReg8(display,0x00,0xae); // display off
wiringPiI2CWriteReg8(display,0x00,0xd5); // clockdiv
wiringPiI2CWriteReg8(display,0x00,0x80);
wiringPiI2CWriteReg8(display,0x00,0xa8); // multiplex
wiringPiI2CWriteReg8(display,0x00,0x3f);
wiringPiI2CWriteReg8(display,0x00,0xd3); // offset
wiringPiI2CWriteReg8(display,0x00,0x00);
wiringPiI2CWriteReg8(display,0x00,0x40); // startline
wiringPiI2CWriteReg8(display,0x00,0x8d); // charge pump
wiringPiI2CWriteReg8(display,0x00,0x14);
wiringPiI2CWriteReg8(display,0x00,0x20); // memory mode
wiringPiI2CWriteReg8(display,0x00,0x00);
wiringPiI2CWriteReg8(display,0x00,0x01);
wiringPiI2CWriteReg8(display,0x00,0xa1); // segregmap
wiringPiI2CWriteReg8(display,0x00,0xc8); // comscandec
wiringPiI2CWriteReg8(display,0x00,0xda); // set com pins
wiringPiI2CWriteReg8(display,0x00,0x12);
wiringPiI2CWriteReg8(display,0x00,0x81); // contrast
wiringPiI2CWriteReg8(display,0x00,0xcf);
wiringPiI2CWriteReg8(display,0x00,0xd9); // precharge
wiringPiI2CWriteReg8(display,0x00,0xf1);
wiringPiI2CWriteReg8(display,0x00,0xdb); // vcom detect
wiringPiI2CWriteReg8(display,0x00,0x40);
wiringPiI2CWriteReg8(display,0x00,0xa4); // resume
wiringPiI2CWriteReg8(display,0x00,0xa6); // normal (not inverted)
wiringPiI2CWriteReg8(display,0x00,0xaf); // display on
}

void render(int display,char *bitmap) {
char m[8], n[8] = {0};
memcpy(&m,bitmap,8);

for(int y = 0; y < 8; y++) {

for(int x = 0; x < 8; x++) {
//choose one:
n[y] |= (m[x] & (1<<(7-y))) >> (7-y) << x ; // 270 grad drehen
}
}

for (int x=7; x >= 0; x--) {
int a = (int)n[x];
wiringPiI2CWriteReg8(display,0x40,a);
}
}

void reset_pos(int display)
{
wiringPiI2CWriteReg8(display,0x00,0xb0);
wiringPiI2CWriteReg8(display,0x00,0x00);
wiringPiI2CWriteReg8(display,0x00,0x10);
}

void clear(int display)
{
reset_pos(display);
for (int x = 0; x < 128; x++) {

for (int y = 0; y < 8; y++) {

wiringPiI2CWriteReg8(display,0x40,0x00);
}
}
}

void clear2(int display)
{
reset_pos(display);
for (int x = 0; x < 128; x++) {

for (int y = 0; y < 8; y++) {

wiringPiI2CWriteReg8(display,0x40,0xff);
}
}
}

int main(int argc, char *argv[])
{

char test[8] = {
0b00011000,
0b00111100,
0b01111110,
0b11111111,
0b01100110,
0b01100110,
0b01100110,
0b00000000,
};

int display = wiringPiI2CSetup(0x3c);
init(display);
clear(display);
render(display,test);
}

// EOF
