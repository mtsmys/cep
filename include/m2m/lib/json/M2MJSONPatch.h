/*******************************************************************************
 * M2MJSONPatch.h : Implementation of JSON Patch(RFC6902) in C
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

#ifndef M2MJSONPATCH_H_
#define M2MJSONPATCH_H_



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



#include "m2m/lib/json/M2MJSON.h"
#include "m2m/lib/json/M2MJSONParser.h"
#include "m2m/lib/json/M2MJSONPointer.h"
#include "m2m/lib/lang/M2MString.h"
#include "m2m/lib/log/M2MLogger.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>



/*******************************************************************************
 * Class variable
 ******************************************************************************/
/**
 * JSON Patch operation name(="op")
 */
#ifndef M2MJSONPatch_OP
#define M2MJSONPatch_OP (unsigned char *)"\"/op\""
#endif /* M2MJSONPatch_OP */


/**
 * JSON Patch pathname(="path")
 */
#ifndef M2MJSONPatch_PATH
#define M2MJSONPatch_PATH (unsigned char *)"\"/path\""
#endif /* M2MJSONPatch_PATH */


/**
 * JSON Patch value data name(="value")
 */
#ifndef M2MJSONPatch_VALUE
#define M2MJSONPatch_VALUE (unsigned char *)"\"/value\""
#endif /* M2MJSONPatch_VALUE */


/**
 * JSON Patch from data name(="from")
 */
#ifndef M2MJSONPatch_FROM
#define M2MJSONPatch_FROM (unsigned char *)"\"/from\""
#endif /* M2MJSONPatch_FROM */


/**
 * JSON Patch add operation name(="add")
 */
#ifndef M2MJSONPatch_ADD
#define M2MJSONPatch_ADD (unsigned char *)"add"
#endif /* M2MJSONPatch_ADD */


/**
 * JSON Patch remove operation name(="remove")
 */
#ifndef M2MJSONPatch_REMOVE
#define M2MJSONPatch_REMOVE (unsigned char *)"remove"
#endif /* M2MJSONPatch_REMOVE */


/**
 * JSON Patch replace operation name(="replace")
 */
#ifndef M2MJSONPatch_REPLACE
#define M2MJSONPatch_REPLACE (unsigned char *)"replace"
#endif /* M2MJSONPatch_REPLACE */


/**
 * JSON Patch move operation name(="move")
 */
#ifndef M2MJSONPatch_MOVE
#define M2MJSONPatch_MOVE (unsigned char *)"move"
#endif /* M2MJSONPatch_MOVE */


/**
 * JSON Patch copy operation name(="copy")
 */
#ifndef M2MJSONPatch_COPY
#define M2MJSONPatch_COPY (unsigned char *)"copy"
#endif /* M2MJSONPatch_COPY */


/**
 * JSON Patch test operation name(="test")
 */
#ifndef M2MJSONPatch_TEST
#define M2MJSONPatch_TEST (unsigned char *)"test"
#endif /* M2MJSONPatch_TEST */



/*******************************************************************************
 * Public method
 ******************************************************************************/
/**
 * This method performs depending upon what the target location references.<br>
 * <br>
 * ・If the target location specifies an array index, a new value is <br>
 *   inserted into the array at the specified index.<br>
 * ・If the target location specifies an object member that does not <br>
 *   already exist, a new member is added to the object.<br>
 * ・If the target location specifies an object member that does exist, <br>
 *   that member’s value is replaced.<br>
 *
 * @param[in,out] json	JSON object
 * @param[in] path		JSONPointer location pathname of adding value
 * @param[in] value		value data object(insertion is shallow)
 * @return				JSON object added value at indicated path or NULL(means error)
 */
M2MJSON *M2MJSONPatch_add (M2MJSON *json, const M2MString *path, const M2MJSON *value);


/**
 * This method copies the value to the indicated target location.<br>
 *
 * @param[in,out] json	JSON object
 * @param[in] from		JSONPointer location pathname of copying value
 * @param[in] path		JSONPointer location pathname of copying value
 * @return
 */
M2MJSON *M2MJSONPatch_copy (M2MJSON *json, const M2MString *from, const M2MString *path);


/**
 * This method parses indicated operation and execute patch command.<br>
 *
 * @param json		target JSON object
 * @param operation	patch operation command string
 * @return			patched JSON object
 */
M2MJSON *M2MJSONPatch_evaluate (M2MJSON *json, const M2MString *operation);


/**
 * This method moves the value to the target location.<br>
 *
 * @param[in,out] json	JSON object
 * @param[in] path		JSONPointer location pathname for moving value
 * @param[in] value
 */
M2MJSON *M2MJSONPatch_move (M2MJSON *json, const M2MString *from, const M2MString *path);


/**
 * This method removes a part of JSON object indicated with expression.<br>
 *
 * @param[in,out] json	JSON object
 * @param[in] path		JSONPointer location pathname for removing value
 */
void M2MJSONPatch_remove (M2MJSON *json, const M2MString *path);


/**
 * This method replaces the value at the target location with a new value.<br>
 *
 * @param[in,out] json	JSON object
 * @param[in] path		JSONPointer location pathname of replacing value
 * @param[in] value		value data object
 */
M2MJSON *M2MJSONPatch_replace (M2MJSON *json, const M2MString *path, const M2MJSON *value);


/**
 * This method tests that a value at the target location is equal to a specified value.<br>
 *
 * @param[in,out] json	JSON object
 * @param[in] path		JSONPointer location pathname of replacing value
 * @param[in] value		value data object
 */
bool M2MJSONPatch_test (M2MJSON *json, const M2MString *path, const M2MJSON *value);



#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2MJSONPATCH_H_ */
