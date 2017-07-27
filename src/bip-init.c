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
	This file contains the bip init function to intialise the bacnet ip stack.
*/

#include "c_types.h"
#include "ip_addr.h"
#include "espconn.h"
#include "user_interface.h"
#include "osapi.h"
#include "net.h"
#include "bip.h"
#include "apdu.h"
#include "handlers.h"


void ICACHE_FLASH_ATTR user_set_station_config() {
	char ssid[32] = "test";
	char password[64] = "test1234";
	struct station_config stationConf;

	stationConf.bssid_set = 0;
	os_memcpy(&stationConf.ssid, ssid, 32);
	os_memcpy(&stationConf.password, password, 32);
	wifi_station_set_config(&stationConf);
}

void ICACHE_FLASH_ATTR setupHandlers() {
	Device_Init(NULL);
	apdu_set_unconfirmed_handler(SERVICE_UNCONFIRMED_WHO_IS,handler_who_is);
	apdu_set_confirmed_handler(SERVICE_CONFIRMED_READ_PROPERTY,handler_read_property);
	apdu_set_confirmed_handler(SERVICE_CONFIRMED_WRITE_PROPERTY,handler_write_property);
}

bool ICACHE_FLASH_ATTR bip_init(
		char *ifname)
{
	if(wifi_get_opmode()!=0x01)
		wifi_set_opmode(0x01);		// Make sure we are in station mode.
	user_set_station_config();		// Setup wifi.
	BACnetESPsocket.type = ESPCONN_UDP;
	BACnetESPsocket.recv_callback = bip_recv_callback;	// Setup the recv callback
	if(bip_get_port()==0)
		bip_set_port(0xBAC0);	// If port hasn't been initialised, set it to BACnet default.
	BACnetESPsocket.proto.udp = &BACnetUDPinfo;	// Setup the protocol pointer
	BACnetESPsocket.proto.udp->local_port = bip_get_port();
	if(espconn_create(&BACnetESPsocket)==0)
		bip_set_socket(1);
}
