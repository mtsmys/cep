/*******************************************************************************
 * M2MIPAddress.h
 *
 * Copyright (c) 2019, Akihisa Yasuda
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

#pragma once

#ifndef M2M_NET_NETWORK_M2MIPADDRESS_H_
#define M2M_NET_NETWORK_M2MIPADDRESS_H_



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



#include "m2m/lang/M2MString.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>



/*******************************************************************************
 * Definition
 ******************************************************************************/
/**
 * IP version independent address structure object.<br>
 * This can interoperate both IPv4 and IPv6.<br>
 */
#ifndef M2MIPAddress
typedef struct addrinfo M2MIPAddress;
#endif /* M2MIPAddress */



/*******************************************************************************
 * Public function
 ******************************************************************************/
/**
 * This method releases allocated memory for IPAddress information object.<br>
 *
 * @param[in,out] address	IP address information object
 */
void M2MIPAddress_delete (M2MIPAddress **address);


/**
 * This method returns a number meaning protocol for socket.<br>
 *
 * @param[in] address	IP address information object
 * @return				a number meaning protocol for socket
 */
int M2MIPAddress_getProtocol (const M2MIPAddress *address);


/**
 * This method returns a number meaning protocol family for socekt.<br>
 *
 * @param[in] address	IP address information object
 * @return				a number meaning protocol family for socekt
 */
int M2MIPAddress_getProtocolFamily (const M2MIPAddress *address);


/**
 * This method returns IP address structure object for socket communication.<br>
 *
 * @param[in] address	IP address information object
 * @return				IP address structure object for socket communication
 */
struct sockaddr *M2MIPAddress_getSocketAddress (const M2MIPAddress *address);


/**
 * This method returns length of socket address[Byte].<br>
 *
 * @param[in] address	IP address information object
 * @return				length of socket address[Byte]
 */
socklen_t M2MIPAddress_getSocketAddressLength (const M2MIPAddress *address);


/**
 * This method returns socket type number.<br>
 *
 * @param[in] address	IP address information object
 * @return				socket type number or 0(means error)
 */
int M2MIPAddress_getSocketType (const M2MIPAddress *address);


/**
 * Constructor.<br>
 *
 * @param[in] hostName		host name string
 * @param[in] port			port number string
 * @param[in] protocol		Transport Layer Protocol(TCP or UDP or SCTP) defined number
 * @param[in] socketType	defined socket type number
 * @return					IP address information object
 */
M2MIPAddress *M2MIPAddress_new (const M2MString *hostName, const M2MString *port, const unsigned int protocol, const unsigned int socketType);


/**
 * Thie method retuns next position of IP address information object.<br>
 *
 * @param[in] address	IP address information object
 * @return				next position of IP address information object
 */
M2MIPAddress *M2MIPAddress_next (const M2MIPAddress *address);



#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2M_NET_NETWORK_M2MIPADDRESS_H_ */
