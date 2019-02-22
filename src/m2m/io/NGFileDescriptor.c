/*******************************************************************************
 * NGFileDescriptor.c
 *
 * Copyright (c) 2015, Akihisa Yasuda
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

#include "ng/io/NGFileDescriptor.h"


/*******************************************************************************
 * Public method
 ******************************************************************************/
/**
 * This method closes indicated socket.<br>
 *
 * @param[in,out] socket	opened socket
 */
void NGFileDescriptor_closeSocket (int socket)
	{
	//===== Check argument =====
	if (socket>0)
		{
		//===== Close socket =====
		if (close(socket)==0)
			{
#ifdef DEBUG
			NGLogger_printDebugMessage((NGString *)"NGFileDescriptor_closeSocket()", __LINE__, (NGString *)"Closed socket");
#endif // DEBUG
			}
		//===== Error handling =====
		else
			{
#ifdef DEBUG
			NGLogger_printErrorMessage((NGString *)"NGFileDescriptor_closeSocket()", __LINE__, (NGString *)"Failed to close socket");
#endif // DEBUG
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
int NGFileDescriptor_getSocket (const NGIPAddress *address)
	{
	//========== Variable ==========
	int fileDescriptor = 0;
	int optval = 1;
#ifdef DEBUG
	NGString MESSAGE[64];
#endif // DEBUG

	//===== Check argument =====
	if (address!=NULL)
		{
		//===== Get socket =====
		if ((fileDescriptor=socket(NGIPAddress_getProtocolFamily(address), NGIPAddress_getSocketType(address), NGIPAddress_getProtocol(address)))>0
				//===== Disable Nagle's algorithm =====
				&& setsockopt(fileDescriptor, IPPROTO_TCP, TCP_NODELAY, (char *)&optval, sizeof(optval))==0
				)
			{
#ifdef DEBUG
			memset(MESSAGE, 0, sizeof(MESSAGE));
			NGString_format(MESSAGE, sizeof(MESSAGE)-1, (NGString *)"Opened socket(= \"%d\")", fileDescriptor);
			NGLogger_printDebugMessage((NGString *)"NGFileDescriptor_getSocket()", __LINE__, MESSAGE);
#endif // DEBUG
			return fileDescriptor;
			}
		//===== Error handling =====
		else
			{
#ifdef DEBUG
			NGLogger_printErrorMessage((NGString *)"NGFileDescriptor_getSocket()", __LINE__, (NGString *)"Failed to get socket");
#endif // DEBUG
			return -1;
			}
		}
	//===== Argument error =====
	else
		{
#ifdef DEBUG
		NGLogger_printErrorMessage((NGString *)"NGFileDescriptor_getSocket()", __LINE__, (NGString *)"Argument \"IPAddress\" is NULL");
#endif // DEBUG
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
unsigned int NGFileDescriptor_read (int socket, const NGIPAddress *address, unsigned char *buffer, const unsigned int bufferLength, const unsigned long readTimeout)
	{
	//========== Variable ==========/
	int readLength = 0;
	const int FLAGS = 0;
	struct timeval timeout;
	fd_set readFDSet;
	int result = 0;
	struct sockaddr *src_addr = NULL;
	socklen_t addrlen = 0;
#ifdef DEBUG
	NGString MESSAGE[64];
#endif // DEBUG

	//===== Check argument =====
	if (socket>0 && address!=NULL && buffer!=NULL && bufferLength>0)
		{
		//===== Receive message and check succeed(returns 0) =====/
		if ((src_addr=NGIPAddress_getSocketAddress(address))!=NULL
				&& (addrlen=NGIPAddress_getSocketAddressLength(address))>=0
				&& (readLength=recvfrom(socket, buffer, bufferLength, FLAGS, src_addr, &addrlen))>=0)
			{
#ifdef DEBUG
			memset(MESSAGE, 0, sizeof(MESSAGE));
			NGString_format(MESSAGE, sizeof(MESSAGE)-1, (NGString *)"Received message(= %d [Byte])", readLength);
			NGLogger_printDebugMessage((NGString *)"NGFileDescriptor_read()", __LINE__, MESSAGE);
#endif // DEBUG
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
#ifdef DEBUG
					memset(MESSAGE, 0, sizeof(MESSAGE));
					NGString_format(MESSAGE, sizeof(MESSAGE)-1, (NGString *)"Received message(= %d [Byte])", readLength);
					NGLogger_printDebugMessage((NGString *)"NGFileDescriptor_read()", __LINE__, MESSAGE);
#endif // DEBUG
					return readLength;
					}
				//===== Error handling =====
				else
					{
#ifdef DEBUG
					NGLogger_printErrorMessage((NGString *)"NGFileDescriptor_read()", __LINE__, (NGString *)"Failed to read data");
#endif // DEBUG
					return 0;
					}
				}
			//===== In the case of read timeout =====
			else if (result==0)
				{
#ifdef DEBUG
				NGLogger_printErrorMessage((NGString *)"NGFileDescriptor_read()", __LINE__, (NGString *)"Read timeout error");
#endif // DEBUG
				return 0;
				}
			//===== Error handling =====
			else
				{
#ifdef DEBUG
				NGLogger_printErrorMessage((NGString *)"NGFileDescriptor_read()", __LINE__, (NGString *)"Failed to read data because of unknown error");
#endif // DEBUG
				return 0;
				}
			}
		//===== Error handling =====/
		else
			{
#ifdef DEBUG
			NGLogger_printErrorMessage((NGString *)"NGFileDescriptor_read()", __LINE__, (NGString *)"Failed to read data from socket because of unknown error");
#endif // DEBUG
			return 0;
			}
		}
	//===== Argument error =====
	else if (socket<=0)
		{
#ifdef DEBUG
		NGLogger_printErrorMessage((NGString *)"NGFileDescriptor_read()", __LINE__, (NGString *)"Argument \"socket\" isn't positive");
#endif // DEBUG
		return 0;
		}
	else if (address==NULL)
		{
#ifdef DEBUG
		NGLogger_printErrorMessage((NGString *)"NGFileDescriptor_read()", __LINE__, (NGString *)"Argument \"address\" is NULL");
#endif // DEBUG
		return 0;
		}
	else if (buffer==NULL)
		{
#ifdef DEBUG
		NGLogger_printErrorMessage((NGString *)"NGFileDescriptor_read()", __LINE__, (NGString *)"Argument \"buffer\" is NULL");
#endif // DEBUG
		return 0;
		}
	else
		{
#ifdef DEBUG
		NGLogger_printErrorMessage((NGString *)"NGFileDescriptor_read()", __LINE__, (NGString *)"Argument \"bufferLength\" isn't positive number");
#endif // DEBUG
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
unsigned long NGFileDescriptor_write (int socket, const NGIPAddress *address, const unsigned char *data, const unsigned long dataLength)
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
#ifdef DEBUG
	NGString MESSAGE[64];
#endif // DEBUG

	//===== Check argument =====
	if (socket>0
			&& address!=NULL
			&& (src_addr=NGIPAddress_getSocketAddress(address))!=NULL
			&& data!=NULL
			&& dataLength>0)
		{
		//=====  =====
		addrlen = NGIPAddress_getSocketAddressLength(address);
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
#ifdef DEBUG
					memset(MESSAGE, 0, sizeof(MESSAGE));
					NGString_format(MESSAGE, sizeof(MESSAGE)-1, (NGString *)"Sent data(= %d [Byte])", result);
					NGLogger_printDebugMessage((NGString *)"NGFileDescriptor_write()", __LINE__, MESSAGE);
#endif // DEBUG
					//===== Append sent message length =====
					sendLength += (unsigned long)result;
					}
				//===== Error handling =====
				else
					{
#ifdef DEBUG
					NGLogger_printErrorMessage((NGString *)"NGFileDescriptor_write()", __LINE__, (NGString *)"Failed to send data");
#endif // DEBUG
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
#ifdef DEBUG
					NGLogger_printInfoMessage((NGString *)"NGFileDescriptor_write()", __LINE__, (NGString *)"Reached the maximum size of the transmission");
#endif // DEBUG
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
#ifdef DEBUG
					memset(MESSAGE, 0, sizeof(MESSAGE));
					NGString_format(MESSAGE, sizeof(MESSAGE)-1, (NGString *)"Sent data(= %d [Byte])", result);
					NGLogger_printDebugMessage((NGString *)"NGFileDescriptor_write()", __LINE__, MESSAGE);
#endif // DEBUG
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
#ifdef DEBUG
					NGLogger_printErrorMessage((NGString *)"NGFileDescriptor_write()", __LINE__, (NGString *)"Failed to send data");
#endif // DEBUG
					}
				}
			return sendLength;
			}
		}
	//===== Argument error =====
	else if (socket<=0)
		{
#ifdef DEBUG
		NGLogger_printErrorMessage((NGString *)"NGFileDescriptor_write()", __LINE__, (NGString *)"Argument \"socket\" isn't positive");
#endif // DEBUG
		return 0;
		}
	else
		{
#ifdef DEBUG
		NGLogger_printErrorMessage((NGString *)"NGFileDescriptor_write()", __LINE__, (NGString *)"Argument \"data\" is NULL");
#endif // DEBUG
		return 0;
		}
	}



/* End Of File */
