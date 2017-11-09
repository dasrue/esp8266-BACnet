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
	This file provides the wifi connection and disconnection system.
*/

#include "c_types.h"
#include "ets_sys.h"
#include "user_interface.h"

#define MAX_WIFI_RETRIES	3
uint8_t retry_counter;

void wifi_activate_temp_ap() {
	struct softap_config ap_config;
	int8_t ssidLen;
	ssidLen = os_sprintf(ap_config.ssid,"ESP8266-BACnet Node %02X%02X",0xFF,0xFF);
	os_strcpy(ap_config.password,"ESP-BACnet");
	ap_config.ssid_len = ssidLen;
	ap_config.channel = 1;				// Choose a reasonably safe channel.
	ap_config.authmode = AUTH_WPA2_PSK;	// Use WPA2
	ap_config.ssid_hidden = 0;			// Don't hide ssid so people can connect to it.
	ap_config.max_connection = 2;		// Allow 2 connections.
	ap_config.beacon_interval = 100;	// 100ms beacon interval
	wifi_set_opmode_current(SOFTAP_MODE);
	wifi_softap_set_config_current(&ap_config);
}

void wifi_handle_event_cb(System_Event_t *evt) {
	if(evt->event == EVENT_STAMODE_DISCONNECTED) {
		if(retry_counter < MAX_WIFI_RETRIES) {
			retry_counter++;
			wifi_station_connect();
		} else {
			wifi_activate_temp_ap();
			retry_counter = 0;
		}
	}
}
