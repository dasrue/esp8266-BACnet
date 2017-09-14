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

void debug_console_task() {
    uint8 uart_buf[128]={0};
    uint16 len = 0;

    os_timer_disarm(&debug_console_timer);
    len = rx_buff_deq(uart_buf, 128 );
    if(len > 0)
    	os_printf("WW Received %s \r\n", uart_buf);
    uint8_t wifiState = wifi_station_get_connect_status();
    os_printf("Wifi state: %s \r\n", wifi_state_to_string(wifiState));
    os_timer_arm(&debug_console_timer, 1000, 0);
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
