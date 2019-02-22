/*******************************************************************************
 * M2MByteOrder.h
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

#ifndef M2M_IO_M2MBYTEORDER_H_
#define M2M_IO_M2MBYTEORDER_H_



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



#include <stdint.h>


/*******************************************************************************
 * Class variable
 ******************************************************************************/
/**
 *
 */
#ifndef M2MByteOrder_Endian
typedef enum
	{
	M2MByteOrder_BIG_ENDIAN,  //!< ByteOrder_BIG_ENDIAN
	M2MByteOrder_LITTLE_ENDIAN//!< ByteOrder_LITTLE_ENDIAN
	} M2MByteOrder_Endian;
#endif /* M2MByteOrder_Endian */



/*******************************************************************************
 * Public method
 ******************************************************************************/
/**
 * @param intNumber
 * @param targetEndian
 * @return
 */
uint32_t M2MByteOrder_convertInteger (const uint32_t intNumber, const M2MByteOrder_Endian targetEndian);


/**
 * @param shortNumber
 * @param targetEndian
 * @return
 */
uint32_t M2MByteOrder_convertShort (const uint16_t shortNumber, const M2MByteOrder_Endian targetEndian);



#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2M_IO_M2MBYTEORDER_H_ */
