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
	This file contains a debug console running through the UART7
*/
#define UART_BUFF_EN  1

#include <stdint.h>     /* for standard integer types uint8_t etc. */
#include "uart_console.h"
#include "osapi.h"
#include "uart_driver.h"
#include "c_types.h"
#include "os_type.h"
#include "user_interface.h"

static ETSTimer debug_console_timer;

void wifi_scanDone_cb(void *arg, STATUS status);

void debug_console_task() {
    uint8 uart_buf[128]={0};
    uint16 len = 0;

    os_timer_disarm(&debug_console_timer);
    len = rx_buff_deq(uart_buf, 128 );
    if(len > 0)
    	os_printf("WW Received %s \r\n", uart_buf);

    uint8_t wifiState = wifi_station_get_connect_status();
    char ssid[64];
    wifi_getSSID(ssid);

    if((wifiState!=STATION_CONNECTING) && (wifiState!=STATION_GOT_IP)) {
    	uart0_sendStr("There was an error connecting to the wifi network \"");
    	uart0_sendStr(ssid);
    	uart0_sendStr("\". The reason was ");
    	uart0_sendStr(wifi_state_to_string(wifiState));
    	uart0_sendStr("\r\nScanning for other wifi networks...\r\n");
    	wifi_station_scan(NULL,wifi_scanDone_cb);
    } else {
    	os_timer_arm(&debug_console_timer, 1000, 0);
    }
}

void ICACHE_FLASH_ATTR debug_console_init() {
	UART_SetPrintPort(UART1);	// Redirect the debug output to the other UART.
	uart_init(BIT_RATE_57600,BIT_RATE_57600);	// Set both UART ports to 57600 baud (something reasonably fast but still reliable)
	os_timer_setfn(&debug_console_timer,(os_timer_func_t *) debug_console_task, NULL);
	os_timer_arm(&debug_console_timer, 1000, 0);
}

char* ICACHE_FLASH_ATTR wifi_state_to_string(uint8_t wifiState) {
	switch(wifiState) {
	case STATION_IDLE:
		return "idle";
	case STATION_CONNECTING:
		return "connecting";
	case STATION_WRONG_PASSWORD:
		return "wrong password";
	case STATION_NO_AP_FOUND:
		return "AP not found";
	case STATION_CONNECT_FAIL:
		return "connection failed";
	case STATION_GOT_IP:
		return "connected";
	}
	return "unknown";
}

void ICACHE_FLASH_ATTR wifi_getSSID(char* ssidBuffer) {		// ssidBuffer should be at least 64 chars.
	struct station_config currentConfig;
	wifi_station_get_config(&currentConfig);	// Get the current wifi config
	os_memcpy(ssidBuffer,currentConfig.ssid,os_strlen(currentConfig.ssid));	// Copy the name into the buffer.
}

void ICACHE_FLASH_ATTR wifi_scanDone_cb(void *arg, STATUS status) {
	if(status==OK) {
		uart0_sendStr("Scan finished. Found the following networks:\r\n");
		struct bss_info *this_bss_info = (struct bss_info*)arg;	// Create a pointer to the first info in the linked list
		uint16_t i = 0;
		char apNumber[8];
		while(this_bss_info!=NULL) {			// Iterate through the linked list.
			itoa(i,apNumber,10);
			uart0_sendStr(apNumber);
			uart0_sendStr(this_bss_info->ssid);	// Print the SSID
			uart0_sendStr("\r\n");				// And a newline afterwards.
			this_bss_info = this_bss_info->next.stqe_next;	// Move to next scan item
			i++;
		}
	}
}

char* ICACHE_FLASH_ATTR itoa (int value, char *result, int base) {
    // check that the base if valid
    if (base < 2 || base > 36) { *result = '\0'; return result; }

    char* ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while ( value );

    // Apply negative sign
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    return result;
}
