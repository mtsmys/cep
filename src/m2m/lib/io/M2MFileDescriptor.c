/*******************************************************************************
 * M2MFileDescriptor.c
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

#include "m2m/lib/io/M2MFileDescriptor.h"


/*******************************************************************************
 * Public method
 ******************************************************************************/
/**
 * This method closes indicated socket.<br>
 *
 * @param[in,out] socket	opened socket
 */
void M2MFileDescriptor_closeSocket (int socket)
	{
	//===== Check argument =====
	if (socket>0)
		{
		//===== Close socket =====
		if (close(socket)==0)
			{
			}
		//===== Error handling =====
		else
			{
			}
		}
	//===== Argument error =====
	else
		{
		}
	return;
	}


/**
 * This method returns opened socket with indicated IP address.<br>
 *
 * @param[in] address	IP address information object
 * @return				opened socket or -1(means error)
 */
int M2MFileDescriptor_getSocket (const M2MIPAddress *address)
	{
	//========== Variable ==========
	int fileDescriptor = 0;
	int optval = 1;

	//===== Check argument =====
	if (address!=NULL)
		{
		//===== Get socket =====
		if ((fileDescriptor=socket(M2MIPAddress_getProtocolFamily(address), M2MIPAddress_getSocketType(address), M2MIPAddress_getProtocol(address)))>0
				//===== Disable Nagle's algorithm =====
				&& setsockopt(fileDescriptor, IPPROTO_TCP, TCP_NODELAY, (char *)&optval, sizeof(optval))==0
				)
			{
			return fileDescriptor;
			}
		//===== Error handling =====
		else
			{
			return -1;
			}
		}
	//===== Argument error =====
	else
		{
		return -1;
		}
	}


/**
 * Receive data with file descriptor.<br>
 *
 * @param[in] socket		file descriptor(>0)
 * @param[in] address		target address for receiving message
 * @param[out] buffer		buffer for received data
 * @param[in] bufferLength	length of buffer[byte]
 * @param[in] readTimeout	read timeout[usec]
 * @return					read data size[Byte] or 0(error happened)
 */
unsigned int M2MFileDescriptor_read (int socket, const M2MIPAddress *address, unsigned char *buffer, const unsigned int bufferLength, const unsigned long readTimeout)
	{
	//========== Variable ==========/
	int readLength = 0;
	const int FLAGS = 0;
	struct timeval timeout;
	fd_set readFDSet;
	int result = 0;
	struct sockaddr *src_addr = NULL;
	socklen_t addrlen = 0;

	//===== Check argument =====
	if (socket>0 && address!=NULL && buffer!=NULL && bufferLength>0)
		{
		//===== Receive message and check succeed(returns 0) =====/
		if ((src_addr=M2MIPAddress_getSocketAddress(address))!=NULL
				&& (addrlen=M2MIPAddress_getSocketAddressLength(address))>=0
				&& (readLength=recvfrom(socket, buffer, bufferLength, FLAGS, src_addr, &addrlen))>=0)
			{
			return readLength;
			}
		//===== In progress sequence of Non-Blocking I/O =====
		else if (errno==EAGAIN || errno==EWOULDBLOCK)
			{
			//===== Set read timeout =====
			timeout.tv_sec = 0;
			timeout.tv_usec = readTimeout;
			//===== Set file descriptor =====
			FD_ZERO(&readFDSet);
			FD_SET(socket , &readFDSet);
			//===== In the case of get socket prepared receiving =====
			if ((result=select(socket+1, &readFDSet, NULL, NULL, &timeout))>0
					&& FD_ISSET(socket, &readFDSet)==1)
				{
				//===== Receive message(this procedure will succeed) =====
				if ((readLength=recvfrom(socket, buffer, bufferLength, FLAGS, src_addr, &addrlen))>0)
					{
					return readLength;
					}
				//===== Error handling =====
				else
					{
					return 0;
					}
				}
			//===== In the case of read timeout =====
			else if (result==0)
				{
				return 0;
				}
			//===== Error handling =====
			else
				{
				return 0;
				}
			}
		//===== Error handling =====/
		else
			{
			return 0;
			}
		}
	//===== Argument error =====
	else if (socket<=0)
		{
		return 0;
		}
	else if (address==NULL)
		{
		return 0;
		}
	else if (buffer==NULL)
		{
		return 0;
		}
	else
		{
		return 0;
		}
	}


/**
 * This method send message with TCP/IP connection network.<br>
 * This method has availability of sending max 655,350,000 [Byte] at once<br>
 * call.<br>
 *
 * @param[in] socket		file descriptor(>0)
 * @param[in] address		target address for sending message
 * @param[in] data			send message
 * @param[in] dataLength	length of send message
 * @return					length of sent message or 0(means error)
 */
unsigned long M2MFileDescriptor_write (int socket, const M2MIPAddress *address, const unsigned char *data, const unsigned long dataLength)
	{
	//========== Variable ==========
	int result = 0L;
	unsigned int size = 0;
	unsigned int repeatTime = 0;
	unsigned long sendLength = 0UL;
	unsigned int i = 0;
	struct sockaddr *src_addr = NULL;
	socklen_t addrlen = 0;
	const int FLAGS = 0;
	const unsigned int REPEAT_MAX = 1000;
	const unsigned int UNSIGNED_INT_MAX = 65535;

	//===== Check argument =====
	if (socket>0
			&& address!=NULL
			&& (src_addr=M2MIPAddress_getSocketAddress(address))!=NULL
			&& data!=NULL
			&& dataLength>0)
		{
		//=====  =====
		addrlen = M2MIPAddress_getSocketAddressLength(address);
		//===== In the case of send message length overs max integer size =====
		if (UNSIGNED_INT_MAX<=dataLength)
			{
			//===== Loop while remaining rest data =====
			while (sendLength<dataLength)
				{
				//===== Check send data size =====
				if (UNSIGNED_INT_MAX<(dataLength-sendLength))
					{
					//===== Set max size =====
					size = UNSIGNED_INT_MAX;
					}
				else
					{
					size = dataLength - sendLength;
					}
				//===== Send message(max size : 65,535[Byte]) =====
				if ((result=sendto(socket, &data[sendLength], size, FLAGS, src_addr, addrlen))>0)
					{
					//===== Append sent message length =====
					sendLength += (unsigned long)result;
					}
				//===== Error handling =====
				else
					{
					}
				//===== Count repeat time =====
				repeatTime++;
				//===== Check repeat time =====
				if (repeatTime<REPEAT_MAX)
					{
					continue;
					}
				else
					{
					break;
					}
				}
			return sendLength;
			}
		//===== In the case of send message length is under max integer size =====
		else
			{
			//===== Loop while existing rest message =====
			for (i=0; i<REPEAT_MAX; i++)
				{
				//===== Send message(max size : 65,535[Byte]) =====
				if ((result=sendto(socket, &data[sendLength], dataLength-sendLength, FLAGS, src_addr, addrlen))>0)
					{
					//===== Append send message length =====
					sendLength += (unsigned long)result;
					//===== Check sent message length =====
					if (sendLength==dataLength)
						{
						return sendLength;
						}
					//===== In the case of repeat sending =====
					else
						{
						continue;
						}
					}
				//===== Error handling =====
				else
					{
					}
				}
			return sendLength;
			}
		}
	//===== Argument error =====
	else if (socket<=0)
		{
		return 0;
		}
	else
		{
		return 0;
		}
	}



/* End Of File */
