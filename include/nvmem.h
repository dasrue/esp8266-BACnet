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

#ifndef _NVMEM_H
#define _NVMEM_H

#include "device.h"

// Choose where to store the parameters.
// This is a 16 bit int passed to the function, and the last 3 zeros are cut off, so as to align it with a 4KB block.
// eg to store data starting at 0x3D000 this value should be set to 0x3D.
// 0x00000 - 0x10000 is 64K of eagle.flash.bin
// 0x10000 is irom. Its space depends on space used. In this example project we use ~250KB
// This means we should have space available after 0x4E000
// Choose 0x65 to give the firmware some room to grow. This should be fine even on a 512KB flash chip.
#define PARAM_FLASH_START_ADDR		0x65

// This byte is used to check if the data has been stored in the flash at some point. If this byte is not present,
// then we know the data has never been initialised.
#define FLASH_INIT_BYTE		0x43

struct nv_wifi_t {
	int status;					// 2 bytes
	char ssid[32];				// 32 bytes
	char password[64];			// 64 bytes
	bool dhcp_en;				// 1 byte
	uint8_t static_ip[4];		// 4 bytes
	uint8_t static_netmask[4];	// 4 bytes
	uint8_t static_gateway[4];	// 4 bytes
};								// 111 bytes

struct nv_bacnet_t {
	uint16_t udp_port;										// 2 bytes
	uint32_t Object_Instance_Number;						// 4 bytes
	char Device_Name[MAX_DEV_NAME_LEN + 1];					// 33 bytes
	char Model_Name[MAX_DEV_MOD_LEN + 1];					// 33 bytes
	char Application_Software_Version[MAX_DEV_VER_LEN + 1];	// 17 bytes
	char Location[MAX_DEV_LOC_LEN + 1];						// 65 bytes
	char Description[MAX_DEV_DESC_LEN + 1];					// 65 bytes
};															// 219 bytes

struct nvmem_data_t {
	uint8_t flash_initState;		// 1 byte
	struct nv_wifi_t nv_wifi;		// 111 bytes
	struct nv_bacnet_t nv_bacnet;	// 219 bytes
};									// 331 bytes

struct nvmem_data_t nvmem_data;

void nvmem_writeData();		// Call this when data needs updating
void nvmem_readData();		// Call this in init function
void nvmem_initData();		// This function will be run when a read fails.


#endif	/* _NVMEM_H */
