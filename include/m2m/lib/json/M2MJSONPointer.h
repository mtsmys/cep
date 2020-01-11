/*******************************************************************************
 * M2MJSONPointer.h : Implementation of JSON Pointer(RFC6901) in C
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

#ifndef M2MJSONPOINTER_H_
#define M2MJSONPOINTER_H_



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



#include "m2m/lib/json/M2MJSON.h"
#include "m2m/lib/lang/M2MString.h"
#include "m2m/lib/log/M2MLogger.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>


/*******************************************************************************
 * Class variable
 ******************************************************************************/
/**
 */
#ifndef M2MJSONPointer_SLASH
#define M2MJSONPointer_SLASH (unsigned char *)"~1"
#endif /* M2MJSONPointer_SLASH */


/**
 */
#ifndef M2MJSONPointer_TILDE
#define M2MJSONPointer_TILDE (unsigned char *)"~0"
#endif /* M2MJSONPointer_TILDE */


/**
 */
#ifndef M2MJSONPointer_WHOLE_SELECTION
#define M2MJSONPointer_WHOLE_SELECTION (unsigned char *)"\"\""
#endif /* M2MJSONPointer_WHOLE_SELECTION */


/*******************************************************************************
 * Public method
 ******************************************************************************/
/**
 * This method decodes pathname string.<br>
 * Kinds of decoding are follow.<br>
 * - "~1" -> "/"<br>
 * - "~0" -> "~"<br>
 * - ""/../.."" -> "/../.."<br>
 *
 * @param[in] path			pathname string for indicating target object
 * @param[out] buffer		buffer for copying decoded pathname string
 * @param[in] bufferLength	length of buffer[Byte]
 * @return					pointer of decoded pathname string or NULL(means error)
 */
M2MString *M2MJSONPointer_decodePath (const M2MString *path, M2MString *buffer, const size_t bufferLength);


/**
 * This method evaluates JSON object with indicated path string. <br>
 * This expression is comma-delimited form in the current version, <br>
 * for example "/hoge/one/fact".<br>
 *
 * @param[in] json		JSON structure object
 * @param[in] path		pathname string for indicating target object
 * @return				JSON object correspond with path or NULL(means error)
 */
M2MJSON *M2MJSONPointer_evaluate (M2MJSON *json, const M2MString *path);


/**
 * This method checks the string is number type or not.<br>
 *
 * @param string	check target string
 * @return			translated number or -1(means error)
 *
int JSONPointer_isNumber (const String *string);
*/


/**
 * This method validates expression of JSON Pointer path.<br>
 *
 * @param[in] path	JSON Pointer expression path string
 * @return			true : valid expression path, false : invalid expression path
 */
bool M2MJSONPointer_validatePathExpression (const M2MString *path);



#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2MJSONPOINTER_H_ */
