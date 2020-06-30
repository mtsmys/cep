/*******************************************************************************
 * M2MJSON.h : JavaScript Object Notation manipulator
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

#ifndef M2MJSON_H_
#define M2MJSON_H_



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



#include "m2m/M2MSystem.h"
#include "m2m/lib/time/M2MDate.h"
#include "m2m/lib/io/M2MHeap.h"
#include "m2m/lib/log/M2MLogLevel.h"
#include "m2m/lib/json/M2MJSONType.h"
#include "m2m/lib/lang/M2MString.h"
#include "m2m/lib/security/M2MCRC32.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>


/*******************************************************************************
 * Class variable
 ******************************************************************************/
/**
 * JSON Object structure
 */
#ifndef M2MJSONObject
typedef struct M2MJSONObject M2MJSONObject;
#endif /* M2MJSONObject */


/**
 * JSON Array structure
 */
#ifndef M2MJSONArray
typedef struct M2MJSONArray M2MJSONArray;
#endif /* M2MJSONArray */


/**
 * JSON type structure.<br>
 * JSON type includes 5 data types which are Array, Boolean, Number, Object,<br>
 * and String.<br>
 *
 * @param array		JSON Array object
 * @param boolean	JSON Boolean object
 * @param number	JSON Number object
 * @param object	JSON Object object
 * @param string	JSON String object
 */
#ifndef M2MJSONValue
typedef struct
	{
	M2MJSONArray *array;
	bool boolean;
	double number;
	M2MJSONObject *object;
	M2MString *string;
	} M2MJSONValue;
#endif /* M2MJSONValue */


/**
 * JavaScript Object Notation structure object
 *
 * @param type	JSON type
 * @param value	JSON value object
 */
#ifndef M2MJSON
typedef struct
	{
	M2MJSONType type;
	M2MJSONValue *value;
	} M2MJSON;
#endif /* M2MJSON */


/**
 * Constant length of hash table array.
 */
#ifndef M2MJSONObjectTable_LENGTH
#define M2MJSONObjectTable_LENGTH (uint32_t)512
#endif /* M2MJSONObjectTable_LENGTH */


/**
 * Hash table object for immediate access to JSON Object
 *
 * @param table		hash table
 * @param counter	number of registered JSONObject node on hash table
 */
#ifndef M2MJSONObjectTable
typedef struct
	{
	M2MJSONObject *table[M2MJSONObjectTable_LENGTH];
	uint32_t counter;
	} M2MJSONObjectTable;
#endif /* M2MJSONObjectTable */


/**
 * JSON Object structure.<br>
 * This object has key and value. The key is unique on JSON Object connection.<br>
 *
 * @param previous			previous JSON Object in array
 * @param next				next JSON Object in array
 * @param hash				hash digest for index
 * @param key				key of JSON Object
 * @param json				value of JSON Object
 * @param table				hash table for immediate access
 * @param sameIndexPrevios	a previous node which corresponds with hash table index
 * @param sameIndexNext		a next node which corresponds with hash table index
 */
#ifndef M2MJSONObject
struct M2MJSONObject
	{
	M2MJSONObject *previous;
	M2MJSONObject *next;
	uint32_t hash;
	M2MString *key;
	M2MJSON *json;
	M2MJSONObjectTable *table;
	M2MJSONObject *sameIndexPrevios;
	M2MJSONObject *sameIndexNext;
	};
#endif /* M2MJSONObject */


/**
 * JSON Array structure.<br>
 *
 * @param previous	previous JSON Array in array
 * @param next		next JSON Array in array
 * @param json		value of JSON Object
 */
#ifndef M2MJSONArray
struct M2MJSONArray
	{
	M2MJSONArray *previous;
	M2MJSONArray *next;
	M2MJSON *json;
	};
#endif /* M2MJSONArray */



/*******************************************************************************
 * Public method
 ******************************************************************************/
/**
 * This method calculates CRC-32 hash by indicated key string.<br>
 *
 * @param[in] key		source data for calculating CRC-32 hash
 * @param[in] keyLength	key data length
 * @return				CRC-32 hash digest
 */
uint32_t M2MJSON_calculateHash (const M2MString *key, const size_t keyLength);


/**
 * Release allocated memory for JSON Array.<br>
 * All element of indicated Array is removed.<br>
 *
 * @param[in,out] self	JSON structure object(including Array as JSON value)
 * @return				Initialized JSON object
 */
M2MJSON *M2MJSON_clearArray (M2MJSON *self);


/**
 * Release allocated memory for JSON Object.
 *
 * @param[in,out] self	JSON structure object(including Object as JSON value)
 * @return				Initialized JSON object
 */
M2MJSON *M2MJSON_clearObject (M2MJSON *self);


/**
 * @param[in,out] self	JSON structure object(including String as JSON value)
 * @return				Initialized JSON object
 */
M2MJSON *M2MJSON_clearString (M2MJSON *self);


/**
 * This method allocates new memory for creating JSONArray object.<br>
 *
 * @return	created new JSON Array object or NULL(means error)
 */
M2MJSONArray *M2MJSON_createNewArray ();


/**
 * This method allocates new memory for creating JSONObject object.<br>
 *
 * @return	created new JSON Object object or NULL(means error)
 */
M2MJSONObject *M2MJSON_createNewObject ();


/**
 * This method deletes JSON Array node. So, caller can't access it later.<br>
 *
 * @param[in,out] array		delete target JSON Array object
 * @return					lead node of JSON Array connection
 */
M2MJSONArray *M2MJSON_deleteArray (M2MJSONArray **array);


/**
 * This method deletes JSON Object node. So, caller can't access it later.<br>
 *
 * @param[in,out] object	delete target JSON Object object
 * @return					lead node of JSON Object connection
 */
M2MJSONObject *M2MJSON_deleteObject (M2MJSONObject **object);


/**
 * This method released allocated memory for JSON object.<br>
 *
 * @param[in,out] self	delete target JSON object
 */
void M2MJSON_delete (M2MJSON **self);


/**
 * @param[in] self
 * @return
 */
M2MJSONArray *M2MJSON_getArray (const M2MJSON *self);


/**
 * @param array
 * @return
 */
uint32_t M2MJSON_getArraySize (M2MJSONArray *array);


/**
 * @param self
 * @return
 */
bool M2MJSON_getBoolean (const M2MJSON *self);


/**
 * @param[in] object	JSON Object structure object
 * @return
 */
uint32_t M2MJSON_getHash (const M2MJSONObject *object);


/**
 * @param array
 * @param index
 */
M2MJSON *M2MJSON_getJSONFromArray (M2MJSONArray *array, const uint32_t index);


/**
 * @param[in] object	JSON Object structure object
 * @param[in] key		key data
 * @param[in] keyLength	length of key data[Byte]
 * @return				detected JSON structure object
 */
M2MJSON *M2MJSON_getJSONFromObject (M2MJSONObject *object, const M2MString *key, const size_t keyLength);


/**
 * @param object
 * @return
 */
M2MString *M2MJSON_getKey (const M2MJSONObject *object);


/**
 * @param array
 * @return
 */
M2MJSONArray *M2MJSON_getLastArray (M2MJSONArray *array);


/**
 * @param object
 * @return
 */
M2MJSONObject *M2MJSON_getLastObject (M2MJSONObject *object);


/**
 * @param array
 * @return
 */
M2MJSONArray *M2MJSON_getNextArray (M2MJSONArray *array);


/**
 * @param object
 * @return
 */
M2MJSONObject *M2MJSON_getNextObject (M2MJSONObject *object);


/**
 * @param self
 * @return
 */
double M2MJSON_getNumber (const M2MJSON *self);


/**
 * @param self
 * @return
 */
M2MJSONObject *M2MJSON_getObject (const M2MJSON *self);


/**
 * @param[in] object
 * @return				Number of JSON Objects
 */
uint32_t M2MJSON_getObjectSize (M2MJSONObject *object);


/**
 * @param array
 * @return
 */
M2MJSONArray *M2MJSON_getPreviousArray (M2MJSONArray *array);


/**
 * @param object
 * @return
 */
M2MJSONObject *M2MJSON_getPreviousObject (M2MJSONObject *object);


/**
 * @param[in] array
 * @return
 */
M2MJSONArray *M2MJSON_getRootArray (M2MJSONArray *array);


/**
 * @param[in] object
 * @return
 */
M2MJSONObject *M2MJSON_getRootObject (M2MJSONObject *object);


/**
 * @param[in] self		JSON structure object
 * @return
 */
M2MString *M2MJSON_getString (const M2MJSON *self);


/**
 * @param[in] self		JSON structure object
 * @return
 */
M2MJSONType M2MJSON_getType (const M2MJSON *self);


/**
 * @param[in] self		JSON structure object
 * @return
 */
M2MJSONValue *M2MJSON_getValue (const M2MJSON *self);


/**
 * This method allocate new memory for construct JSON structure object.<br>
 * So caller must use "JSON_delete()" for release the memory.<br>
 *
 * @return	JSON structure object
 */
M2MJSON *M2MJSON_new ();


/**
 * This method searches JSON Object from hash table and same index chain.<br>
 *
 * @param[in] object	JSON Object
 * @param[in] key		key string
 * @param[in] keyLength	length of key string[Byte]
 * @return				detected JSON Object object or NULL
 */
M2MJSONObject *M2MJSON_searchObject (M2MJSONObject *object, const M2MString *key, const size_t keyLength);


/**
 * @param[in,out] array
 * @param[in] json
 * @return
 */
M2MJSONArray *M2MJSON_setJSONToArray (M2MJSONArray *array, M2MJSON *json);


/**
 * @param[in,out] object
 * @param[in] key
 * @param[in] keyLength
 * @param[in] json
 * @return
 */
M2MJSONObject *M2MJSON_setJSONToObject (M2MJSONObject *object, const M2MString *key, const size_t keyLength, M2MJSON *json);


/**
 * @param[in,out] array
 * @param[in] next
 * @return
 */
M2MJSONArray *M2MJSON_setNextArray (M2MJSONArray *array, M2MJSONArray *next);


/**
 * @param[in,out] object
 * @param[in] next
 * @return
 */
M2MJSONObject *M2MJSON_setNextObject (M2MJSONObject *object, M2MJSONObject *next);


/**
 * @param[in,out] array
 * @param[in] previous
 * @return
 */
M2MJSONArray *M2MJSON_setPreviousArray (M2MJSONArray *array, M2MJSONArray *previous);


/**
 * @param[in,out] object
 * @param[in] previous
 * @return
 */
M2MJSONObject *M2MJSON_setPreviousObject (M2MJSONObject *object, M2MJSONObject *previous);


/**
 * @param[in,out] self		JSON structure object
 * @param type				JSON data type(enum type)
 * @return					JSON structure object set data type or NULL(means error)
 */
M2MJSON *M2MJSON_setType (M2MJSON *self, const M2MJSONType type);


/**
 * This method translates from indicated JSON object to JSON format string.<br>
 *
 * @param[in] self		JSON structure object
 * @param[out] buffer	buffer for copying JSON format string(Caution! this variable is allocated in this method, so caller must release!)
 * @return				JSON format string or NULL(means error)
 */
M2MString *M2MJSON_toString (M2MJSON *self, M2MString **buffer);



#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2MJSON_H_ */
