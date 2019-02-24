// ### BOILERPLATE ###
// Meter Clock Firmware
// Copyright (C) 2007 Peter Todd <pete@petertodd.org>
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// ### BOILERPLATE ###

#include <common.h>
#include <ds3231.h>
#include <i2c.h>
#include <metrics.h>

#define slave_addr_write b(11010000)
#define slave_addr_read b(11010001)

ds3231_time_t ds3231_time;

void init_ds3231(){
  init_i2c();
}

uint8_t from_bcd(uint8_t d){
  return (d & b(00001111)) + (((d & b(11110000)) >> 4) * 10);
}

uint8_t to_bcd(uint8_t d){
  return (((d / 10) << 4) & b(11110000)) | ((d % 10) & b(00001111));
}

uint8_t read_from_ds3231(uint8_t addr){
  uint8_t r;
  i2c_reset();
  i2c_start();
  i2c_write(slave_addr_write);
  i2c_write(addr);
  i2c_start();
  i2c_write(slave_addr_read);
  r = i2c_read(true);
  i2c_stop();
  return r;
}

void read_time_from_ds3231(){
  ds3231_time.secs = from_bcd(read_from_ds3231(0));
  ds3231_time.mins = from_bcd(read_from_ds3231(1));
  ds3231_time.hours = from_bcd(read_from_ds3231(2));

  // get temperature, every time
  new_temp_sample(read_from_ds3231(0x11));
}

void write_time_to_ds3231(){
  i2c_reset();
  i2c_start();
  i2c_write(slave_addr_write);
  i2c_write(0);
  i2c_write(to_bcd(ds3231_time.secs));
  i2c_write(to_bcd(ds3231_time.mins));
  i2c_write(to_bcd(ds3231_time.hours));
  i2c_stop();
}

// Local Variables:
// mode: C
// fill-column: 76
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
// vim: et:sw=2:sts=2:ts=2:cino=>2s,{s,\:s,+s,t0,g0,^-2,e-2,n-2,p2s,(0,=s:
