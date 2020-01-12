/*******************************************************************************
 * M2MJSONParser.h
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

#ifndef M2MJSONPARSER_H_
#define M2MJSONPARSER_H_



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



#include "m2m/lib/io/M2MFile.h"
#include "m2m/lib/json/M2MJSON.h"
#include "m2m/lib/lang/M2MString.h"
#include "m2m/lib/log/M2MLogger.h"
#include "m2m/lib/log/M2MFileAppender.h"
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>


/*******************************************************************************
 * Public method
 ******************************************************************************/
/**
 * This method parses JSON string from indicated file, and create new JSON<br>
 * structure object.<br>
 *
 * @param[in] filePath	JSON file path( includes extension ".json").
 * @return				JSON structure object or NULL(means error)
 */
M2MJSON *M2MJSONParser_parseFile (const M2MString *filePath);


/**
 * This method parses JSON string and create new JSON structure object.<br>
 *
 * @param[in] string	UTF-8 character string in the form of JSON
 * @return 				JSON structure object or NULL(means error)
 */
M2MJSON *M2MJSONParser_parseString (const M2MString *string);


/**
 * This method check the indicated string is JSON format or not.<br>
 *
 * @param[in] json	JSON format string
 * @return			true : valid form, false : invalid form
 */
bool M2MJSONParser_validateJSONSyntax (const M2MString *json);



#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2MJSONPARSER_H_ */
