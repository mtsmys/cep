/*******************************************************************************
 * M2MJSONParser.c
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

#include "m2m/lib/json/M2MJSONParser.h"


/*******************************************************************************
 * Declaration
 ******************************************************************************/
/**
 * @param[in] string	JSON string
 * @param[out] object
 * @return
 */
static unsigned char *this_setObject (const M2MString *string, M2MJSONObject **object);


/**
 * @param[in] string
 * @param[out] array
 * @return
 */
static unsigned char *this_setArray (M2MString *string, M2MJSONArray **array);



/*******************************************************************************
 * Private method
 ******************************************************************************/
/**
 * @param[in] string
 * @param[out] json
 * @return
 */
static unsigned char *this_createNewJSONBoolean (unsigned char *string, M2MJSON **json)
	{
	//========== Variable ==========
#ifdef DEBUG
	unsigned char MESSAGE[128];
#endif // DEBUG
	const size_t TRUE_LENGTH = M2MString_length((M2MString *)"true");
	const size_t FALSE_LENGTH = M2MString_length((M2MString *)"false");

	//===== Check argument =====
	if (string!=NULL)
		{
		//===== Allocate new memory for JSON =====
		if (((*json)=M2MJSON_new())!=NULL)
			{
			//===== Set JSON data type =====
			M2MJSON_setType((*json), M2MJSONType_BOOLEAN);
			//=====  =====
			if (M2MString_compare(string, (M2MString *)"true", TRUE_LENGTH)==0
					|| M2MString_compare(string, (M2MString *)"TRUE", TRUE_LENGTH)==0)
				{
				(*json)->value->boolean = true;
				string += TRUE_LENGTH;
				}
			//=====  =====
			else if (M2MString_compare(string, (M2MString *)"false", FALSE_LENGTH)==0
					|| M2MString_compare(string, (M2MString *)"FALSE", FALSE_LENGTH)==0)
				{
				(*json)->value->boolean = false;
				string += FALSE_LENGTH;
				}
			//=====  =====
			else
				{
#ifdef DEBUG
				memset(MESSAGE, 0, sizeof(MESSAGE));
				M2MString_format(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Indicated string doesn't start with \"true\" or \"false\"(=\"%s\")", string);
				NGLogger_printErrorMessage((M2MString *)"M2MJSONParser.this_createNewBooleanJSON()", __LINE__, MESSAGE);
#endif // DEBUG
				}
			}
		//===== Error handling =====
		else
			{
#ifdef DEBUG
			NGLogger_printErrorMessage((M2MString *)"M2MJSONParser.this_createNewBooleanJSON()", __LINE__, (M2MString *)"Failed to allocate new structure object for JSON Boolean");
#endif // DEBUG
			}
		}
	//===== Argument error =====
	else
		{
#ifdef DEBUG
		NGLogger_printErrorMessage((M2MString *)"M2MJSONParser.this_createNewBooleanJSON()", __LINE__, (M2MString *)"Argument \"string\" is NULL");
#endif // DEBUG
		}
	//=====  =====
	return string;
	}


/**
 * @param[in] numberM2MString
 * @param[in] numberM2MStringLength
 * @param[out] json
 * @return
 */
static unsigned char *this_createNewJSONNumber (M2MString *numberM2MString, const size_t numberM2MStringLength, M2MJSON **json)
	{
	//========== Variable ==========
	M2MString number[numberM2MStringLength+1];

	//===== Check argument =====
	if (numberM2MString!=NULL && 0<numberM2MStringLength && numberM2MStringLength<=M2MString_length(numberM2MString) && json!=NULL)
		{
		//===== Create new JSON object =====
		if (((*json)=M2MJSON_new())!=NULL)
			{
			//===== Set JSON data type =====
			M2MJSON_setType((*json), M2MJSONType_NUMBER);
			//===== Pick-up number string and convert into number =====
			memset(number, 0, numberM2MStringLength+1);
			memcpy(number, numberM2MString, numberM2MStringLength);
			//===== Set number into new allocated JSON =====
			(*json)->value->number = M2MString_convertFromStringToDouble(number, numberM2MStringLength);
			//===== Return proceeded string =====
			return (numberM2MString += numberM2MStringLength);
			}
		//===== Error handling =====
		else
			{
#ifdef DEBUG
			NGLogger_printErrorMessage((M2MString *)"M2MJSONParser.this_createNewJSONNumber()", __LINE__, (M2MString *)"Failed to create new \"JSON\" structure object");
#endif // DEBUG
			return NULL;
			}
		}
	//===== Argument error =====
	else if (numberM2MString==NULL)
		{
#ifdef DEBUG
		NGLogger_printErrorMessage((M2MString *)"M2MJSONParser.this_createNewJSONNumber()", __LINE__, (M2MString *)"Argument \"numberM2MString\" is NULL");
#endif // DEBUG
		return NULL;
		}
	else if (numberM2MStringLength<=0 || M2MString_length(numberM2MString)<numberM2MStringLength)
		{
#ifdef DEBUG
		NGLogger_printErrorMessage((M2MString *)"M2MJSONParser.this_createNewJSONNumber()", __LINE__, (M2MString *)"Argument \"numberM2MStringLength\" is invalid");
#endif // DEBUG
		return NULL;
		}
	else
		{
#ifdef DEBUG
		NGLogger_printErrorMessage((M2MString *)"M2MJSONParser.this_createNewJSONNumber()", __LINE__, (M2MString *)"Argument \"JSON\" is NULL");
#endif // DEBUG
		return NULL;
		}
	}


/**
 * @param[in] string
 * @param[in] stringLength
 * @param[out] json
 * @return
 */
static M2MString *this_createNewJSONString (M2MString *string, const size_t stringLength, M2MJSON **json)
	{
	//========== Variable ==========
	const size_t DOUBLE_QUOTATION_LENGTH = M2MString_length(M2MString_DOUBLE_QUOTATION);

	//===== Check argument =====
	if (string!=NULL && stringLength>0 && json!=NULL)
		{
		//===== Remove "\"" from string =====
		if (M2MString_compare(string, M2MString_DOUBLE_QUOTATION, DOUBLE_QUOTATION_LENGTH)==0)
			{
			string += DOUBLE_QUOTATION_LENGTH;
			}
		else
			{
			}
		//===== Create new JSON object =====
		if (((*json)=M2MJSON_new())!=NULL)
			{
			//===== Set data type =====
			M2MJSON_setType((*json), M2MJSONType_STRING);
			//===== Allocate new memory for copying value data =====
			if (((*json)->value->string=(M2MString *)NGHeap_malloc(stringLength+1))!=NULL)
				{
				//===== Set data value =====
				memcpy((*json)->value->string, string, stringLength);
				//===== Proceed string position =====
				string += stringLength;
				//=====  =====
				if (M2MString_compare(string, M2MString_DOUBLE_QUOTATION, DOUBLE_QUOTATION_LENGTH)==0)
					{
					string += DOUBLE_QUOTATION_LENGTH;
					}
				//=====  =====
				else
					{
					}
				}
			//===== Error handling =====
			else
				{
#ifdef DEBUG
				NGLogger_printErrorMessage((M2MString *)"M2MJSONParser.this_createNewJSONM2MString()", __LINE__, (M2MString *)"Failed to allocate new memory for copying string");
#endif // DEBUG
				M2MJSON_delete(json);
				}
			}
		//===== Error handling =====
		else
			{
#ifdef DEBUG
			NGLogger_printErrorMessage((M2MString *)"M2MJSONParser.this_createNewJSONM2MString()", __LINE__, (M2MString *)"Failed to allocate new memory for creating \"JSON\" structure object");
#endif // DEBUG
			}
		}
	//===== Argument error =====
	else if (string==NULL)
		{
#ifdef DEBUG
		NGLogger_printErrorMessage((M2MString *)"M2MJSONParser.this_createNewJSONM2MString()", __LINE__, (M2MString *)"Argument \"string\" is NULL");
#endif // DEBUG
		}
	else if (stringLength<=0)
		{
#ifdef DEBUG
		NGLogger_printErrorMessage((M2MString *)"M2MJSONParser.this_createNewJSONM2MString()", __LINE__, (M2MString *)"Argument \"stringLength\" isn't positive");
#endif // DEBUG
		}
	else
		{
#ifdef DEBUG
		NGLogger_printErrorMessage((M2MString *)"M2MJSONParser.this_createNewJSONM2MString()", __LINE__, (M2MString *)"Argument \"M2MJSON **\" is NULL");
#endif // DEBUG
		}
	return string;
	}


/**
 * @param[in] string	parsing target string
 * @return				proceeded string or NULL(means error)
 */
static size_t this_detectNumberM2MStringLength (M2MString *string)
	{
	//========== Variable ==========
	M2MString *index = NULL;
	const size_t ZERO_LENGTH	= M2MString_length((M2MString *)"0");
	const size_t ONE_LENGTH		= M2MString_length((M2MString *)"1");
	const size_t TWO_LENGTH		= M2MString_length((M2MString *)"2");
	const size_t THREE_LENGTH	= M2MString_length((M2MString *)"3");
	const size_t FOUR_LENGTH	= M2MString_length((M2MString *)"4");
	const size_t FIVE_LENGTH	= M2MString_length((M2MString *)"5");
	const size_t SIX_LENGTH		= M2MString_length((M2MString *)"6");
	const size_t SEVEN_LENGTH	= M2MString_length((M2MString *)"7");
	const size_t EIGHT_LENGTH	= M2MString_length((M2MString *)"8");
	const size_t NINE_LENGTH	= M2MString_length((M2MString *)"9");
	const size_t MINUS_LENGTH	= M2MString_length((M2MString *)"-");
	const size_t DOT_LENGTH		= M2MString_length(M2MString_DOT);

	//===== Check argument =====
	if ((index=string)!=NULL)
		{
		//=====  =====
		while (index!=NULL
				&& (M2MString_compare(index, (M2MString *)"0", ZERO_LENGTH)==0
						|| M2MString_compare(index, (M2MString *)"1", ONE_LENGTH)==0
						|| M2MString_compare(index, (M2MString *)"2", TWO_LENGTH)==0
						|| M2MString_compare(index, (M2MString *)"3", THREE_LENGTH)==0
						|| M2MString_compare(index, (M2MString *)"4", FOUR_LENGTH)==0
						|| M2MString_compare(index, (M2MString *)"5", FIVE_LENGTH)==0
						|| M2MString_compare(index, (M2MString *)"6", SIX_LENGTH)==0
						|| M2MString_compare(index, (M2MString *)"7", SEVEN_LENGTH)==0
						|| M2MString_compare(index, (M2MString *)"8", EIGHT_LENGTH)==0
						|| M2MString_compare(index, (M2MString *)"9", NINE_LENGTH)==0
						|| M2MString_compare(index, (M2MString *)"-", MINUS_LENGTH)==0
						|| M2MString_compare(index, M2MString_DOT, DOT_LENGTH)==0))
			{
			index++;
			}
		return M2MString_length(string)-M2MString_length(index);
		}
	//===== Argument error =====
	else
		{
#ifdef DEBUG
		NGLogger_printErrorMessage((M2MString *)"M2MJSONParser.this_detectNumberM2MString()", __LINE__, (M2MString *)"Argument \"string\" is NULL");
#endif // DEBUG
		return 0;
		}
	}


/**
 * This method fetch key string from indicated JSON string.<br>
 * This method allocate new memory for fetched key string, so caller must<br>
 * release the key string after.<br>
 *
 * @param string	parsing target string
 * @param key		allocated and fetched key string
 * @return			proceeded string
 */
static M2MString *this_getObjectKey (const M2MString *string, M2MString **key)
	{
	//========== Variable ==========
	M2MString *start = NULL;
	M2MString *end = NULL;
	size_t keyLength = 0;
	const size_t DOUBLE_QUOTATION_LENGTH = M2MString_length(M2MString_DOUBLE_QUOTATION);

	//===== Get start point of key =====
	if (string!=NULL && M2MString_length(string)>0 && key!=NULL)
		{
		//===== Get start position of key =====
		if ((start=M2MString_indexOf(string, M2MString_DOUBLE_QUOTATION))!=NULL)
			{
			//===== Increment pointer =====
			start += DOUBLE_QUOTATION_LENGTH;
			//===== Get end point of key =====
			if (start!=NULL && (end=M2MString_indexOf(start, M2MString_DOUBLE_QUOTATION))!=NULL
					&& (keyLength=M2MString_length(start)-M2MString_length(end))>0)
				{
				//===== Get key =====
				if (((*key)=(M2MString *)NGHeap_malloc(keyLength+1))!=NULL)
					{
					//===== Copy key data =====
					memcpy((*key), start, keyLength);
					//===== Increment pointer =====
					end += DOUBLE_QUOTATION_LENGTH;
					return end;
					}
				//===== Error handling =====
				else
					{
#ifdef DEBUG
					NGLogger_printErrorMessage((M2MString *)"M2MJSONParser.this_getObjectKey()", __LINE__, (M2MString *)"Failed to allocate new memory for copying key data");
#endif // DEBUG
					return NULL;
					}
				}
			//===== Error handling =====
			else
				{
#ifdef DEBUG
				NGLogger_printErrorMessage((M2MString *)"M2MJSONParser.this_getObjectKey()", __LINE__, (M2MString *)"Failed to detect end position of key data");
#endif // DEBUG
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
#ifdef DEBUG
			NGLogger_printErrorMessage((M2MString *)"M2MJSONParser.this_getObjectKey()", __LINE__, (M2MString *)"Failed to detect start position of key data");
#endif // DEBUG
			return NULL;
			}
		}
	//===== Argument error =====
	else if (string==NULL)
		{
#ifdef DEBUG
		NGLogger_printErrorMessage((M2MString *)"M2MJSONParser.this_getObjectKey()", __LINE__, (M2MString *)"Argument \"string\" is NULL");
#endif // DEBUG
		return NULL;
		}
	else if (M2MString_length(string)<=0)
		{
#ifdef DEBUG
		NGLogger_printErrorMessage((M2MString *)"M2MJSONParser.this_getObjectKey()", __LINE__, (M2MString *)"Length of argument \"string\" isn't positive");
#endif // DEBUG
		return NULL;
		}
	else
		{
#ifdef DEBUG
		NGLogger_printErrorMessage((M2MString *)"M2MJSONParser.this_getObjectKey()", __LINE__, (M2MString *)"Argument \"key\" is NULL");
#endif // DEBUG
		return NULL;
		}
	}


/**
 *
 */
static bool this_isDecimalNumber (const M2MString *string, const size_t length)
	{
	//============ Variable ============
	unsigned int i = 0;
	const M2MString *MINUS_SIGN = (M2MString *)"-";
	const M2MString *DECIMAL_NUMBER = (M2MString *)"0.";
	const M2MString *MINUS_DECIMAL_NUMBER = (M2MString *)"-0.";

	//======  ======
	if (string!=NULL && 0<length)
		{
		for (i=0; i<length; i++)
			{
			if (i==0)
				{
				if (isdigit(string[i])!=0
						|| M2MString_compare(&string[i], MINUS_SIGN, M2MString_length(MINUS_SIGN))==0)
					{
					continue;
					}
				else
					{
					return false;
					}
				}
			else if (i==1)
				{
				if (isdigit(string[i])!=0
						|| M2MString_compare(string, DECIMAL_NUMBER, M2MString_length(DECIMAL_NUMBER))==0)
					{
					continue;
					}
				else
					{
					return false;
					}
				}
			else if (i==2)
				{
				if (isdigit(string[i])!=0
						|| M2MString_compare(string, MINUS_DECIMAL_NUMBER, M2MString_length(MINUS_DECIMAL_NUMBER))==0)
					{
					continue;
					}
				else
					{
					return false;
					}
				}
			else
				{
				if (isdigit(string[i])!=0)
					{
					continue;
					}
				else
					{
					return false;
					}
				}
			}
		return true;
		}
	//======  ======
	else
		{
		return false;
		}
	}


/**
 * Proceed pointer of string when "\r\n" or "\n" or "\t" or " " exists.<br>
 *
 * @param[in,out] string
 * @return
 */
static unsigned char *this_proceedString (const M2MString *string)
	{
	//========== Variable ==========
	M2MString *index = NULL;
	int zenkakuSpace = 1;
	const unsigned int ZENKAKU_SPACE_LENGTH = M2MString_length(M2MString_ZENKAKU_SPACE);

	//===== Check argument =====
	if (string!=NULL)
		{
		//===== Get index from indicated string =====
		index = (M2MString *)string;
		//===== Loop while existing standard space =====
		while (isspace(*index)!=0
						|| (M2MString_length(index)>=ZENKAKU_SPACE_LENGTH && (zenkakuSpace=M2MString_compare(index, M2MString_ZENKAKU_SPACE, ZENKAKU_SPACE_LENGTH))==0))
			{
			//===== In the case of zenkakuspace =====
			if (zenkakuSpace==0)
				{
				index += ZENKAKU_SPACE_LENGTH;
				zenkakuSpace = 1;
				}
			//===== Other case =====
			else
				{
				//===== Check rest of string =====
				if (M2MString_length(index)>0)
					{
					index++;
					}
				//===== Error handling =====
				else
					{
					return NULL;
					}
				}
			}
		//===== Return proceeded position =====
		return index;
		}
	//===== Argument error =====
	else
		{
		return NULL;
		}
	}


/**
 * @param[in] string
 * @param[in] array
 * @return
 */
static M2MString *this_setArray (M2MString *string, M2MJSONArray **array)
	{
	//========== Variable ==========
	M2MJSON *json = NULL;
	M2MJSONArray *childArray = NULL;
	M2MJSONObject *object = NULL;
	M2MString *index = NULL;
	M2MString *end = NULL;
	unsigned int numberLength = 0;
	const size_t RIGHT_SQUARE_BRACKET_LENGTH = M2MString_length(M2MString_RIGHT_SQUARE_BRACKET);

	//===== Check argument =====
	if (string!=NULL && ((*array)=M2MJSON_createNewArray())!=NULL)
		{
		//===== Set start position of Object =====
		index = M2MString_indexOf(string, M2MString_LEFT_SQUARE_BRACKET);
		index++;
		index = this_proceedString(index);

		//===== Loop while arrived at end character(="}") =====
		while (index!=NULL && M2MString_compare(index, M2MString_RIGHT_SQUARE_BRACKET, RIGHT_SQUARE_BRACKET_LENGTH)!=0)
			{
			//===== Remove space =====
			index = this_proceedString(index);
			//===== In the case of Array =====
			if (M2MString_compare(index, M2MString_LEFT_SQUARE_BRACKET, M2MString_length(M2MString_LEFT_SQUARE_BRACKET))==0)
				{
				//===== Get value(=Array) =====
				index = this_setArray(index, &childArray);
				//===== Set-up JSON =====
				if ((json=M2MJSON_new())!=NULL)
					{
					M2MJSON_setType(json, M2MJSONType_ARRAY);
					json->value->array = childArray;
					//===== Set key & value into Object =====
					M2MJSON_setJSONToArray((*array), json);
					}
				//===== Error handling =====
				else
					{
					}
				}
			//===== In the case of Boolean =====
			else if (M2MString_compare(index, (M2MString *)"true", M2MString_length((M2MString *)"true"))==0 || M2MString_compare(index, (M2MString *)"TRUE", M2MString_length((M2MString *)"TRUE"))==0
					|| M2MString_compare(index, (M2MString *)"false", M2MString_length((M2MString *)"false"))==0 || M2MString_compare(index, (M2MString *)"FALSE", M2MString_length((M2MString *)"FALSE"))==0)
				{
				index = this_createNewJSONBoolean(index, &json);
				M2MJSON_setJSONToArray((*array), json);
				}
			//===== In the case of Number =====
			else if ((numberLength=this_detectNumberM2MStringLength(index))>0
					&& this_isDecimalNumber(index, numberLength)==true)
				{
				index = this_createNewJSONNumber(index, numberLength, &json);
				M2MJSON_setJSONToArray((*array), json);
				}
			//===== In the case of Object =====
			else if (M2MString_compare(index, M2MString_LEFT_CURLY_BRACKET, M2MString_length(M2MString_LEFT_CURLY_BRACKET))==0)
				{
				//===== Get value(=Object) =====
				index = this_setObject(index, &object);
				//===== Set-up JSON =====
				if ((json=M2MJSON_new())!=NULL)
					{
					M2MJSON_setType(json, M2MJSONType_OBJECT);
					json->value->object = object;
					//===== Set key & value into Object =====
					M2MJSON_setJSONToArray((*array), json);
					}
				//===== Error handling =====
				else
					{
					}
				}
			//===== In the case of M2MString =====
			else if (M2MString_compare(index, M2MString_DOUBLE_QUOTATION, M2MString_length(M2MString_DOUBLE_QUOTATION))==0)
				{
				//===== Get end position of M2MString =====
				if ((index=M2MString_indexOf(index, M2MString_DOUBLE_QUOTATION))!=NULL
						&& (index+=M2MString_length(M2MString_DOUBLE_QUOTATION))!=NULL
						&& (end=M2MString_indexOf(index, M2MString_DOUBLE_QUOTATION))!=NULL)
					{
					//===== Get value(=M2MString) =====
					index = this_createNewJSONString(index, M2MString_length(index)-M2MString_length(end), &json);
					//===== Set key & value into Object =====
					M2MJSON_setJSONToArray((*array), json);
					}
				//===== Error handling =====
				else
					{
					}
				}
			//===== Error handling =====
			else
				{
				}
			//=====  =====
			while (index!=NULL
					&& M2MString_compare(index, M2MString_COMMA, M2MString_length(M2MString_COMMA))!=0
					&& M2MString_compare(index, M2MString_RIGHT_SQUARE_BRACKET, M2MString_length(M2MString_RIGHT_SQUARE_BRACKET))!=0)
				{
				index++;
				}
			//=====  =====
			if (M2MString_compare(index, M2MString_COMMA, M2MString_length(M2MString_COMMA))==0)
				{
				index++;
				}
			else
				{
				}
			}
		//=====  =====
		return index;
		}
	//=====  =====
	else
		{
		return NULL;
		}
	}


/**
 * @param [in]string	JSON string which starts with "{" or "\""
 * @param [out]object	JSON Object object
 * @return				proceed string pointer by parsing process
 */
static M2MString *this_setObject (const M2MString *string, M2MJSONObject **object)
	{
	//========== Variable ==========
	M2MJSONObject *childObject = NULL;
	M2MJSONArray *array = NULL;
	M2MJSON *json = NULL;
	M2MString *index = NULL;
	M2MString *end = NULL;
	M2MString *key = NULL;
	unsigned int numberLength = 0;
	const size_t RIGHT_CURLY_BRACKET_LENGTH = M2MString_length(M2MString_RIGHT_CURLY_BRACKET);

	//===== Check argument =====
	if (string!=NULL)
		{
		//===== In the case of not existing M2MJSONObject node =====
		if ((*object)==NULL)
			{
			(*object) = M2MJSON_createNewObject();
			}
		//===== In the case of existing M2MJSONObject node =====
		else
			{
			}
		//===== Set start position of key string =====
		index = M2MString_indexOf(string, M2MString_DOUBLE_QUOTATION);
		//===== Loop while arrived at end character(="}") =====
		while (index!=NULL && M2MString_compare(index, M2MString_RIGHT_CURLY_BRACKET, RIGHT_CURLY_BRACKET_LENGTH)!=0)
			{
			//========== Get key ==========
			if (index!=NULL && (index=this_getObjectKey(index, &key))!=NULL)
				{
				//===== Proceed into value position =====
				index = M2MString_indexOf(index, M2MString_COLON);
				index++;
				index = this_proceedString(index);

				//========== Get value data ==========
				//===== In the case of Array =====
				if (M2MString_compare(index, M2MString_LEFT_SQUARE_BRACKET, M2MString_length(M2MString_LEFT_SQUARE_BRACKET))==0)
					{
					//===== Get value(=Array) =====
					index = this_setArray(index, &array);
					//===== Set-up JSON =====
					if ((json=M2MJSON_new())!=NULL)
						{
						M2MJSON_setType(json, M2MJSONType_ARRAY);
						json->value->array = array;
						//===== Set key & value into Object =====
						(*object) = M2MJSON_setJSONToObject((*object), key, M2MString_length(key), json);
						}
					//===== Error handling =====
					else
						{
						}
					}
				//===== In the case of Boolean =====
				else if (M2MString_compare(index, (M2MString *)"true", M2MString_length((M2MString *)"true"))==0 || M2MString_compare(index, (M2MString *)"TRUE", M2MString_length((M2MString *)"TRUE"))==0
						|| M2MString_compare(index, (M2MString *)"false", M2MString_length((M2MString *)"false"))==0 || M2MString_compare(index, (M2MString *)"FALSE", M2MString_length((M2MString *)"FALSE"))==0)
					{
					index = this_createNewJSONBoolean(index, &json);
					(*object) = M2MJSON_setJSONToObject((*object), key, M2MString_length(key), json);
					}
				//===== In the case of Number =====
				else if ((numberLength=this_detectNumberM2MStringLength(index))>0
						&& this_isDecimalNumber(index, numberLength)==true)
					{
					index = this_createNewJSONNumber(index, numberLength, &json);
					(*object) = M2MJSON_setJSONToObject((*object), key, M2MString_length(key), json);
					}
				//===== In the case of Object =====
				else if (M2MString_compare(index, M2MString_LEFT_CURLY_BRACKET, M2MString_length(M2MString_LEFT_CURLY_BRACKET))==0)
					{
					//===== Get value(=Object) =====
					index = this_setObject(index, &childObject);
					//===== Set-up JSON =====
					if ((json=M2MJSON_new())!=NULL)
						{
						M2MJSON_setType(json, M2MJSONType_OBJECT);
						json->value->object = childObject;
						//===== Set key & value into Object =====
						(*object) = M2MJSON_setJSONToObject((*object), key, M2MString_length(key), json);
						}
					//===== Error handling =====
					else
						{
						}
					}
				//===== In the case of M2MString =====
				else if (M2MString_compare(index, M2MString_DOUBLE_QUOTATION, M2MString_length(M2MString_DOUBLE_QUOTATION))==0)
					{
					//===== Get end position of M2MString =====
					if ((index=M2MString_indexOf(index, M2MString_DOUBLE_QUOTATION))!=NULL
							&& (index+=M2MString_length(M2MString_DOUBLE_QUOTATION))!=NULL
							&& (end=M2MString_indexOf(index, M2MString_DOUBLE_QUOTATION))!=NULL)
						{
						//===== Get value(=M2MString) =====
						index = this_createNewJSONString(index, M2MString_length(index)-M2MString_length(end), &json);
						//===== Set key & value into Object =====
						(*object) = M2MJSON_setJSONToObject((*object), key, M2MString_length(key), json);
						}
					//===== Error handling =====
					else
						{
						}
					}
				//===== Error handling =====
				else
					{
					}
				//===== Release allocated memory for key string =====
				NGHeap_free(key);
				}
			//===== Error handling =====
			else
				{
				return index;
				}
			//===== Loop while not existing "," or "}" or NULL =====
			while (index!=NULL
					&& M2MString_compare(index, M2MString_COMMA, M2MString_length(M2MString_COMMA))!=0
					&& M2MString_compare(index, M2MString_RIGHT_CURLY_BRACKET, M2MString_length(M2MString_RIGHT_CURLY_BRACKET))!=0)
				{
				//===== Go to next character =====
				index++;
				}
			//===== In the case of including ","  =====
			if (M2MString_compare(index, M2MString_COMMA, M2MString_length(M2MString_COMMA))==0)
				{
				//===== Set start position of key string for next Object node =====
				index = M2MString_indexOf(index, M2MString_DOUBLE_QUOTATION);
				}
			else
				{
				}
			}
		//=====  =====
		return index;
		}
	//===== Argument error =====
	else
		{
		return NULL;
		}
	}


/**
 * @param[in] string
 * @return
 */
static bool this_validateObjectSyntax (const M2MString *string)
	{
	//========== Variable ==========
	int counter = 0;
	M2MString *index = NULL;

	//===== Check argument =====
	if (string!=NULL && M2MString_length(string)>0)
		{
		//===== Check start position of JSON Object =====
		if ((index=M2MString_indexOf(string, M2MString_LEFT_CURLY_BRACKET))!=NULL)
			{
			//===== Count number of JSON Object =====
			counter++;
			//===== Check existence of key =====
			if (index++!=NULL
					&& (index=M2MString_indexOf(index, M2MString_DOUBLE_QUOTATION))!=NULL
					&& index++!=NULL
					&& (index=M2MString_indexOf(index, M2MString_DOUBLE_QUOTATION))!=NULL
					&& index++!=NULL
					&& (index=M2MString_indexOf(index, M2MString_COLON))!=NULL)
				{
				//===== Check start position of JSON Object =====
				while ((index=M2MString_indexOf(index, M2MString_LEFT_CURLY_BRACKET))!=NULL)
					{
					//===== Count number of JSON Object =====
					counter++;
					//===== Check existence of key =====
					if (index++!=NULL
							&& (index=M2MString_indexOf(index, M2MString_DOUBLE_QUOTATION))!=NULL
							&& index++!=NULL
							&& (index=M2MString_indexOf(index, M2MString_DOUBLE_QUOTATION))!=NULL
							&& index++!=NULL
							&& (index=M2MString_indexOf(index, M2MString_COLON))!=NULL)
						{
						}
					//===== Error handling =====
					else
						{
						return false;
						}
					}
				//===== Check end position of JSON Object =====
				if ((index=M2MString_indexOf(string, M2MString_RIGHT_CURLY_BRACKET))!=NULL)
					{
					counter--;
					index++;
					//===== Check end position of JSON Object =====
					while ((index=M2MString_indexOf(index, M2MString_RIGHT_CURLY_BRACKET))!=NULL)
						{
						counter--;
						index++;
						}
					//===== Check number of bracket pair =====
					if (counter==0)
						{
						return true;
						}
					//===== Error handling =====
					else
						{
						return false;
						}
					}
				//=====  =====
				else
					{
					return false;
					}
				}
			//===== Error handling =====
			else
				{
				return false;
				}
			}
		//=====  =====
		else
			{
			return false;
			}
		}
	else
		{
		return false;
		}
	}



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
M2MJSON *M2MJSONParser_parseFile (const M2MString *filePath)
	{
	//========== Variable ==========
	M2MJSON *json = NULL;
	M2MFile *file = NULL;
	M2MString *string = NULL;
	size_t stringLength = 0;

	//===== Check argument =====
	if (filePath!=NULL && M2MString_length(filePath)>0)
		{
		//===== Create new File object =====
		if ((file=M2MFile_new(filePath))!=NULL)
			{
			//===== Open file =====
			if (M2MFile_open(file)!=NULL)
				{
				//===== Parse JSON string =====
				if ((stringLength=M2MFile_read(file, &string))>0
						&& (json=M2MJSONParser_parseString(string))!=NULL)
					{
					//===== Release allocated memory =====
					NGHeap_free(string);
					//===== Close file =====
					M2MFile_delete(&file);
					return json;
					}
				//===== Error handling =====
				else if (stringLength<=0)
					{
#ifdef DEBUG
					NGLogger_printErrorMessage((M2MString *)"M2MJSONParser_parseFile()", __LINE__, (M2MString *)"Failed to read file");
#endif // DEBUG
					//===== Close file =====
					M2MFile_delete(&file);
					return NULL;
					}
				else
					{
#ifdef DEBUG
					NGLogger_printErrorMessage((M2MString *)"M2MJSONParser_parseFile()", __LINE__, (M2MString *)"Failed to parse JSON string");
#endif // DEBUG
					//===== Release allocated memory =====
					NGHeap_free(string);
					//===== Close file =====
					M2MFile_delete(&file);
					return NULL;
					}
				}
			//===== Error handling =====
			else
				{
#ifdef DEBUG
				NGLogger_printErrorMessage((M2MString *)"M2MJSONParser_parseFile()", __LINE__, (M2MString *)"Failed to open file");
#endif // DEBUG
				//===== Close file =====
				M2MFile_delete(&file);
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			NGLogger_printErrorMessage((M2MString *)"M2MJSONParser_parseFile()", __LINE__, (M2MString *)"Failed to create new File object");
			return NULL;
			}
		}
	//===== Argument error =====
	else
		{
#ifdef DEBUG
		NGLogger_printErrorMessage((M2MString *)"M2MJSONParser_parseFile()", __LINE__, (M2MString *)"Argument \"filePath\" is NULL or vacant");
#endif // DEBUG
		return NULL;
		}
	}


/**
 * This method parses JSON string and create new JSON structure object.<br>
 *
 * @param[in] string	UTF-8 character string in the form of JSON
 * @return 				JSON structure object or NULL(means error)
 */
M2MJSON *M2MJSONParser_parseString (const M2MString *string)
	{
	//========== Variable ==========
	M2MJSON *json = NULL;
	M2MJSONObject *object = NULL;
	M2MJSONObject *lastObject = NULL;
	M2MJSONArray *array = NULL;
	M2MJSONArray *lastArray = NULL;
	M2MString *index = NULL;

	//===== Check argument =====
	if ((index=this_proceedString(string))!=NULL && (json=M2MJSON_new())!=NULL)
		{
		//===== In the case of Object =====
		if (M2MString_compare(index, M2MString_LEFT_CURLY_BRACKET, M2MString_length(M2MString_LEFT_CURLY_BRACKET))==0
				&& this_validateObjectSyntax(index)==true)
			{
			//===== Create new JSON Object =====
			index = this_setObject(index, &object);
			//===== Connect Object into new network =====
			M2MJSON_setType(json, M2MJSONType_OBJECT);
			json->value->object = object;
			//===== Get rid of trash =====
			index = this_proceedString(index);
			//===== (*index)!=NTBS is very important condition! =====
			while (index!=NULL && (*index)!=M2MString_NTBS)
				{
				//===== Detect "\"" for start point of key =====
				if (M2MString_compare(index, M2MString_DOUBLE_QUOTATION, M2MString_length(M2MString_DOUBLE_QUOTATION))==0)
					{
					//===== Create new JSON Object =====
					index = this_setObject(index, &object);
					//===== Connect Object into existing network =====
					lastObject = M2MJSON_getLastObject(M2MJSON_getObject(json));
					M2MJSON_setNextObject(lastObject, object);
					M2MJSON_setPreviousObject(object, lastObject);
					M2MJSON_setNextObject(object, NULL);
					//===== Get rid of trash =====
					index = this_proceedString(index);
					}
				//===== In the case of having next Object yet =====
				else if (M2MString_compare(index, M2MString_COMMA, M2MString_length(M2MString_COMMA))==0)
					{
					index += M2MString_length(M2MString_COMMA);
					index = this_proceedString(index);
					}
				//===== Go to next character =====
				else
					{
					index++;
					}
				}
			return json;
			}
		//===== In the case of Array =====
		else if (M2MString_compare(index, M2MString_LEFT_SQUARE_BRACKET, M2MString_length(M2MString_LEFT_SQUARE_BRACKET))==0)
			{
			//===== Create new JSON Array =====
			index = this_setArray(index, &array);
			//===== Connect Array into new network =====
			M2MJSON_setType(json, M2MJSONType_ARRAY);
			json->value->array = array;
			//===== Get rid of trash =====
			index = this_proceedString(index);
			//===== (*index)!=NTBS is very important condition! =====
			while (index!=NULL && (*index)!=M2MString_NTBS)
				{
				if (M2MString_compare(index, M2MString_LEFT_SQUARE_BRACKET, M2MString_length(M2MString_LEFT_SQUARE_BRACKET))==0)
					{
					//===== Create new JSON Array =====
					index = this_setArray(index, &array);
					//===== Connect Object into existing network =====
					lastArray = M2MJSON_getLastArray(M2MJSON_getArray(json));
					M2MJSON_setNextArray(lastArray, array);
					M2MJSON_setPreviousArray(array, lastArray);
					M2MJSON_setNextArray(array, NULL);
					//===== Get rid of trash =====
					index = this_proceedString(index);
					}
				else if (M2MString_compare(index, M2MString_COMMA, M2MString_length(M2MString_COMMA))==0)
					{
					index += M2MString_length(M2MString_COMMA);
					index = this_proceedString(index);
					}
				else
					{
					index++;
					}
				}
			return json;
			}
		//===== Error handling =====
		else
			{
			return NULL;
			}
		}
	//===== Error handling =====
	else if (string==NULL || index==NULL)
		{
		return NULL;
		}
	else
		{
		return NULL;
		}
	}


/**
 * This method check the indicated string is JSON format or not.<br>
 *
 * @param[in] string	string in the form of JSON
 * @return				true : valid form, false : invalid form
 */
bool M2MJSONParser_validateJSONSyntax (const M2MString *string)
	{
	//========== Variable ==========
	M2MJSON *json = NULL;

	//===== Check argument =====
	if (string!=NULL && M2MString_length(string)>0)
		{
		//===== Parse indicated string =====
		if ((json=M2MJSONParser_parseString(string))!=NULL)
			{
			//===== Release allocated memory =====
			M2MJSON_delete(&json);
			return true;
			}
		//===== Error handling =====
		else
			{
#ifdef DEBUG
			NGLogger_printErrorMessage((M2MString *)"M2MJSONParser_validateJSONSyntax()", __LINE__, (M2MString *)"Argument \"string\" is invalid JSON form");
#endif // DEBUG
			return false;
			}
		}
	//===== Argument error =====
	else
		{
#ifdef DEBUG
		NGLogger_printErrorMessage((M2MString *)"M2MJSONParser_validateJSONSyntax()", __LINE__, (M2MString *)"Argument \"string\" is NULL or vacant");
#endif // DEBUG
		return false;
		}
	}



/* End Of File */
