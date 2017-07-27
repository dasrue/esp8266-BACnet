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

struct nv_wifi_t {
	int status;
	char ssid[32];
	char password[64];
};

struct nv_bacnet_t {
	uint32_t Object_Instance_Number;
	BACNET_CHARACTER_STRING My_Object_Name;
	char *Vendor_Name;
	uint16_t Vendor_Identifier;
	char Model_Name[MAX_DEV_MOD_LEN + 1];
	char Application_Software_Version[MAX_DEV_VER_LEN + 1];
	char Location[MAX_DEV_LOC_LEN + 1];
	char Description[MAX_DEV_DESC_LEN + 1];
};

struct nvmem_data_t {
	struct nv_wifi_t nv_wifi;
	struct nv_bacnet_t nv_bacnet;
};

struct nvmem_data_t nvmem_data;




#endif	/* _NVMEM_H */
