/*******************************************************************************
 * M2MFileDescriptor.h :
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

#ifndef M2M_IO_M2MFILEDESCRIPTOR_H_
#define M2M_IO_M2MFILEDESCRIPTOR_H_



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



#include "m2m/lang/M2MString.h"
#include "m2m/net/network/M2MIPAddress.h"
#include <errno.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


/*******************************************************************************
 * Public method
 ******************************************************************************/
/**
 * This method closes indicated socket.<br>
 *
 * @param[in,out] socket	opened socket
 */
void M2MFileDescriptor_closeSocket (int socket);


/**
 * This method returns opened socket with indicated IP address.<br>
 *
 * @param[in] address	IP address information object
 * @return				opened socket or -1(means error)
 */
int M2MFileDescriptor_getSocket (const M2MIPAddress *address);


/**
 * Receive data with TCP/IP connection network.<br>
 *
 * @param[in] socket		TCP/IP socket
 * @param[in] address		target address for receiving message
 * @param[out] buffer		buffer for received data
 * @param[in] bufferLength	length of buffer[byte]
 * @param[in] readTimeout	read timeout[usec]
 * @return					received data size[Byte] or 0(error happened)
 */
unsigned int M2MFileDescriptor_read (int socket, const M2MIPAddress *address, unsigned char *buffer, const unsigned int bufferLength, const unsigned long readTimeout);


/**
 * This method send message with TCP/IP connection network.<br>
 *
 * @param[in] socket		TCP/IP socket
 * @param[in] address		target address for sending message
 * @param[in] data			send message
 * @param[in] dataLength	length of send message
 * @return					length of sent message
 */
unsigned long M2MFileDescriptor_write (int socket, const M2MIPAddress *address, const unsigned char *data, const unsigned long dataLength);



#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2M_IO_M2MFILEDESCRIPTOR_H_ */
