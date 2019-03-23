/*******************************************************************************
 * M2MByteOrder.c
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

#include "m2m/lib/io/M2MByteOrder.h"


/*******************************************************************************
 * Private method
 ******************************************************************************/
/**
 * @param[in] hostlong
 * @return
 */
static uint32_t this_htonl (const uint32_t hostlong)
	{
	//========== Variable ==========
	uint32_t netlong = 0;
	uint8_t *p = (uint8_t *)&netlong;

	p[0] = (hostlong >> 24) & 0xff;
	p[1] = (hostlong >> 16) & 0xff;
	p[2] = (hostlong >>  8) & 0xff;
	p[3] = hostlong & 0xff;
	return netlong;
	}


/**
 * @param[in] hostshort
 * @return
 */
static uint16_t this_htons (const uint16_t hostshort)
	{
	//========== Variable ==========
	uint16_t netshort = 0;
	uint8_t *p = (uint8_t *)&netshort;

	p[0] = (hostshort >> 8) & 0xff;
	p[1] = hostshort & 0xff;
	return netshort;
	}


/**
 * @param[in] netlong
 * @return
 */
static uint32_t this_ntohl (const uint32_t netlong)
	{
	//========== Variable ==========
	uint8_t *p = (uint8_t *)&netlong;

	return (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
	}


/**
 * @param[in] netshort
 * @return
 */
static uint16_t this_ntohs (const uint16_t netshort)
	{
	//========== Variable ==========
	uint8_t *p = (uint8_t *)&netshort;

	return (p[0] << 8) | p[1];
	}


/*******************************************************************************
 * Public method
 ******************************************************************************/
/**
 * @param[in] intNumber
 * @param[in] targetEndian
 * @return
 */
uint32_t M2MByteOrder_convertInteger (const uint32_t intNumber, const M2MByteOrder_Endian targetEndian)
	{
	if (targetEndian==M2MByteOrder_BIG_ENDIAN)
		{
		return this_htonl(intNumber);
		}
	else
		{
		return this_ntohl(intNumber);
		}
	}


/**
 * @param[in] shortNumber
 * @param[in] targetEndian
 * @return
 */
uint32_t M2MByteOrder_convertShort (const uint16_t shortNumber, const M2MByteOrder_Endian targetEndian)
	{
	if (targetEndian==M2MByteOrder_BIG_ENDIAN)
		{
		return this_htons(shortNumber);
		}
	else
		{
		return this_ntohs(shortNumber);
		}
	}



/* End Of File */
