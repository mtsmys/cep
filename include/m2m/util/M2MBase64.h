/*******************************************************************************
 * M2MBase64.h
 *
 * Copyright (c) 2014, Akihisa Yasuda
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

#ifndef M2M_UTIL_M2MBASE64_H_
#define M2M_UTIL_M2MBASE64_H_


#include "m2m/lang/M2MString.h"



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



/*******************************************************************************
 * Definition
 ******************************************************************************/
/**
 * 4[Byte] for string
 */
#ifndef M2MBase64_FOUR_WORD
#define M2MBase64_FOUR_WORD (unsigned int)4
#endif /* M2MBase64_FOUR_WORD */


/**
 * 3[Byte] for data
 */
#ifndef M2MBase64_THREE_OCTET
#define M2MBase64_THREE_OCTET (unsigned int)3
#endif /* M2MBase64_THREE_OCTET */


/**
 * Max string size of 1 line when adding "\r\n"
 */
#ifndef M2MBase64_CHUNK_LENGTH
#define M2MBase64_CHUNK_LENGTH (unsigned int)76
#endif /* M2MBase64_CHUNK_LENGTH */


/*******************************************************************************
 * Public function
 ******************************************************************************/
/**
 * Convert the argument string to binary with Base64 inverse transformation. <br>
 * Buffering of inversely converted binary data is executed inside this <br>
 * function, so caller must call "M2MHeap_free ()" function to prevent <br>
 * memory leak after the relevant binary data is used.
 *
 * @param string		Target string for Base64 decoding
 * @param stringLength	Length of string[Byte]
 * @param buffer		Buffer for copying decoded data Base64(buffering is executed in this function)
 * @return				Length of decoded data[Byte]
 */
size_t M2MBase64_decode (const M2MString *string, const unsigned long stringLength, unsigned char **buffer);


/**
 * Convert the argument binary data to string by Base64 conversion. <br>
 * Since buffering of converted character string is executed inside <br>
 * this function, calling "M2MHeap_free()" function must be called on <br>
 * the caller side to prevent memory leak after using the character string.<br>
 * In addition, if you wish to delimit the converted character string <br>
 * with a line feed code ("\r\m") every 76 characters, set "chunk" of <br>
 * the argument to true. <br>
 * If you want to put all the converted character strings on one line, <br>
 * set "chunk" to false.<br>
 *
 * @param src		Binary data to be converted to Base64 string
 * @param srcLength	Binary data size[Byte]
 * @param string	Pointer of converted Base64 string (buffering is executed within this function)
 * @param chunk		true: Insert a line feed code (="\r\n") every 76 characters，false: Don't insert a line feed code
 * @return			Pointer of converted Base64 string or NULL (in case of error)
 */
M2MString *M2MBase64_encode (const unsigned char *src, const size_t srcLength, M2MString **string, const bool chunk);



#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2M_UTIL_M2MBASE64_H_ */
