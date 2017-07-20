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

#include <string.h>		// To provide memcpy
#include "net.h"
#include "espconn.h"

struct espconn *socketStructs[MAX_NUM_SOCKETS];		// Array of pointers to esp8266 sockets
struct rxPktBuffer_t rxPktBuffer[MAX_NUM_SOCKETS];	// Array of receive data structures

int sendto(
	int socketNum,		
	const uint8_t *data,	// We don't modify data so ddeclare it as const.
	int dataLen,
	int flags,
	const struct sockaddr *target,
	int targetStructLen) 
{
	struct espconn *bacSocket = socketNumberToPointer(socketNum);
	if(bacSocket==NULL)
		return -20;		// Return if the socket doesn't exist
	if(bacSocket->type!=ESPCONN_UDP)
		return -21;		// Return if the socket is in the wrong mode (BACnet/IP uses UDP)
	bacSocket->proto.udp->remote_port = target->port;
	memcpy(&(bacSocket->proto.udp->remote_ip[0]),target->ip,4);
	int16_t packetStatus = espconn_sendto(bacSocket, data, dataLen)==0;
	if(packetStatus==ESPCONN_OK)
		return dataLen;			// If sending succeeded, return number of bytes sent
	else
		return packetStatus;	// Else return the error code (these are all negative)
}

struct espconn * socketNumberToPointer(
	int socketNum)
{
	if((socketNum >= MAX_NUM_SOCKETS)||(socketNum < 0))
		return NULL;	// Return NULL if socket number is invalid
	return socketStructs[socketNum];
}

void recv_callback(
	void *arg,
	char *pdata,
	unsigned short len)
{
	struct espconn *thisSocket = arg;
	struct sockaddr remoteInfo;
	int socketNum;
	for(socketNum = 0; socketNum < MAX_NUM_SOCKETS; socketNum++) {
		if(socketStructs[socketNum]!=NULL) {
			if(socketStructs[socketNum]->proto.udp->local_port==thisSocket->proto.udp->local_port) {
				break;		// Once we find the socket related to the port of the data we rxed, then break.
			}
		}
	}
	if(socketNum==MAX_NUM_SOCKETS)	// Failed to find socket number
		return;
	rxPktBuffer[socketNum].pktDataLen=len;
	remoteInfo->port = thisSocket->proto.udp->remote_port;
	memcpy(remoteInfo->ip,thisSocket->proto.udp->remote_ip,4);
	memcpy(rxPktBuffer[socketNum].remoteInfo,remoteInfo,sizeof(remoteInfo));
	rxPktBuffer[socketNum].remoteInfoLen = sizeof(remoteInfo);
	rxPktBuffer[socketNum].pktData = malloc(len);	// Remember to free the data once done
	if(rxPktBuffer[socketNum].pktData==NULL)
		return;
	memcpy(rxPktBuffer[socketNum].pktData,pdata, len);
}
