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
	This file contains a debug console running through the UART
*/
#define UART_BUFF_EN  1

#include <stdint.h>     /* for standard integer types uint8_t etc. */
#include "uart_console.h"
#include "osapi.h"
#include "uart_driver.h"
#include "c_types.h"
#include "os_type.h"
#include "user_interface.h"
#include "mem.h"

static ETSTimer wifi_check_timer;
//static ETSTimer wifi_connect_timer;

void wifi_scanDone_cb(void *arg, STATUS status);

char** ssid_list;
uint16_t ssid_list_len;

#if 0	// Some old functions. Decided to use a state machine instead.
void ICACHE_FLASH_ATTR wifi_check_task() {
    os_timer_disarm(&wifi_check_timer);
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
    	os_timer_arm(&wifi_check_timer, 1000, 0);
    }
}

void ICACHE_FLASH_ATTR wifi_connect_task() {
	os_timer_disarm(&wifi_connect_timer);
	uint8 uart_buf[8];
	uint16 len = 0;
	len = rx_buff_deq(uart_buf, 8);
	uart0_sendStr(".");
	if(len > 0) {
		uart_buf[(len >= 8) ? 7 : len] = 0;		// Null terminate the string
		uart0_sendStr(uart_buf);
		uart0_sendStr("\r\nConnecting to ");
		int selection = myAtoi(uart_buf);
		if((selection < ssid_list_len) && (selection >= 0)) {
			uart0_sendStr(ssid_list[selection]);
		}
		uart0_sendStr("\r\n");
	} else {
		os_timer_arm(&wifi_connect_timer, 1000, 0);
	}
}
#endif

void ICACHE_FLASH_ATTR debug_console_init() {
	//UART_SetPrintPort(UART1);	// Redirect the debug output to the other UART.
	uart_init(BIT_RATE_57600,BIT_RATE_57600);	// Set both UART ports to 57600 baud (something reasonably fast but still reliable)
	os_timer_setfn(&wifi_check_timer,(os_timer_func_t *) uart_console_process, NULL);
	os_timer_arm(&wifi_check_timer, 100, 1);
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
	os_strcpy(ssidBuffer,currentConfig.ssid);	// Copy the name into the buffer.
}

void ICACHE_FLASH_ATTR wifi_getIP(char* myIP) {		// myIP should be a string of at least 17 chars
	struct ip_info currentIpInfo;
	wifi_get_ip_info(STATION_IF,&currentIpInfo);
	os_sprintf(myIP,"%u.%u.%u.%u",IP2STR(&(currentIpInfo.ip.addr)));
}
void ICACHE_FLASH_ATTR wifi_scanDone_cb(void *arg, STATUS status) {
	if(status==OK) {
		if(ssid_list!=NULL) {
			uint16_t i;
			for(i = 0; i < ssid_list_len; i++) {
				os_free(ssid_list[i]);	// Free the memory used for each of the SSID strings.
			}
			os_free(ssid_list);	// Then free the memory used for the string pointer array.
		}
		ssid_list = os_malloc(sizeof(char**));
		if(ssid_list==NULL)
			return;
		ssid_list_len = 1;
		uart0_sendStr("Scan finished. Found the following networks:\r\n");
		struct bss_info *this_bss_info = (struct bss_info*)arg;	// Create a pointer to the first info in the linked list
		uint16_t i = 0;
		char apNumber[8];
		while(this_bss_info!=NULL) {			// Iterate through the linked list.
			char** tempPtr = os_realloc(ssid_list,sizeof(char**)*(ssid_list_len+1));
			if(tempPtr!=NULL)
				ssid_list = tempPtr;
			else
				break;
			ssid_list[i] = os_malloc(this_bss_info->ssid_len+1);
			if(ssid_list[i]!=NULL)
				os_memcpy(ssid_list[i],this_bss_info->ssid,this_bss_info->ssid_len+1);
			itoa(i,apNumber,10);
			uart0_sendStr(apNumber);
			uart0_sendStr("\t\t");
			uart0_sendStr(this_bss_info->ssid);	// Print the SSID
			uart0_sendStr("\r\n");				// And a newline afterwards.
			this_bss_info = this_bss_info->next.stqe_next;	// Move to next scan item
			i++;
			ssid_list_len++;
		}
		uart0_sendStr("Enter the number of the network you wish to connect to:\r\n");
		//os_timer_setfn(&wifi_connect_timer,(os_timer_func_t *) wifi_connect_task, NULL);
		//os_timer_arm(&wifi_connect_timer, 1000, 0);
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

// A utility function to check whether x is numeric
bool ICACHE_FLASH_ATTR isNumericChar(char x)
{
    return (x >= '0' && x <= '9')? true: false;
}

// A simple atoi() function. If the given string contains
// any invalid character, then this function returns 0
int ICACHE_FLASH_ATTR myAtoi(char *str) {
    if (*str == NULL)
       return 0;

    int res = 0;  // Initialize result
    int sign = 1;  // Initialize sign as positive
    int i = 0;  // Initialize index of first digit

    // If number is negative, then update sign
    if (str[0] == '-')
    {
        sign = -1;
        i++;  // Also update index of first digit
    }

    // Iterate through all digits of input string and update result
    for (; str[i] != '\0'; ++i)
    {
        if (isNumericChar(str[i]) == false)
            return 0; // You may add some lines to write error message
                      // to error stream
        res = res*10 + str[i] - '0';
    }

    // Return result with sign
    return sign*res;
}


char* uart_console_getLine_currentQueue;
uint8_t uart_console_getLine_currentIndex;
const uint8_t uart_console_getLine_bufferSize = 70;

int16_t ICACHE_FLASH_ATTR uart_console_getLine(char* thisLine, uint16_t maxLen) {
	// This function will get chars from the console until a newline char is sent. This function should be run over and over.
	// It will return the length of the line (not including newline/cr chars) once the line has been fully sent.

	if(uart_console_getLine_currentQueue == NULL) {		// If this variable is NULL, then we need to start a new transaction.
		uart_console_getLine_currentQueue = os_malloc(uart_console_getLine_bufferSize);
		if(uart_console_getLine_currentQueue==NULL)		// If malloc failed, return -10.
			return -10;
		uart_console_getLine_currentIndex = 0;
	}

	if(uart_console_getLine_currentIndex >= uart_console_getLine_bufferSize)	// If buffer is full, delete the last char to make room for more data.
		uart_console_getLine_currentIndex--;

	uint16_t amountOfData = rx_buff_deq(&uart_console_getLine_currentQueue[uart_console_getLine_currentIndex], uart_console_getLine_bufferSize - uart_console_getLine_currentIndex);
	uart_console_getLine_currentIndex+=amountOfData;
	uint16_t i;
	for(i=0; i < uart_console_getLine_currentIndex; i++) {
		if(uart_console_getLine_currentQueue[i]=='\n' || uart_console_getLine_currentQueue[i]=='\r') {
			uint16_t cpyLen = (i > maxLen) ? maxLen : i;	// Copy length is minimum of maxlen or index.
			os_memcpy(thisLine,uart_console_getLine_currentQueue,cpyLen);
			os_free(uart_console_getLine_currentQueue);
			uart_console_getLine_currentQueue = NULL;
			return uart_console_getLine_currentIndex - 1;
		}
	}
	return -1;	// If we have not found \r or \n in our buffer, then return -1 to signifiy that more data needs to be collected.
}


enum uart_console_state_t {
	console_init = 0,
	console_wifi_connect,
	console_wifi_connecting,
	console_wifi_get_action,
	console_wifi_scanning,
	console_wifi_get_ssid,
	console_wifi_get_bssid,
	console_wifi_get_passwd,
	console_bacnet_get_id,
	console_idle
};

enum uart_console_state_t uart_console_state;

void ICACHE_FLASH_ATTR uart_console_process() {
	char line_buf[65];
	int16_t lineLen;
	struct station_config currentConfig;
	switch(uart_console_state) {
	case console_init:
		uart0_sendStr("Initialising... Please wait...\r\n");
		uart_console_state = console_wifi_connect;	// Connect to saved wifi, and print it out.
		break;
	case console_wifi_connect:
		;
		char ssidBuffer[64];
		wifi_getSSID(ssidBuffer);
		if(os_strcmp(ssidBuffer,"")==0) {
			wifi_station_get_config(&currentConfig);	// Get the current wifi config
			os_strcpy(currentConfig.ssid,"No Network");
			wifi_station_set_config(&currentConfig);
			wifi_getSSID(ssidBuffer);
		}
		uart0_sendStr("Connecting to ");
		uart0_sendStr(ssidBuffer);
		uart0_sendStr("\r\n");
		uart_console_state = console_wifi_connecting;	// Go to state where we wait for wifi to connect.
		break;
	case console_wifi_connecting:
		;
		uint8_t wifiState = wifi_station_get_connect_status();
		if((wifiState!=STATION_CONNECTING) && (wifiState!=STATION_GOT_IP)) {
			//wifi_station_set_reconnect_policy(false);
			uart0_sendStr("There was an error connecting to the wifi network. The reason was ");
			uart0_sendStr(wifi_state_to_string(wifiState));
			uart0_sendStr("\r\n");
			uart0_sendStr("What would you like to do?\r\n");
			uart0_sendStr("\t0 - Retry connection\r\n");
			uart0_sendStr("\t1 - Enter new SSID\r\n");
			uart0_sendStr("\t2 - Enter new BSSID (Hidden network)\r\n");
			uart0_sendStr("\t3 - Scan for new network\r\n");
			uart_console_state = console_wifi_get_action;
		}
		if(wifiState==STATION_GOT_IP) {
			char myIP[17];
			uart0_sendStr("Connected OK. My IP is ");
			wifi_getIP(myIP);
			uart0_sendStr(myIP);
			uart0_sendStr("\r\n");
			uart_console_state = console_idle;
		}
		break;
	case console_wifi_get_action:
		;
		char uart_buf[8];
		uint16_t len = rx_buff_deq(uart_buf, 8);
		if(len > 0) {
			if(len > 7)
				len = 7;	// Prepare for null termination. Need to overwrite last char.
			uart_buf[len] = '\0';	// Null terminate input string.
			int selection = myAtoi(uart_buf);
			switch(selection) {
			case 0:		// Retry connection
				uart0_sendStr("Retrying connection...\r\n");
				wifi_station_connect();
				uart_console_state = console_wifi_connect;
				break;
			case 1:		// Enter new SSID.
				uart0_sendStr("Enter the new SSID:\r\n");
				uart_console_state = console_wifi_get_ssid;
				break;
			case 2:		// Enter new BSSID
				uart0_sendStr("Enter the new BSSID:\r\n");
				uart_console_state = console_wifi_get_bssid;
				break;
			case 3:
				uart0_sendStr("Scanning for networks... Please wait.\r\n");
				wifi_station_scan(NULL,wifi_scanDone_cb);
				uart_console_state = console_wifi_scanning;
				break;
			default:
				break;
			}
		}
		break;
	case console_wifi_get_ssid:
		lineLen = uart_console_getLine(line_buf, 32);
		if(lineLen >= 0) {	// Try to get a line from the console
			line_buf[lineLen] = '\0';	// Null terminate string before sending it
			uart0_sendStr(line_buf);
			uart0_sendStr("\r\n");
			wifi_station_get_config(&currentConfig);	// Get the current wifi config
			os_strncpy(currentConfig.ssid,line_buf,32);
			currentConfig.ssid[31] = '\0';
			currentConfig.password[0] = '\0';
			currentConfig.bssid_set = 0;
			wifi_station_set_config(&currentConfig);
			uart0_sendStr("Enter password:\r\n");
			uart_console_state = console_wifi_get_passwd;
		}
		break;
	case console_wifi_get_bssid:
		lineLen = uart_console_getLine(line_buf, 32);
		if(lineLen >= 18) {	// Try to get a line from the console
			int tempRes;
			char tempChars[3];
			uint8_t bssid[6], i;
			for(i = 0; i < 6; i++ ) {
				os_memcpy(tempChars,&line_buf[3*i],3);
				tempChars[2] = '\0';
				tempRes = myAtoi(tempChars);
				if(tempRes > 255 || tempRes < 0)
					return;
				bssid[i] = (uint8_t) tempRes;	// Convert ints to uint8s

			}
			uart0_sendStr(line_buf);
			uart0_sendStr("\r\n");
			wifi_station_get_config(&currentConfig);	// Get the current wifi config
			os_memcpy(currentConfig.bssid,bssid,6);
			wifi_station_set_config(&currentConfig);
			uart_console_state = console_wifi_get_passwd;
		}
		break;
	case console_wifi_get_passwd:
		lineLen = uart_console_getLine(line_buf, 64);
		if(lineLen >= 1) {	// Try to get a line from the console
			line_buf[lineLen] = '\0';	// Null terminate string before using it
			//uart0_sendStr(line_buf);
			uint8_t i;
			for(i = 0; i < lineLen; i++) {
				uart_tx_one_char(UART0, '*');	// Transmit stars instead of password... super secure
			}
			uart0_sendStr("\r\n");
			wifi_station_get_config(&currentConfig);	// Get the current wifi config
			os_memcpy(currentConfig.password,line_buf,lineLen+1);
			wifi_station_set_config(&currentConfig);
			uart0_sendStr("Connecting to ");
			uart0_sendStr(currentConfig.ssid);
			uart0_sendStr("...\r\n");
			wifi_station_connect();
			uart_console_state = console_wifi_connect;
		}
		break;
	case console_wifi_scanning:
		if(ssid_list_len > 0) {
			lineLen = uart_console_getLine(line_buf, 8);
			if(lineLen >= 1) {
				line_buf[lineLen] = '\0';
				uart0_sendStr(line_buf);
				uart0_sendStr("\r\n");
				int selection = myAtoi(line_buf);
				if(selection >= ssid_list_len) {
					uart0_sendStr("Invalid network specified.\r\n");
					uart0_sendStr("Scanning for networks... Please wait.\r\n");
					uint16_t i;
					for(i = 0; i < ssid_list_len; i++) {
						os_free(ssid_list[i]);
					}
					os_free(ssid_list);
					ssid_list = NULL;
					wifi_station_scan(NULL,wifi_scanDone_cb);
				} else {
					uart0_sendStr("DEBUG: Getting config \r\n");
					wifi_station_get_config(&currentConfig);	// Get the current wifi config
					uart0_sendStr("DEBUG: Setting SSID \r\n");
					os_strcpy(currentConfig.ssid,ssid_list[selection]);
					uart0_sendStr("DEBUG: Setting config \r\n");
					wifi_station_set_config(&currentConfig);
					uart0_sendStr("Enter password for ");
					uart0_sendStr(currentConfig.ssid);
					uart0_sendStr(".\r\n");
					uart_console_state = console_wifi_get_passwd;
				}
			}
		}
		break;
	default:
		break;
	}
}
