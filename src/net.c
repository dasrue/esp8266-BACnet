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

#include "c_types.h"
#include <string.h>		// To provide memcpy
#include <stdlib.h>
#include "net.h"
#include "ip_addr.h"
#include "espconn.h"
#include "bip.h"
#include "mem.h"

//struct espconn *socketStructs[MAX_NUM_SOCKETS];		// Array of pointers to esp8266 sockets
//struct rxPktBuffer_t rxPktBuffer[MAX_NUM_SOCKETS];	// Array of receive data structures

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

struct espconn * ICACHE_FLASH_ATTR socketNumberToPointer(
	int socketNum)
{
	if((socketNum >= MAX_NUM_SOCKETS)||(socketNum < 0))
		return NULL;	// Return NULL if socket number is invalid
	return socketStructs[socketNum];
}

void bip_recv_callback(
	void *arg,
	char *pdata,
	unsigned short len)
{
	struct espconn *thisSocket = arg;
	struct sockaddr_in sin;
	int function;
	BACNET_ADDRESS src;
	uint16_t i;

	sin.sin_family = AF_INET;
	sin.sin_port = thisSocket->proto.udp->remote_port;			// Copy over remote port
	memcpy(sin.sin_addr,thisSocket->proto.udp->remote_ip,4);	// Copy over remote IP

	if(len > MAX_MPDU)
		return;

    /* no problem, just no bytes */
    if(len == 0)
        return;

    /* the signature of a BACnet/IP packet */
    if (pdata[0] != BVLL_TYPE_BACNET_IP)
        return;

    if (bvlc_for_non_bbmd(&sin, pdata, len) > 0) {
        /* Handled, usually with a NACK. */
        return;
    }

    function = bvlc_get_function_code();        /* aka, pdu[1] */
    if ((function == BVLC_ORIGINAL_UNICAST_NPDU) ||
        (function == BVLC_ORIGINAL_BROADCAST_NPDU)) {
        /* ignore messages from me */
        if ((sin.sin_addr.s_addr == bip_get_addr()) &&
            (sin.sin_port == bip_get_port())) {
            len = 0;
        } else {
            /* data in src->mac[] is in network format */
            src->mac_len = 6;
            memcpy(&src->mac[0], &sin.sin_addr.s_addr, 4);
            memcpy(&src->mac[4], &sin.sin_port, 2);
            /* FIXME: check destination address */
            /* see if it is broadcast or for us */
            /* decode the length of the PDU - length is inclusive of BVLC */
            (void) decode_unsigned16(&pdata[2], &len);
            /* subtract off the BVLC header */
            len -= 4;
            if (len < MAX_MPDU) {
                /* shift the buffer to return a valid PDU */
                for (i = 0; i < len; i++) {
                	pdata[i] = pdata[4 + i];
                }
            }
            /* ignore packets that are too large */
            /* clients should check my max-apdu first */
            else {
                len = 0;
            }
        }
    } else if (function == BVLC_FORWARDED_NPDU) {
        memcpy(&sin.sin_addr.s_addr, &pdata[4], 4);
        memcpy(&sin.sin_port, &pdata[8], 2);
        if ((sin.sin_addr.s_addr == bip_get_addr()) &&
            (sin.sin_port == bip_get_port())) {
            /* ignore messages from me */
            len = 0;
        } else {
            /* data in src->mac[] is in network format */
            src->mac_len = 6;
            memcpy(&src->mac[0], &sin.sin_addr.s_addr, 4);
            memcpy(&src->mac[4], &sin.sin_port, 2);
            /* FIXME: check destination address */
            /* see if it is broadcast or for us */
            /* decode the length of the PDU - length is inclusive of BVLC */
            (void) decode_unsigned16(&pdata[2], &len);
            /* subtract off the BVLC header */
            len -= 10;
            if (len < MAX_MPDU) {
                /* shift the buffer to return a valid PDU */
                for (i = 0; i < len; i++) {
                    pdata[i] = pdata[4 + 6 + i];
                }
            } else {
                /* ignore packets that are too large */
                /* clients should check my max-apdu first */
                len = 0;
            }
        }
    }
    npdu_handler(&src, pdata, len);

}

#if 0
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
	remoteInfo.port = thisSocket->proto.udp->remote_port;
	memcpy(remoteInfo.ip,thisSocket->proto.udp->remote_ip,4);
	memcpy(&(rxPktBuffer[socketNum].remoteInfo),&remoteInfo,sizeof(remoteInfo));
	rxPktBuffer[socketNum].remoteInfoLen = sizeof(remoteInfo);
	rxPktBuffer[socketNum].pktData = os_malloc(len);	// Remember to free the data once done
	if(rxPktBuffer[socketNum].pktData==NULL)
		return;
	memcpy(rxPktBuffer[socketNum].pktData,pdata, len);
}
#endif
