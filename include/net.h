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
	This file contains helper functions to bridge between the BACnet/IP stack
	and the esp8266 UDP SDK.
*/

#ifndef NET_H
#define NET_H

#include "c_types.h"
#include "ip_addr.h"
#include "espconn.h"
#include "osapi.h"

#define MAX_NUM_SOCKETS 8


#define AF_INET		2
#define AF_INET6	10

struct in_addr {
	uint32_t s_addr;
};

struct sockaddr {
	int16_t family;
	uint16_t port;
	uint8_t ip[4];
	//uint8_t zero[8];
};

struct sockaddr_in {
	uint16_t sin_family;
	uint16_t sin_port;
	struct in_addr sin_addr;
	//uint8_t sin_zero[8];
};

struct rxPktBuffer_t {
	struct sockaddr remoteInfo;
	int remoteInfoLen;
	uint8_t *pktData;
	unsigned short pktDataLen;
};

struct espconn BACnetESPsocket;

#if 0
int sendto(
	int socketNum,		
	const uint8_t *data,	// We don't modify data so ddeclare it as const.
	int dataLen,
	int flags,
	const struct sockaddr *target,
	int targetStructLen);
#endif

void bip_recv_callback(
	void *arg,
	char *pdata,
	unsigned short len);

#if 0
struct espconn * ICACHE_FLASH_ATTR socketNumberToPointer(
	int socketNum);
#endif

#endif /* NET_H */
