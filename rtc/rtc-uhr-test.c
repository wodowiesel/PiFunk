/***********************************************************************************/
/*                                                                                 */
/* Funktionen zur Ansteuerung des DS1307 RealTimeClocks                            */
/* ----------------------------------------------------                            */
/*                                                                                 */
/* (C) 2003  Markus Willimann azsol GmbH                                           */
/*                                                                                 */
/* Version 1.0  4.08.2003                                                          */
/*                                                                                 */
/***********************************************************************************/

//********************** Definitionen ***************
#define DS1307_I2C_INPUT_ADDR           0xD0            // Adresse ueber die auf den DS1307 geschr. wird
#define DS1307_I2C_OUTPUT_ADDR          0xD1            // Adresse ueber die auf den DS1307 geschr. wird


//****************************** Funktionen ****************************************

/***********************************************************************************/
/* Fkt.   rtc_bcd_to_hex: wandelt BCE zu HEX Zahl um                               */
/*                                                                                 */
/***********************************************************************************/
char rtc_bcd_to_hex(char v)
{
  char n;

  n=v; 
  n=n & 0xF0;
  n=n/16;
  v =v & 0x0F;
  n=n*10+v;
  return(n);
}

/***********************************************************************************/
/* Fkt.   rtc_hex_to_bcd: wandelt HEX zu BCD Zahl um                               */
/*                                                                                 */
/***********************************************************************************/
char rtc_hex_to_bcd(char v)
{
  char n;

  n=v; 
  n=n/10;
  n=n*16;
  v =v % 10;
  n=n+v;
  return(n);
}

/***********************************************************************************/
/* Fkt.   write_rtc_ram: Schreibt ein Byte ueber I2C auf den DS1307                */
/*                                                                                 */
/***********************************************************************************/
char write_rtc_ram(char ad,char  d)
{ 
  d=rtc_hex_to_bcd(d);
  i2c_start();
  i2c_write(DS1307_I2C_INPUT_ADDR);
  i2c_write(ad);
  i2c_write(d);
  i2c_stop();
  delay_ms(11);
}

/***********************************************************************************/
/* Fkt.   read_rtc_ram: Liest ein Byte ueber I2C aus dem DS1307                    */
/*                                                                                 */
/***********************************************************************************/
char read_rtc_ram(char ad)
{
  char d;

  i2c_start();
  i2c_write(DS1307_I2C_INPUT_ADDR);
  i2c_write(ad);
  i2c_start();
  i2c_write(DS1307_I2C_OUTPUT_ADDR);
  d=i2c_read(0);
  i2c_stop();
  d=rtc_bcd_to_hex(d); 
  return(d);
}

/***********************************************************************************/
/* Fkt.   gettime: Liest Sekunden, Minuten und Stunden ein                         */
/*                                                                                 */
/***********************************************************************************/
void gettime()
{ 
  system_time_ss        = read_rtc_ram(0);                      
  delay_us(5);
  system_time_mm        = read_rtc_ram(1);
  delay_us(5);
  system_time_hh        = read_rtc_ram(2);
  delay_us(5);
  system_time_dayofweek = read_rtc_ram(3);
  delay_us(5);
  system_time_day       = read_rtc_ram(4);
  delay_us(5);
  system_time_month     = read_rtc_ram(5);
  delay_us(5);
  system_time_year      = read_rtc_ram(6);
  
}

/***********************************************************************************/
/* Fkt.   init_uhr: Initialisiert den DS1307                                       */
/*        Achtung nach einem Stromausfall wird init_uhr() aufgerufen, da der DS1307*/
/*        Batterie gepuffer ist darf nicht alles geresettet werden!!               */
/***********************************************************************************/
void init_uhr()
{
   system_time_mm = read_rtc_ram(0);            // alten Minutenwert vor dem ueberschreiben Lesen
   write_rtc_ram(0x00,00);                      // CH=0 
   write_rtc_ram(0,system_time_mm);             // alten Minutenwert wieder speichern
   system_time_hh = read_rtc_ram(2);            // alten Stundenwert vor dem ueberschreiben Lesen
   write_rtc_ram(0x02,00);                      // auf 24h Mode stellen
   write_rtc_ram(2,system_time_hh);             // alten Stundenwert wieder speichern
   write_rtc_ram(0x07,00);                      //OSC ON  
}   

/***********************************************************************************/
/* Fkt.   set-Funktionen: um alle Parameter zu schreiben.                          */
/*                                                                                 */
/***********************************************************************************/
void setsecond(char d)   {write_rtc_ram(0,d);}
void setminute(char d)   {write_rtc_ram(1,d);}
void sethour(char d)     {write_rtc_ram(2,d);}
void setdayofweek(char d){write_rtc_ram(3,d);}
void setday(char d)      {write_rtc_ram(4,d);}
void setmonth(char d)    {write_rtc_ram(5,d);}
void setyear(char d)     {write_rtc_ram(6,d);}



