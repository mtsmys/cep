/*******************************************************************************
 * M2MBase64.c : Base 64 translation library
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

#include "m2m/lib/util/M2MBase64.h"


/*******************************************************************************
 * Public method
 ******************************************************************************/
/**
 * Convert the argument string to binary with Base64 inverse transformation. <br>
 * Buffering of inversely converted binary data is executed inside this <br>
 * function, so caller must call "M2MHeap_free ()" function to prevent <br>
 * memory leak after the relevant binary data is used.
 *
 * @param[in] string		Target string for Base64 decoding
 * @param[in] stringLength	Length of string[Byte]
 * @param[out] buffer		Buffer for copying decoded data Base64(buffering is executed in this function)
 * @return					Length of decoded data[Byte]
 */
size_t M2MBase64_decode (const M2MString *string, const unsigned long stringLength, unsigned char **buffer)
	{
	//========== Variable ==========
	unsigned long i = 0;
	unsigned long j = 0;
	unsigned long repeatNumber = 0;
	size_t bufferLength = 0;
	signed char word = 0;
	unsigned long index = 0;
	unsigned char dest = 0;
	unsigned long chunkLength = 0;
	unsigned char *chunk = NULL;
	unsigned char tmpBuffer[M2MBase64_FOUR_WORD];
	unsigned int wordIndex = 0;
	const signed char DECODING_TABLE[128] =
		{
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
		52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
		-1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
		15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
		-1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
		41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1
		};
	const unsigned int DECODING_TABLE_LENGTH = sizeof(DECODING_TABLE);
	const unsigned long CRLF_LENGTH = M2MString_length(M2MString_CRLF);

	//===== Check argument =====
	if (string!=NULL)
		{
		//===== Check existing of CRLF(=chunk data) =====
		chunk = (unsigned char *)string;
		while ((chunk=M2MString_indexOf(chunk, M2MString_CRLF))!=NULL)
			{
			chunkLength += CRLF_LENGTH;
			chunk += CRLF_LENGTH;
			}
		//===== Check argument =====
		if ((repeatNumber=(stringLength-chunkLength)/M2MBase64_FOUR_WORD)>0)
			{
			//===== Check existing of padding =====
			switch (M2MString_length(M2MString_indexOf(string, M2MString_EQUAL)))
				{
				//===== There is no padding =====
				case 0:
					//===== Detect decoded data length =====
					bufferLength = repeatNumber * M2MBase64_THREE_OCTET;
					break;
				//===== There is one padding =====
				case 1:
					//===== Detect decoded data length =====
					repeatNumber--;
					bufferLength = (repeatNumber * M2MBase64_THREE_OCTET) + 2;
					break;
				//===== There are two paddings =====
				case 2:
					//===== Detect decoded data length =====
					repeatNumber--;
					bufferLength = (repeatNumber * M2MBase64_THREE_OCTET) + 1;
					break;
				//===== Error handling =====
				default:
					return 0;
				}
			}
		//===== Error handling =====
		else
			{
			return 0;
			}
		//===== Allocate new memory for decoded data =====
		*buffer = (unsigned char *)M2MHeap_malloc(bufferLength+1);
		memset(*buffer, 0, bufferLength+1);
		memset(tmpBuffer, 0, sizeof(tmpBuffer));
		//===== Base64 decode =====
		for (i=0; i<stringLength; i++)
			{
			//===== Select a Base64 word and ASCII code =====
			if ((word=string[i])>=0 && word<DECODING_TABLE_LENGTH && (word=DECODING_TABLE[word])>=0)
				{
				tmpBuffer[wordIndex] = word;
				//===== In the case of conversion(4 -> 3[Byte]) =====
				if (wordIndex==(M2MBase64_FOUR_WORD-1))
					{
					for(j=0; j<M2MBase64_THREE_OCTET; j++)
						{
						dest = tmpBuffer[j] << (j * 2 + 2) | tmpBuffer[j + 1] >> ((2 - j) * 2);
						memcpy(&((*buffer)[index+j]), &dest, 1);
						}
					index += M2MBase64_THREE_OCTET;
					//===== Reset variables for next decode =====
					memset(tmpBuffer, 0, sizeof(tmpBuffer));
					wordIndex = 0;
					}
				else
					{
					wordIndex++;
					}
				}
			//===== Error handling =====
			else
				{
				// do nothing
				}
			}
		//===== Decode words including padding =====
		for (i=0; i<(bufferLength-index); i++)
			{
			dest = tmpBuffer[i] << (i * 2 + 2) | tmpBuffer[i+1] >> ((2 - i) * 2);
			memcpy(&((*buffer)[index+i]), &dest, 1);
			}
		//===== Return decoded data size[Byte] =====
		return bufferLength;
		}
	//===== Argument error =====
	else
		{
		return 0;
		}
	}


/**
 * Convert the argument binary data to string by Base64 conversion. <br>
 * Since buffering of converted character string is executed inside <br>
 * this function, calling "M2MHeap_free()" function must be called on <br>
 * the caller side to prevent memory leak after using the character string.<br>
 * In addition, if you wish to delimit the converted character string <br>
 * with a line feed code ("\r\m") every 76 characters, set "chunk" of <br>
 * the argument to true. <br>
 * If you want to put all the converted character strings on one line, <br>
 * set "chunk" to false. <br>
 *
 * @param[in] src		Binary data to be converted to Base64 string
 * @param[in] srcLength	Binary data size[Byte]
 * @param[out] string	Pointer of converted Base64 string (buffering is executed within this function)
 * @param[in] chunk		true: Insert a line feed code (="\r\n") every 76 characters, false: Don't insert a line feed code
 * @return				Pointer of converted Base64 string or NULL (in case of error)
 */
M2MString *M2MBase64_encode (const unsigned char *src, const size_t srcLength, M2MString **string, const bool chunk)
	{
	//========== Variable ==========
	size_t stringLength = 0;
	size_t i = 0;
	size_t index = 0;
	size_t remainder = 0;
	const M2MString *ENCODING_TABLE = (M2MString *)"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	const size_t CRLF_LENGTH = M2MString_length(M2MString_CRLF);
	const size_t CHUNK_TIME = (size_t)(M2MBase64_CHUNK_LENGTH / M2MBase64_FOUR_WORD);
	const size_t REPEAT_NUMBER = srcLength / (size_t)M2MBase64_THREE_OCTET;

	//===== Check argument =====
	if (src!=NULL && srcLength>0 && string!=NULL)
		{
		//===== In the case of padding is unnecessary =====
		if ((remainder=srcLength%(size_t)M2MBase64_THREE_OCTET)==0)
			{
			stringLength = REPEAT_NUMBER * (size_t)M2MBase64_FOUR_WORD;
			}
		//===== In the case of padding is necessary =====
		else
			{
			stringLength = (REPEAT_NUMBER + 1) * (size_t)M2MBase64_FOUR_WORD;
			}
		//===== In the case of inserting a line feed code =====
		if (chunk==true && M2MBase64_CHUNK_LENGTH<stringLength)
			{
			stringLength += (stringLength / (size_t)M2MBase64_CHUNK_LENGTH) * CRLF_LENGTH;
			}
		//===== In the case of not inserting a line feed code =====
		else
			{
			// do nothing
			}
		//===== Get heap memory =====
		if (((*string)=(M2MString *)M2MHeap_malloc(stringLength+1))!=NULL)
			{
			index = 0;
			//===== Repeat Base64 conversion =====
			for (i=0; i<REPEAT_NUMBER; i++)
				{
				memcpy(&((*string)[index  ]), &ENCODING_TABLE[(src[i*M2MBase64_THREE_OCTET  ]      >>2)], 1);
				memcpy(&((*string)[index+1]), &ENCODING_TABLE[(src[i*M2MBase64_THREE_OCTET  ]&0x03)<<4|src[i*M2MBase64_THREE_OCTET+1]>>4], 1);
				memcpy(&((*string)[index+2]), &ENCODING_TABLE[(src[i*M2MBase64_THREE_OCTET+1]&0x0f)<<2|src[i*M2MBase64_THREE_OCTET+2]>>6], 1);
				memcpy(&((*string)[index+3]), &ENCODING_TABLE[(src[i*M2MBase64_THREE_OCTET+2]&0x3f)], 1);
				index += (size_t)M2MBase64_FOUR_WORD;
				//===== In the case of inserting a line feed code =====
				if (chunk==true && (i+1)%CHUNK_TIME==0)
					{
					memcpy(&((*string)[index]), M2MString_CRLF, CRLF_LENGTH);
					index += (size_t)CRLF_LENGTH;
					}
				//===== In the case of not inserting a line feed code =====
				else
					{
					}
				}
			//===== Separated by data size =====
			switch (remainder)
				{
				//===== In the case of divisible by 3[Byte] =====
				case 0:
					break;
				//===== In the case of surplus is 1[Byte] =====
				case 1:
					memcpy(&((*string)[index  ]), &ENCODING_TABLE[(src[REPEAT_NUMBER*M2MBase64_THREE_OCTET]      >>2)], 1);
					memcpy(&((*string)[index+1]), &ENCODING_TABLE[(src[REPEAT_NUMBER*M2MBase64_THREE_OCTET]&0x03)<<4|0x00], 1);
					index += 2;
					break;
				//===== In the case of surplus is 2[Byte] =====
				case 2:
					memcpy(&((*string)[index  ]), &ENCODING_TABLE[(src[REPEAT_NUMBER*M2MBase64_THREE_OCTET  ]      >>2)], 1);
					memcpy(&((*string)[index+1]), &ENCODING_TABLE[(src[REPEAT_NUMBER*M2MBase64_THREE_OCTET  ]&0x03)<<4|src[REPEAT_NUMBER*3+1]>>4], 1);
					memcpy(&((*string)[index+2]), &ENCODING_TABLE[(src[REPEAT_NUMBER*M2MBase64_THREE_OCTET+1]&0x0f)<<2|0x00], 1);
					index += 3;
					break;
				}
			//===== Adding padding character ("=") =====
			for (i=0; i<=(stringLength-index); i++)
				{
				memcpy(&((*string)[index]), M2MString_EQUAL, 1);
				index++;
				}
			//===== Return encoded Base64 string =====
			return (*string);
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


/* End Of File */
