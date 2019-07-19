#include <stdio.h>
#include <common.h>
#include <Wire.h>

#define DS3231_I2C_ADDR             0x68
#define DS3231_TEMPERATURE_MSB      0x11
#define DS3231_TEMPERATURE_LSB      0x12

byte temp_msb;
byte temp_lsb;

void setup()
{
  Serial.begin(9600);
  Wire.begin();
}

void loop()
{
  temp_msb = DS3231_get_MSB();
  temp_lsb = DS3231_get_LSB();

  Serial.print(temp_msb);

  switch(temp_lsb)
  {
  case 0:
    Serial.println(".00");
    break;
  case 1 :
    Serial.println(".25");
    break;
  case 2:
    Serial.println(".50");
    break;
  case 3:
    Serial.println(".75");
    break;
  }


  delay(2000);
}

byte DS3231_get_MSB()
{
  Wire.beginTransmission(DS3231_I2C_ADDR);
  Wire.write(DS3231_TEMPERATURE_MSB);
  Wire.endTransmission();

  Wire.requestFrom(DS3231_I2C_ADDR, 1);
  temp_msb = Wire.read();

}

byte DS3231_get_LSB()
{

  Wire.beginTransmission(DS3231_I2C_ADDR);
  Wire.write(DS3231_TEMPERATURE_LSB);
  Wire.endTransmission();

  Wire.requestFrom(DS3231_I2C_ADDR, 1);
  temp_lsb = Wire.read() >> 6;


}
