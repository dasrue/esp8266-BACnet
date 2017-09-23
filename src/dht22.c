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
	This file contains an interface to the DHT11 sensor.
*/

#include <stdint.h>			// uint types
#include "c_types.h"		// ICACHE_FLASH_ATTR define
#include "osapi.h"			// os_delay_us
#include "gpio.h"			// gpio_output_set
#include "user_interface.h"	// system_get_time
#include "dht22.h"

int8_t ICACHE_FLASH_ATTR dht22_read(uint16_t* temp, uint16_t* humid) {
	uint32_t startTime, lowTime, highTime;
	uint8_t tempStore[5];	// We should get 5 bytes of data from DHT every time.
	gpio_output_set(0, DHT22_PIN, DHT22_PIN, 0);	// Set DHT pin as output and output a low signal.
	os_delay_us(500);		// Wait 500us for DHT22 to respond.
	gpio_output_set(0, 0, 0, DHT22_PIN);	// Set DHT pin  input
	startTime = system_get_time();
	while(gpio_input_get() & DHT22_PIN) {		// Wait for DHT22 to pull pin low. If this times out, then read failed.
		if((system_get_time()-startTime) > 50)	// If we waited longer than 50us, then the device is not responding.
			return DHT_ERR_NORESP;
	}
	startTime = system_get_time();
	while(!(gpio_input_get() & DHT22_PIN)) {	// Wait for DHT22 to pull pin high
		if((system_get_time()-startTime) > 100)	// If we waited longer than 100us, then the device is not responding.
			return DHT_ERR_NORESP;
	}
	lowTime = system_get_time() - startTime;
	if(lowTime < 60)			// Low time should be 80us. Give +-20us tolerance.
		return DHT_ERR_TIMING;
	startTime = system_get_time();
	while(gpio_input_get() & DHT22_PIN) {	// Wait for DHT22 to pull pin low
		if((system_get_time()-startTime) > 100)	// If we waited longer than 100us, then the device is not responding.
			return DHT_ERR_NORESP;
	}
	highTime = system_get_time() - startTime;
	if(highTime < 60)			// High time should be 80us. Give +-20us tolerance.
		return DHT_ERR_TIMING;
	for(uint8_t i=0; i<8*5; i++) {
		startTime = system_get_time();
		while(!(gpio_input_get() & DHT22_PIN)) {	// Wait for DHT22 to pull pin high
			if((system_get_time()-startTime) > 70)	// If we waited longer than 70us, then the device is not responding.
				return DHT_ERR_NORESP;
		}
		lowTime = system_get_time() - startTime;
		if(lowTime < 30)			// Low time should be 50us. Give +-20us tolerance.
			return DHT_ERR_TIMING;
		startTime = system_get_time();
		while(gpio_input_get() & DHT22_PIN) {	// Wait for DHT22 to pull pin low
			if((system_get_time()-startTime) > 90)	// If we waited longer than 90us, then the device is not responding.
				return DHT_ERR_NORESP;
		}
		highTime = system_get_time() - startTime;
		if(highTime < 6)
			return DHT_ERR_TIMING;
		if(highTime < 50) {	// Bit is 0
			tempStore[i/8] &= ~(1 << i%8);	// Clear bit in tempStore
		} else {			// Bit is 1
			tempStore[i/8] |= (1 << i%8);	// Set bit in tempStore
		}
	}
	uint16_t checksumValue = tempStore[0] + tempStore[1] + tempStore[2] + tempStore[3];	// Calculate checksum.
	if(tempStore[4]!=(checksumValue&0xFF))
		return DHT_ERR_CSUM;
	*temp = (tempStore[2] << 8) & tempStore[3];	// High byte is "integral", low byte is "decimal"
	*humid = (tempStore[0] << 8) & tempStore[1];	// High byte is "integral", low byte is "decimal"
	return DHT_OK;
}
