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
#include "driver/i2c_master_user.h"
#include "htu21d.h"
#include "osapi.h"

#define HTU21D_ADDR	0x40

static ETSTimer htu21d_timer;
uint8_t htu21d_timer_state;

void htu21d_timerFunc(void* arg);

void ICACHE_FLASH_ATTR htu21d_init() {
	i2c_master_gpio_init();
	i2c_master_init();
	os_timer_setfn(&htu21d_timer,(os_timer_func_t *) htu21d_timerFunc, NULL);
	os_timer_arm(&htu21d_timer, 1000, 0);
}

int ICACHE_FLASH_ATTR htu21d_startTempMeasurement() {
	i2c_master_start();
	i2c_master_writeByte(HTU21D_ADDR << 1);	// Send HTU21D addr, write mode
	if(i2c_master_checkAck()==0)
		return -1;
	i2c_master_writeByte(0xF3);		// Start HTU21D temp measurement
	if(i2c_master_checkAck()==0)
		return -2;
	i2c_master_stop();
	return 0;
}

int ICACHE_FLASH_ATTR htu21d_startHumidMeasurement() {
	i2c_master_start();
	i2c_master_writeByte(HTU21D_ADDR << 1);	// Send HTU21D addr, write mode
	if(i2c_master_checkAck()==0)
		return -1;
	i2c_master_writeByte(0xF5);		// Start HTU21D temp measurement
	if(i2c_master_checkAck()==0)
		return -2;
	i2c_master_stop();
	return 0;
}

int ICACHE_FLASH_ATTR htu21d_getTemp(float* temp) {
	i2c_master_start();
	i2c_master_writeByte((HTU21D_ADDR << 1 )| 1);	// Send HTU21D addr, read mode
	if(i2c_master_checkAck()==0)
		return -1;
	uint16_t tempVal;
	tempVal = i2c_master_readByte() << 8;
	i2c_master_send_ack();
	tempVal |= i2c_master_readByte();
	i2c_master_send_ack();
	*temp = ((float)tempVal / 65536)*175.72 - 46.85;
	i2c_master_stop();
	return 0;
}

int ICACHE_FLASH_ATTR htu21d_getHumid(float* humid) {
	i2c_master_start();
	i2c_master_writeByte((HTU21D_ADDR << 1 )| 1);	// Send HTU21D addr, read mode
	if(i2c_master_checkAck()==0)
		return -1;
	uint16_t tempVal;
	tempVal = i2c_master_readByte() << 8;
	i2c_master_send_ack();
	tempVal |= i2c_master_readByte();
	i2c_master_send_ack();
	*humid = ((float)tempVal / 65536)*125 - 6;
	i2c_master_stop();
	return 0;
}

void ICACHE_FLASH_ATTR htu21d_timerFunc(void* arg) {
	arg = 0;	//unused
	switch(htu21d_timer_state) {
	case 0:		// Start a temp measurement
		if(htu21d_startTempMeasurement() < 0) {
			os_timer_arm(&htu21d_timer, 100, 0);
		} else {
			htu21d_timer_state = 1;
			os_timer_arm(&htu21d_timer, 50, 0);		// Wait 50ms for temperature
		}
		break;
	case 1:		// Get temperature measurement
		if(htu21d_getTemp(&currentTemperature) < 0) {
			htu21d_timer_state = 0;
			os_timer_arm(&htu21d_timer, 100, 0);
		} else {
			htu21d_timer_state = 2;
			os_timer_arm(&htu21d_timer, 10, 0);
		}
		break;
	case 2:		// Start a humid measurement
		if(htu21d_startHumidMeasurement() < 0) {
			os_timer_arm(&htu21d_timer, 100, 0);
		} else {
			htu21d_timer_state = 3;
			os_timer_arm(&htu21d_timer, 16, 0);		// Wait 16ms for humidity
		}
		break;
	case 3:		// Get humidity measurement
		if(htu21d_getHumid(&currentHumidity) < 0) {
			htu21d_timer_state = 2;
			os_timer_arm(&htu21d_timer, 100, 0);
		} else {
			htu21d_timer_state = 0;
			os_timer_arm(&htu21d_timer, 1000, 0);	// Wait 1 second till next measurement
		}
		break;
	}
}
