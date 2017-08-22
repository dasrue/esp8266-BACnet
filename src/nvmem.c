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
	system_param_load(PARAM_FLASH_START_ADDR,0,(void*)&nvmem_data,sizeof(nvmem_data));
	if(nvmem_data.flash_initState!=FLASH_INIT_BYTE) {	// Data has neve been initialised
		nvmem_initData();
	}
}

void ICACHE_FLASH_ATTR nvmem_initData() {

	// Wifi init
	nvmem_data.nv_wifi.status = 0x03;	// Set wifi status to 3 to show not connected -> we have to broadcast an AP to make the config page avaliable
	strcpy(nvmem_data.nv_wifi.ssid,"ESPBACnet_Setup");
	strcpy(nvmem_data.nv_wifi.password,"bacnet");
	nvmem_data.nv_wifi.dhcp_en = 1;

	// bacnet init
	nvmem_data.nv_bacnet.udp_port = 0xBAC0;		// Default bacnet port
	nvmem_data.nv_bacnet.Object_Instance_Number = 4321;	// Use a default device number of 4321
	strcpy(nvmem_data.nv_bacnet.Device_Name,"ESP-BACnet");
	strcpy(nvmem_data.nv_bacnet.Model_Name,"ESP-BACnet");
	strcpy(nvmem_data.nv_bacnet.Application_Software_Version,"0.0.0 Alpha 0");	// Pretty accurate representation of the state of this repo lol
	strcpy(nvmem_data.nv_bacnet.Location,"42 Wallaby Way, Sydney");
	strcpy(nvmem_data.nv_bacnet.Description,"ESP-BACnet Device");
	nvmem_data.flash_initState = FLASH_INIT_BYTE;

	// Write the data
	nvmem_writeData();
}
