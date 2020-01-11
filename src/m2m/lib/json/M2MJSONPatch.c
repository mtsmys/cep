/*******************************************************************************
 * M2MJSONPatch.c : Implementation of JSON Patch(RFC6902) in C
 *
 * Copyright (c) 2016, Akihisa Yasuda
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

#include "m2m/lib/json/M2MJSONPatch.h"


/*******************************************************************************
 * Declaration
 ******************************************************************************/
/**
 * This method removes indicated node with path from JSON object.<br>
 *
 * @param[in,out] json		JSON object
 * @param[in] decodedPath	decoded JSONPointer location pathname
 */
static void this_removeJSONArray (M2MJSON *json, M2MString *decodedPath);


/**
 * This method removes indicated node with path from JSON object.<br>
 *
 * @param[in,out] json		JSON object
 * @param[in] decodedPath	decoded JSONPointer location pathname
 */
static void this_removeJSONObject (M2MJSON *json, M2MString *decodedPath);



/*******************************************************************************
 * Private method
 ******************************************************************************/
/**
 * @param[in,out] object	JSON Object object
 * @param[in] decodedPath	decoded JSONPointer location pathname
 * @return
 */
static M2MJSONObject *this_detectJSONObject (M2MJSONObject *object, M2MString *decodedPath)
	{
	//========== Variable ==========
	M2MJSON *json = NULL;
	M2MString *index = NULL;
	const size_t SLASH_LENGTH = M2MString_length(M2MString_SLASH);
#ifdef DEBUG
	M2MString MESSAGE[256];
#endif // DEBUG

	//===== Check argument =====
	if ((object=M2MJSON_getRootObject(object))!=NULL && decodedPath!=NULL && M2MString_length(decodedPath)>0)
		{
#ifdef DEBUG
		memset(MESSAGE, 0, sizeof(MESSAGE));
		M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Now start to detect JSON Object with pathname(=\"%s\")", decodedPath);
		NGLogger_printDebugMessage((M2MString *)"M2MJSONPatch.this_detectJSONObject()", __LINE__, MESSAGE);
#endif // DEBUG
		//===== Loop while existing expression =====
		while ((index=M2MString_indexOf(decodedPath, M2MString_SLASH))!=NULL)
			{
			//===== Get value(=JSON) from key =====
			if ((json=M2MJSON_getJSONFromObject(object, decodedPath, M2MString_length(decodedPath)-M2MString_length(index)))!=NULL)
				{
				//===== Go to next JSON Object =====
				object = M2MJSON_getObject(json);
				}
			//===== Error handling =====
			else
				{
				NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch.this_detectJSONObject()", __LINE__, (M2MString *)"Failed to detect M2MJSON with argument \"decodedPath\"");
				return NULL;
				}
			//===== Proceed string position for next key =====
			index += SLASH_LENGTH;
			//===== Update "decodedPath" pointer =====
			decodedPath = index;
			}
		//===== Get JSON Object with last key =====
		if ((object=M2MJSON_searchObject(object, decodedPath, M2MString_length(decodedPath)))!=NULL)
			{
#ifdef DEBUG
			memset(MESSAGE, 0, sizeof(MESSAGE));
			M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Detected M2MJSON Object with the last key(=\"%s\")", M2MJSON_getKey(object));
			NGLogger_printDebugMessage((M2MString *)"M2MJSONPatch.this_detectJSONObject()", __LINE__, MESSAGE);
#endif // DEBUG
			return object;
			}
		//===== Error handling =====
		else
			{
			NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch.this_detectJSONObject()", __LINE__, (M2MString *)"Failed to detect M2MJSON Object with the last key string");
			return NULL;
			}
		}
	//===== Argument error =====
	else if (object==NULL)
		{
		NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch.this_detectJSONObject()", __LINE__, (M2MString *)"Failed to get root of M2MJSONObject by argument \"M2MJSONObject\"");
		return NULL;
		}
	else
		{
		NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch.this_detectJSONObject()", __LINE__, (M2MString *)"Argument \"expression\" is NULL or vacant");
		return NULL;
		}
	}


/**
 * This function executes JSON Patch operation with indicated command.<br>
 *
 * @param json	JSON object
 * @param patch	JSON Patch operation
 * @return		Executed result of JSON Patch operation
 */
static M2MJSON *this_executeOperation (M2MJSON *json, const M2MJSON *patch)
	{
	//========== Variable ==========
	M2MJSON *object = NULL;
	M2MString *op = NULL;
	size_t opLength = 0;
	M2MString *path = NULL;
#ifdef DEBUG
	M2MString MESSAGE[256];
#endif // DEBUG

	//===== Check argument =====
	if (json!=NULL && patch!=NULL)
		{
		//===== Get "op" & "path" values =====
		if ((object=M2MJSONPointer_evaluate((M2MJSON *)patch, M2MJSONPatch_OP))!=NULL
				&& M2MJSON_getType(object)==M2MJSONType_STRING
				&& (op=M2MJSON_getString(object))!=NULL
				&& (opLength=M2MString_length(op))>0
				&& (object=M2MJSONPointer_evaluate((M2MJSON *)patch, M2MJSONPatch_PATH))!=NULL
				&& M2MJSON_getType(object)==M2MJSONType_STRING
				&& (path=M2MJSON_getString(object))!=NULL)
			{
#ifdef DEBUG
			memset(MESSAGE, 0, sizeof(MESSAGE));
			M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"JSON Patch operation name = \"%s\"", op);
			NGLogger_printDebugMessage((M2MString *)"M2MJSONPatch.this_executeOperation()", __LINE__, MESSAGE);
			memset(MESSAGE, 0, sizeof(MESSAGE));
			M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"JSON Patch path = \"%s\"", path);
			NGLogger_printDebugMessage((M2MString *)"M2MJSONPatch.this_executeOperation()", __LINE__, MESSAGE);
#endif // DEBUG
			//===== In the case of "add" operation =====
			if (M2MString_compare(op, M2MJSONPatch_ADD, opLength)==0)
				{
				return M2MJSONPatch_add(json, path, M2MJSONPointer_evaluate((M2MJSON *)patch, M2MJSONPatch_VALUE));
				}
			//===== In the case of "copy" operation =====
			else if (M2MString_compare(op, M2MJSONPatch_COPY, opLength)==0)
				{
				return M2MJSONPatch_copy(json, M2MJSON_getString(M2MJSONPointer_evaluate((M2MJSON *)patch, M2MJSONPatch_FROM)), path);
				}
			//===== In the case of "move" operation =====
			else if (M2MString_compare(op, M2MJSONPatch_MOVE, opLength)==0)
				{
				return M2MJSONPatch_move(json, M2MJSON_getString(M2MJSONPointer_evaluate((M2MJSON *)patch, M2MJSONPatch_FROM)), path);
				}
			//===== In the case of "remove" operation =====
			else if (M2MString_compare(op, M2MJSONPatch_REMOVE, opLength)==0)
				{
				M2MJSONPatch_remove(json, path);
				return json;
				}
			//===== In the case of "replace" operation =====
			else if (M2MString_compare(op, M2MJSONPatch_REPLACE, opLength)==0)
				{
				return M2MJSONPatch_replace(json, path, M2MJSONPointer_evaluate((M2MJSON *)patch, M2MJSONPatch_VALUE));
				}
			//===== In the case of "test" operation =====
			else if (M2MString_compare(op, M2MJSONPatch_TEST, opLength)==0)
				{
				if (M2MJSONPatch_test(json, path, M2MJSONPointer_evaluate((M2MJSON *)patch, M2MJSONPatch_VALUE))==true)
					{
					return json;
					}
				//===== Error handling =====
				else
					{
					NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch.this_executeOperation()", __LINE__, (M2MString *)"Failed to execute \"test\" operation");
					return NULL;
					}
				}
			//===== Error handling =====
			else
				{
				NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch.this_executeOperation()", __LINE__, (M2MString *)"Argument \"operation\" string is invalid format");
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch.this_executeOperation()", __LINE__, (M2MString *)"Argument \"operation\" string is invalid format");
			return NULL;
			}
		}
	//===== Argument error =====
	else if (json==NULL)
		{
		NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch.this_executeOperation()", __LINE__, (M2MString *)"Argument \"json\" object is NULL");
		return NULL;
		}
	else
		{
		NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch.this_executeOperation()", __LINE__, (M2MString *)"Argument \"patch\" object is NULL");
		return NULL;
		}
	}


/**
 * This method removes indicated node with path from JSON object.<br>
 *
 * @param[in,out] json		JSON object
 * @param[in] decodedPath	decoded JSON Pointer path expression
 */
static void this_remove (M2MJSON *json, M2MString *decodedPath)
	{
	//========== Variable ==========
	size_t decodedPathLength = 0;
	M2MJSONType type = M2MJSONType_NULL;
#ifdef DEBUG
	M2MString MESSAGE[256];
#endif // DEBUG

	//===== Check argument =====
	if (json!=NULL && decodedPath!=NULL)
		{
		//===== In the case of selected all object =====
		if ((decodedPathLength=M2MString_length(decodedPath))==0)
			{
			//===== Delete whole JSON object =====
			M2MJSON_delete(&json);
#ifdef DEBUG
			memset(MESSAGE, 0, sizeof(MESSAGE));
			M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Removed whole \"JSON\" object belonging with argument \"decodedPath\"(=\"%s\")", decodedPath);
			NGLogger_printDebugMessage((M2MString *)"M2MJSONPatch.this_remove()", __LINE__, MESSAGE);
#endif // DEBUG
			return;
			}
		//===== In the case of single data equals "/" =====
		else if (decodedPathLength==1
				&& M2MString_compare(M2MString_SLASH, decodedPath, decodedPathLength)==0)
			{
			//===== In the case of boolean =====
			if ((type=M2MJSON_getType(json))==M2MJSONType_BOOLEAN)
				{
				M2MJSON_setType(json, M2MJSONType_NULL);
				memset(M2MJSON_getValue(json), 0, sizeof(M2MJSONValue));
#ifdef DEBUG
				memset(MESSAGE, 0, sizeof(MESSAGE));
				M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Removed a JSON boolean belonging with argument \"decodedPath\"(=\"%s\")", decodedPath);
				NGLogger_printDebugMessage((M2MString *)"M2MJSONPatch.this_remove()", __LINE__, MESSAGE);
#endif // DEBUG
				return;
				}
			//===== In the case of null =====
			else if (type==M2MJSONType_NULL)
				{
				M2MJSON_setType(json, M2MJSONType_NULL);
				memset(M2MJSON_getValue(json), 0, sizeof(M2MJSONValue));
#ifdef DEBUG
				memset(MESSAGE, 0, sizeof(MESSAGE));
				M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Removed a JSON null belonging with argument \"decodedPath\"(=\"%s\")", decodedPath);
				NGLogger_printDebugMessage((M2MString *)"M2MJSONPatch.this_remove()", __LINE__, MESSAGE);
#endif // DEBUG
				return;
				}
			//===== In the case of number =====
			else if (type==M2MJSONType_NUMBER)
				{
				M2MJSON_setType(json, M2MJSONType_NULL);
				memset(M2MJSON_getValue(json), 0, sizeof(M2MJSONValue));
#ifdef DEBUG
				memset(MESSAGE, 0, sizeof(MESSAGE));
				M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Removed a JSON number belonging with argument \"decodedPath\"(=\"%s\")", decodedPath);
				NGLogger_printDebugMessage((M2MString *)"M2MJSONPatch.this_remove()", __LINE__, MESSAGE);
#endif // DEBUG
				return;
				}
			//===== In the case of string =====
			else if (type==M2MJSONType_STRING)
				{
				M2MJSON_clearString(json);
#ifdef DEBUG
				memset(MESSAGE, 0, sizeof(MESSAGE));
				M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Removed a JSON string belonging with argument \"decodedPath\"(=\"%s\")", decodedPath);
				NGLogger_printDebugMessage((M2MString *)"M2MJSONPatch.this_remove()", __LINE__, MESSAGE);
#endif // DEBUG
				return;
				}
			//===== Error handling =====
			else
				{
				NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch.this_remove()", __LINE__, (M2MString *)"Argument \"decodedPath\" expression doesn't match with \"JSONType\" of argument \"JSON *\"");
				return;
				}
			}
		//===== In the case of structured data =====
		else
			{
			//===== In the case of array =====
			if ((type=M2MJSON_getType(json))==M2MJSONType_ARRAY)
				{
				this_removeJSONArray(json, decodedPath);
				return;
				}
			//===== In the case of object =====
			else if (type==M2MJSONType_OBJECT)
				{
				this_removeJSONObject(json, decodedPath);
				return;
				}
			else
				{
				NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch.this_remove()", __LINE__, (M2MString *)"Argument \"decodedPath\" expression doesn't match with \"JSONType\" of argument \"JSON *\"");
				return;
				}
			}
		}
	//===== Argument error =====
	else if (json==NULL)
		{
		NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch.this_remove()", __LINE__, (M2MString *)"Argument \"json\" is NULL");
		return;
		}
	else
		{
		NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch.this_remove()", __LINE__, (M2MString *)"Argument \"decodedPath\" is NULL");
		return;
		}
	}


/**
 * This method removes indicated node with path from JSON object.<br>
 *
 * @param[in,out] json		JSON object
 * @param[in] decodedPath	decoded JSON Pointer path expression
 */
static void this_removeJSONArray (M2MJSON *json, M2MString *decodedPath)
	{
	//========== Variable ==========
	M2MString *index = NULL;
	M2MString *key = NULL;
	M2MJSONArray *array = NULL;
	M2MJSONArray *previous = NULL;
	M2MJSONArray *next = NULL;
	uint32_t number = 0;
	uint32_t i = 0;
	M2MString MESSAGE[128];

	//===== Check argument =====
	if (json!=NULL && (array=M2MJSON_getArray(json))!=NULL
			&& decodedPath!=NULL && M2MString_length(decodedPath)>0
			&& M2MString_compare(decodedPath, M2MString_SLASH, M2MString_length(M2MString_SLASH))==0
			&& (decodedPath+=M2MString_length(M2MString_SLASH))!=NULL)
		{
#ifdef DEBUG
		memset(MESSAGE, 0, sizeof(MESSAGE));
		M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Now start to remove a JSON Array with argument \"decodedPath\"(=\"%s\")", decodedPath);
		NGLogger_printDebugMessage((M2MString *)"M2MJSONPatch.this_removeJSONArray()", __LINE__, MESSAGE);
#endif // DEBUG
		//===== In the case of stripping with slash =====
		if ((index=M2MString_indexOf(decodedPath, M2MString_SLASH))!=NULL)
			{
			//=====  =====
			if (M2MString_appendWithLength(&key, decodedPath, M2MString_length(decodedPath)-M2MString_length(index))!=NULL)
				{
				//===== Get index number of JSON array =====
				number = M2MString_convertFromStringToUnsignedInteger(key, M2MString_length(key));
				NGHeap_free(key);
				this_remove(M2MJSON_getJSONFromArray(array, number), index);
				}
			//===== Error handling =====
			else
				{
				NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch.this_removeJSONArray()", __LINE__, (M2MString *)"Failed to fetch index number of array from \"decodedPath\"");
				return;
				}
			}
		//===== In the case of last key =====
		else
			{
			//===== Get index number of JSON array =====
			number = M2MString_convertFromStringToUnsignedInteger(decodedPath, M2MString_length(decodedPath));
			//===== Check index number of JSON array =====
			if ((array=M2MJSON_getRootArray(array))!=NULL)
				{
				//===== Loop while reached target index =====
				for (i=0; i<number; i++)
					{
					//===== Check existence of JSON array =====
					if (M2MJSON_getNextArray(array)!=NULL)
						{
						array = M2MJSON_getNextArray(array);
						}
					//===== Error handling =====
					else
						{
						NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch.this_removeJSONArray()", __LINE__, (M2MString *)"Indicated index number of \"decodedPath\" is invalid");
						return;
						}
					}
				//=====  =====
				if ((previous=M2MJSON_getPreviousArray(array))!=NULL)
					{
					//=====  =====
					if ((next=M2MJSON_getNextArray(array))!=NULL)
						{
						M2MJSON_setNextArray(previous, next);
						M2MJSON_setPreviousArray(next, previous);
						M2MJSON_deleteArray(&array);
						return;
						}
					//=====  =====
					else
						{
						M2MJSON_setNextArray(previous, NULL);
						M2MJSON_deleteArray(&array);
						return;
						}
					}
				//===== Error handling =====
				else
					{
					NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch.this_removeJSONArray()", __LINE__, (M2MString *)"Internal error! Root node of \"JSONArray\" is NULL");
					return;
					}
				}
			//===== Error handling =====
			else
				{
				NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch.this_removeJSONArray()", __LINE__, (M2MString *)"Failed to get root node of \"JSONArray\" object");
				return;
				}
			}
		}
	//===== Argument error =====
	else if (json==NULL)
		{
		NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch.this_removeJSONArray()", __LINE__, (M2MString *)"Argument \"JSON\" is NULL");
		return;
		}
	else if (array==NULL)
		{
		NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch.this_removeJSONArray()", __LINE__, (M2MString *)"Argument \"JSON\" doesn't own \"JSONArray\" object");
		return;
		}
	else
		{
		memset(MESSAGE, 0, sizeof(MESSAGE));
		M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Format of argument \"decodedPath\"(=\"%s\") is invalid", decodedPath);
		NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch.this_removeJSONArray()", __LINE__, MESSAGE);
		return;
		}
	}


/**
 * This method removes indicated node with path from JSON object.<br>
 *
 * @param[in,out] json		JSON object
 * @param[in] decodedPath	decoded JSON Pointer path expression
 */
static void this_removeJSONObject (M2MJSON *json, M2MString *decodedPath)
	{
	//========== Variable ==========
	M2MJSONObject *object = NULL;
	M2MString *index = NULL;
	M2MString MESSAGE[128];

	//===== Check argument =====
	if (json!=NULL
			&& (object=M2MJSON_getObject(json))!=NULL && (object=M2MJSON_getRootObject(object))!=NULL
			&& decodedPath!=NULL && M2MString_length(decodedPath)>0
			&& M2MString_compare(decodedPath, M2MString_SLASH, M2MString_length(M2MString_SLASH))==0
			&& (decodedPath+=M2MString_length(M2MString_SLASH))!=NULL)
		{
#ifdef DEBUG
		memset(MESSAGE, 0, sizeof(MESSAGE));
		M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Now start to remove a JSON Object with argument \"decodedPath\"(=\"%s\")", decodedPath);
		NGLogger_printDebugMessage((M2MString *)"M2MJSONPatch.this_removeJSONObject()", __LINE__, MESSAGE);
#endif // DEBUG
		//=====  =====
		if ((index=M2MString_indexOf(decodedPath, M2MString_SLASH))!=NULL)
			{
			//===== Go to next node =====
			this_remove(M2MJSON_getJSONFromObject(object, decodedPath, M2MString_length(decodedPath)-M2MString_length(index)), index);
			return;
			}
		//=====  =====
		else
			{
			//===== Delete JSON Object =====
			if ((object=this_detectJSONObject(object, decodedPath))!=NULL
					&& (object=M2MJSON_deleteObject(&object))!=NULL)
				{
				//===== In the case of JSON Object =====
				if (M2MJSON_getType(json)==M2MJSONType_OBJECT)
					{
					json->value->object = object;
					return;
					}
				//===== Error handling =====
				else
					{
					NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch.this_removeM2MJSONObject()", __LINE__, (M2MString *)"Unknown error has happened!");
					return;
					}
				}
			//===== In the case of not existing rest JSON Object =====
			else
				{
				json->type = M2MJSONType_NULL;
				memset(json->value, 0, sizeof(M2MJSONValue));
				return;
				}
			}
		}
	//===== Argument error =====
	else if (json==NULL)
		{
		NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch.this_removeJSONObject()", __LINE__, (M2MString *)"Argument \"JSON\" is NULL");
		return;
		}
	else if (object==NULL)
		{
		NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch.this_removeJSONObject()", __LINE__, (M2MString *)"Argument \"JSON\" doesn't own \"M2MJSONObject\" object as \"JSONValue\"");
		return;
		}
	else if (decodedPath==NULL || M2MString_length(decodedPath)<=0)
		{
		NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch.this_removeJSONObject()", __LINE__, (M2MString *)"Argument \"decodedPath\" is NULL or vacant");
		return;
		}
	else
		{
		memset(MESSAGE, 0, sizeof(MESSAGE));
		M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Format of argument \"decodedPath\"(=\"%s\") is invalid", decodedPath);
		NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch.this_removeJSONObject()", __LINE__, MESSAGE);
		return;
		}
	}


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
 * @param[in,out] json	Append target JSON object
 * @param[in] path		JSONPointer location pathname of adding value
 * @param[in] value		a value data object(insertion is shallow)
 * @return				JSON object added value at indicated path or NULL(means error)
 */
M2MJSON *M2MJSONPatch_add (M2MJSON *json, const M2MString *path, const M2MJSON *value)
	{
	//========== Variable ==========
	const size_t PATH_LENGTH = M2MString_length(path);
	M2MString decodedPath[PATH_LENGTH+1];
	M2MString *lastIndex = NULL;
	M2MString parentPath[PATH_LENGTH];
	size_t parentPathLength = 0;
#ifdef DEBUG
	M2MString MESSAGE[256];
#endif // DEBUG

	//===== Check argument =====
	if (json!=NULL && path!=NULL && PATH_LENGTH>0 && value!=NULL)
		{
#ifdef DEBUG
		memset(MESSAGE, 0, sizeof(MESSAGE));
		M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Now execute JSON Patch \"add\" operation");
		NGLogger_printDebugMessage((M2MString *)"M2MJSONPatch_add()", __LINE__, MESSAGE);
#endif // DEBUG
		//===== Initialize copy buffer =====
		memset(parentPath, 0, sizeof(parentPath));
		//===== Decode JSON Pointer path =====
		if (M2MJSONPointer_decodePath(path, decodedPath, sizeof(decodedPath))!=NULL)
			{
			//===== In the case of exchange of root node =====
			if (M2MString_length(decodedPath)==0
					|| (M2MString_length(decodedPath)==1 && M2MString_compare(decodedPath, M2MString_SLASH, M2MString_length(M2MString_SLASH))==0))
				{
				return (json=(M2MJSON *)value);
				}
			//=====  =====
			else if ((lastIndex=M2MString_lastIndexOf(decodedPath, M2MString_SLASH))!=NULL
					&& (parentPathLength=M2MString_length(decodedPath)-M2MString_length(lastIndex))>0
					&& memcpy(&parentPath[0], &decodedPath[0], parentPathLength)!=NULL)
				{
				}
			//===== Error handling =====
			else
				{
				NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch_add()", __LINE__, (M2MString *)"The decode path based on argument \"path\" is invalid style");
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch_add()", __LINE__, (M2MString *)"Failed to decode argument \"path\" for adding JSON object");
			return NULL;
			}
		}
	//===== Argument error =====
	else if (json==NULL)
		{
		NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch_add()", __LINE__, (M2MString *)"Argument \"json\" is NULL");
		return NULL;
		}
	else if (path==NULL || PATH_LENGTH<=0)
		{
		NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch_add()", __LINE__, (M2MString *)"Argument \"path\" is NULL or vacant");
		return NULL;
		}
	else
		{
		NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch_add()", __LINE__, (M2MString *)"Argument \"value\" is NULL");
		return NULL;
		}
	}


/**
 * This method copies the value to the indicated target location.<br>
 *
 * @param[in,out] json	JSON object
 * @param[in] from		JSONPointer location pathname of copying value
 * @param[in] path		JSONPointer location pathname for copying value
 * @return				copied value or NULL(means error)
 */
M2MJSON *M2MJSONPatch_copy (M2MJSON *json, const M2MString *from, const M2MString *path)
	{
	//========== Variable ==========
	const size_t PATH_LENGTH = M2MString_length(path);
	M2MString decodedPath[PATH_LENGTH+1];
#ifdef DEBUG
	M2MString MESSAGE[256];
#endif // DEBUG

	//===== Check argument =====
	if (json!=NULL && path!=NULL && PATH_LENGTH>0)
		{
#ifdef DEBUG
		memset(MESSAGE, 0, sizeof(MESSAGE));
		M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Now execute JSON Patch \"copy\" operation");
		NGLogger_printDebugMessage((M2MString *)"M2MJSONPatch_copy()", __LINE__, MESSAGE);
#endif // DEBUG
		//===== Decode JSON Pointer path =====
		if (M2MJSONPointer_decodePath(path, decodedPath, sizeof(decodedPath))!=NULL)
			{
			}
		//===== Error handling =====
		else
			{
			NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch_copy()", __LINE__, (M2MString *)"Failed to decode argument \"path\" for adding JSON object");
			return NULL;
			}
		}
	//===== Argument error =====
	else if (json==NULL)
		{
		NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch_copy()", __LINE__, (M2MString *)"Argument \"M2MJSON\" is NULL");
		return NULL;
		}
	else
		{
		NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch_copy()", __LINE__, (M2MString *)"Argument \"path\" is NULL or vacant");
		return NULL;
		}
	}


/**
 * This method parses indicated operation and execute patch command.<br>
 *
 * @param json		target JSON object
 * @param operation	patch operation command string
 * @return			patched JSON object
 */
M2MJSON *M2MJSONPatch_evaluate (M2MJSON *json, const M2MString *operation)
	{
	//========== Variable ==========
	size_t operationLength = 0;
	M2MJSON *patch = NULL;
	M2MJSONArray *array = NULL;
	uint32_t arraySize = 0;
	uint32_t i = 0;
#ifdef DEBUG
	M2MString *jsonString = NULL;
	M2MString MESSAGE[512];
#endif // DEBUG

	//===== Check argument =====
	if (json!=NULL && operation!=NULL && (operationLength=M2MString_length(operation))>0)
		{
#ifdef DEBUG
		NGLogger_printDebugMessage((M2MString *)"M2MJSONPatch_evaluate()", __LINE__, (M2MString *)"===== Start-up JSON Evaluation =====");
		M2MJSON_toString(json, &jsonString);
		memset(MESSAGE, 0, sizeof(MESSAGE));
		M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Argument: \"json\"(=%s)", jsonString);
		NGLogger_printDebugMessage((M2MString *)"M2MJSONPatch_evaluate()", __LINE__, MESSAGE);
		NGHeap_free(jsonString);
		memset(MESSAGE, 0, sizeof(MESSAGE));
		M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Argument: \"operation\"(=%s)", operation);
		NGLogger_printDebugMessage((M2MString *)"M2MJSONPatch_evaluate()", __LINE__, MESSAGE);
#endif // DEBUG
		//===== Parse operation string and create new JSON patch object =====
		if ((patch=M2MJSONParser_parseString(operation))!=NULL)
			{
#ifdef DEBUG
			NGLogger_printDebugMessage((M2MString *)"M2MJSONPatch_evaluate()", __LINE__, (M2MString *)"Now execute JSON Patch operation");
#endif // DEBUG
			//===== In the case of JSON Array(=any operations) =====
			if (M2MJSON_getType(patch)==M2MJSONType_ARRAY
					&& (array=M2MJSON_getRootArray(M2MJSON_getArray(patch)))!=NULL
					&& (arraySize=M2MJSON_getArraySize(array))>0)
				{
#ifdef DEBUG
				NGLogger_printDebugMessage((M2MString *)"M2MJSONPatch_evaluate()", __LINE__, (M2MString *)"JSON Patch format is JSON Array(= any operations)");
#endif // DEBUG
				//===== Execute all operation =====
				for (i=0; i<arraySize; i++)
					{
					//===== Execute a operation =====
					if ((json=this_executeOperation(json, M2MJSON_getJSONFromArray(array, i)))!=NULL)
						{
#ifdef DEBUG
						NGLogger_printDebugMessage((M2MString *)"M2MJSONPatch_evaluate()", __LINE__, (M2MString *)"Succeed to a JSON Patch operation");
#endif // DEBUG
						}
					//===== Error handling =====
					else
						{
						NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch_evaluate()", __LINE__, (M2MString *)"Failed to operate JSON Patch command");
						break;
						}
					}
				//===== Release allocated memory =====
				M2MJSON_delete(&patch);
				return json;
				}
			//===== In the case of JSON Object(=1 operation) =====
			else if (M2MJSON_getType(patch)==M2MJSONType_OBJECT)
				{
#ifdef DEBUG
				NGLogger_printDebugMessage((M2MString *)"M2MJSONPatch_evaluate()", __LINE__, (M2MString *)"JSON Patch format is JSON Object(= 1 operation)");
#endif // DEBUG
				//===== Execute a operation =====
				if ((json=this_executeOperation(json, patch))!=NULL)
					{
#ifdef DEBUG
					NGLogger_printDebugMessage((M2MString *)"M2MJSONPatch_evaluate()", __LINE__, (M2MString *)"Succeed to a JSON Patch operation");
#endif // DEBUG
					}
				//===== Error handling =====
				else
					{
					NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch_evaluate()", __LINE__, (M2MString *)"Failed to operate JSON Patch command");
					}
				//===== Release allocated memory =====
				M2MJSON_delete(&patch);
				return json;
				}
			//===== Error handling =====
			else
				{
				NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch_evaluate()", __LINE__, (M2MString *)"Argument \"operation\" string is invalid format");
				//===== Release allocated memory =====
				M2MJSON_delete(&patch);
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch_evaluate()", __LINE__, (M2MString *)"Failed to create new M2MJSON object from argument \"operation\" string");
			return NULL;
			}
		}
	//===== Argument error =====
	else if (json==NULL)
		{
		NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch_evaluate()", __LINE__, (M2MString *)"Argument \"M2MJSON\" is NULL");
		return NULL;
		}
	else
		{
		NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch_evaluate()", __LINE__, (M2MString *)"Argument \"operation\" is NULL or vacant");
		return NULL;
		}
	}


/**
 * This method moves the value to the target location.<br>
 *
 * @param[in,out] json	JSON object
 * @param[in] path		JSONPointer location pathname of moving value
 * @param[in] value		JSONPointer location pathname for moving value
 */
M2MJSON *M2MJSONPatch_move (M2MJSON *json, const M2MString *from, const M2MString *path)
	{
	//========== Variable ==========
	const size_t PATH_LENGTH = M2MString_length(path);
	M2MString decodedPath[PATH_LENGTH+1];
#ifdef DEBUG
	M2MString MESSAGE[256];
#endif // DEBUG

	//===== Check argument =====
	if (json!=NULL && path!=NULL && PATH_LENGTH>0)
		{
#ifdef DEBUG
		memset(MESSAGE, 0, sizeof(MESSAGE));
		M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Now execute JSON Patch \"move\" operation");
		NGLogger_printDebugMessage((M2MString *)"M2MJSONPatch_move()", __LINE__, MESSAGE);
#endif // DEBUG
		//===== Decode JSON Pointer path =====
		if (M2MJSONPointer_decodePath(path, decodedPath, sizeof(decodedPath))!=NULL)
			{
			}
		//===== Error handling =====
		else
			{
			NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch_move()", __LINE__, (M2MString *)"Failed to decode argument \"path\" for adding JSON object");
			return NULL;
			}
		}
	//===== Argument error =====
	else if (json==NULL)
		{
		NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch_move()", __LINE__, (M2MString *)"Argument \"M2MJSON\" is NULL");
		return NULL;
		}
	else
		{
		NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch_move()", __LINE__, (M2MString *)"Argument \"path\" is NULL or vacant");
		return NULL;
		}
	}


/**
 * This method removes a part of JSON object indicated with expression.<br>
 *
 * @param[in,out] json	JSON object
 * @param[in] path		JSONPointer location pathname for removing value
 */
void M2MJSONPatch_remove (M2MJSON *json, const M2MString *path)
	{
	//========== Variable ==========
	const size_t PATH_LENGTH = M2MString_length(path);
	M2MString decodedPath[PATH_LENGTH+1];
#ifdef DEBUG
	M2MString MESSAGE[256];
#endif // DEBUG

	//===== Check argument =====
	if (json!=NULL && path!=NULL && PATH_LENGTH>0)
		{
#ifdef DEBUG
		memset(MESSAGE, 0, sizeof(MESSAGE));
		M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Now execute JSON Patch \"remove\" operation");
		NGLogger_printDebugMessage((M2MString *)"M2MJSONPatch_remove()", __LINE__, MESSAGE);
#endif // DEBUG
		//===== Decode JSON Pointer path =====
		if (M2MJSONPointer_decodePath(path, decodedPath, sizeof(decodedPath))!=NULL)
			{
			this_remove(json, decodedPath);
			return;
			}
		//===== Error handling =====
		else
			{
			NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch_remove()", __LINE__, (M2MString *)"Failed to decode argument \"path\" for removing object");
			return;
			}
		}
	//===== Argument error =====
	else if (json==NULL)
		{
		NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch_remove()", __LINE__, (M2MString *)"Argument \"M2MJSON\" is NULL");
		return;
		}
	else
		{
		NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch_remove()", __LINE__, (M2MString *)"Argument \"path\" is NULL or vacant");
		return;
		}
	}


/**
 * This method replaces the value at the target location with a new value.<br>
 *
 * @param[in,out] json	JSON object
 * @param[in] path		JSONPointer location pathname of replacing value
 * @param[in] value		value data object
 */
M2MJSON *M2MJSONPatch_replace (M2MJSON *json, const M2MString *path, const M2MJSON *value)
	{
	//========== Variable ==========
	const size_t PATH_LENGTH = M2MString_length(path);
	M2MString DECODED_PATH[PATH_LENGTH+1];
#ifdef DEBUG
	M2MString MESSAGE[256];
#endif // DEBUG

	//===== Check argument =====
	if (json!=NULL && path!=NULL && PATH_LENGTH>0 && value!=NULL)
		{
#ifdef DEBUG
		memset(MESSAGE, 0, sizeof(MESSAGE));
		M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Now execute JSON Patch \"replace\" operation");
		NGLogger_printDebugMessage((M2MString *)"M2MJSONPatch_replace()", __LINE__, MESSAGE);
#endif // DEBUG
		//===== Decode JSON Pointer path =====
		if (M2MJSONPointer_decodePath(path, DECODED_PATH, sizeof(DECODED_PATH))!=NULL)
			{
			}
		//===== Error handling =====
		else
			{
			NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch_replace()", __LINE__, (M2MString *)"Failed to decode argument \"path\" for adding JSON object");
			return NULL;
			}
		}
	//===== Argument error =====
	else if (json==NULL)
		{
		NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch_replace()", __LINE__, (M2MString *)"Argument \"M2MJSON\" is NULL");
		return NULL;
		}
	else if (path==NULL || PATH_LENGTH<=0)
		{
		NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch_replace()", __LINE__, (M2MString *)"Argument \"path\" is NULL or vacant");
		return NULL;
		}
	else
		{
		NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch_replace()", __LINE__, (M2MString *)"Argument \"value\" is NULL");
		return NULL;
		}
	}


/**
 * This method tests that a value at the target location is equal to a specified value.<br>
 *
 * @param[in,out] json	JSON object
 * @param[in] path		JSONPointer location pathname of replacing value
 * @param[in] value		value data object
 */
bool M2MJSONPatch_test (M2MJSON *json, const M2MString *path, const M2MJSON *value)
	{
	//========== Variable ==========
	const size_t PATH_LENGTH = M2MString_length(path);
	M2MString DECODED_PATH[PATH_LENGTH+1];
#ifdef DEBUG
	M2MString MESSAGE[256];
#endif // DEBUG

	//===== Check argument =====
	if (json!=NULL && path!=NULL && PATH_LENGTH>0 && value!=NULL)
		{
#ifdef DEBUG
		memset(MESSAGE, 0, sizeof(MESSAGE));
		M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Now execute JSON Patch \"test\" operation");
		NGLogger_printDebugMessage((M2MString *)"M2MJSONPatch_test()", __LINE__, MESSAGE);
#endif // DEBUG
		//===== Decode JSON Pointer path =====
		if (M2MJSONPointer_decodePath(path, DECODED_PATH, sizeof(DECODED_PATH))!=NULL)
			{
			}
		//===== Error handling =====
		else
			{
			NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch_test()", __LINE__, (M2MString *)"Failed to decode argument \"path\" for adding JSON object");
			return false;
			}
		}
	//===== Argument error =====
	else if (json==NULL)
		{
		NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch_test()", __LINE__, (M2MString *)"Argument \"M2MJSON\" is NULL");
		return false;
		}
	else if (path==NULL || PATH_LENGTH<=0)
		{
		NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch_test()", __LINE__, (M2MString *)"Argument \"path\" is NULL or vacant");
		return false;
		}
	else
		{
		NGLogger_printErrorMessage((M2MString *)"M2MJSONPatch_test()", __LINE__, (M2MString *)"Argument \"value\" is NULL");
		return false;
		}
	}



/* End Of File */
