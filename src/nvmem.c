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
	This file contains functions to retrieve and store information in the ESP8266's
	flash memory.
*/

#include "c_types.h"
#include "nvmem.h"

void ICACHE_FLASH_ATTR nvmem_writeData() {	// Load data from ram cache into flash
	system_param_save_with_protect(PARAM_FLASH_START_ADDR,(void*)&nvmem_data,sizeof(nvmem_data));
}
void ICACHE_FLASH_ATTR nvmem_readData() {		// Load data from flash into ram cache
	if(!(system_param_load(PARAM_FLASH_START_ADDR,0,(void*)&nvmem_data,sizeof(nvmem_data)))) {
		nvmem_initData();
	} else if(nvmem_data.udp_port==0xFFFF) {	// Data has never been initialised
		nvmem_initData();
	}
}

void ICACHE_FLASH_ATTR nvmem_initData() {


	// bacnet init
	nvmem_data.udp_port = 0xBAC0;		// Default bacnet port
	nvmem_data.Object_Instance_Number = 4321;	// Use a default device number of 4321
	strcpy(nvmem_data.Device_Name,"ESP-BACnet");
	strcpy(nvmem_data.Model_Name,"ESP-BACnet");
	strcpy(nvmem_data.Application_Software_Version,"0.0.0 Alpha 0");	// Pretty accurate representation of the state of this repo lol
	strcpy(nvmem_data.Location,"42 Wallaby Way, Sydney");
	strcpy(nvmem_data.Description,"ESP-BACnet Device");

	// Write the data
	nvmem_writeData();
}
