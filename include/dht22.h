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

#ifndef _DHT22_H
#define _DHT22_H

#define DHT_OK			0
#define DHT_ERR_NORESP	-10
#define DHT_ERR_TIMING	-11
#define DHT_ERR_CSUM	-12

#define DHT22_PIN		(1<<4)		// Use pin 4.

int8_t dht22_read(uint16_t* temp, uint16_t* humid);

#endif // _DHT22_H
