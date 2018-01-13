/*******************************************************************************
 * M2MString.c
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

#include "m2m/lang/M2MString.h"


/*******************************************************************************
 * Private function
 ******************************************************************************/
/**
 * Print out error message to standard error output.<br>
 *
 * @param[in] methodName	String indicating function name
 * @param[in] lineNumber	Line number in source file (can be embedded with "__LINE__")
 * @param[in] message		Message string indicating error content
 */
static void this_printErrorMessage (const M2MString *methodName, const unsigned int lineNumber, const M2MString *message)
	{
	//========== Variable ==========
	const size_t TIME_LENGTH = 128;
	M2MString TIME[TIME_LENGTH];
	const size_t ERROR_MESSAGE_LENGTH = 256;
	M2MString errorMessage[ERROR_MESSAGE_LENGTH];

	//===== Check argument =====
	if (methodName!=NULL && M2MString_length(methodName)>0
			&& message!=NULL && M2MString_length(message)>0)
		{
		//===== Initialize buffer =====
		memset(TIME, 0, TIME_LENGTH);
		memset(errorMessage, 0, ERROR_MESSAGE_LENGTH);
		//===== Get local time string =====
		if (M2MString_getLocalTime(TIME, TIME_LENGTH)>0)
			{
			fprintf(stderr, (M2MString *)"[ERROR]%s %s:%d[l]: %s\n", TIME, methodName, lineNumber, message);
			}
		//===== Error handling =====
		else
			{
			// do nothing
			}
		}
	//===== Argument error =====
	else
		{
		// do nothing
		}
	return;
	}



/*******************************************************************************
 * Public function
 ******************************************************************************/
/**
 * This function adds "string" into after the "self" string.<br>
 * Memory allocation is executed in this function, so caller must release<br>
 * the memory of "self" string.<br>
 *
 * @param[in,out] self	The original string or NULL("self" = "self + string")
 * @param[in] string	additional string
 * @return				Pointer of connected string or NULL (in case of error)
 */
M2MString *M2MString_append (M2MString **self, const M2MString *string)
	{
	//========== Variable ==========
	M2MString *tmp = NULL;
	size_t selfLength = 0;
	size_t stringLength = 0;
	const M2MString *METHOD_NAME = (M2MString *)"M2MString_append()";

	//===== Check argument =====
	if (self!=NULL
			&& string!=NULL && (stringLength=M2MString_length(string))>0)
		{
		//===== In the case of concatenation string existing =====
		if ((*self)!=NULL)
			{
			//===== Preparation for temporarily copying the original string =====
			if( (selfLength=M2MString_length((*self)))>0
				&& (tmp=(M2MString *)M2MHeap_malloc(selfLength+1))!=NULL)
				{
				//===== Temporarily copy the source string =====
				memcpy(tmp, (*self), selfLength);
				//===== Release heap memory of consolidation string =====
				M2MHeap_free((*self));
				//===== Acquire heap memory of concatenated string =====
				if (((*self)=(M2MString *)M2MHeap_malloc(selfLength+stringLength+1))!=NULL)
					{
					//===== Concatenate strings =====
					memcpy(&((*self)[0]), tmp, selfLength);
					memcpy(&((*self)[selfLength]), string, stringLength);
					//===== Release temporary heap memory =====
					M2MHeap_free(tmp);
					//===== Return pointer of concatenated string =====
					return (*self);
					}
				//===== Error handling =====
				else
					{
					M2MHeap_free(tmp);
					this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to get heap memory for copying string into pointer");
					return NULL;
					}
				}
			//===== Error handling =====
			else
				{
				this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to get heap memory for copying string temporary");
				return NULL;
				}
			}
		//===== In the case of not existing of the concatenation string =====
		else
			{
			//===== Acquire heap memory of string copy =====
			if (((*self)=(M2MString *)M2MHeap_malloc(stringLength+1))!=NULL)
				{
				//===== Copy string =====
				memcpy((*self), string, stringLength);
				//===== Returns a pointer to the string =====
				return (*self);
				}
			//===== Error handling =====
			else
				{
				this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to get heap memory for copying string into pointer");
				return NULL;
				}
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"self\" pointer is NULL");
		return NULL;
		}
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"string\" string is NULL or vacant");
		return NULL;
		}
	}


/**
 * Add the string after the "self" string. <br>
 * The length [Byte] of the additional string is specified by argument.<br>
 *
 * @param[in,out] self		The original string to be added to the string (the string after addition is self = self + string)
 * @param[in] string		String to be added
 * @param[in] stringLength	Length of the string to be added[Byte]
 * @return					Pointer of the buffer to which the string was added or NULL (in case of error)
 */
M2MString *M2MString_appendLength (M2MString **self, const M2MString *string, const size_t stringLength)
	{
	//========== Variable ==========
	M2MString *tmp = NULL;
	size_t thisLength = 0;
	const M2MString *METHOD_NAME = (M2MString *)"M2MString_appendLength()";

	//===== Check argument =====
	if (self!=NULL && string!=NULL && 0<stringLength && stringLength<=M2MString_length(string))
		{
		//===== When the string of the concatenation source exists =====
		if ((*self)!=NULL)
			{
			//===== Preparation for temporarily copying the original string =====
			if( (thisLength=M2MString_length((*self)))>0
				&& (tmp=(M2MString *)M2MHeap_malloc(thisLength+1))!=NULL)
				{
				//===== Temporarily copy the source string =====
				memcpy(tmp, (*self), thisLength);
				//===== Rlease heap memory of consolidation source =====
				M2MHeap_free((*self));
				//===== Get heap memory of concatenated string =====
				if (((*self)=(M2MString *)M2MHeap_malloc(thisLength+stringLength+1))!=NULL)
					{
					//===== Concatenate strings =====
					memcpy(&((*self)[0]), tmp, thisLength);
					memcpy(&((*self)[thisLength]), string, stringLength);
					//===== Release temporary buffer heap memory =====
					M2MHeap_free(tmp);
					//===== Return pointer of concatenated string =====
					return (*self);
					}
				//===== Error handling =====
				else
					{
					M2MHeap_free(tmp);
					this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to get heap memory for copying string into pointer");
					return NULL;
					}
				}
			//===== Error handling =====
			else
				{
				this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to get heap memory for copying string temporary");
				return NULL;
				}
			}
		//===== When the string of the concatenation source does not exist =====
		else
			{
			//===== Get heap memory for copying string =====
			if (((*self)=(M2MString *)M2MHeap_malloc(stringLength+1))!=NULL)
				{
				//===== Copy string =====
				memcpy((*self), string, stringLength);
				//===== Return pointer of concatenated string =====
				return (*self);
				}
			//===== Error handling =====
			else
				{
				this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to get heap memory for copying string into pointer");
				return NULL;
				}
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"self\" pointer is NULL");
		return NULL;
		}
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"string\" string is NULL or \"stringLength\" is invalid");
		return NULL;
		}
	}


/**
 * Compares the two strings specified by the argument and returns the result.<br>
 *
 * @param[in] self		The original string to be compared
 * @param[in] string	Another string to be compared
 * @return				0: two are equal, negative: In case of self < string, positive: In case of self > string
 */
signed int M2MString_compareTo (const M2MString *self, const M2MString *string)
	{
	//========== Variable ==========
	size_t selfLength = 0;
	const M2MString *METHOD_NAME = (M2MString *)"M2MString_compareTo()";

	//===== Check argument =====
	if (self!=NULL && (selfLength=M2MString_length(self))>0
			&& string!=NULL)
		{
		return memcmp(self, string, selfLength);
		}
	//===== Argument error =====
	else if (self==NULL || selfLength<=0)
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"self\" string is NULL or vacant");
		return -1;
		}
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"string\" string is NULL or vacant");
		return -1;
		}
	}


/**
 * This method converts character code with "iconv".<br>
 *
 * @param fromString			Conversion target string
 * @param fromCharacterSetName	Character set name of original string
 * @param toCharacterSetName	Convert character set name
 * @param toString				Pointer for copying converted string(buffering is executed in this method)
 * @return						Pointer of converted string or NULL(means error)
 */
M2MString *M2MString_convertCharacterSet (const M2MString *fromString, const M2MString *fromCharacterSetName, const M2MString *toCharacterSetName, M2MString **toString)
	{
	//========== Variable ==========
	iconv_t conversionDescryptor = NULL;
	size_t srcLength = M2MString_length(fromString);
	size_t bufferLength = srcLength * 2;
	M2MString src[srcLength+1];
	M2MString buffer[bufferLength+1];
	M2MString *srcPointer = NULL;
	M2MString *bufferPointer = NULL;

	//===== Check argument =====
	if (fromString!=NULL && srcLength>0 && fromCharacterSetName!=NULL && toCharacterSetName!=NULL && toString!=NULL)
		{
		//===== Initialize buffer =====
		memset(src, 0, sizeof(src));
		memcpy(src, fromString, srcLength);
		memset(buffer, 0, sizeof(buffer));
		//===== Get conversion descriptor =====
		if ((conversionDescryptor=iconv_open(toCharacterSetName, fromCharacterSetName))>0)
			{
			//===== Get pointer for call "iconv" =====
			srcPointer = src;
			bufferPointer = buffer;
			//===== Convert character code =====
			if (iconv(conversionDescryptor, (char **)&srcPointer, &srcLength, (char **)&bufferPointer, &bufferLength)==0)
				{
				//===== Allocate new memory =====
				if ((bufferLength=M2MString_length(buffer))>0
						&& ((*toString)=(M2MString *)M2MHeap_malloc(bufferLength+1))!=NULL)
					{
					//===== Copy converted string =====
					memcpy((*toString), buffer, bufferLength);
					iconv_close(conversionDescryptor);
					return (*toString);
					}
				//===== Error handling =====
				else
					{
					iconv_close(conversionDescryptor);
					return NULL;
					}
				}
			//===== Error handling =====
			else
				{
				iconv_close(conversionDescryptor);
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			return NULL;
			}
		}
	//===== Argument error =====
	else if (fromString==NULL || srcLength<=0)
		{
		return NULL;
		}
	else if (fromCharacterSetName==NULL)
		{
		return NULL;
		}
	else if (toCharacterSetName==NULL)
		{
		return NULL;
		}
	else
		{
		return NULL;
		}
	}


/**
 * Convert double value into a string and copies it to the pointer. <br>
 * Since buffering of arrays is executed inside this function, so call <br>
 * "M2MHeap_free()" function on the caller side in order to prevent memory <br>
 * leak after using the string. <br>
 *
 * @param[in] number	Real number to be converted
 * @param[out] string	Pointer for copying the converted string (buffering is executed inside the function)
 * @return				Copied string or NULL (in case of error)
 */
M2MString *M2MString_convertFromDoubleToString (const double number, M2MString **string)
	{
	//========== Variable ==========
	M2MString tmpBuffer[128];
	size_t stringLength = 0;
	const M2MString *METHOD_NAME = (M2MString *)"M2MString_convertFromDoubleToString()";

	//===== Check argument =====
	if (string!=NULL)
		{
		//===== Initialize array =====
		memset(tmpBuffer, 0, sizeof(tmpBuffer));
		//===== Convert from long to string =====
		if (snprintf(tmpBuffer, sizeof(tmpBuffer), "%f", number)>=0
				&& (stringLength=M2MString_length(tmpBuffer))>0)
			{
			//===== Get heap memory for copying =====
			if (((*string)=(M2MString *)M2MHeap_malloc(stringLength+1))!=NULL)
				{
				memcpy((*string), tmpBuffer, stringLength);
				return (*string);
				}
			//===== Error handling =====
			else
				{
				this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to get heap memory for copying string into pointer");
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to convert double number into string");
			return NULL;
			}
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"string\" pointer is NULL");
		return NULL;
		}
	}


/**
 * M2MString converter from hexadecimal string into long number.<br>
 *
 * @param self			Hexadecimal string
 * @param selfLength	Length of hexadecimal string[Byte]
 * @return				Converted number from hexadecimal string
 */
uint32_t M2MString_convertFromHexadecimalStringToUnsignedLong (const M2MString *self, const size_t selfLength)
	{
	//========== Variable ==========
	uint32_t decimal = 0;
	uint32_t i = 0;
	int16_t n = 0;
	M2MString c;

	//===== Check argument ======
	if (self!=NULL && 0<selfLength && selfLength<=M2MString_length(self))
		{
		//=====  ======
		for (i=0; i<selfLength; i++)
			{
			//=====  ======
			if ('0'<=self[i] && self[i]<='9')
				{
				n = self[i] - '0';
				}
			//=====  ======
			else if ('a'<=(c=tolower(self[i])) && c<='f')
				{
				n = c - 'a' + 10;
				}
			//=====  ======
			else
				{
				}
			//=====  ======
			decimal = decimal *16 + n;
			}
		return decimal;
		}
	//===== Argument error ======
	else
		{
		return 0;
		}
	}


/**
 * Convert the line feed code from LF to CRLF for the argument string.<br>
 *
 * @param[in] self		The original string to convert line feed code
 * @param[out] string	Pointer to store string corrected line feed code (buffering itself is executed inside the function)
 * @return				Pointer of CSV string with corrected line feed code or NULL (in case of error)
 */
M2MString *M2MString_convertFromLFToCRLF (const M2MString *self, M2MString **string)
	{
	//========== Variable ==========
	M2MString *lineStart = NULL;
	M2MString *lineEnd = NULL;
	size_t lineLength = 0;
	const size_t LF_LENGTH = M2MString_length(M2MString_LF);
	const size_t CRLF_LENGTH = M2MString_length(M2MString_CRLF);
	const M2MString *METHOD_NAME = (M2MString *)"M2MString_convertFromLFToCRLF()";

	//===== Check argument =====
	if (self!=NULL && M2MString_length(self)>0
			&& string!=NULL)
		{
		//===== When there is even one CRLF in the string (correction is not executed) =====
		if ((lineEnd=M2MString_indexOf(self, M2MString_CRLF))!=NULL)
			{
			//===== Copy original string as it is =====
			if (M2MString_append(string, self)!=NULL)
				{
				return (*string);
				}
			//===== Error handling =====
			else
				{
				this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to append string");
				return NULL;
				}
			}
		//===== When there is no CRLF in the string (to execute correction) =====
		else
			{
			//===== Get the start position of input data =====
			lineStart = (M2MString *)self;
			//===== Repeat search until LF disappears =====
			while ((lineEnd=M2MString_indexOf(lineStart, M2MString_LF))!=NULL)
				{
				//===== When string data exists in one line =====
				if ((lineLength=M2MString_length(lineStart)-M2MString_length(lineEnd))>0)
					{
					//===== Copy string data =====
					M2MString_appendLength(string, lineStart, lineLength);
					//===== Add CRLF =====
					M2MString_appendLength(string, M2MString_CRLF, CRLF_LENGTH);
					}
				//===== When string data doesn't exist =====
				else
					{
					//===== Add CRLF =====
					M2MString_append(string, M2MString_CRLF);
					}
				//===== Move the pointer to the beginning of the next line =====
				lineEnd += LF_LENGTH;
				lineStart = lineEnd;
				}
			//===== When string data of the last line exists =====
			if ((lineLength=M2MString_length(lineStart))>0)
				{
				//===== Copy string data =====
				M2MString_appendLength(string, lineStart, lineLength);
				}
			//===== When string data of the last line doesn't exist =====
			else
				{
				// do nothing
				}
			//===== Return output data =====
			return (*string);
			}
		}
	//===== Argument error =====
	else if (self==NULL || M2MString_length(self)<=0)
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"self\" string is NULL or vacant");
		return NULL;
		}
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"string\" pointer is NULL");
		return NULL;
		}
	}


/**
 * Convert signed integer value into a string and copies it to the pointer. <br>
 * Since buffering of arrays is executed inside this function, so call <br>
 * "M2MHeap_free()" function on the caller side in order to prevent memory <br>
 * leak after using the string. <br>
 *
 * @param[in] number	Signed integer number to be converted
 * @param[out] string	Pointer for copying the converted string (buffering is executed inside the function)
 * @return				Copied string or NULL (in case of error)
 */
M2MString *M2MString_convertFromSignedIntegerToString (const signed int number, M2MString **string)
	{
	//========== Variable ==========
	M2MString tmpBuffer[128];
	size_t stringLength = 0;
	const M2MString *METHOD_NAME = (M2MString *)"M2MString_convertFromSignedIntegerToString()";

	//===== Check argument =====
	if (string!=NULL)
		{
		//===== Initialize array =====
		memset(tmpBuffer, 0, sizeof(tmpBuffer));
		//===== Convert from long to string =====
		if (snprintf(tmpBuffer, sizeof(tmpBuffer), "%d", number)>=0
				&& (stringLength=M2MString_length(tmpBuffer))>0)
			{
			//===== Get heap memory for copying =====
			if (((*string)=(M2MString *)M2MHeap_malloc(stringLength+1))!=NULL)
				{
				memcpy((*string), tmpBuffer, stringLength);
				return (*string);
				}
			//===== Error handling =====
			else
				{
				this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to get heap memory for copying string into pointer");
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to convert integer number into string");
			return NULL;
			}
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"string\" pointer is NULL");
		return NULL;
		}
	}


/**
 * Convert signed long value into a string and copies it to the pointer. <br>
 * Since buffering of arrays is executed inside this function, so call <br>
 * "M2MHeap_free()" function on the caller side in order to prevent memory <br>
 * leak after using the string. <br>
 *
 * @param[in] number	Signed long number to be converted
 * @param[out] string	Pointer for copying the converted string (buffering is executed inside the function)
 * @return				Copied string or NULL (in case of error)
 */
M2MString *M2MString_convertFromSignedLongToString (const signed long number, M2MString **string)
	{
	//========== Variable ==========
	M2MString tmpBuffer[128];
	size_t stringLength = 0;
	const M2MString *METHOD_NAME = (M2MString *)"M2MString_convertFromSignedLongToString()";

	//===== Check argument =====
	if (string!=NULL)
		{
		//===== Initialize array =====
		memset(tmpBuffer, 0, sizeof(tmpBuffer));
		//===== Convert from long to string =====
		if (snprintf(tmpBuffer, sizeof(tmpBuffer), "%ld", number)>=0
				&& (stringLength=M2MString_length(tmpBuffer))>0)
			{
			//===== Get heap memory for copying =====
			if (((*string)=(unsigned char *)M2MHeap_malloc(stringLength+1))!=NULL)
				{
				memcpy((*string), tmpBuffer, stringLength);
				return (*string);
				}
			//===== Error handling =====
			else
				{
				this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to get heap memory for copying string into pointer");
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to convert long integer number into string");
			return NULL;
			}
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"string\" pointer is NULL");
		return NULL;
		}
	}


/**
 * Convert the argument string to double number.<br>
 *
 * @param[in] string		String indicating double integer
 * @param[in] stringLength	Size of string[Byte]
 * @return					Double converted from string
 */
double M2MString_convertFromStringToDouble (const M2MString *string, const size_t stringLength)
	{
	//========== Variable ==========
	M2MString STRING_ARRAY[stringLength+1];
	const M2MString *METHOD_NAME = (M2MString *)"M2MString_convertFromStringToDouble()";

	//===== Check argument =====
	if (string!=NULL && stringLength<=M2MString_length(string))
		{
		//===== Initialize buffer =====
		memset(STRING_ARRAY, 0, sizeof(STRING_ARRAY));
		//===== Copy string to buffer =====
		memcpy(STRING_ARRAY, string, stringLength);
		//===== Convert string to double number =====
		return strtod(STRING_ARRAY, NULL);
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"string\" string is NULL or stringLength is invalid");
		return (double)-1;
		}
	}


/**
 * Convert the argument string to long integer number.<br>
 *
 * @param[in] string		String indicating signed long integer
 * @param[in] stringLength	Size of string[Byte]
 * @return					Signed long integer converted from string
 */
long M2MString_convertFromStringToLong (const M2MString *string, const size_t stringLength)
	{
	//========== Variable ==========
	M2MString STRING_ARRAY[stringLength+1];
	M2MString MESSAGE[1024];
	const M2MString *METHOD_NAME = (M2MString *)"M2MString_convertFromStringToLong()";

	//===== Check argument =====
	if (string!=NULL && stringLength<=M2MString_length(string))
		{
		//===== Initialize buffer =====
		memset(STRING_ARRAY, 0, sizeof(STRING_ARRAY));
		//===== Copy string to buffer =====
		memcpy(STRING_ARRAY, string, stringLength);
		//===== Convert string to long number =====
		return atoi(STRING_ARRAY);
		}
	//===== Argument error =====
	else if (string==NULL)
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"string\" string is NULL");
		return (int)-1;
		}
	else
		{
		memset(MESSAGE, 0, sizeof(MESSAGE));
		snprintf(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Argument error! Length of indicated \"string\" string (=\"%s\") is shorter than stringLength(=\"%zu\")", string, stringLength);
		this_printErrorMessage(METHOD_NAME, __LINE__, MESSAGE);
		return (int)-1;
		}
	}


/**
 * Convert the argument string to integer number.<br>
 *
 * @param[in] string		String indicating signed integer
 * @param[in] stringLength	Size of string[Byte]
 * @return					Signed integer converted from string
 */
signed int M2MString_convertFromStringToSignedInteger (const M2MString *string, const size_t stringLength)
	{
	//========== Variable ==========
	M2MString buffer[stringLength+1];
	const M2MString *METHOD_NAME = (M2MString *)"M2MString_convertFromStringToSignedInteger()";

	//===== Check argument =====
	if (string!=NULL && stringLength<=M2MString_length(string))
		{
		//===== Initialize buffer =====
		memset(buffer, 0, sizeof(buffer));
		//===== Copy string to buffer =====
		memcpy(buffer, string, stringLength);
		//===== Convert string to signed integer number =====
		return atoi(buffer);
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"string\" string is NULL or stringLength is invalid");
		return (signed int)0;
		}
	}


/**
 * This method convert from string to 64bit integer number.<br>
 *
 * @param[in] string		String indicating signed long
 * @param[in] stringLength	Size of string for convert[Byte]
 * @return					Signed 64bit integer number converted from string
 */
int64_t M2MString_convertFromStringToSignedLongLong (const M2MString *string, const size_t stringLength)
	{
	//========== Variable ==========
	M2MString buffer[stringLength+1];
	const uint32_t BASE = 10;
	const M2MString *METHOD_NAME = (M2MString *)"M2MString_convertFromStringToSignedLongLong()";

	//===== Check argument =====
	if (string!=NULL && M2MString_length(string)>=stringLength)
		{
		//===== Copy string into buffer =====
		memset(buffer, 0, sizeof(buffer));
		memcpy(buffer, string, stringLength);
		//===== Convert string into integer =====
		return (int64_t)strtoll(buffer, NULL, BASE);
		}
	//===== Argument error =====
	else if (string==NULL)
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"string\" string is NULL");
		return (int64_t)0L;
		}
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"stringLength\" is bigger than length of \"string\"");
		return (int64_t)0L;
		}
	}


/**
 * This method converts from unsigned long to string.<br>
 * Generated string is allocated in this method, so caller must free it.<br>
 *
 * @param[in] number		Conversion target number
 * @param[out] buffer		Array for copying integer string
 * @param[in] bufferLength	Length of array[Byte]
 * @return					Converted hexadecimal number string or NULL (in case of error)
 */
M2MString *M2MString_convertFromUnsignedLongToHexadecimalString (const uint32_t number, M2MString *buffer, const size_t bufferLength)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MString_convertFromUnsignedLongToHexadecimalString()";

	//===== Check argument =====
	if (buffer!=NULL && bufferLength>0)
		{
		//===== Initialize temporary buffer =====
		memset(buffer, 0, bufferLength);
		//===== Convert from unsigned long to string =====
		if (M2MString_format(buffer, bufferLength-1, "%lx", number)>0)
			{
			return buffer;
			}
		//===== Error handling =====
		else
			{
			this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to convert unsigned long to hexadecimal number");
			return NULL;
			}
		}
	//===== Argument error =====
	else if (buffer==NULL)
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"buffer\" is NULL");
		return NULL;
		}
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"bufferLength\" isn't positive");
		return NULL;
		}
	}


/**
 * Compare the two strings and return result.<br>
 *
 * @param[in] one		String to be compared
 * @param[in] another	Another string to be compared
 * @param[in] length	Length of string to be compared [byte]
 * @return				true: When the same case, false: When they do not match
 */
bool M2MString_equals (const M2MString *one, const M2MString *another, const size_t length)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MString_equals()";

	//===== Check argument =====
	if (one!=NULL && another!=NULL && length>0)
		{
		//===== When the strings match =====
		if (memcmp(one, another, length)==0)
			{
			return true;
			}
		//===== When the strings don't match =====
		else
			{
			return false;
			}
		}
	//===== Argument error =====
	else if (one==NULL)
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"one\" string is NULL");
		return false;
		}
	else if (another==NULL)
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"another\" string is NULL");
		return false;
		}
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"length\" is invalid");
		return false;
		}
	}


/**
 * This method converts variables into string in the indicated format.<br>
 *
 * @param buffer		Buffer for copying translated strings
 * @param bufferLength	Length of buffer[Byte]
 * @param format		Format for translation into string
 * @return				Length of converted strings[Byte] or -1(means error)
 */
int M2MString_format (M2MString *buffer, const size_t bufferLength, const M2MString *format, ...)
	{
	//========== Variable ==========
	va_list variableList;
	int result = 0;
	const M2MString *METHOD_NAME = (M2MString *)"M2MString_format()";

	//===== Check argument =====
	if (buffer!=NULL && bufferLength>0
			&& format!=NULL && M2MString_length(format)>0)
		{
		va_start(variableList, format);
		result = vsnprintf(buffer, bufferLength, format, variableList);
		va_end(variableList);
		return result;
		}
	//===== Argument error =====
	else if (buffer==NULL)
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"buffer\" is NULL");
		return -1;
		}
	else if (bufferLength<=0)
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"bufferLength\" isn't positive");
		return -1;
		}
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"format\" string is NULL or vacant");
		return -1;
		}
	}


/**
 * Copy string indicating the local calendar time to the "buffer" array. <br>
 * Since buffering is not performed inside this function, so the memory <br>
 * area must be reserved on the caller side. <br>
 *
 * @param[out] buffer		A buffer for copying string indicating the time of the local calendar
 * @param[in] bufferLength	Size of the buffer [byte]
 * @return					Integer indicating the size of the local time character string copied to the buffer [bytes]
 */
unsigned int M2MString_getLocalTime (M2MString *buffer, const size_t bufferLength)
	{
	//========== Variable ==========
	struct timeval currentTime;
	struct tm *localCalendar = NULL;
	M2MString *miliSecond = NULL;
	unsigned int miliSecondLength = 0;
	const M2MString *METHOD_NAME = (M2MString *)"M2MString_getLocalTime()";

	//===== Check argument =====
	if (buffer!=NULL && bufferLength>0)
		{
		//===== Initialize array =====
		memset(buffer, 0, bufferLength);
		//===== Get local calendar time =====
		if (gettimeofday(&currentTime, NULL)==0
				&& (localCalendar=localtime(&(currentTime.tv_sec)))!=NULL)
			{
			//===== Convert date to string =====
			strftime(buffer, bufferLength-1, "%Y/%m/%d %H:%M:%S:", localCalendar);
			//===== Convert microsecond structure to string =====
			if (M2MString_convertFromSignedLongToString((currentTime.tv_usec/1000), &miliSecond)!=NULL)
				{
				//===== Confirm the length of the converted string =====
				if ((miliSecondLength=M2MString_length(miliSecond))>0
						&& miliSecondLength<(bufferLength-M2MString_length(buffer)-1))
					{
					//===== Copy converted string into buffer =====
					memcpy(&(buffer[M2MString_length(buffer)]), miliSecond, miliSecondLength);
					//===== Release heap memory =====
					M2MHeap_free(miliSecond);
					return M2MString_length(buffer);
					}
				//===== Error handling =====
				else
					{
					//===== Release heap memory =====
					M2MHeap_free(miliSecond);
					this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to copy milliseconds string because buffer size is short");
					return 0;
					}
				}
			//===== Error handling =====
			else
				{
				this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to convert from milliseconds to string");
				return 0;
				}
			}
		//===== Error handling =====
		else
			{
			this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to get local calendar time");
			return 0;
			}
		}
	//===== Argument error =====
	else if (buffer==NULL)
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"buffer\"is NULL");
		return 0;
		}
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"bufferLength\" isn't positive");
		return 0;
		}
	}


/**
 * Returns the pointer that the "keyword" string first appears. <br>
 * If "keyword" string isn't found, returns NULL.
 *
 * @param[in] string	Search target string
 * @param[in] keyword	Detection string
 * @return				Pointer indicating keyword start position or NULL (in case of error)
 */
M2MString *M2MString_indexOf (const M2MString *string, const M2MString *keyword)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MString_indexOf()";

	//===== Check argument =====
	if (string!=NULL && keyword!=NULL)
		{
		//===== Return the result of search =====
		return (M2MString *)strstr(string, keyword);
		}
	//===== Argument error =====
	else if (string==NULL)
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"string\" string is NULL");
		return NULL;
		}
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"keyword\" string is NULL");
		return NULL;
		}
	}


/**
 * Returns the pointer that the "keyword" string last appears. <br>
 * If "keyword" string isn't found, returns NULL.
 *
 * @param[in] string	Search target string
 * @param[in] keyword	Detection string
 * @return				Pointer indicating keyword last position or NULL (in case of error)
 */
M2MString *M2MString_lastIndexOf (const M2MString *string, const M2MString *keyword)
	{
	//========== Variable ==========
	M2MString *lastIndex = NULL;
	M2MString *index = (M2MString *)string;
	const M2MString *METHOD_NAME = (M2MString *)"M2MString_lastIndexOf()";

	//===== Check argument =====
	if (string!=NULL && keyword!=NULL)
		{
		//===== Repeat until arriving the last =====
		while ((index=strstr(index, keyword))!=NULL)
			{
			lastIndex = index;
			index++;
			}
		//===== Return the last index =====
		return lastIndex;
		}
	//===== Argument error =====
	else if (string==NULL)
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"string\" string is NULL");
		return NULL;
		}
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"fromIndex\" string is NULL");
		return NULL;
		}
	}


/**
 * Get the length of the string. <br>
 *
 * @param[in] self	String (always be null terminated)
 * @return			Length of string or 0 (in case of error)
 */
size_t M2MString_length (const M2MString *self)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MString_length()";

	//===== Check argument =====
	if (self!=NULL)
		{
		return strlen(self);
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"self\" string is NULL");
		return 0;
		}
	}


/* End Of File */
