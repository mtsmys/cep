/*******************************************************************************
 * M2MJSONPointer.c : Implementation of JSON Pointer(RFC6901) in C
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

#include "m2m/lib/json/M2MJSONPointer.h"


/*******************************************************************************
 * Declaration
 ******************************************************************************/
/**
 * This method fetches key string from indicated JSON Pointer path string.<br>
 *
 * @param[in] path			JSON Pointer expression path string
 * @param[in] keyLength		length of key[Byte]
 * @param[out] buffer		buffer for copying fetched key string
 * @param[in] bufferLength	length of buffer[Byte]
 * @return					pointer of fetched key string or NULL(means error)
 */
static M2MString *this_fetchKeyFromPath (const M2MString *path, const size_t keyLength, M2MString *buffer, const size_t bufferLength);



/*******************************************************************************
 * Private method
 ******************************************************************************/
/**
 * This function detects a JSON object with indicated path.<br>
 *
 * @param[in,out] object	JSON structure object
 * @param[in] path			JSON Pointer expression path string
 * @return					detected JSON object or NULL(means error)
 */
static M2MJSON *this_detectJSON (M2MJSON *json, M2MString *path)
	{
	//========== Variable ==========
	M2MJSONObject *object = NULL;
	M2MJSONArray *array = NULL;
	M2MString *index = NULL;
	int number = 0;
	M2MString MESSAGE[256];
	const size_t PATH_LENGTH = M2MString_length(path);
	M2MString key[PATH_LENGTH+1];
#ifdef DEBUG
	M2MString *jsonString = NULL;
#endif // DEBUG

	//===== Check argument =====
	if (json!=NULL && path!=NULL && PATH_LENGTH>0)
		{
#ifdef DEBUG
		M2MJSON_toString(json, &jsonString);
		memset(MESSAGE, 0, sizeof(MESSAGE));
		M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Now starts to detect a JSON from this JSON(=\"%s\")", jsonString);
		NGLogger_printDebugMessage((M2MString *)"M2MJSONPointer.this_detectJSON()", __LINE__, MESSAGE);
		NGHeap_free(jsonString);
		memset(MESSAGE, 0, sizeof(MESSAGE));
		M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Now starts to detect a JSON with indicated path(=\"%s\")", path);
		NGLogger_printDebugMessage((M2MString *)"M2MJSONPointer.this_detectJSON()", __LINE__, MESSAGE);
#endif // DEBUG
		//===== Initialization =====
		memset(key, 0, sizeof(key));
		//===== Increment position =====
		if (M2MString_compare(path, M2MString_SLASH, M2MString_length(M2MString_SLASH))==0
				&& (path+=M2MString_length(M2MString_SLASH))!=NULL)
			{
			//===== Loop while existing a value =====
			while ((index=M2MString_indexOf(path, M2MString_SLASH))!=NULL)
				{
				//===== Fetch key string =====
				if (this_fetchKeyFromPath(path, M2MString_length(path)-M2MString_length(index), key, sizeof(key))!=NULL)
					{
					//===== In the case of JSON Array =====
					if (M2MJSON_getType(json)==M2MJSONType_ARRAY
							&& (array=M2MJSON_getArray(json))!=NULL
							&& (array=M2MJSON_getRootArray(array))!=NULL
							&& (number=M2MString_convertFromStringToSignedInteger(key, M2MString_length(key)))>=0)
						{
						//===== Select JSON value from JSON Array =====
						if ((json=M2MJSON_getJSONFromArray(array, number))!=NULL)
							{
#ifdef DEBUG
							memset(MESSAGE, 0, sizeof(MESSAGE));
							M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Detected JSONArray with indicated key(=\"%d\")", number);
							NGLogger_printDebugMessage((M2MString *)"M2MJSONPointer.this_detectJSON()", __LINE__, MESSAGE);
#endif // DEBUG
							}
						//===== Error handling =====
						else
							{
							memset(MESSAGE, 0, sizeof(MESSAGE));
							M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Failed to detect JSON with indicated expression(=\"%s\")", path);
							NGLogger_printErrorMessage((M2MString *)"M2MJSONPointer.this_detectJSON()", __LINE__, MESSAGE);
							return NULL;
							}
						}
					//===== In the case of JSON Object =====
					else if (M2MJSON_getType(json)==M2MJSONType_OBJECT
							&& (object=M2MJSON_getObject(json))!=NULL)
						{
						//===== Get value(=JSON) from key =====
						if ((json=M2MJSON_getJSONFromObject(object, key, M2MString_length(key)))!=NULL)
							{
#ifdef DEBUG
							memset(MESSAGE, 0, sizeof(MESSAGE));
							M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Detected JSONObject with indicated key(=\"%s\")", key);
							NGLogger_printDebugMessage((M2MString *)"M2MJSONPointer.this_detectJSON()", __LINE__, MESSAGE);
#endif // DEBUG
							}
						//===== Error handling =====
						else
							{
							memset(MESSAGE, 0, sizeof(MESSAGE));
							M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Failed to detect JSON with indicated expression(=\"%s\")", path);
							NGLogger_printErrorMessage((M2MString *)"M2MJSONPointer.this_detectJSON()", __LINE__, MESSAGE);
							return NULL;
							}
						}
					//===== Error handling =====
					else
						{
						NGLogger_printErrorMessage((M2MString *)"M2MJSONPointer.this_detectJSON()", __LINE__, (M2MString *)"Argument \"JSON\" doesn't own \"JSONObject\" or \"JSONArray\" as \"JSONValue\"");
						return NULL;
						}
					//===== Proceed string position for next key =====
					index += M2MString_length(M2MString_SLASH);
					path = index;
					}
				//===== Error handling =====
				else
					{
					memset(MESSAGE, 0, sizeof(MESSAGE));
					M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Failed to fetch key string from path(=\"%s\")", path);
					NGLogger_printErrorMessage((M2MString *)"M2MJSONPointer.this_detectJSON()", __LINE__, MESSAGE);
					return NULL;
					}
				}
			//===== Fetch key string =====
			if (this_fetchKeyFromPath(path, M2MString_length(path), key, sizeof(key))!=NULL)
				{
				//===== In the case of JSON Array =====
				if (M2MJSON_getType(json)==M2MJSONType_ARRAY
						&& (array=M2MJSON_getArray(json))!=NULL
						&& (array=M2MJSON_getRootArray(array))!=NULL
						&& (number=M2MString_convertFromStringToSignedInteger(key, M2MString_length(key)))>=0)
					{
					//===== Select JSON value from JSON Array =====
					if ((json=M2MJSON_getJSONFromArray(array, number))!=NULL)
						{
#ifdef DEBUG
						memset(MESSAGE, 0, sizeof(MESSAGE));
						M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Detected JSONArray with indicated key(=\"%d\")", number);
						NGLogger_printDebugMessage((M2MString *)"M2MJSONPointer.this_detectJSON()", __LINE__, MESSAGE);
#endif // DEBUG
						return json;
						}
					//===== Error handling =====
					else
						{
						NGLogger_printErrorMessage((M2MString *)"M2MJSONPointer.this_detectJSON()", __LINE__, (M2MString *)"Failed to detect JSON by argument \"expression\"");
						return NULL;
						}
					}
				//===== In the case of JSON Object =====
				else if (M2MJSON_getType(json)==M2MJSONType_OBJECT
						&& (object=M2MJSON_getObject(json))!=NULL)
					{
					//===== Get value(=JSON) from key =====
					if ((json=M2MJSON_getJSONFromObject(object, key, M2MString_length(key)))!=NULL)
						{
#ifdef DEBUG
						memset(MESSAGE, 0, sizeof(MESSAGE));
						M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Detected JSONObject with indicated key(=\"%s\")", key);
						NGLogger_printDebugMessage((M2MString *)"M2MJSONPointer.this_detectJSON()", __LINE__, MESSAGE);
#endif // DEBUG
						return json;
						}
					//===== Error handling =====
					else
						{
						NGLogger_printErrorMessage((M2MString *)"M2MJSONPointer.this_detectJSON()", __LINE__, (M2MString *)"Failed to detect JSON by argument \"expression\"");
						return NULL;
						}
					}
				//===== Error handling =====
				else
					{
					NGLogger_printErrorMessage((M2MString *)"M2MJSONPointer.this_detectJSON()", __LINE__, (M2MString *)"Argument \"JSON\" doesn't own \"JSONObject\" or \"JSONArray\" as \"JSONValue\"");
					return NULL;
					}
				}
			//===== Error handling =====
			else
				{
				memset(MESSAGE, 0, sizeof(MESSAGE));
				M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Failed to fetch key string from path(=\"%s\")", path);
				NGLogger_printErrorMessage((M2MString *)"M2MJSONPointer.this_detectJSON()", __LINE__, MESSAGE);
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			NGLogger_printErrorMessage((M2MString *)"M2MJSONPointer.this_detectJSON()", __LINE__, (M2MString *)"Failed to detect JSON because of invalid format");
			return NULL;
			}
		}
	//===== Argument error =====
	else if (json==NULL)
		{
		NGLogger_printErrorMessage((M2MString *)"M2MJSONPointer.this_detectJSON()", __LINE__, (M2MString *)"Argument \"JSON\" is NULL");
		return NULL;
		}
	else
		{
		NGLogger_printErrorMessage((M2MString *)"M2MJSONPointer.this_detectJSON()", __LINE__, (M2MString *)"Argument \"path\" is NULL or vacant");
		return NULL;
		}
	}


/**
 * This method fetches key string from indicated JSON Pointer path string.<br>
 *
 * @param[in] path			JSON Pointer expression path string
 * @param[in] keyLength		length of key[Byte]
 * @param[out] buffer		buffer for copying fetched key string
 * @param[in] bufferLength	length of buffer[Byte]
 * @return					pointer of fetched key string or NULL(means error)
 */
static M2MString *this_fetchKeyFromPath (const M2MString *path, const size_t keyLength, M2MString *buffer, const size_t bufferLength)
	{
	//========== Variable ==========
#ifdef DEBUG
	M2MString MESSAGE[256];
#endif // DEBUG

	//===== Check argument =====
	if (path!=NULL
			&& 0<keyLength && keyLength<=M2MString_length(path)
			&& keyLength<=bufferLength)
		{
		memset(buffer, 0, bufferLength);
		memcpy(buffer, path, keyLength);
#ifdef DEBUG
		memset(MESSAGE, 0, sizeof(MESSAGE));
		M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Fetched key string(=\"%s\") from path", buffer);
		NGLogger_printDebugMessage((M2MString *)"M2MJSONPointer.this_fetchKeyFromPath()", __LINE__, MESSAGE);
#endif // DEBUG
		return buffer;
		}
	//===== Argument error =====
	else if (path==NULL)
		{
		NGLogger_printErrorMessage((M2MString *)"M2MJSONPointer.this_fetchKeyFromPath()", __LINE__, (M2MString *)"Argument \"path\" is NULL");
		return NULL;
		}
	else if (keyLength<=0 || M2MString_length(path)<keyLength)
		{
		NGLogger_printErrorMessage((M2MString *)"M2MJSONPointer.this_fetchKeyFromPath()", __LINE__, (M2MString *)"Argument \"keyLength\" isn't positive or overs length of \"path\" string");
		return NULL;
		}
	else
		{
		NGLogger_printErrorMessage((M2MString *)"M2MJSONPointer.this_fetchKeyFromPath()", __LINE__, (M2MString *)"Argument \"keyLength\" overs \"bufferLength\"");
		return NULL;
		}
	}



/*******************************************************************************
 * Public method
 ******************************************************************************/
/**
 * This method decodes pathname string.<br>
 * Kinds of decoding are follow.<br>
 * - "~1" -> "/"<br>
 * - "~0" -> "~"<br>
 * - ""/../.."" -> "/../.."<br>
 * <br>
 * [CAUTION}<br>
 * If you use the return value of this as an argument of M2MJSONPointer_evaluate(),<br>
 * it's a mistake. The decoding process is valuable for internal function.<br>
 *
 * @param[in] path			JSON Pointer expression path string
 * @param[out] buffer		buffer for copying decoded pathname string
 * @param[in] bufferLength	length of buffer[Byte]
 * @return					pointer of decoded pathname string or NULL(means error)
 */
M2MString *M2MJSONPointer_decodePath (const M2MString *path, M2MString *buffer, const size_t bufferLength)
	{
	//========== Variable ==========
	const size_t PATH_LENGTH = M2MString_length(path);
	M2MString DECODED_SLASH_PATH[PATH_LENGTH+1];
	M2MString DECODED_TILDE_PATH[PATH_LENGTH+1];
	size_t decodedTildePathLength = 0;
	M2MString MESSAGE[256];
	const size_t DOUBLE_QUOTATION_LENGTH = M2MString_length(M2MString_DOUBLE_QUOTATION);

	//===== Check argument =====
	if (path!=NULL && PATH_LENGTH>0 && bufferLength>0)
		{
#ifdef DEBUG
		memset(MESSAGE, 0, sizeof(MESSAGE));
		M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Argument \"path\" = \"%s\"", path);
		NGLogger_printDebugMessage((M2MString *)"M2MJSONPointer_decodePath()", __LINE__, MESSAGE);
#endif // DEBUG
		//===== Decode pathname expression(1st: "~1"->"/", and 2nd: "~0"->"~") =====
		if (M2MString_replace(path, M2MJSONPointer_SLASH, M2MString_SLASH, DECODED_SLASH_PATH, sizeof(DECODED_SLASH_PATH))!=NULL
				&& M2MString_replace(DECODED_SLASH_PATH, M2MJSONPointer_TILDE, M2MString_TILDE, DECODED_TILDE_PATH, sizeof(DECODED_TILDE_PATH))!=NULL)
			{
#ifdef DEBUG
			memset(MESSAGE, 0, sizeof(MESSAGE));
			M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Decoded pathname string = \"%s\"", DECODED_TILDE_PATH);
			NGLogger_printDebugMessage((M2MString *)"M2MJSONPointer_decodePath()", __LINE__, MESSAGE);
#endif // DEBUG
			//===== Remove double quotation =====
			if ((decodedTildePathLength=M2MString_length(DECODED_TILDE_PATH))>0
					&& M2MString_compare(DECODED_TILDE_PATH, M2MString_DOUBLE_QUOTATION, DOUBLE_QUOTATION_LENGTH)==0
					&& M2MString_compare(&(DECODED_TILDE_PATH[decodedTildePathLength-1]), M2MString_DOUBLE_QUOTATION, DOUBLE_QUOTATION_LENGTH)==0
					&& bufferLength>(decodedTildePathLength-DOUBLE_QUOTATION_LENGTH-DOUBLE_QUOTATION_LENGTH))
				{
				//===== Copy decoded path =====
				memset(buffer, 0, bufferLength);
				memcpy(buffer, &(DECODED_TILDE_PATH[DOUBLE_QUOTATION_LENGTH]), decodedTildePathLength-DOUBLE_QUOTATION_LENGTH-DOUBLE_QUOTATION_LENGTH);
#ifdef DEBUG
				memset(MESSAGE, 0, sizeof(MESSAGE));
				M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Removed double quotation pathname string = \"%s\"", buffer);
				NGLogger_printDebugMessage((M2MString *)"M2MJSONPointer_decodePath()", __LINE__, MESSAGE);
#endif // DEBUG
				return buffer;
				}
			//===== Remove double quotation(not including back slash) =====
			else if ((decodedTildePathLength=M2MString_length(DECODED_TILDE_PATH))>0
					&& bufferLength>decodedTildePathLength)
				{
				//===== Copy decoded path =====
				memset(buffer, 0, bufferLength);
				memcpy(buffer, &(DECODED_TILDE_PATH[0]), decodedTildePathLength);
				return buffer;
				}
			//===== Error handling =====
			else
				{
				memset(MESSAGE, 0, sizeof(MESSAGE));
				M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Format of argument \"path\"(=\"%s\") is invalid", path);
				NGLogger_printErrorMessage((M2MString *)"M2MJSONPointer_decodePath()", __LINE__, MESSAGE);
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			NGLogger_printErrorMessage((M2MString *)"M2MJSONPointer_decodePath()", __LINE__, (M2MString *)"Failed to decode escape string");
			return NULL;
			}
		}
	//===== Argument error =====
	else if (path==NULL || PATH_LENGTH<=0)
		{
		NGLogger_printErrorMessage((M2MString *)"M2MJSONPointer_decodePath()", __LINE__, (M2MString *)"Argument \"path\" is NULL or vacant");
		return NULL;
		}
	else
		{
		NGLogger_printErrorMessage((M2MString *)"M2MJSONPointer_decodePath()", __LINE__, (M2MString *)"Argument \"bufferLength\" isn't positive");
		return NULL;
		}
	}


/**
 * This method evaluates JSON object with indicated path string. <br>
 * This expression is comma-delimited form in the current version, <br>
 * for example "/hoge/one/fact".<br>
 *
 * @param[in] json		JSON structure object
 * @param[in] path		JSON Pointer expression path string
 * @return				JSON object correspond with path or NULL(means error)
 */
M2MJSON *M2MJSONPointer_evaluate (M2MJSON *json, const M2MString *path)
	{
	//========== Variable ==========
	const size_t PATH_LENGTH = M2MString_length(path);
	M2MString DECODED_PATH[PATH_LENGTH+1];
#ifdef DEBUG
	M2MString MESSAGE[256];
#endif // DEBUG

	//===== Check argument =====
	if (json!=NULL && path!=NULL && PATH_LENGTH>0)
		{
		//===== In the case of selected all JSON objects =====
		if (M2MString_compare(path, M2MJSONPointer_WHOLE_SELECTION, M2MString_length(M2MJSONPointer_WHOLE_SELECTION))==0)
			{
			return json;
			}
		//===== Check path expression =====
		else if (M2MJSONPointer_validatePathExpression(path)==true
				&& M2MJSONPointer_decodePath(path, DECODED_PATH, sizeof(DECODED_PATH))!=NULL)
			{
#ifdef DEBUG
			memset(MESSAGE, 0, sizeof(MESSAGE));
			M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Now evaluate indicated path(=\"%s\")", DECODED_PATH);
			NGLogger_printDebugMessage((M2MString *)"M2MJSONPointer_evaluate()", __LINE__, MESSAGE);
#endif // DEBUG
			return this_detectJSON(json, DECODED_PATH);
			}
		//===== Error handling =====
		else
			{
			NGLogger_printErrorMessage((M2MString *)"M2MJSONPointer_evaluate()", __LINE__, (M2MString *)"Argument \"path\" doesn't start with \"/\" character");
			return NULL;
			}
		}
	//===== Argument error =====
	else if (json==NULL)
		{
		NGLogger_printErrorMessage((M2MString *)"M2MJSONPointer_evaluate()", __LINE__, (M2MString *)"Argument \"JSON\" is NULL");
		return NULL;
		}
	else
		{
		NGLogger_printErrorMessage((M2MString *)"M2MJSONPointer_evaluate()", __LINE__, (M2MString *)"Argument \"path\" is NULL");
		return NULL;
		}
	}


/**
 * This method checks the string is number type or not.<br>
 *
 * @param string	check target string
 * @return			translated number or -1(means error)
 *
int M2MJSONPointer_isNumber (const M2MString *string)
	{
	//========== Variable ==========
	unsigned int i = 0;
	size_t stringLength = 0;

	//===== Check argument =====
	if (string!=NULL && (stringLength=M2MString_length(string))>0)
		{
		//===== Loop while whole character =====
		for (i=0; i<stringLength; i++)
			{
			//===== Check a character is number type or not =====
			if (M2MString_isNumber(string[i])==true)
				{
				continue;
				}
			//===== Error handling =====
			else
				{
#ifdef DEBUG
				NGLogger_printDebugMessage((M2MString *)"M2MJSONPointer_isNumber()", __LINE__, (M2MString *)"Argument \"string\" includes a character which isn't number type");
#endif // DEBUG
				return -1;
				}
			}
		//===== Convert string to integer =====
		return M2MString_convertFromM2MStringToSignedInteger(string, stringLength);
		}
	//===== Argument error =====
	else
		{
#ifdef DEBUG
		NGLogger_printErrorMessage((M2MString *)"M2MJSONPointer_isNumber()", __LINE__, (M2MString *)"Argument \"string\" is NULL or vacant");
#endif // DEBUG
		return -1;
		}
	}
*/


/**
 * This method validates expression of JSON Pointer path.<br>
 *
 * @param[in] path	JSON Pointer expression path string
 * @return			true : valid expression path, false : invalid expression path
 */
bool M2MJSONPointer_validatePathExpression (const M2MString *path)
	{
	//========== Variable ==========
	M2MString *index = NULL;
	const size_t DOUBLE_QUOTATION_LENGTH = M2MString_length(M2MString_DOUBLE_QUOTATION);
#ifdef DEBUG
	M2MString MESSAGE[256];
#endif // DEBUG

	//===== Check argument =====
	if ((index=(M2MString *)path)!=NULL && M2MString_length(index)>0)
		{
		//===== Check path expression starts with "\"" =====
		if (M2MString_compare(index, M2MString_DOUBLE_QUOTATION, DOUBLE_QUOTATION_LENGTH)==0
				&& (index+=M2MString_length(M2MString_DOUBLE_QUOTATION))!=NULL)
			{
			//===== Check path expression includes "/" =====
			if (M2MString_compare(index, M2MString_SLASH, M2MString_length(M2MString_SLASH))==0
					&& M2MString_compare(&(index[M2MString_length(index)-1]), M2MString_DOUBLE_QUOTATION, DOUBLE_QUOTATION_LENGTH)==0)
				{
#ifdef DEBUG
				memset(MESSAGE, 0, sizeof(MESSAGE));
				M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Argument \"path\"(=%s) is right format", path);
				NGLogger_printDebugMessage((M2MString *)"M2MJSONPointer_validatePathExpression()", __LINE__, MESSAGE);
#endif // DEBUG
				return true;
				}
			//===== Error handling =====
			else
				{
				NGLogger_printErrorMessage((M2MString *)"M2MJSONPointer_validatePathExpression()", __LINE__, (M2MString *)"Argument \"path\" doesn't starts with \"\"/\"");
				return false;
				}
			}
		//===== Error handling =====
		else
			{
			NGLogger_printErrorMessage((M2MString *)"M2MJSONPointer_validatePathExpression()", __LINE__, (M2MString *)"Argument \"path\" doesn't start with \"\"\"");
			return false;
			}
		}
	//===== Argument error =====
	else
		{
		NGLogger_printErrorMessage((M2MString *)"M2MJSONPointer_validatePathExpression()", __LINE__, (M2MString *)"Argument \"path\" is NULL or vacant");
		return false;
		}
	}



/* End Of File */
