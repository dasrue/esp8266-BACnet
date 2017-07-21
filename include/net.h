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

#define MAX_NUM_SOCKETS 8

struct sockaddr {
	int16_t family;
	uint16_t port;
	uint8_t ip[4];
	uint8_t zero[8];
};

struct rxPktBuffer_t {
	struct sockaddr remoteInfo;
	int remoteInfoLen;
	uint8_t *pktData;
	unsigned short pktDataLen;
};

int ICACHE_FLASH_ATTR sendto(
	int socketNum,		
	const uint8_t *data,	// We don't modify data so ddeclare it as const.
	int dataLen,
	int flags,
	const struct sockaddr *target,
	int targetStructLen);

struct espconn * ICACHE_FLASH_ATTR socketNumberToPointer(
	int socketNum);

#endif /* NET_H */
