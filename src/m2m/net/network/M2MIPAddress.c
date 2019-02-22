/*******************************************************************************
 * M2MIPAddress.c
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

#include "m2m/net/network/M2MIPAddress.h"


/*******************************************************************************
 * Public method
 ******************************************************************************/
/**
 * This method releases allocated memory for IPAddress information object.<br>
 *
 * @param[in,out] address	IP address information object
 */
void M2MIPAddress_delete (M2MIPAddress **address)
	{
	//===== Check argument =====
	if (address!=NULL && (*address)!=NULL)
		{
		//===== Released allocated memory =====
		freeaddrinfo((*address));
		}
	//===== Argument error =====
	else
		{
		}
	return;
	}


/**
 * This method returns a number meaning protocol for socket.<br>
 *
 * @param[in] address	IP address information object
 * @return				a number meaning protocol for socket
 */
int M2MIPAddress_getProtocol (const M2MIPAddress *address)
	{
	//===== Check argument =====
	if (address!=NULL)
		{
		return address->ai_protocol;
		}
	//===== Argument error =====
	else
		{
		return 0;
		}
	}


/**
 * This method returns a number meaning protocol family for socekt.<br>
 *
 * @param[in] address	IP address information object
 * @return				a number meaning protocol family for socekt
 */
int M2MIPAddress_getProtocolFamily (const M2MIPAddress *address)
	{
	//===== Check argument =====
	if (address!=NULL)
		{
		return address->ai_family;
		}
	//===== Argument error =====
	else
		{
		return 0;
		}
	}


/**
 * This method returns IP address structure object for socket communication.<br>
 *
 * @param[in] address	IP address information object
 * @return				IP address structure object for socket communication
 */
struct sockaddr *M2MIPAddress_getSocketAddress (const M2MIPAddress *address)
	{
	//===== Check argument =====
	if (address!=NULL)
		{
		return address->ai_addr;
		}
	//===== Argument error =====
	else
		{
		return NULL;
		}
	}


/**
 * This method returns length of socket address[Byte].<br>
 *
 * @param[in] address	IP address information object
 * @return				length of socket address[Byte]
 */
socklen_t M2MIPAddress_getSocketAddressLength (const M2MIPAddress *address)
	{
	//===== Check argument =====
	if (address!=NULL)
		{
		return address->ai_addrlen;
		}
	//===== Argument error =====
	else
		{
		return 0;
		}
	}


/**
 * This method returns socket type number.<br>
 *
 * @param[in] address	IP address information object
 * @return				socket type number or 0(means error)
 */
int M2MIPAddress_getSocketType (const M2MIPAddress *address)
	{
	//===== Check argument =====
	if (address!=NULL)
		{
		return address->ai_socktype;
		}
	//===== Argument error =====
	else
		{
		return 0;
		}
	}


/**
 * Constructor.<br>
 *
 * @param[in] hostName		host name string
 * @param[in] port			port number string
 * @param[in] protocol		Transport Layer Protocol(TCP or UDP or SCTP) defined number
 * @param[in] socketType	defined socket type number
 * @return					IP address information object
 */
M2MIPAddress *M2MIPAddress_new (const M2MString *hostName, const M2MString *port, const unsigned int protocol, const unsigned int socketType)
	{
	//========== Variable ==========
	M2MIPAddress hints;
	M2MIPAddress *address = NULL;

	//===== Check argument =====
	if ((hostName!=NULL || port!=NULL))
		{
		//===== Set-up specific address information =====
		memset(&hints, 0, sizeof(M2MIPAddress));
		hints.ai_family = AF_UNSPEC; // not specified IP version
		hints.ai_flags = 0;
		hints.ai_protocol = protocol; // 0: allow any protocol
		hints.ai_socktype = socketType;
		//===== Resolve name(and get IP information) =====
		if (getaddrinfo(hostName, port, &hints, &address)==0)
			{
			return address;
			}
		//===== Error handling =====
		else
			{
			return NULL;
			}
		}
	//===== Argument error =====
	else
		{
		return NULL;
		}
	}


/**
 * Thie method retuns next position of IP address information object.<br>
 *
 * @param[in] address	IP address information object
 * @return				next position of IP address information object
 */
M2MIPAddress *M2MIPAddress_next (const M2MIPAddress *address)
	{
	//===== Check argument =====
	if (address!=NULL)
		{
		return address->ai_next;
		}
	//===== Argument error =====
	else
		{
		return NULL;
		}
	}


/* End Of File */
