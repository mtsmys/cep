/*******************************************************************************
 * JSON.c : JSON(JavaScript Object Notation) container including accessor.
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

#include "m2m/lib/json/M2MJSON.h"


/*******************************************************************************
 * Declaration
 ******************************************************************************/
/**
 * This method released key data owned by JAON Object.<br>
 *
 * @param[in,out] object	JSON Object structure
 */
static void this_deleteObjectKey (M2MJSONObject *object);


/**
 * This method released allocated memory for JSONObjectTable object.<br>
 *
 * @param[in,out] object	JSON Object object
 */
static void this_deleteObjectTable (M2MJSONObject *object);


/**
 * This method returns number of index of indicated hash table.<br>
 *
 * @param[in] table	hash table for immediate JSON Object access
 * @return			number of index of hash table or 0(almost means error)
 */
static uint32_t this_getObjectTableCounter (const M2MJSONObjectTable *table);


/**
 * This method returns index number of hash table.<br>
 *
 * @param[in] hash	hash digest number
 * @return			index number of hash table
 */
static uint32_t this_getObjectTableIndex (const uint32_t hash);


/**
 * This method returns length of hash table.<br>
 *
 * @return	length of hash table[Byte]
 */
static uint32_t this_getObjectTableLength ();


/**
 * This method returns hash table for immediate access to JSON Object.<br>
 *
 * @param[in] object	JSON Object structure object
 * @return				hash table for JSON Object
 */
static M2MJSONObjectTable *this_getObjectTable (const M2MJSONObject *object);


/**
 * This method returns next JSON Object node which has same index.<br>
 *
 * @param[in] object	JSON Object object
 * @return				next JSON Object node of same index or NULL(means error)
 */
static M2MJSONObject *this_getSameIndexNext (const M2MJSONObject *object);


/**
 * This method returns previous JSON Object node which has same index.<br>
 *
 * @param[in] object	JSON Object object
 * @return				previous JSON Object node of same index or NULL(means error)
 */
static M2MJSONObject *this_getSameIndexPrevious (const M2MJSONObject *object);


/**
 * Display the debug level log message in standard out.
 *
 * @param[in] functionName		String indicating function name
 * @param[in] lineNumber		Line number in source file (can be embedded with "__LINE__")
 * @param[in] message			Message string
 */
static void this_printDebugMessage (const M2MString *functionName, const uint32_t lineNumber, const M2MString *message);


/**
 * Display the error level log message in standard out.
 *
 * @param[in] functionName		String indicating function name
 * @param[in] lineNumber		Line number in source file (can be embedded with "__LINE__")
 * @param[in] message			Message string
 */
static void this_printErrorMessage (const M2MString *functionName, const uint32_t lineNumber, const M2MString *message);


/**
 * This method searches JSON Object from hash table(not search same index chain).<br>
 *
 * @param[in] object	JSON Object
 * @param[in] key		key string
 * @param[in] keyLength	length of key string[Byte]
 * @return				JSON Object presented in the specified index or NULL(means error)
 */
static M2MJSONObject *this_searchObjectTable (M2MJSONObject *object, const M2MString *key, const size_t keyLength);


/**
 * This method sets next JSON Object which has same index into JSON Object.<br>
 *
 * @param[in,out] object	JSON Object object
 * @param[in] next			next JSON Object node of same index
 * @return
 */
static M2MJSONObject *this_setSameIndexNext (M2MJSONObject *object, M2MJSONObject *next);


/**
 * This method sets previous JSON Object which has same index into JSON Object.<br>
 *
 * @param[in,out] object	JSON Object object
 * @param[in] next			previous JSON Object node of same index
 * @return
 */
static M2MJSONObject *this_setSameIndexPrevious (M2MJSONObject *object, M2MJSONObject *previous);


/**
 * This method translate from JSON Array to JSON form string.<br>
 *
 * @param[in] array		JSON Array object
 * @param[out] buffer	buffer for copying translated JSON form string
 * @return				translated JSON form string or NULL(means error)
 */
static M2MString *this_toM2MStringFromArray (M2MJSONArray *array, M2MString **buffer);


/**
 * This method translate from JSON Boolean to JSON form string.<br>
 *
 * @param[in] self		JSON object
 * @param[out] buffer	buffer for copying translated JSON form string
 * @return				translated JSON form string or NULL(means error)
 */
static M2MString *this_toM2MStringFromBoolean (M2MJSON *self, M2MString **buffer);


/**
 * This method translate from JSON Number to JSON form string.<br>
 *
 * @param[in] self		JSON object
 * @param[out] buffer	buffer for copying translated JSON form string
 * @return				translated JSON form string or NULL(means error)
 */
static M2MString *this_toM2MStringFromNumber (M2MJSON *self, M2MString **buffer);


/**
 * This method translate from JSON Object to JSON form string.<br>
 *
 * @param[in] object	JSON object
 * @param[out] buffer	buffer for copying translated JSON form string
 * @return				translated JSON form string or NULL(means error)
 */
static M2MString *this_toM2MStringFromObject (M2MJSONObject *object, M2MString **buffer);


/**
 * This method translate from JSON Array to JSON form string.<br>
 *
 * @param[in] self		JSON object
 * @param[out] buffer	buffer for copying translated JSON form string
 * @return				translated JSON form string or NULL(means error)
 */
static M2MString *this_toStringFromString (M2MJSON *self, M2MString **buffer);



/*******************************************************************************
 * Private method
 ******************************************************************************/
/**
 * This method allocates new memory for creating JSONObjectTable object.<br>
 *
 * @return	created new JSONObjectTable object
 */
static M2MJSONObjectTable *this_createNewJSONObjectTable ()
	{
	//========== Variable ==========
	M2MJSONObjectTable *table = NULL;
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON.this_createNewJSONObjectTable()";

	//===== Allocate new memory for creating JSONObjectTable =====
	if ((table=(M2MJSONObjectTable *)M2MHeap_malloc(sizeof(M2MJSONObjectTable)))!=NULL)
		{
		this_printDebugMessage(METHOD_NAME, __LINE__, (M2MString *)"Created new \"JSONObjectTable\" object as a hash table");
		return table;
		}
	//===== Error handling =====
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to allocate new memory for creating \"JSONObjectTable\" object");
		return NULL;
		}
	}


/**
 * This method decreases the counter of hash table.<br>
 *
 * @param[in,out] table	hash table for immediate JSON Object access
 */
static void this_decreaseObjectTableCounter (M2MJSONObjectTable *table)
	{
	//========== Variable ==========
	M2MString MESSAGE[64];
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON.this_decreaseObjectTableCounter()";

	//===== Check argument =====
	if (table!=NULL)
		{
		table->counter = table->counter - 1;
		memset(MESSAGE, 0, sizeof(MESSAGE));
		M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Decreased counter number into %u", table->counter);
		this_printDebugMessage(METHOD_NAME, __LINE__, MESSAGE);
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSONObjectTable\" is NULL");
		}
	return;
	}


/**
 * @param[in,out] array
 */
static void this_deleteArray (M2MJSONArray **array)
	{
	//========== Variable ==========
	M2MJSON *json = NULL;
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON.this_deleteArray()";

	//===== Check argument =====
	if (array!=NULL && (*array)!=NULL)
		{
		//===== Release allocated memory for JSON Array =====
		if ((json=(*array)->json)!=NULL)
			{
			M2MJSON_delete(&json);
			}
		//===== Error handling =====
		else
			{
			}
		//===== Delete JSOn Array =====
		M2MHeap_free((*array));
		this_printDebugMessage(METHOD_NAME, __LINE__, (M2MString *)"Released allocated memory for \"M2MJSONArray\" object");
		}
	//===== Argument error =====
	else
		{
		}
	return;
	}


/**
 * This method release allocated memory for a JSON Object.<br>
 *
 * @param[in,out] object	release target JSON Object
 */
static void this_deleteObject (M2MJSONObject **object)
	{
	//========== Variable ==========
	uint32_t index = 0;
	M2MJSONObjectTable *table = NULL;
	M2MJSONObject *node = NULL;
	M2MJSONObject *sameIndexPrevious = NULL;
	M2MJSONObject *sameIndexNext = NULL;
	M2MJSON *json = NULL;
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON.this_deleteObject()";

	//===== Check argument =====
	if (object!=NULL && (*object)!=NULL)
		{
		//===== Check hash table =====
		if ((table=this_getObjectTable((*object)))!=NULL)
			{
			//=====  =====
			index = this_getObjectTableIndex(M2MJSON_getHash((*object)));
			//===== Get index node =====
			if ((node=table->table[index])!=NULL)
				{
				//===== In the case of existing only one node =====
				if (node==(*object))
					{
					table->table[index] = NULL;
					this_decreaseObjectTableCounter(table);
					}
				//===== In the case of existing same index node =====
				else
					{
					//===== Get previous node of same index =====
					if ((sameIndexPrevious=this_getSameIndexPrevious((*object)))!=NULL)
						{
						//===== Get next node of same index =====
						if ((sameIndexNext=this_getSameIndexNext((*object)))!=NULL)
							{
							this_setSameIndexNext(sameIndexPrevious, sameIndexNext);
							this_setSameIndexPrevious(sameIndexNext, sameIndexPrevious);
							this_printDebugMessage(METHOD_NAME, __LINE__, (M2MString *)"Removed indicated \"JSONObject\" from closed addressing table");
							}
						//=====  =====
						else
							{
							this_setSameIndexNext(sameIndexPrevious, NULL);
							}
						}
					//===== Error handling =====
					else
						{
						this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Internal error! Previous node of same index is NULL");
						}
					}
				}
			//===== Error handling =====
			else
				{
				this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Internal error! Index position of hash table is NULL");
				}
			}
		//===== Error handling =====
		else
			{
			this_printDebugMessage(METHOD_NAME, __LINE__, (M2MString *)"There is no hash table for immediate JSON Object access");
			}
		//===== Delete hash table =====
		this_deleteObjectTable((*object));
		//===== Delete JSON Object key string =====
		this_deleteObjectKey((*object));
		//===== Get JSON which is JSON Object value =====
		if ((json=(*object)->json)!=NULL)
			{
			M2MJSON_delete(&json);
			}
		//===== Error handling =====
		else
			{
			}
		//===== Delete JSOn Object =====
		M2MHeap_free((*object));
		this_printDebugMessage(METHOD_NAME, __LINE__, (M2MString *)"Released allocated memory for \"JSONObject\" object");
		}
	//===== Argument error =====
	else
		{
		}
	return;
	}


/**
 * This method released key data owned by JAON Object.<br>
 *
 * @param[in,out] object	JSON Object structure
 */
static void this_deleteObjectKey (M2MJSONObject *object)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON.this_deleteObjectKey()";

	//===== Check argument =====
	if (object!=NULL)
		{
		//===== Check existing of key =====
		if (M2MJSON_getKey(object)!=NULL)
			{
			M2MHeap_free(object->key);
			this_printDebugMessage(METHOD_NAME, __LINE__, (M2MString *)"Released allocated memory for \"JSONObject\" key string");
			}
		//===== In the case of not existing key =====
		else
			{
			// do nothing
			}
		}
	//===== Argument error =====
	else
		{
		}
	return;
	}


/**
 * This method released allocated memory for JSONObjectTable object.<br>
 *
 * @param[in,out] object	JSON Object object
 */
static void this_deleteObjectTable (M2MJSONObject *object)
	{
	//========== Variable ==========
	M2MJSONObjectTable *table = NULL;
	uint32_t counter = 0;
	M2MString MESSAGE[128];
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON.this_deleteObjectTable()";

	//===== Check argument =====
	if (object!=NULL)
		{
		//===== Get hash table object =====
		if ((table=this_getObjectTable(object))!=NULL)
			{
			//===== Check number of entry object =====
			if ((counter=this_getObjectTableCounter(table))==0)
				{
				M2MHeap_free(table);
				this_printDebugMessage(METHOD_NAME, __LINE__, (M2MString *)"Released allocated memory for \"JSONObjectTable\" object");
				}
			//===== In the case of not releasing hash table =====
			else
				{
				memset(MESSAGE, 0, sizeof(MESSAGE));
				M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Not releases allocated memory because there are %u \"JSONObject\" nodes", counter);
				this_printDebugMessage(METHOD_NAME, __LINE__, MESSAGE);
				}
			}
		//===== Error handling =====
		else
			{
			this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSONObject\" doesn't have a \"JSONObjectTable\" object as member variable");
			}
		}
	//===== Argument error =====
	else
		{
		}
	return;
	}


/**
 * This method returns number of index of indicated hash table.<br>
 *
 * @param[in] table	hash table for immediate JSON Object access
 * @return			number of index of hash table or 0(almost means error)
 */
static uint32_t this_getObjectTableCounter (const M2MJSONObjectTable *table)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON.this_getObjectTableCounter()";

	//===== Check argument =====
	if (table!=NULL)
		{
		return table->counter;
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSONObjectTable\" is NULL");
		return 0;
		}
	}


/**
 * This method returns index number of hash table.<br>
 *
 * @param[in] hash	hash digest number
 * @return			index number of hash table
 */
static uint32_t this_getObjectTableIndex (const uint32_t hash)
	{
	//========== Variable ==========
	uint32_t index = 0;
	M2MString MESSAGE[64];
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON.this_getObjectTableIndex()";

	//===== Get index of hash table =====
	index = ((hash & 0xFFFFFFFF) % this_getObjectTableLength());
	memset(MESSAGE, 0, sizeof(MESSAGE));
	M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Get index(=\"%u\") for hash table", index);
	this_printDebugMessage(METHOD_NAME, __LINE__, MESSAGE);
	return index;
	}


/**
 * This method returns length of hash table.<br>
 *
 * @return	length of hash table[Byte]
 */
static uint32_t this_getObjectTableLength ()
	{
	return M2MJSONObjectTable_LENGTH;
	}


/**
 * This method returns hash table for immediate access to JSON Object.<br>
 *
 * @param[in] object	JSON Object structure object
 * @return				hash table for JSON Object
 */
static M2MJSONObjectTable *this_getObjectTable (const M2MJSONObject *object)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON.this_getObjectTable()";

	//===== Check argument =====
	if (object!=NULL)
		{
		return object->table;
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSONObject\" is NULL");
		return NULL;
		}
	}


/**
 * This method returns the last JSON Object node which has same index.<br>
 *
 * @param[in] object	JSON Object object
 * @return				the last JSON Object node which has same index or NULL(means error)
 */
static M2MJSONObject *this_getSameIndexEnd (M2MJSONObject *object)
	{
	//========== Variable ==========
	M2MJSONObject *sameIndexNext = NULL;
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON.this_getSameIndexEnd()";

	//===== Check argument =====
	if (object!=NULL)
		{
		//===== Loop while  =====
		while ((sameIndexNext=this_getSameIndexNext(object))!=NULL)
			{
			object = sameIndexNext;
			}
		return object;
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSONObject\" is NULL");
		return NULL;
		}
	}


/**
 * This method returns next JSON Object node which has same index.<br>
 *
 * @param[in] object	JSON Object object
 * @return				next JSON Object node which has same index or NULL(means error)
 */
static M2MJSONObject *this_getSameIndexNext (const M2MJSONObject *object)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON.this_getSameIndexNext()";

	//===== Check argument =====
	if (object!=NULL)
		{
		return (object->sameIndexNext);
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSONObject\" is NULL");
		return NULL;
		}
	}


/**
 * This method returns previous JSON Object node which has same index.<br>
 *
 * @param[in] object	JSON Object object
 * @return				previous JSON Object node which has same index or NULL(means error)
 */
static M2MJSONObject *this_getSameIndexPrevious (const M2MJSONObject *object)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON.this_getSameIndexPrevious()";

	//===== Check argument =====
	if (object!=NULL)
		{
		return (object->sameIndexPrevios);
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSONObject\" is NULL");
		return NULL;
		}
	}


/**
 * This method increases the counter of hash table.<br>
 *
 * @param[in,out] table	hash table for immediate JSON Object access
 */
static void this_increaseObjectTableCounter (M2MJSONObjectTable *table)
	{
	//========== Variable ==========
	M2MString MESSAGE[64];
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON.this_increaseObjectTableCounter()";

	//===== Check argument =====
	if (table!=NULL)
		{
		table->counter = table->counter + 1;
		memset(MESSAGE, 0, sizeof(MESSAGE));
		M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Increased counter number(=\"%u\")", table->counter);
		this_printDebugMessage(METHOD_NAME, __LINE__, MESSAGE);
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSONObjectTable\" is NULL");
		}
	return;
	}


/**
 * This method checks indicated object(connection) is empty or not.<br>
 *
 * @param[in] object	JSON Object object
 * @return				true : indicated is empty, false : not empty
 */
static bool this_isEmpty (M2MJSONObject *object)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON.this_isEmpty()";

	//===== Check argument =====
	if (object!=NULL)
		{
		//===== Check whether it owns key =====
		if (M2MJSON_getKey(object)!=NULL)
			{
			return false;
			}
		//===== In the case of indicated node doesn't have key =====
		else
			{
			//===== Check ahead =====
			if ((object=M2MJSON_getPreviousObject(object))!=NULL
					&& M2MJSON_getKey(object)!=NULL)
				{
				return false;
				}
			//===== In the case of empty =====
			else
				{
				return true;
				}
			}
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSONObject\" is NULL");
		return true;
		}
	}


/**
 * Display the debug level log message in standard out.
 *
 * @param[in] functionName		String indicating function name
 * @param[in] lineNumber		Line number in source file (can be embedded with "__LINE__")
 * @param[in] message			Message string
 */
static void this_printDebugMessage (const M2MString *functionName, const uint32_t lineNumber, const M2MString *message)
	{
	//========== Variable ==========
	M2MString *logMessage = NULL;

	//===== Create new log message =====
	if (M2MLogger_createNewLogMessage(M2MLogLevel_DEBUG, functionName, lineNumber, message, &logMessage)!=NULL)
		{
		//===== Print out log =====
		M2MSystem_outPrintln(logMessage);
		//===== Release allocated memory =====
		M2MHeap_free(logMessage);
		}
	//===== Error handling =====
	else
		{
		}
	return;
	}


/**
 * Display the error level log message in standard out.
 *
 * @param[in] functionName		String indicating function name
 * @param[in] lineNumber		Line number in source file (can be embedded with "__LINE__")
 * @param[in] message			Message string
 */
static void this_printErrorMessage (const M2MString *functionName, const uint32_t lineNumber, const M2MString *message)
	{
	//========== Variable ==========
	M2MString *logMessage = NULL;

	//===== Create new log message =====
	if (M2MLogger_createNewLogMessage(M2MLogLevel_ERROR, functionName, lineNumber, message, &logMessage)!=NULL)
		{
		//===== Print out log =====
		M2MSystem_errPrintln(logMessage);
		//===== Release allocated memory =====
		M2MHeap_free(logMessage);
		}
	//===== Error handling =====
	else
		{
		}
	return;
	}


/**
 * This method searches JSON Object from hash table(not search same index chain).<br>
 *
 * @param[in] object	JSON Object
 * @param[in] key		key string
 * @param[in] keyLength	length of key string[Byte]
 * @return				JSON Object presented in the specified index or NULL(means error)
 */
static M2MJSONObject *this_searchObjectTable (M2MJSONObject *object, const M2MString *key, const size_t keyLength)
	{
	//========== Variable ==========
	M2MJSONObjectTable *table = NULL;
	M2MString MESSAGE[256];
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON.this_searchObjectTable()";

	//===== Check argument =====
	if (object!=NULL && key!=NULL && keyLength>0)
		{
		//===== Search hash table =====
		if ((table=this_getObjectTable(object))!=NULL
				&& (object=table->table[this_getObjectTableIndex(M2MJSON_calculateHash(key, keyLength))])!=NULL)
			{
			memset(MESSAGE, 0, sizeof(MESSAGE));
			M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Detected \"JSONObject\" which correspond with indicated key(= \"%s\" )", key);
			this_printDebugMessage(METHOD_NAME, __LINE__, MESSAGE);
			return object;
			}
		//===== Error handling =====
		else if (table==NULL)
			{
			this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSONObject\" hasn't hash table");
			return NULL;
			}
		else
			{
			memset(MESSAGE, 0, sizeof(MESSAGE));
			M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"There is no \"JSONObject\" which correspond with indicated key(= \"%s\" )", key);
			this_printDebugMessage(METHOD_NAME, __LINE__, MESSAGE);
			return NULL;
			}
		}
	//===== Argument error =====
	else if (object==NULL)
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSONObject\" is NULL");
		return NULL;
		}
	else if (key==NULL)
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"key\" is NULL");
		return NULL;
		}
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"keyLength\" isn't positive");
		return NULL;
		}
	}


/**
 * This method sets hash digest into indicated JSON Object object.<br>
 *
 * @param[in] object	JSON Object structure object
 * @param[in] key		key string
 * @param[in] keyLength	length of key string
 * @return				JSON Object set hash or NULL(means error)
 */
static M2MJSONObject *this_setHash (M2MJSONObject *object, const M2MString *key, const size_t keyLength)
	{
	//========== Variable ==========
	M2MString MESSAGE[256];
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON.this_setHash()";

	//===== Check argument =====
	if (object!=NULL && key!=NULL && keyLength>0)
		{
		//===== Set hash digest =====
		object->hash = M2MJSON_calculateHash(key, keyLength);
		memset(MESSAGE, 0, sizeof(MESSAGE));
		M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Set hash number(=\"%u\") into \"JSONObject\" which correspond with indicated key(= \"%s\" )", object->hash, key);
		this_printDebugMessage(METHOD_NAME, __LINE__, MESSAGE);
		return object;
		}
	//===== Argument error =====
	else if (object==NULL)
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSONObject\" is NULL");
		return NULL;
		}
	else if (key==NULL)
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"key\" is NULL");
		return NULL;
		}
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"keyLength\" isn't positive");
		return NULL;
		}
	}


/**
 * This method sets key string into indicated JSON Object object.<br>
 *
 * @param[in] object	JSON Object structure object
 * @param[in] key		key string
 * @param[in] keyLength	length of key string
 * @return				JSON Object set key string or NULL(means error)
 */
static M2MJSONObject *this_setKey (M2MJSONObject *object, const M2MString *key, const size_t keyLength)
	{
	//========== Variable ==========
	M2MString MESSAGE[256];
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON.this_setKey()";

	//===== Check argument =====
	if (object!=NULL && key!=NULL && 0<keyLength)
		{
		//===== Initialization =====
		this_deleteObjectKey(object);
		//===== Allocate new memory =====
		if ((object->key=(M2MString *)M2MHeap_malloc(keyLength+1))!=NULL)
			{
			//===== Set key =====
			memcpy(object->key, key, keyLength);
			//===== Set hash digest =====
			this_setHash(object, key, keyLength);
			memset(MESSAGE, 0, sizeof(MESSAGE));
			M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Set the key string(=\"%s\") in JSON Object", key);
			this_printDebugMessage(METHOD_NAME, __LINE__, MESSAGE);
			//===== Termination =====
			return object;
			}
		//===== Error handling =====
		else
			{
			this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to allocate new memory for copying argument \"key\" string");
			return NULL;
			}
		}
	//===== Argument error =====
	else if (object==NULL)
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSONObject\" is NULL");
		return NULL;
		}
	else if (key==NULL)
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"key\" is NULL");
		return NULL;
		}
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"keyLength\" isn't positive");
		return NULL;
		}
	}


/**
 * This method sets hash table into JSON Object object.<br>
 *
 * @param[in,out] object	JSON Object object
 * @param[in] table			hash table for immediate access
 * @return					JSON Object set hash table or NULL(means error)
 */
static M2MJSONObject *this_setObjectTable (M2MJSONObject *object, M2MJSONObjectTable *table)
	{
	//===== Check argument =====
	if (object!=NULL && table!=NULL)
		{
		object->table = table;
		return object;
		}
	//===== Argument error =====
	else if (object==NULL)
		{
		return NULL;
		}
	else
		{
		return NULL;
		}
	}


/**
 * This method sets next JSON Object which has same index into JSON Object.<br>
 *
 * @param[in,out] object	JSON Object object
 * @param[in] next			next JSON Object node of same index
 * @return					JSON Object set same index chain or NULL(means error)
 */
static M2MJSONObject *this_setSameIndexNext (M2MJSONObject *object, M2MJSONObject *next)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON.this_setSameIndexNext()";

	//===== Check argument =====
	if (object!=NULL)
		{
		object->sameIndexNext = next;
		return object;
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSONObject\" is NULL");
		return NULL;
		}
	}


/**
 * This method sets previous JSON Object which has same index into JSON Object.<br>
 *
 * @param[in,out] object	JSON Object object
 * @param[in] next			previous JSON Object node of same index
 * @return					JSON Object set same index chain or NULL(means error)
 */
static M2MJSONObject *this_setSameIndexPrevious (M2MJSONObject *object, M2MJSONObject *previous)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON.this_setSameIndexPrevious()";

	//===== Check argument =====
	if (object!=NULL && previous!=NULL)
		{
		object->sameIndexPrevios = previous;
		return object;
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSONObject\" is NULL");
		return NULL;
		}
	}


/**
 * This method translate from JSON Array to JSON form string.<br>
 *
 * @param[in] array		JSON Array object
 * @param[out] string	buffer for copying translated JSON form string
 * @return				translated JSON form string or NULL(means error)
 */
static unsigned char *this_toM2MStringFromArray (M2MJSONArray *array, M2MString **buffer)
	{
	//========== Variable ==========
	M2MJSON *json = NULL;
	M2MJSONType type;
	M2MString *jsonM2MString = NULL;
	M2MString *copy = NULL;
	size_t stringLength = 0;
	size_t jsonM2MStringLength = 0;
	const size_t LEFT_SQUARE_BRACKET_LENGTH = M2MString_length(M2MString_LEFT_SQUARE_BRACKET);
	const size_t COMMA_LENGTH = M2MString_length(M2MString_COMMA);
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON.this_toM2MStringFromArray()";

	//===== Check argument =====
	if (array!=NULL && buffer!=NULL)
		{
		//===== Check argument =====
		if ((array=M2MJSON_getRootArray(array))!=NULL)
			{
			//========== In the case of existing JSON Object is only one ==========
			if (M2MJSON_getArraySize(array)==1)
				{
				//===== Get JSON from Array node =====
				if ((json=array->json)!=NULL)
					{
					if ((type=M2MJSON_getType(json))==M2MJSONType_ARRAY)
						{
						this_toM2MStringFromArray(M2MJSON_getArray(json), &jsonM2MString);
						}
					else if (type==M2MJSONType_BOOLEAN)
						{
						this_toM2MStringFromBoolean(json, &jsonM2MString);
						}
					else if (type==M2MJSONType_NUMBER)
						{
						this_toM2MStringFromNumber(json, &jsonM2MString);
						}
					else if (type==M2MJSONType_OBJECT)
						{
						this_toM2MStringFromObject(M2MJSON_getObject(json), &jsonM2MString);
						}
					else if (type==M2MJSONType_STRING)
						{
						this_toStringFromString(json, &jsonM2MString);
						}
					else
						{
						return NULL;
						}
					//===== Check existence of JSON string =====
					if (jsonM2MString!=NULL)
						{
						//===== Get length of JSON string =====
						if ((jsonM2MStringLength=M2MString_length(jsonM2MString))>0)
							{
							//===== Allocate new memory for copying JSON string =====
							if (((*buffer)=(M2MString *)M2MHeap_malloc(LEFT_SQUARE_BRACKET_LENGTH+jsonM2MStringLength+1))!=NULL)
								{
								//===== Connect strings =====
								memcpy(&((*buffer)[0]), M2MString_LEFT_SQUARE_BRACKET, LEFT_SQUARE_BRACKET_LENGTH);
								memcpy(&((*buffer)[LEFT_SQUARE_BRACKET_LENGTH]), jsonM2MString, jsonM2MStringLength);
								//===== Release allocated memory =====
								M2MHeap_free(jsonM2MString);
								//===== Update string length =====
								stringLength = M2MString_length((*buffer));
								}
							//===== Error handling =====
							else
								{
								//===== Release allocated memory =====
								M2MHeap_free(jsonM2MString);
								return NULL;
								}
							}
						//===== Error handling =====
						else
							{
							//===== Release allocated memory =====
							M2MHeap_free(jsonM2MString);
							return NULL;
							}
						}
					//===== Error handling =====
					else
						{
						return NULL;
						}
					}
				//===== Error handling =====
				else
					{
					return NULL;
					}
				}
			//========== In the case of existing JSON Object is many ==========
			else
				{
				//===== Loop while not achieving to last node =====
				while (M2MJSON_getNextArray(array)!=NULL)
					{
					//===== Get JSON from Array node =====
					if ((json=array->json)!=NULL)
						{
						if ((type=M2MJSON_getType(json))==M2MJSONType_ARRAY)
							{
							this_toM2MStringFromArray(M2MJSON_getArray(json), &jsonM2MString);
							}
						else if (type==M2MJSONType_BOOLEAN)
							{
							this_toM2MStringFromBoolean(json, &jsonM2MString);
							}
						else if (type==M2MJSONType_NUMBER)
							{
							this_toM2MStringFromNumber(json, &jsonM2MString);
							}
						else if (type==M2MJSONType_OBJECT)
							{
							this_toM2MStringFromObject(M2MJSON_getObject(json), &jsonM2MString);
							}
						else if (type==M2MJSONType_STRING)
							{
							this_toStringFromString(json, &jsonM2MString);
							}
						//===== Error handling =====
						else
							{
							if ((*buffer)!=NULL)
								{
								M2MHeap_free((*buffer));
								}
							else
								{
								}
							return NULL;
							}
						//===== Check existence of string =====
						if (jsonM2MString!=NULL)
							{
							if ((jsonM2MStringLength=M2MString_length(jsonM2MString))>0)
								{
								//===== In the case of first Array node(need to append "[") =====
								if (stringLength==0)
									{
									(*buffer) = (M2MString *)M2MHeap_malloc(LEFT_SQUARE_BRACKET_LENGTH+jsonM2MStringLength+1);
									//===== Connect strings =====
									memcpy(&((*buffer)[0]), M2MString_LEFT_SQUARE_BRACKET, LEFT_SQUARE_BRACKET_LENGTH);
									memcpy(&((*buffer)[LEFT_SQUARE_BRACKET_LENGTH]), jsonM2MString, jsonM2MStringLength);
									//===== Release memory =====
									M2MHeap_free(jsonM2MString);
									}
								//===== In the case of 2nd Array node later(need to append ",") =====
								else
									{
									//=====  =====
									copy = (M2MString *)M2MHeap_malloc(stringLength+1);
									memcpy(copy, (*buffer), stringLength);
									//=====  =====
									M2MHeap_free((*buffer));
									(*buffer) = (M2MString *)M2MHeap_malloc(stringLength+COMMA_LENGTH+jsonM2MStringLength+1);
									//===== Connect strings =====
									memcpy(&((*buffer)[0]), copy, stringLength);
									memcpy(&((*buffer)[stringLength]), M2MString_COMMA, COMMA_LENGTH);
									memcpy(&((*buffer)[stringLength+COMMA_LENGTH]), jsonM2MString, jsonM2MStringLength);
									//===== Release memory =====
									M2MHeap_free(copy);
									M2MHeap_free(jsonM2MString);
									}
								//===== Update string length =====
								stringLength = M2MString_length((*buffer));
								}
							//===== Error handling =====
							else
								{
								//===== Release memory =====
								M2MHeap_free(jsonM2MString);
								if ((*buffer)!=NULL)
									{
									M2MHeap_free((*buffer));
									}
								else
									{
									}
								return NULL;
								}
							}
						//===== Error handling =====
						else
							{
							if ((*buffer)!=NULL)
								{
								M2MHeap_free((*buffer));
								}
							else
								{
								}
							return NULL;
							}
						}
					//===== Error handling =====
					else
						{
						if ((*buffer)!=NULL)
							{
							M2MHeap_free((*buffer));
							}
						else
							{
							}
						return NULL;
						}
					//===== Go to next Array node =====
					array = M2MJSON_getNextArray(array);
					}
				//===== Check last Array node =====
				if ((json=array->json)!=NULL)
					{
					if ((type=M2MJSON_getType(json))==M2MJSONType_ARRAY)
						{
						this_toM2MStringFromArray(M2MJSON_getArray(json), &jsonM2MString);
						}
					else if (type==M2MJSONType_BOOLEAN)
						{
						this_toM2MStringFromBoolean(json, &jsonM2MString);
						}
					else if (type==M2MJSONType_NUMBER)
						{
						this_toM2MStringFromNumber(json, &jsonM2MString);
						}
					else if (type==M2MJSONType_OBJECT)
						{
						this_toM2MStringFromObject(M2MJSON_getObject(json), &jsonM2MString);
						}
					else if (type==M2MJSONType_STRING)
						{
						this_toStringFromString(json, &jsonM2MString);
						}
					//===== Error handling =====
					else
						{
						if ((*buffer)!=NULL)
							{
							M2MHeap_free((*buffer));
							}
						else
							{
							}
						return NULL;
						}
					//===== Check existence of JSON string =====
					if (jsonM2MString!=NULL)
						{
						//===== Get length of JSON string =====
						if ((jsonM2MStringLength=M2MString_length(jsonM2MString))>0)
							{
							//===== Copy string into temporary buffer =====
							copy = (M2MString *)M2MHeap_malloc(stringLength+1);
							memcpy(copy, (*buffer), stringLength);
							//===== Allocate new memory for connecting string =====
							M2MHeap_free((*buffer));
							(*buffer) = (M2MString *)M2MHeap_malloc(stringLength+COMMA_LENGTH+jsonM2MStringLength+1);
							//===== Connect strings =====
							memcpy(&((*buffer)[0]), copy, stringLength);
							memcpy(&((*buffer)[stringLength]), M2MString_COMMA, COMMA_LENGTH);
							memcpy(&((*buffer)[stringLength+COMMA_LENGTH]), jsonM2MString, jsonM2MStringLength);
							//===== Release memory =====
							M2MHeap_free(copy);
							M2MHeap_free(jsonM2MString);
							//===== Update string length =====
							stringLength = M2MString_length((*buffer));
							}
						//===== Error handling =====
						else
							{
							M2MHeap_free(jsonM2MString);
							if ((*buffer)!=NULL)
								{
								M2MHeap_free((*buffer));
								}
							else
								{
								}
							return NULL;
							}
						}
					//===== Error handling =====
					else
						{
						if ((*buffer)!=NULL)
							{
							M2MHeap_free((*buffer));
							}
						else
							{
							}
						return NULL;
						}
					}
				//===== Error handling =====
				else
					{
					if ((*buffer)!=NULL)
						{
						M2MHeap_free((*buffer));
						}
					else
						{
						}
					return NULL;
					}
				}
			//===== Append "]" to close JSON Array syntax =====
			if (M2MString_append(buffer, M2MString_RIGHT_SQUARE_BRACKET)!=NULL)
				{
				return (*buffer);
				}
			else
				{
				M2MHeap_free((*buffer));
				return NULL;
				}
			}
		else
			{
			return NULL;
			}
		}
	//===== Argument error =====
	else if (array==NULL)
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSONArray\" is NULL");
		return NULL;
		}
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"");
		return NULL;
		}
	}


/**
 * This method translate from JSON Boolean object to JSON form string.<br>
 *
 * @param[in] self		JSON object
 * @param[out] buffer	buffer for copying translated JSON form string
 * @return				translated JSON form string or NULL(means error)
 */
static M2MString *this_toM2MStringFromBoolean (M2MJSON *self, M2MString **buffer)
	{
	//========== Variable ==========
	M2MString boolean[8];
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON.this_toM2MStringFromBoolean()";

	//===== Check argument =====
	if (self!=NULL && buffer!=NULL)
		{
		//===== Check JSON type =====
		if (M2MJSON_getType(self)==M2MJSONType_BOOLEAN
				&& M2MString_convertFromBooleanToString(M2MJSON_getBoolean(self), boolean, sizeof(boolean))!=NULL)
			{
			return M2MString_append(buffer, boolean);
			}
		//===== Error handling =====
		else
			{
			this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Type of Argument \"JSON\" isn't \"M2MJSONType_BOOLEAN\"");
			return NULL;
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSON\" is NULL");
		return NULL;
		}
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"string\" is NULL");
		return NULL;
		}
	}


/**
 * This method translate from JSON Number to JSON form string.<br>
 *
 * @param[in] self		JSON object
 * @param[out] buffer	buffer for copying translated JSON form string
 * @return				translated JSON form string or NULL(means error)
 */
static M2MString *this_toM2MStringFromNumber (M2MJSON *self, M2MString **buffer)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON.this_toM2MStringFromNumber()";

	//===== Check argument =====
	if (self!=NULL && buffer!=NULL)
		{
		//===== Check JSON type =====
		if (M2MJSON_getType(self)==M2MJSONType_NUMBER
				&& M2MString_convertFromDoubleToString(M2MJSON_getNumber(self), buffer)!=NULL)
			{
			return *buffer;
			}
		//===== Error handling =====
		else
			{
			this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Type of Argument \"JSON\" isn't \"M2MJSONType_NUMBER\"");
			return NULL;
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSON\" is NULL");
		return NULL;
		}
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"string\" is NULL");
		return NULL;
		}
	}


/**
 * This method translate from JSON Object to JSON form string.<br>
 *
 * @param[in] object	JSON object
 * @param[out] buffer	buffer for copying translated JSON form string
 * @return				translated JSON form string or NULL(means error)
 */
static unsigned char *this_toM2MStringFromObject (M2MJSONObject *object, M2MString **buffer)
	{
	//========== Variable ==========
	M2MJSON *this = NULL;
	M2MJSONType type;
	M2MString *jsonM2MString = NULL;
	M2MString *copy = NULL;
	M2MString *key = NULL;
	size_t stringLength = 0;
	size_t jsonM2MStringLength = 0;
	const size_t COMMA_LENGTH = M2MString_length(M2MString_COMMA);
	const size_t LEFT_CURLY_BRACKET_LENGTH = M2MString_length(M2MString_LEFT_CURLY_BRACKET);
	const size_t COLON_LENGTH = M2MString_length(M2MString_COLON);
	const size_t DOUBLE_QUOTATION_LENGTH = M2MString_length(M2MString_DOUBLE_QUOTATION);

	//===== Check argument =====
	if ((object=M2MJSON_getRootObject(object))!=NULL)
		{
		//========== In the case of existing JSON Object is only one ==========
		if (M2MJSON_getObjectSize(object)==1)
			{
			//===== Get JSON from Object node =====
			if ((key=M2MJSON_getKey(object))!=NULL && (this=object->json)!=NULL)
				{
				if ((type=M2MJSON_getType(this))==M2MJSONType_ARRAY)
					{
					this_toM2MStringFromArray(M2MJSON_getArray(this), &jsonM2MString);
					}
				else if (type==M2MJSONType_BOOLEAN)
					{
					this_toM2MStringFromBoolean(this, &jsonM2MString);
					}
				else if (type==M2MJSONType_NUMBER)
					{
					this_toM2MStringFromNumber(this, &jsonM2MString);
					}
				else if (type==M2MJSONType_OBJECT)
					{
					this_toM2MStringFromObject(M2MJSON_getObject(this), &jsonM2MString);
					}
				else if (type==M2MJSONType_STRING)
					{
					this_toStringFromString(this, &jsonM2MString);
					}
				else
					{
					return NULL;
					}
				//===== Check existence of string =====
				if (jsonM2MString!=NULL)
					{
					if (jsonM2MString!=NULL && (jsonM2MStringLength=M2MString_length(jsonM2MString))>0)
						{
						//===== Allocate new memory for connecting string =====
						(*buffer) = (M2MString *)M2MHeap_malloc(LEFT_CURLY_BRACKET_LENGTH+DOUBLE_QUOTATION_LENGTH+M2MString_length(key)+DOUBLE_QUOTATION_LENGTH+COLON_LENGTH+jsonM2MStringLength+1);
						//===== Connect strings =====
						memcpy(&((*buffer)[0]), M2MString_LEFT_CURLY_BRACKET, LEFT_CURLY_BRACKET_LENGTH);
						memcpy(&((*buffer)[LEFT_CURLY_BRACKET_LENGTH]), M2MString_DOUBLE_QUOTATION, DOUBLE_QUOTATION_LENGTH);
						memcpy(&((*buffer)[LEFT_CURLY_BRACKET_LENGTH+DOUBLE_QUOTATION_LENGTH]), key, M2MString_length(key));
						memcpy(&((*buffer)[LEFT_CURLY_BRACKET_LENGTH+DOUBLE_QUOTATION_LENGTH+M2MString_length(key)]), M2MString_DOUBLE_QUOTATION, DOUBLE_QUOTATION_LENGTH);
						memcpy(&((*buffer)[LEFT_CURLY_BRACKET_LENGTH+DOUBLE_QUOTATION_LENGTH+M2MString_length(key)+DOUBLE_QUOTATION_LENGTH]), M2MString_COLON, COLON_LENGTH);
						memcpy(&((*buffer)[LEFT_CURLY_BRACKET_LENGTH+DOUBLE_QUOTATION_LENGTH+M2MString_length(key)+DOUBLE_QUOTATION_LENGTH+COLON_LENGTH]), jsonM2MString, jsonM2MStringLength);
						//===== Release memory =====
						M2MHeap_free(jsonM2MString);
						//===== Update string length =====
						stringLength = M2MString_length((*buffer));
						}
					//===== Error handling =====
					else
						{
						//===== Release memory =====
						M2MHeap_free(jsonM2MString);
						return NULL;
						}
					}
				//===== Error handling =====
				else
					{
					return NULL;
					}
				}
			//===== Error handling =====
			else
				{
				return NULL;
				}
			}
		//========== In the case of existing JSON Object is many ==========
		else
			{
			//========== Loop while not achieving to last Object node ==========
			while (M2MJSON_getNextObject(object)!=NULL)
				{
				//===== Get JSON from Object node =====
				if ((key=M2MJSON_getKey(object))!=NULL && (this=object->json)!=NULL)
					{
					if ((type=M2MJSON_getType(this))==M2MJSONType_ARRAY)
						{
						this_toM2MStringFromArray(M2MJSON_getArray(this), &jsonM2MString);
						}
					else if (type==M2MJSONType_BOOLEAN)
						{
						this_toM2MStringFromBoolean(this, &jsonM2MString);
						}
					else if (type==M2MJSONType_NUMBER)
						{
						this_toM2MStringFromNumber(this, &jsonM2MString);
						}
					else if (type==M2MJSONType_OBJECT)
						{
						this_toM2MStringFromObject(M2MJSON_getObject(this), &jsonM2MString);
						}
					else if (type==M2MJSONType_STRING)
						{
						this_toStringFromString(this, &jsonM2MString);
						}
					//===== Error handling =====
					else
						{
						if ((*buffer)!=NULL)
							{
							M2MHeap_free((*buffer));
							}
						else
							{
							}
						return NULL;
						}
					//===== Check existence of string =====
					if (jsonM2MString!=NULL)
						{
						if (jsonM2MString!=NULL && (jsonM2MStringLength=M2MString_length(jsonM2MString))>0)
							{
							//===== In the case of first Array node(need to append "[") =====
							if (stringLength==0)
								{
								//===== Allocate new memory for connecting string =====
								M2MHeap_free((*buffer));
								(*buffer) = (M2MString *)M2MHeap_malloc(LEFT_CURLY_BRACKET_LENGTH+DOUBLE_QUOTATION_LENGTH+M2MString_length(key)+DOUBLE_QUOTATION_LENGTH+COLON_LENGTH+jsonM2MStringLength+1);
								//===== Connect strings =====
								memcpy(&((*buffer)[0]), M2MString_LEFT_CURLY_BRACKET, LEFT_CURLY_BRACKET_LENGTH);
								memcpy(&((*buffer)[LEFT_CURLY_BRACKET_LENGTH]), M2MString_DOUBLE_QUOTATION, DOUBLE_QUOTATION_LENGTH);
								memcpy(&((*buffer)[LEFT_CURLY_BRACKET_LENGTH+DOUBLE_QUOTATION_LENGTH]), key, M2MString_length(key));
								memcpy(&((*buffer)[LEFT_CURLY_BRACKET_LENGTH+DOUBLE_QUOTATION_LENGTH+M2MString_length(key)]), M2MString_DOUBLE_QUOTATION, DOUBLE_QUOTATION_LENGTH);
								memcpy(&((*buffer)[LEFT_CURLY_BRACKET_LENGTH+DOUBLE_QUOTATION_LENGTH+M2MString_length(key)+DOUBLE_QUOTATION_LENGTH]), M2MString_COLON, COLON_LENGTH);
								memcpy(&((*buffer)[LEFT_CURLY_BRACKET_LENGTH+DOUBLE_QUOTATION_LENGTH+M2MString_length(key)+DOUBLE_QUOTATION_LENGTH+COLON_LENGTH]), jsonM2MString, jsonM2MStringLength);
								//===== Release memory =====
								M2MHeap_free(jsonM2MString);
								}
							//===== In the case of 2nd Array node later(need to append ",") =====
							else
								{
								//=====  =====
								copy = (M2MString *)M2MHeap_malloc(stringLength+1);
								memcpy(copy, (*buffer), stringLength);
								//===== Allocate new memory for connecting string =====
								M2MHeap_free((*buffer));
								(*buffer) = (M2MString *)M2MHeap_malloc(stringLength+COMMA_LENGTH+DOUBLE_QUOTATION_LENGTH+M2MString_length(key)+DOUBLE_QUOTATION_LENGTH+COLON_LENGTH+jsonM2MStringLength+1);
								//===== Connect strings =====
								memcpy(&((*buffer)[0]), copy, stringLength);
								memcpy(&((*buffer)[stringLength]), M2MString_COMMA, COMMA_LENGTH);
								memcpy(&((*buffer)[stringLength+COMMA_LENGTH]), M2MString_DOUBLE_QUOTATION, DOUBLE_QUOTATION_LENGTH);
								memcpy(&((*buffer)[stringLength+COMMA_LENGTH+DOUBLE_QUOTATION_LENGTH]), key, M2MString_length(key));
								memcpy(&((*buffer)[stringLength+COMMA_LENGTH+DOUBLE_QUOTATION_LENGTH+M2MString_length(key)]), M2MString_DOUBLE_QUOTATION, DOUBLE_QUOTATION_LENGTH);
								memcpy(&((*buffer)[stringLength+COMMA_LENGTH+DOUBLE_QUOTATION_LENGTH+M2MString_length(key)+DOUBLE_QUOTATION_LENGTH]), M2MString_COLON, COLON_LENGTH);
								memcpy(&((*buffer)[stringLength+COMMA_LENGTH+DOUBLE_QUOTATION_LENGTH+M2MString_length(key)+DOUBLE_QUOTATION_LENGTH+COLON_LENGTH]), jsonM2MString, jsonM2MStringLength);
								//===== Release memory =====
								M2MHeap_free(copy);
								M2MHeap_free(jsonM2MString);
								}
							//===== Update string length =====
							stringLength = M2MString_length((*buffer));
							}
						//===== Error handling =====
						else
							{
							M2MHeap_free(jsonM2MString);
							if ((*buffer)!=NULL)
								{
								M2MHeap_free((*buffer));
								}
							else
								{
								}
							return NULL;
							}
						}
					//===== Error handling =====
					else
						{
						if ((*buffer)!=NULL)
							{
							M2MHeap_free((*buffer));
							}
						else
							{
							}
						return NULL;
						}
					}
				//===== Error handling =====
				else
					{
					if ((*buffer)!=NULL)
						{
						M2MHeap_free((*buffer));
						}
					else
						{
						}
					return NULL;
					}
				//===== Go to next Array node =====
				object = M2MJSON_getNextObject(object);
				}
			//========== Check last Array node ==========
			if ((key=M2MJSON_getKey(object))!=NULL && (this=object->json)!=NULL)
				{
				if ((type=M2MJSON_getType(this))==M2MJSONType_ARRAY)
					{
					this_toM2MStringFromArray(M2MJSON_getArray(this), &jsonM2MString);
					}
				else if (type==M2MJSONType_BOOLEAN)
					{
					this_toM2MStringFromBoolean(this, &jsonM2MString);
					}
				else if (type==M2MJSONType_NUMBER)
					{
					this_toM2MStringFromNumber(this, &jsonM2MString);
					}
				else if (type==M2MJSONType_OBJECT)
					{
					this_toM2MStringFromObject(M2MJSON_getObject(this), &jsonM2MString);
					}
				else if (type==M2MJSONType_STRING)
					{
					this_toStringFromString(this, &jsonM2MString);
					}
				//===== Error handling =====
				else
					{
					if ((*buffer)!=NULL)
						{
						M2MHeap_free((*buffer));
						}
					else
						{
						}
					return NULL;
					}
				//===== Check existence of string =====
				if (jsonM2MString!=NULL)
					{
					if (jsonM2MString!=NULL && (jsonM2MStringLength=M2MString_length(jsonM2MString))>0)
						{
						//===== Copy string into temporary buffer =====
						copy = (M2MString *)M2MHeap_malloc(stringLength+1);
						memcpy(copy, (*buffer), stringLength);
						//===== Allocate new memory for connecting string =====
						M2MHeap_free((*buffer));
						(*buffer) = (M2MString *)M2MHeap_malloc(stringLength+COMMA_LENGTH+DOUBLE_QUOTATION_LENGTH+M2MString_length(key)+DOUBLE_QUOTATION_LENGTH+COLON_LENGTH+jsonM2MStringLength+1);
						//===== Connect strings =====
						memcpy(&((*buffer)[0]), copy, stringLength);
						memcpy(&((*buffer)[stringLength]), M2MString_COMMA, COMMA_LENGTH);
						memcpy(&((*buffer)[stringLength+COMMA_LENGTH]), M2MString_DOUBLE_QUOTATION, DOUBLE_QUOTATION_LENGTH);
						memcpy(&((*buffer)[stringLength+COMMA_LENGTH+DOUBLE_QUOTATION_LENGTH]), key, M2MString_length(key));
						memcpy(&((*buffer)[stringLength+COMMA_LENGTH+DOUBLE_QUOTATION_LENGTH+M2MString_length(key)]), M2MString_DOUBLE_QUOTATION, DOUBLE_QUOTATION_LENGTH);
						memcpy(&((*buffer)[stringLength+COMMA_LENGTH+DOUBLE_QUOTATION_LENGTH+M2MString_length(key)+DOUBLE_QUOTATION_LENGTH]), M2MString_COLON, COLON_LENGTH);
						memcpy(&((*buffer)[stringLength+COMMA_LENGTH+DOUBLE_QUOTATION_LENGTH+M2MString_length(key)+DOUBLE_QUOTATION_LENGTH+COLON_LENGTH]), jsonM2MString, jsonM2MStringLength);
						//===== Release memory =====
						M2MHeap_free(copy);
						M2MHeap_free(jsonM2MString);
						//===== Update string length =====
						stringLength = M2MString_length((*buffer));
						}
					//===== Error handling =====
					else
						{
						M2MHeap_free(jsonM2MString);
						if ((*buffer)!=NULL)
							{
							M2MHeap_free((*buffer));
							}
						else
							{
							}
						return NULL;
						}
					}
				//===== Error handling =====
				else
					{
					if ((*buffer)!=NULL)
						{
						M2MHeap_free((*buffer));
						}
					else
						{
						}
					return NULL;
					}
				}
			//===== Error handling =====
			else
				{
				if ((*buffer)!=NULL)
					{
					M2MHeap_free((*buffer));
					}
				else
					{
					}
				return NULL;
				}
			}
		//===== Append "}" to close syntax =====
		if (M2MString_append(buffer, M2MString_RIGHT_CURLY_BRACKET)!=NULL)
			{
			return (*buffer);
			}
		else
			{
			M2MHeap_free((*buffer));
			return NULL;
			}
		}
	//===== Argument error =====
	else
		{
		return NULL;
		}
	}


/**
 * This method translate from JSON M2MString to JSON form string.<br>
 *
 * @param[in] self		JSON object
 * @param[out] buffer	buffer for copying translated JSON form string
 * @return				translated JSON form string or NULL(means error)
 */
static unsigned char *this_toStringFromString (M2MJSON *self, M2MString **string)
	{
	//========== Variable ==========
	M2MString *value = NULL;
	size_t valueLength = 0;
	const unsigned int DOUBLE_QUOTATION_LENGTH = M2MString_length(M2MString_DOUBLE_QUOTATION);
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON.this_toStringFromString()";

	//===== Check argument =====
	if (self!=NULL && string!=NULL)
		{
		//===== Check JSON type =====
		if (M2MJSON_getType(self)==M2MJSONType_STRING)
			{
			//===== Get string value and length =====
			if ((value=M2MJSON_getString(self))!=NULL
					&& (valueLength=M2MString_length(value))>0)
				{
				//===== Allocate new memory for JSON M2MString =====
				if (((*string)=(unsigned char *)M2MHeap_malloc(DOUBLE_QUOTATION_LENGTH+valueLength+DOUBLE_QUOTATION_LENGTH+1))!=NULL)
					{
					//===== Copy string value into buffer =====
					memcpy(&((*string)[0]), M2MString_DOUBLE_QUOTATION, DOUBLE_QUOTATION_LENGTH);
					memcpy(&((*string)[DOUBLE_QUOTATION_LENGTH]), value, valueLength);
					memcpy(&((*string)[DOUBLE_QUOTATION_LENGTH+valueLength]), M2MString_DOUBLE_QUOTATION, DOUBLE_QUOTATION_LENGTH);
					return (*string);
					}
				//===== Error handling =====
				else
					{
					this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to allocate new memory for copying string value");
					return NULL;
					}
				}
			//===== Error handling =====
			else if (value==NULL)
				{
				this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to get string value from JSON");
				return NULL;
				}
			else
				{
				this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to get length of string value from JSON");
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Type of Argument \"JSON\" isn't \"M2MJSONType_STRING\"");
			return NULL;
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSON\" is NULL");
		return NULL;
		}
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"string\" is NULL");
		return NULL;
		}
	}



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
uint32_t M2MJSON_calculateHash (const M2MString *key, const size_t keyLength)
	{
	return M2MCRC32_getValue(key, keyLength);
	}


/**
 * Release allocated memory for JSON Array and return a initialized JSON<br>
 * object which has no value.<br>
 * All element of indicated Array are removed.<br>
 *
 * @param[in,out] self	JSON structure object(including Array as JSON value)
 * @return				Initialized JSON structure object which has no value
 */
M2MJSON *M2MJSON_clearArray (M2MJSON *self)
	{
	//========== Variable ==========
	M2MJSON *tmpJSON = NULL;
	M2MJSONArray *array = NULL;
	M2MJSONArray *iterator = NULL;
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON_clearArray()";

	//===== Check argument =====
	if (self!=NULL)
		{
		//=====  =====
		if ((array=M2MJSON_getArray(self))!=NULL
				&& (iterator=M2MJSON_getRootArray(array))!=NULL)
			{
			//===== Loop while existing next Array element =====
			while (M2MJSON_getNextArray(iterator)!=NULL)
				{
				//===== Get next JSON Array temporarily =====
				array = M2MJSON_getNextArray(iterator);
				//===== Release allocated memory for JSON Array =====
				if ((tmpJSON=iterator->json)!=NULL)
					{
					M2MJSON_delete(&tmpJSON);
					}
				else
					{
					}
				M2MHeap_free(iterator);
				//===== Go to next element =====
				iterator = array;
				}
			//===== Release allocated memory for JSON Array =====
			if ((tmpJSON=iterator->json)!=NULL)
				{
				M2MJSON_delete(&tmpJSON);
				}
			else
				{
				}
			M2MHeap_free(iterator);
			//===== Initialize JSON =====
			M2MJSON_setType(self, M2MJSONType_NULL);
			memset(M2MJSON_getValue(self), 0, sizeof(M2MJSONValue));
			//===== Return initialized JSON =====
			return self;
			}
		//===== Error handling =====
		else if (array==NULL)
			{
			this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to get \"JSONArray\" object");
			return self;
			}
		else
			{
			this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to get root node of \"JSONArray\" object");
			return self;
			}
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSON\" is NULL");
		return NULL;
		}
	}


/**
 * Release allocated memory for JSON Object and return a initialized JSON<br>
 * object which has no value.<br>
 *
 * @param[in,out] self	JSON structure object
 * @return				Initialized JSON structure object which has no value
 */
M2MJSON *M2MJSON_clearObject (M2MJSON *self)
	{
	//========== Variable ==========
	M2MJSONObject *object = NULL;
	M2MJSONObject *next = NULL;
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON_clearObject()";

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Get root node of JSON Object =====
		if ((object=M2MJSON_getObject(self))!=NULL
				&& (object=M2MJSON_getRootObject(object))!=NULL)
			{
			//===== Loop while existing next JSON Object =====
			while ((next=M2MJSON_getNextObject(object))!=NULL)
				{
				//===== Release allocated memory for JSON Object =====
				this_deleteObject(&object);
				//===== Go to next JSON Object =====
				object = next;
				}
			//===== Release allocated memory for last JSON Object =====
			this_deleteObject(&object);
			//===== Initialize JSON =====
			M2MJSON_setType(self, M2MJSONType_NULL);
			memset(M2MJSON_getValue(self), 0, sizeof(M2MJSONValue));
			//===== Return initialized JSON =====
			return self;
			}
		//===== Error handling =====
		else if (object==NULL)
			{
			this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to get \"JSONObject\" object");
			return self;
			}
		else
			{
			this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to get root node of \"JSONObject\" object");
			return self;
			}
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSON\" is NULL");
		return NULL;
		}
	}


/**
 * @param[in,out] self	JSON structure object(including M2MString as JSON value)
 * @return				Initialized JSON oject
 */
M2MJSON *M2MJSON_clearString (M2MJSON *self)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON_clearString()";

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Get string value =====
		if (M2MJSON_getString(self)!=NULL)
			{
			//===== Release allocated memory =====
			M2MHeap_free(self->value->string);
			//===== Initialize JSON =====
			M2MJSON_setType(self, M2MJSONType_NULL);
			memset(M2MJSON_getValue(self), 0, sizeof(M2MJSONValue));
			}
		//===== Argument error =====
		else
			{
			this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSON\" hasn't string value");
			}
		//===== Return initialized JSON =====
		return self;
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSON\" is NULL");
		return NULL;
		}
	}


/**
 * This method allocates new memory for creating JSONArray object.<br>
 *
 * @return	created new JSON Array object or NULL(means error)
 */
M2MJSONArray *M2MJSON_createNewArray ()
	{
	//========== Variable ==========
	M2MJSONArray *array = NULL;
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON_createNewArray()";

	//===== Allocate new memory for JSON Array =====
	if ((array=(M2MJSONArray *)M2MHeap_malloc(sizeof(M2MJSONArray)))!=NULL)
		{
		//===== Initialization =====
		M2MJSON_setPreviousArray(array, array);
		M2MJSON_setNextArray(array, NULL);
		this_printDebugMessage(METHOD_NAME, __LINE__, (M2MString *)"Created new \"JSONArray\" object");
		return array;
		}
	//===== Error handling =====
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to allocate new memory for creating \"JSONArray\" object");
		return NULL;
		}
	}


/**
 * This method allocates new memory for creating JSONObject object.<br>
 *
 * @return	created new JSON Object object or NULL(means error)
 */
M2MJSONObject *M2MJSON_createNewObject ()
	{
	//========== Variable ==========
	M2MJSONObject *object = NULL;
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON_createNewObject()";

	//===== Allocate new memory for JSON Object =====
	if ((object=(M2MJSONObject *)M2MHeap_malloc(sizeof(M2MJSONObject)))!=NULL)
		{
		//===== Initialization =====
		M2MJSON_setPreviousObject(object, object);
		M2MJSON_setNextObject(object, NULL);
		this_setSameIndexPrevious(object, object);
		this_setSameIndexNext(object, NULL);
		this_printDebugMessage(METHOD_NAME, __LINE__, (M2MString *)"Created new \"JSONObject\" object");
		return object;
		}
	//===== Error handling =====
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to allocate new memory for creating \"JSONObject\" object");
		return NULL;
		}
	}


/**
 * This method deletes JSON Array node. So, caller can't access it later.<br>
 *
 * @param[in,out] array		delete target JSON Array object
 * @return					lead node of JSON Array connection
 */
M2MJSONArray *M2MJSON_deleteArray (M2MJSONArray **array)
	{
	//========== Variable ==========
	uint32_t size = 0;
	M2MJSONArray *previous = NULL;
	M2MJSONArray *next = NULL;

	//===== Check argument =====
	if (array!=NULL && (*array)!=NULL)
		{
		//===== In the case of existing node is 0 =====
		if ((size=M2MJSON_getArraySize((*array)))==0)
			{
			return NULL;
			}
		//===== In the case of existing node is only one =====
		else if (size==1)
			{
			//===== Release allocated memory for JSON Array =====
			this_deleteArray(array);
			return NULL;
			}
		//===== In the case of existing node is 2 =====
		else if (size==2)
			{
			//===== In the case of lead node =====
			if (M2MJSON_getPreviousArray((*array))==(*array))
				{
				//===== Disconnect =====
				next = M2MJSON_getNextArray((*array));
				M2MJSON_setPreviousArray(next, next);
				//===== Release allocated memory for JSON Array =====
				this_deleteArray(array);
				return next;
				}
			//===== In the case of last node =====
			else if (M2MJSON_getNextArray((*array))==NULL)
				{
				//===== Disconnect =====
				previous = M2MJSON_getPreviousArray((*array));
				M2MJSON_setNextArray(previous, NULL);
				//===== Release allocated memory for JSON Array =====
				this_deleteArray(array);
				return previous;
				}
			//===== Error handling =====
			else
				{
				return NULL;
				}
			}
		//===== In the case of existing node is many =====
		else
			{
			//===== In the case of lead node =====
			if (M2MJSON_getPreviousArray((*array))==(*array))
				{
				//===== Disconnect =====
				next = M2MJSON_getNextArray((*array));
				M2MJSON_setPreviousArray(next, next);
				//===== Release allocated memory for JSON Array =====
				this_deleteArray(array);
				return next;
				}
			//===== In the case of last node =====
			else if (M2MJSON_getNextArray((*array))==NULL)
				{
				//===== Disconnect =====
				previous = M2MJSON_getPreviousArray((*array));
				M2MJSON_setNextArray(previous, NULL);
				//===== Release allocated memory for JSON Array =====
				this_deleteArray(array);
				return previous;
				}
			//===== In the case of middle node =====
			else
				{
				//===== Disconnect =====
				previous = M2MJSON_getPreviousArray((*array));
				next = M2MJSON_getNextArray((*array));
				M2MJSON_setNextArray(previous, next);
				M2MJSON_setPreviousArray(next, previous);
				//===== Release allocated memory for JSON Array =====
				this_deleteArray(array);
				return M2MJSON_getRootArray(previous);
				}
			}
		}
	//===== Argument error =====
	else
		{
		return NULL;
		}
	}


/**
 * This method deletes JSON Object node. So, caller can't access it later.<br>
 *
 * @param[in,out] object	delete target JSON Object node
 * @return					lead node of JSON Object connection
 */
M2MJSONObject *M2MJSON_deleteObject (M2MJSONObject **object)
	{
	//========== Variable ==========
	uint32_t size = 0;
	M2MJSONObject *previous = NULL;
	M2MJSONObject *next = NULL;

	//===== Check argument =====
	if ((*object)!=NULL)
		{
		//===== In the case of existing node is 0 =====
		if ((size=M2MJSON_getObjectSize((*object)))==0)
			{
			return NULL;
			}
		//===== In the case of existing node is only one =====
		else if (size==1)
			{
			//===== Delete hash table =====
			this_deleteObjectTable((*object));
			//===== Release allocated memory for JSON Object =====
			this_deleteObject(object);
			return NULL;
			}
		//===== In the case of existing node is 2 =====
		else if (size==2)
			{
			//===== In the case of lead node =====
			if (M2MJSON_getPreviousObject((*object))==(*object))
				{
				//===== Disconnect =====
				next = M2MJSON_getNextObject((*object));
				M2MJSON_setPreviousObject(next, next);
				//===== Release allocated memory for JSON Object =====
				this_deleteObject(object);
				return next;
				}
			//===== In the case of last node =====
			else if (M2MJSON_getNextObject((*object))==NULL)
				{
				//===== Disconnect =====
				previous = M2MJSON_getPreviousObject((*object));
				M2MJSON_setNextObject(previous, NULL);
				//===== Release allocated memory for JSON Object =====
				this_deleteObject(object);
				return previous;
				}
			//===== Error handling =====
			else
				{
				return NULL;
				}
			}
		//===== In the case of existing node is many =====
		else
			{
			//===== In the case of lead node =====
			if (M2MJSON_getPreviousObject((*object))==(*object))
				{
				//===== Disconnect =====
				next = M2MJSON_getNextObject((*object));
				M2MJSON_setPreviousObject(next, next);
				//===== Release allocated memory for JSON Object =====
				this_deleteObject(object);
				return next;
				}
			//===== In the case of last node =====
			else if (M2MJSON_getNextObject((*object))==NULL)
				{
				//===== Disconnect =====
				previous = M2MJSON_getPreviousObject((*object));
				M2MJSON_setNextObject(previous, NULL);
				//===== Release allocated memory for JSON Object =====
				this_deleteObject(object);
				return previous;
				}
			//===== Error handling =====
			else
				{
				//===== Disconnect =====
				previous = M2MJSON_getPreviousObject((*object));
				next = M2MJSON_getNextObject((*object));
				M2MJSON_setNextObject(previous, next);
				M2MJSON_setPreviousObject(next, previous);
				//===== Release allocated memory for JSON Object =====
				this_deleteObject(object);
				return M2MJSON_getRootObject(previous);
				}
			}
		}
	//===== Argument error =====
	else
		{
		return NULL;
		}
	}


/**
 * This method released allocated memory for JSON object.<br>
 *
 * @param[in,out] self	delete target JSON object
 */
void M2MJSON_delete (M2MJSON **self)
	{
	//========== Variable ==========
	M2MJSONType type;
	M2MJSONValue *value = NULL;

	//===== Check argument =====
	if (self!=NULL && (*self)!=NULL)
		{
		//===== In the case of JSON Array =====
		if ((type=M2MJSON_getType((*self)))==M2MJSONType_ARRAY)
			{
			(*self) = M2MJSON_clearArray((*self));
			}
		//===== In the case of JSON Boolean =====
		else if (type==M2MJSONType_BOOLEAN)
			{
			}
		//===== In the case of JSON Number =====
		else if (type==M2MJSONType_NUMBER)
			{
			}
		//===== In the case of JSON Object =====
		else if (type==M2MJSONType_OBJECT)
			{
			(*self) = M2MJSON_clearObject((*self));
			}
		//===== In the case of JSON M2MString =====
		else if (type==M2MJSONType_STRING)
			{
			(*self) = M2MJSON_clearString((*self));
			}
		else
			{
			}
		//===== Check existence of JSON =====
		if ((*self)!=NULL)
			{
			//===== Check existence of JSON value =====
			if ((value=M2MJSON_getValue((*self)))!=NULL)
				{
				//===== Release allocated memory for JSON value =====
				M2MHeap_free(value);
				}
			else
				{
				}
			//===== Release allocated memory for JSON =====
			M2MHeap_free((*self));
			}
		else
			{
			}
		}
	//===== Argument error =====
	else
		{
		}
	return;
	}


/**
 * @param[in] self
 * @return
 */
M2MJSONArray *M2MJSON_getArray (const M2MJSON *self)
	{
	//========== Variable ==========
	M2MJSONValue *value = NULL;
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON_getArray()";

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Check argument =====
		if (M2MJSON_getType(self)==M2MJSONType_ARRAY)
			{
			//===== Get JSON value =====
			if ((value=M2MJSON_getValue(self))!=NULL)
				{
				return value->array;
				}
			//===== Error handling =====
			else
				{
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Type of Argument \"JSON\" isn't \"M2MJSONType_ARRAY\"");
			return NULL;
			}
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSON\" is NULL");
		return NULL;
		}
	}


/**
 * @param[in] array
 * @return
 */
uint32_t M2MJSON_getArraySize (M2MJSONArray *array)
	{
	//========== Variable ==========
	uint32_t size = 0;
	M2MJSONArray *next = NULL;
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON_getArraySize()";

	//===== Check argument =====
	if (array!=NULL)
		{
		//===== Get root node =====
		if ((array=M2MJSON_getRootArray(array))!=NULL)
			{
			//===== Count first node =====
			size++;
			//===== Loop while not reaching last node =====
			while ((next=M2MJSON_getNextArray(array))!=NULL)
				{
				//===== Count next node =====
				size++;
				//===== Go to next node =====
				array = next;
				}
			//===== Return number of nodes =====
			return size;
			}
		//===== Error handling =====
		else
			{
			this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to get root node of \"JSONArray\" object");
			return 0;
			}
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSONArray\" is NULL");
		return 0;
		}
	}


/**
 * @param json
 * @return
 */
bool M2MJSON_getBoolean (const M2MJSON *self)
	{
	//========== Variable ==========
	M2MJSONValue *value = NULL;
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON_getBoolean()";

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Get type of JSON =====
		if (M2MJSON_getType(self)==M2MJSONType_BOOLEAN)
			{
			//===== Get value of JSON =====
			if ((value=M2MJSON_getValue(self))!=NULL)
				{
				return value->boolean;
				}
			//===== Error handling =====
			else
				{
				this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Value of Argument \"JSON\" is NULL");
				return false;
				}
			}
		//===== Error handling =====
		else
			{
			this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Type of Argument \"JSON\" isn't \"M2MJSONType_BOOLEAN\"");
			return false;
			}
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSON\" is NULL");
		return false;
		}
	}


/**
 * @param[in] object
 * @return
 */
uint32_t M2MJSON_getHash (const M2MJSONObject *object)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON_getHash()";

	//===== Check argument =====
	if (object!=NULL)
		{
		return object->hash;
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSONObject\" is NULL");
		return 0;
		}
	}


/**
 * @param array
 * @param index
 * @return
 */
M2MJSON *M2MJSON_getJSONFromArray (M2MJSONArray *array, const uint32_t index)
	{
	//========== Variable ==========
	M2MJSON *json = NULL;
	uint32_t i = 0;
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON_getJSONFromArray()";

	//===== Check argument =====
	if (array!=NULL && M2MJSON_getArraySize(array)>index)
		{
		//===== Check argument =====
		if ((array=M2MJSON_getRootArray(array))!=NULL)
			{
			//===== Go to object index =====
			for (i=0; i<index; i++)
				{
				//===== Check existence of JSONArray object =====
				if (M2MJSON_getNextArray(array)!=NULL)
					{
					//===== Get next JSONArray object =====
					array = M2MJSON_getNextArray(array);
					}
				//===== Error handling =====
				else
					{
					this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Indicated index number is invalid");
					return NULL;
					}
				}
			return (json=array->json);
			}
		//===== Error handling =====
		else
			{
			this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to get root node of \"JSONArray\" object");
			return NULL;
			}
		}
	//===== Argument error =====
	else if (array==NULL)
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSONArray\" is NULL");
		return NULL;
		}
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"index\" number overs the size of \"JSONArray\"");
		return NULL;
		}
	}


/**
 * @param[in] object
 * @param[in] key
 * @param[in] keyLength
 * @return
 */
M2MJSON *M2MJSON_getJSONFromObject (M2MJSONObject *object, const M2MString *key, const size_t keyLength)
	{
	//========== Variable ==========
	M2MString MESSAGE[256];
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON_getJSONFromObject()";

	//===== Check argument =====
	if ((object=M2MJSON_getRootObject(object))!=NULL && key!=NULL && keyLength>0)
		{
		//===== Search JSON with key =====
		if ((object=M2MJSON_searchObject(object, key, keyLength))!=NULL)
			{
			return object->json;
			}
		//===== Error handling =====
		else
			{
			memset(MESSAGE, 0, sizeof(MESSAGE));
			M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Failed to search JSON object with indicated key(= \"%s\" )", key);
			this_printErrorMessage(METHOD_NAME, __LINE__, MESSAGE);
			return NULL;
			}
		}
	//===== Argument error =====
	else if (object==NULL)
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSONObject\" is NULL");
		return NULL;
		}
	else if (key==NULL)
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"key\" is NULL");
		return NULL;
		}
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"keyLength\" isn't positive");
		return NULL;
		}
	}


/**
 * @param object
 * @return
 */
unsigned char *M2MJSON_getKey (const M2MJSONObject *object)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON_getKey()";

	//===== Check argument =====
	if (object!=NULL)
		{
		return object->key;
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSONObject\" is NULL");
		return NULL;
		}
	}


/**
 * @param array
 * @return
 */
M2MJSONArray *M2MJSON_getLastArray (M2MJSONArray *array)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON_getLastArray()";

	//===== Check argument =====
	if (array!=NULL)
		{
		while (M2MJSON_getNextArray(array)!=NULL)
			{
			array = M2MJSON_getNextArray(array);
			}
		return array;
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSONArray\" is NULL");
		return NULL;
		}
	}


/**
 * @param object
 * @return
 */
M2MJSONObject *M2MJSON_getLastObject (M2MJSONObject *object)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON_getLastObject()";

	//===== Check argument =====
	if (object!=NULL)
		{
		while (M2MJSON_getNextObject(object)!=NULL)
			{
			object = M2MJSON_getNextObject(object);
			}
		return object;
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSONObject\" is NULL");
		return NULL;
		}
	}


/**
 * @param array
 * @return
 */
M2MJSONArray *M2MJSON_getNextArray (M2MJSONArray *array)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON_getNextArray()";

	//===== Check argument =====
	if (array!=NULL)
		{
		return array->next;
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSONArray\" is NULL");
		return NULL;
		}
	}


/**
 * @param object
 * @return
 */
M2MJSONObject *M2MJSON_getNextObject (M2MJSONObject *object)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON_getNextObject()";

	//===== Check argument =====
	if (object!=NULL)
		{
		return object->next;
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSONObject\" is NULL");
		return NULL;
		}
	}


/**
 * @param[in] self
 * @return
 */
double M2MJSON_getNumber (const M2MJSON *self)
	{
	//========== Variable ==========
	M2MJSONValue *value = NULL;
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON_getNumber()";

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Get type of JSON =====
		if (M2MJSON_getType(self)==M2MJSONType_NUMBER)
			{
			//===== Get value of JSON =====
			if ((value=M2MJSON_getValue(self))!=NULL)
				{
				return value->number;
				}
			//===== Error handling =====
			else
				{
				this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Value of Argument \"JSON\" is NULL");
				return 0.0;
				}
			}
		//===== Error handling =====
		else
			{
			this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Type of Argument \"JSON\" isn't \"M2MJSONType_NUMBER\"");
			return 0.0;
			}
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSON\" is NULL");
		return 0.0;
		}
	}


/**
 * @param[in] self
 * @return
 */
M2MJSONObject *M2MJSON_getObject (const M2MJSON *self)
	{
	//========== Variable ==========
	M2MJSONValue *value = NULL;
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON_getObject()";

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Get type of JSON =====
		if (M2MJSON_getType(self)==M2MJSONType_OBJECT)
			{
			//===== Get value of JSON =====
			if ((value=M2MJSON_getValue(self))!=NULL)
				{
				return value->object;
				}
			//===== Error handling =====
			else
				{
				this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Value of Argument \"JSON\" is NULL");
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Type of Argument \"JSON\" isn't \"M2MJSONType_OBJECT\"");
			return NULL;
			}
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSON\" is NULL");
		return NULL;
		}
	}


/**
 * @param[in] object
 * @return				Number of JSON Objects
 */
uint32_t M2MJSON_getObjectSize (M2MJSONObject *object)
	{
	//========== Variable ==========
	uint32_t size = 0;
	M2MString MESSAGE[64];
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON_getObjectSize()";

	//===== Check argument =====
	if (object!=NULL)
		{
		//===== Get root node =====
		if ((object=M2MJSON_getRootObject(object))!=NULL)
			{
			//===== Count 1st node =====
			size++;
			//===== Loop while not reaching last node =====
			while (M2MJSON_getNextObject(object)!=NULL)
				{
				//===== Count a node =====
				size++;
				//===== Go to next node =====
				object = M2MJSON_getNextObject(object);
				}
			memset(MESSAGE, 0, sizeof(MESSAGE));
			M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Number of \"JSONObject\" is %d", size);
			this_printDebugMessage(METHOD_NAME, __LINE__, MESSAGE);
			//===== Return number of nodes =====
			return size;
			}
		//===== Error handling =====
		else
			{
			this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Root node of \"JSONObject\" is NULL");
			return 0;
			}
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSONObject\" is NULL");
		return 0;
		}
	}


/**
 * @param array
 * @return
 */
M2MJSONArray *M2MJSON_getPreviousArray (M2MJSONArray *array)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON_getPreviousArray()";

	//===== Check argument =====
	if (array!=NULL)
		{
		return array->previous;
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSONArray\" is NULL");
		return NULL;
		}
	}


/**
 * @param[in] object
 * @return
 */
M2MJSONObject *M2MJSON_getPreviousObject (M2MJSONObject *object)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON_getPreviousObject()";

	//===== Check argument =====
	if (object!=NULL)
		{
		return object->previous;
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSONObject\" is NULL");
		return NULL;
		}
	}


/**
 * @param array
 * @return
 */
M2MJSONArray *M2MJSON_getRootArray (M2MJSONArray *array)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON_getRootArray()";

	//===== Check argument =====
	if (array!=NULL)
		{
		//===== Loop while achieving previous node =====
		while (M2MJSON_getPreviousArray(array)!=array)
			{
			array = M2MJSON_getPreviousArray(array);
			}
		return array;
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSONArray\" is NULL");
		return NULL;
		}
	}


/**
 * @param object
 * @return
 */
M2MJSONObject *M2MJSON_getRootObject (M2MJSONObject *object)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON_getRootObject()";

	//===== Check argument =====
	if (object!=NULL)
		{
		//===== Loop while achieving previous node =====
		while (M2MJSON_getPreviousObject(object)!=object)
			{
			object = M2MJSON_getPreviousObject(object);
			}
		return object;
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSONObject\" is NULL");
		return NULL;
		}
	}


/**
 * @param json
 * @return
 */
unsigned char *M2MJSON_getString (const M2MJSON *self)
	{
	//========== Variable ==========
	M2MJSONValue *value = NULL;
	M2MString MESSAGE[64];
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON_getString()";

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Check JSON type =====
		if (M2MJSON_getType(self)==M2MJSONType_STRING)
			{
			//===== Get JSON value =====
			if ((value=M2MJSON_getValue(self))!=NULL)
				{
				return value->string;
				}
			//===== Error handling =====
			else
				{
				this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Value of Argument \"JSON\" is NULL");
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			memset(MESSAGE, 0, sizeof(MESSAGE));
			M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Argument \"JSON\" type(=\"%s\") isn't \"%s\"", M2MJSONType_toString(M2MJSON_getType(self)), M2MJSONType_toString(M2MJSONType_STRING));
			this_printErrorMessage(METHOD_NAME, __LINE__, MESSAGE);
			return NULL;
			}
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSON\" is NULL");
		return NULL;
		}
	}


/**
 * @param self
 * @return
 */
M2MJSONType M2MJSON_getType (const M2MJSON *self)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON_getType()";

	//===== Check argument =====
	if (self!=NULL)
		{
		return self->type;
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSON\" is NULL");
		return M2MJSONType_NULL;
		}
	}


/**
 * @param self
 * @return
 */
M2MJSONValue *M2MJSON_getValue (const M2MJSON *self)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON_getValue()";

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Check existence of value =====
		if (self->value!=NULL)
			{
			return self->value;
			}
		//===== Error handling =====
		else
			{
			this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Value of Argument \"JSON\" is NULL");
			return NULL;
			}
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSON\" is NULL");
		return NULL;
		}
	}


/**
 * This method allocate new memory for construct JSON structure object.<br>
 * So caller must use "M2MJSON_delete()" for release the memory.<br>
 *
 * @return	JSON structure object
 */
M2MJSON *M2MJSON_new ()
	{
	//========== Variable ==========
	M2MJSON *self = NULL;
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON_new()";

	//===== Allocate new memory for JSON object =====
	if ((self=(M2MJSON *)M2MHeap_malloc(sizeof(M2MJSON)))!=NULL)
		{
		//===== Initialize JSON object =====
		self->type = M2MJSONType_NULL;
		//===== Allocate new memory for JSON value =====
		if ((self->value=(M2MJSONValue *)M2MHeap_malloc(sizeof(M2MJSONValue)))!=NULL)
			{
			return self;
			}
		//===== Error handling =====
		else
			{
			this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to allocate new memory for creating JSONValue structure object");
			M2MJSON_delete(&self);
			return NULL;
			}
		}
	//===== Error handling =====
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to allocate new memory for creating JSON structure object");
		return NULL;
		}
	}


/**
 * This method searches JSON Object from hash table and same index chain.<br>
 *
 * @param[in] object	JSON Object
 * @param[in] key		key string
 * @param[in] keyLength	length of key string[Byte]
 * @return				detected JSON Object object or NULL
 */
M2MJSONObject *M2MJSON_searchObject (M2MJSONObject *object, const M2MString *key, const size_t keyLength)
	{
	//========== Variable ==========
	M2MString MESSAGE[256];
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON_searchObject()";

	//===== Check argument =====
	if (object!=NULL && key!=NULL && keyLength>0)
		{
		//===== Search in hash table =====
		if ((object=this_searchObjectTable(object, key, keyLength))!=NULL)
			{
			//===== Loop while existing same index chain =====
			while (this_getSameIndexNext(object)!=NULL)
				{
				//===== In the case of same key =====
				if (memcmp(M2MJSON_getKey(object), key, keyLength)==0)
					{
					return object;
					}
				//===== In the case of not correspond with key =====
				else
					{
					//===== Go to next same index object =====
					object = this_getSameIndexNext(object);
					}
				}
			//===== In the case of same key =====
			if (memcmp(M2MJSON_getKey(object), key, keyLength)==0)
				{
				return object;
				}
			//===== In the case of not correspond with key =====
			else
				{
				memset(MESSAGE, 0, sizeof(MESSAGE));
				M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"There is no \"JSONObject\" which corresponds with indicated key(= \"%s\" )", key);
				this_printErrorMessage(METHOD_NAME, __LINE__, MESSAGE);
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			memset(MESSAGE, 0, sizeof(MESSAGE));
			M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"There is no \"JSONObject\" which corresponds with indicated key(= \"%s\" )", key);
			this_printDebugMessage(METHOD_NAME, __LINE__, MESSAGE);
			return NULL;
			}
		}
	//===== Argument error =====
	else if (object==NULL)
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSONObject\" is NULL");
		return NULL;
		}
	else if (key==NULL)
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"key\" is NULL");
		return NULL;
		}
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"keyLength\" isn't positive");
		return NULL;
		}
	}


/**
 *
 * @param[in] array
 * @param[in] json
 * @return
 */
M2MJSONArray *M2MJSON_setJSONToArray (M2MJSONArray *array, M2MJSON *json)
	{
	//========== Variable ==========
	M2MJSONArray *node = NULL;
	M2MJSONArray *iterator = NULL;
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON_setJSONToArray()";

	//===== Check argument =====
	if (array!=NULL && json!=NULL)
		{
		if ((iterator=M2MJSON_getRootArray(array))!=NULL)
			{
			//===== In the case of first node is empty =====
			if (iterator->json==NULL || M2MJSON_getType(iterator->json)==M2MJSONType_NULL)
				{
				//===== In the case of existing JSON =====
				if (iterator->json!=NULL)
					{
					//===== Initialization =====
					M2MJSON_delete(&(iterator->json));
					}
				//===== In the case of not existing JSON =====
				else
					{
					// do nothing
					}
				//===== Set JSON to Array =====
				iterator->json = json;
				//===== Return the Array(for reuse) =====
				return iterator;
				}
			//===== In the case of first node isn't empty =====
			else
				{
				//===== Create new node =====
				if ((node=M2MJSON_createNewArray())!=NULL)
					{
					//===== Set JSON to Array =====
					node->json = json;
					//===== Get last Array node =====
					iterator = M2MJSON_getLastArray(iterator);
					//===== Connect Array into existing network =====
					M2MJSON_setPreviousArray(node, iterator);
					M2MJSON_setNextArray(node, NULL);
					M2MJSON_setNextArray(iterator, node);
					return node;
					}
				//===== Error handling =====
				else
					{
					this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to create new JSONArray structure object");
					return NULL;
					}
				}
			}
		//===== Error handling =====
		else
			{
			this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to get root node of JSON array");
			return NULL;
			}
		}
	//===== Argument error =====
	else if (array==NULL)
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSONArray\" is NULL");
		return NULL;
		}
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSON\" is NULL");
		return NULL;
		}
	}


/**
 * This value setting is "Shallow Copy". So, don't remove the value<br>
 * structure.<br>
 * If remove the value structure, the value belongs to Object is removed<br>
 * too.<br>
 *
 * @param[in,out] object	JSON Object which is set key string and JSON as value
 * @param[in] key			key string
 * @param[in] keyLength		length of key string
 * @param[in] json			JSON which is set as value
 * @return
 */
M2MJSONObject *M2MJSON_setJSONToObject (M2MJSONObject *object, const M2MString *key, const size_t keyLength, M2MJSON *json)
	{
	//========== Variable ==========
	M2MJSONObject *existingNode = NULL;
	M2MJSONObject *previous = NULL;
	M2MJSONObject *newNode = NULL;
	M2MJSONObjectTable *table = NULL;
	uint32_t index = 0;
	M2MString MESSAGE[256];
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON_setJSONToObject()";

	//===== Check argument =====
	if (object!=NULL && key!=NULL && keyLength>0 && json!=NULL)
		{
		//===== In the case of 1st node =====
		if (this_isEmpty(object)==true)
			{
			//===== Get root node =====
			if ((object=M2MJSON_getRootObject(object))!=NULL)
				{
				//===== Get hash table =====
				if ((table=this_getObjectTable(object))!=NULL)
					{
					// do nothing
					}
				//===== In the case of not existing hash table =====
				else
					{
					memset(MESSAGE, 0, sizeof(MESSAGE));
					M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Argument \"JSONObject\"(whose key is \"%s\") hasn't \"JSONObjectTable\" object", M2MJSON_getKey(object));
					this_printDebugMessage(METHOD_NAME, __LINE__, MESSAGE);
					//===== Create new hash table =====
					if ((table=this_createNewJSONObjectTable())!=NULL)
						{
						this_setObjectTable(object, table);
						}
					//===== Error handling =====
					else
						{
						this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to allocate new memory for creating \"JSONObjectTable\" object");
						return NULL;
						}
					}
				//===== Set key string and hash =====
				this_setKey(object, key, keyLength);
				//===== Set JSON as value =====
				object->json = json;
				//===== Set hash table =====
				index = this_getObjectTableIndex(M2MJSON_getHash(object));
				table->table[index] = object;
				this_increaseObjectTableCounter(table);
				memset(MESSAGE, 0, sizeof(MESSAGE));
				M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Set hash table in index(=%d) generated with key(= \"%s\" )", index, key);
				this_printDebugMessage(METHOD_NAME, __LINE__, MESSAGE);
				return object;
				}
			//===== Error handling =====
			else
				{
				this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to get root node of \"JSONObject\"");
				return NULL;
				}
			}
		//===== In the case of there are existing nodes =====
		else if ((table=this_getObjectTable(object))!=NULL)
			{
			//===== In the case of existing a node whose key is same =====
			if ((existingNode=M2MJSON_searchObject(object, key, keyLength))!=NULL)
				{
				//===== Update JSON value =====
				M2MJSON_delete(&(existingNode->json));
				existingNode->json = json;
				return existingNode;
				}
			//===== In the case of existing a node has same index =====
			else if ((existingNode=this_searchObjectTable(object, key, keyLength))!=NULL)
				{
				//===== Create new node =====
				newNode = M2MJSON_createNewObject();
				//===== Set key =====
				this_setKey(newNode, key, keyLength);
				//===== Set value =====
				newNode->json = json;
				//===== Set connection =====
				previous = M2MJSON_getLastObject(object);
				M2MJSON_setNextObject(previous, newNode);
				M2MJSON_setPreviousObject(newNode, previous);
				M2MJSON_setNextObject(newNode, NULL);
				//===== Set hash table =====
				this_setObjectTable(newNode, table);
				existingNode = this_getSameIndexEnd(existingNode);
				this_setSameIndexNext(existingNode, newNode);
				this_setSameIndexPrevious(newNode, existingNode);
				this_setSameIndexNext(newNode, NULL);
				memset(MESSAGE, 0, sizeof(MESSAGE));
				M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Set hash table in same index(=%d) generated with key(= \"%s\" )", this_getObjectTableIndex(M2MJSON_getHash(newNode)), key);
				this_printDebugMessage(METHOD_NAME, __LINE__, MESSAGE);
				return newNode;
				}
			//=====  =====
			else
				{
				//===== Create new node =====
				newNode = M2MJSON_createNewObject();
				//===== Set key =====
				this_setKey(newNode, key, keyLength);
				//===== Set value =====
				newNode->json = json;
				//===== Set connection =====
				previous = M2MJSON_getLastObject(object);
				M2MJSON_setNextObject(previous, newNode);
				M2MJSON_setPreviousObject(newNode, previous);
				M2MJSON_setNextObject(newNode, NULL);
				//===== Set hash table =====
				this_setObjectTable(newNode, table);
				index = this_getObjectTableIndex(M2MJSON_getHash(newNode));
				table->table[index] = newNode;
				this_increaseObjectTableCounter(table);
				memset(MESSAGE, 0, sizeof(MESSAGE));
				M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Set hash table in index(=%d) generated with key(= \"%s\" )", index, key);
				this_printDebugMessage(METHOD_NAME, __LINE__, MESSAGE);
				return newNode;
				}
			}
		//===== Error handling =====
		else
			{
			this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSONObject\" hasn't \"JSONObjectTable\" object");
			return NULL;
			}
		}
	//===== Argument error =====
	else if (object==NULL)
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSONObject\" is NULL");
		return NULL;
		}
	else if (key==NULL)
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"key\" is NULL");
		return NULL;
		}
	else if (keyLength<=0)
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"keyLength\" isn't positive");
		return NULL;
		}
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSON\" is NULL");
		return NULL;
		}
	}


/**
 * @param[in,out] array
 * @param[in] next
 * @return
 */
M2MJSONArray *M2MJSON_setNextArray (M2MJSONArray *array, M2MJSONArray *next)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON_setNextArray()";

	//===== Check argument =====
	if (array!=NULL)
		{
		array->next = next;
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"array\" is NULL");
		}
	return array;
	}


/**
 * @param[in,out] object
 * @param[in] next
 * @return
 */
M2MJSONObject *M2MJSON_setNextObject (M2MJSONObject *object, M2MJSONObject *next)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON_setNextObject()";

	//===== Check argument =====
	if (object!=NULL)
		{
		object->next = next;
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"object\" is NULL");
		}
	return object;
	}


/**
 * @param array
 * @param previous
 * @return
 */
M2MJSONArray *M2MJSON_setPreviousArray (M2MJSONArray *array, M2MJSONArray *previous)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON_setPreviousArray()";

	//===== Check argument =====
	if (array!=NULL && previous!=NULL)
		{
		array->previous = previous;
		}
	//===== Argument error =====
	else if (array==NULL)
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"array\" is NULL");
		}
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"previous\" is NULL");
		}
	return array;
	}


/**
 * @param object
 * @param previous
 * @return
 */
M2MJSONObject *M2MJSON_setPreviousObject (M2MJSONObject *object, M2MJSONObject *previous)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON_setPreviousObject()";

	//===== Check argument =====
	if (object!=NULL && previous!=NULL)
		{
		object->previous = previous;
		}
	//===== Argument error =====
	else if (object==NULL)
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"object\" is NULL");
		}
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"previous\" is NULL");
		}
	return object;
	}


/**
 * @param[in,out] self
 * @param[in] type
 * @return
 */
M2MJSON *M2MJSON_setType (M2MJSON *self, const M2MJSONType type)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON_setType()";

	//===== Check argument =====
	if (self!=NULL)
		{
		self->type = type;
		return self;
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSON\" is NULL");
		return NULL;
		}
	}


/**
 * This method translates from indicated JSON object to JSON format string.<br>
 *
 * @param[in] self		JSON structure object
 * @param[out] string	buffer for copying JSON format string(Caution! this variable is allocated in this method, so caller must release!)
 * @return				JSON format string or NULL(means error)
 */
M2MString *M2MJSON_toString (M2MJSON *self, M2MString **buffer)
	{
	//========== Variable ==========
	M2MJSONType type;
	const M2MString *METHOD_NAME = (M2MString *)"M2MJSON_toString()";

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Get JSON type =====
		if ((type=M2MJSON_getType(self))==M2MJSONType_ARRAY)
			{
			return this_toM2MStringFromArray(M2MJSON_getArray(self), buffer);
			}
		else if (type==M2MJSONType_BOOLEAN)
			{
			return this_toM2MStringFromBoolean(self, buffer);
			}
		else if (type==M2MJSONType_NULL)
			{
			this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Type of Argument \"JSON\" is \"M2MJSONType_NULL\"");
			return NULL;
			}
		else if (type==M2MJSONType_NUMBER)
			{
			return this_toM2MStringFromNumber(self, buffer);
			}
		else if (type==M2MJSONType_OBJECT)
			{
			return this_toM2MStringFromObject(M2MJSON_getObject(self), buffer);
			}
		else if (type==M2MJSONType_STRING)
			{
			return this_toStringFromString(self, buffer);
			}
		//===== Error handling =====
		else
			{
			this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Type of Argument \"JSON\" is invalid");
			return NULL;
			}
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument \"JSON\" is NULL");
		return NULL;
		}
	}


/* End Of File */
