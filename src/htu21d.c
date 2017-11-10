/*####COPYRIGHTBEGIN####
 -------------------------------------------
Copyright (c) 2017 Brad L.

THE FOLLOWING LICENSE APPLIES ONLY TO FILES IN THE esp8266-BACnet/src FOLDER.
SEE OTHER FILES FOR LICENSE TERMS.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 -------------------------------------------
####COPYRIGHTEND####*/

/*
	This file provides the interface to the HTU21D temp/humidity sensor
*/

#include "c_types.h"
#include "user_interface.h"
#include "driver/i2c_master.h"

#define HTU21D_ADDR	0x40

void ICACHE_FLASH_ATTR htu21d_init() {
	i2c_master_gpio_init();
	i2c_master_init();
}

void ICACHE_FLASH_ATTR htu21d_startTempMeasurement() {
	i2c_master_start();
	i2c_master_writeByte(HTU21D_ADDR << 1);	// Send HTU21D addr, write mode
	if(i2c_master_checkAck()==0)
		return;
	i2c_master_writeByte(0xF3);		// Start HTU21D temp measurement
	if(i2c_master_checkAck()==0)
		return;
	i2c_master_stop();
}

void ICACHE_FLASH_ATTR htu21d_startHumidMeasurement() {
	i2c_master_start();
	i2c_master_writeByte(HTU21D_ADDR << 1);	// Send HTU21D addr, write mode
	if(i2c_master_checkAck()==0)
		return;
	i2c_master_writeByte(0xF5);		// Start HTU21D temp measurement
	if(i2c_master_checkAck()==0)
		return;
	i2c_master_stop();
}

void ICACHE_FLASH_ATTR htu21d_getTemp(float* temp) {
	i2c_master_start();
	i2c_master_writeByte((HTU21D_ADDR << 1 )| 1);	// Send HTU21D addr, read mode
	if(i2c_master_checkAck()==0)
		return;
	uint16_t tempVal;
	tempVal = i2c_master_readByte() << 8;
	i2c_master_send_ack();
	tempVal |= i2c_master_readByte();
	i2c_master_send_ack();
	*temp = ((float)tempVal / 65536)*175.72 - 46.85;
	i2c_master_stop();
}

void ICACHE_FLASH_ATTR htu21d_getHumid(float* humid) {
	i2c_master_start();
	i2c_master_writeByte((HTU21D_ADDR << 1 )| 1);	// Send HTU21D addr, read mode
	if(i2c_master_checkAck()==0)
		return;
	uint16_t tempVal;
	tempVal = i2c_master_readByte() << 8;
	i2c_master_send_ack();
	tempVal |= i2c_master_readByte();
	i2c_master_send_ack();
	*humid = ((float)tempVal / 65536)*125 - 6;
	i2c_master_stop();
}
