/*******************************************************************************
 * M2MJSONType.h
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

#pragma once

#ifndef M2MJSONTYPE_H_
#define M2MJSONTYPE_H_



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



#include <stdio.h>


/*******************************************************************************
 * Class variable
 ******************************************************************************/
/**
 * JSON type enumeration.<br>
 * A number of JSON type is 6, they are<br>
 * <br>
 * @param M2MJSONType_ARRAY
 * @param M2MJSONType_BOOLEAN
 * @param M2MJSONType_NULL
 * @param M2MJSONType_NUMBER
 * @param M2MJSONType_OBJECT
 * @param M2MJSONType_STRING
 */
#ifndef M2MJSONType
typedef enum
	{
	M2MJSONType_ARRAY,
	M2MJSONType_BOOLEAN,
	M2MJSONType_NULL,
	M2MJSONType_NUMBER,
	M2MJSONType_OBJECT,
	M2MJSONType_STRING
	} M2MJSONType;
#endif /* M2MJSONType */


/*******************************************************************************
 * Public method
 ******************************************************************************/
/**
 * This method returns definition statement matches with the JSON type.<br>
 *
 * @param self	JSON type
 * @return		JSON type name string
 */
unsigned char *M2MJSONType_toString (const M2MJSONType self);



#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2MJSONTYPE_H_ */
