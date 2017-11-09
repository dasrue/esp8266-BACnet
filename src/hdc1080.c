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
	This file provides the interface to the HDC1080 temp/humidity sensor
*/

#include "c_types.h"
#include "user_interface.h"

#define HDC1080_ADDR	0b01000000

void ICACHE_FLASH_ATTR hdc1080_startMeasurement() {
	i2c_master_start();
	i2c_master_writeByte(HDC1080_ADDR << 1);	// Send HDC1080 addr, write mode
	if(i2c_master_checkAck()==0)
		return;
	i2c_master_writeByte(0x02);		// Set HDC1080 pointer to configuration register (0x02)
	if(i2c_master_checkAck()==0)
		return;
	uint16_t cfgRegVal = (1 << 12) ;	// Set HDC1080 to measure both temp and humidity, in 14 bit mode
	i2c_master_writeByte(cfgRegVal >> 8);	// Write MSB of config register value
	if(i2c_master_checkAck()==0)
		return;
	i2c_master_writeByte(cfgRegVal);		// Write LSB of config register value
	if(i2c_master_checkAck()==0)
		return;
	i2c_master_stop();

	i2c_master_start();
	i2c_master_writeByte(HDC1080_ADDR << 1);	// Send HDC1080 addr, write mode
	if(i2c_master_checkAck()==0)
		return;
	i2c_master_writeByte(0x00);		// Set HDC1080 pointer to temperature register (0x00)
	if(i2c_master_checkAck()==0)
		return;
	i2c_master_stop();
}

void ICACHE_FLASH_ATTR hdc1080_getTempHumid(float* temp, float* humid) {
	i2c_master_start();
	i2c_master_writeByte((HDC1080_ADDR << 1 )| 1);	// Send HDC1080 addr, read mode
	if(i2c_master_checkAck()==0)
		return;
	uint16_t tempVal;
	tempVal = i2c_master_readByte() << 8;
	if(i2c_master_checkAck()==0)
		return;
	tempVal |= i2c_master_readByte();
	if(i2c_master_checkAck()==0)
			return;
	*temp = ((float)tempVal / 65536)*165 - 40;
	tempVal = i2c_master_readByte() << 8;
	if(i2c_master_checkAck()==0)
		return;
	tempVal |= i2c_master_readByte();
	if(i2c_master_checkAck()==0)
		return;
	*humid = ((float)tempVal / 65536)*100;
	i2c_master_stop();
}
