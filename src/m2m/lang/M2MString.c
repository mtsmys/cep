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
 * This method copies local time string into indicated "buffer" memory.<br>
 * Output string format is "yyyy/MM/dd HH:mm:ss.SSS";
 * This method doesn't allocation, so caller needs to prepare memory<br>
 * before call this method.<br>
 *
 * @param[out] buffer		memory buffer for copying local time string
 * @param[in] bufferLength	memory buffer length(max size)
 * @return					length of local time string or 0(means error)
 */
static size_t this_getLocalTimeString (M2MString *buffer, const size_t bufferLength);


/**
 * Initialize "errorno" variable.<br>
 */
static void this_initErrorNumber ();



/*******************************************************************************
 * Private function
 ******************************************************************************/
/**
 * Copy the log message to the argument "buffer" pointer.<br>
 * Buffering of array for copying is executed inside the function.<br>
 * Therefore, it is necessary for caller to call the "M2MHeap_free()" function <br>
 * in order to prevent memory leak after using the variable.<br>
 *
 * @param[in] functionName		String indicating function name
 * @param[in] lineNumber		Line number in source file (can be embedded with "__LINE__")
 * @param[in] message			Message string
 * @param[out] buffer			Buffer to copy the created log message
 * @return						The pointer of "buffer" copied the created log message string or NULL (in case of error)
 */
M2MString *this_createNewLogMessage (const M2MString *functionName, const uint32_t lineNumber, const M2MString *message, M2MString **buffer)
	{
	//========== Variable ==========
	M2MString *logLevelString = (M2MString *)"ERROR";
	M2MString time[64];
	M2MString lineNumberString[16];
	M2MString errnoMessage[256];
	M2MString threadID[128];
	size_t functionNameLength = 0;
	size_t messageLength = 0;

	//===== Check argument =====
	if (functionName!=NULL && (functionNameLength=M2MString_length(functionName))>0
			&& message!=NULL && (messageLength=M2MString_length(message))>0
			&& buffer!=NULL)
		{
		//===== Get line number string =====
		memset(lineNumberString, 0, sizeof(lineNumberString));
		snprintf(lineNumberString, sizeof(lineNumberString)-1, (M2MString *)"%d", lineNumber);
		//===== Initialize array =====
		memset(time, 0, sizeof(time));
		//===== Get current time string from local calendar ======
		if (this_getLocalTimeString(time, sizeof(time))>0
				&& M2MSystem_getThreadIDString(threadID, sizeof(threadID))!=NULL)
			{
			//===== In the case of existing error number =====
			if (errno!=0 && strerror_r(errno, errnoMessage, sizeof(errnoMessage))==0)
				{
				//===== Create new log message string =====
				if (M2MString_append(buffer, M2MString_LEFT_SQUARE_BRACKET)!=NULL
						&& M2MString_append(buffer, time)!=NULL
						&& M2MString_append(buffer, M2MString_RIGHT_SQUARE_BRACKET)!=NULL

						&& M2MString_append(buffer, M2MString_LEFT_SQUARE_BRACKET)!=NULL
						&& M2MString_append(buffer, logLevelString)!=NULL
						&& M2MString_append(buffer, M2MString_RIGHT_SQUARE_BRACKET)!=NULL

						&& M2MString_append(buffer, M2MString_LEFT_SQUARE_BRACKET)!=NULL
						&& M2MString_append(buffer, (M2MString *)"tid=")!=NULL
						&& M2MString_append(buffer, threadID)!=NULL
						&& M2MString_append(buffer, M2MString_RIGHT_SQUARE_BRACKET)!=NULL

						&& M2MString_append(buffer, M2MString_LEFT_SQUARE_BRACKET)!=NULL
						&& M2MString_append(buffer, functionName)!=NULL
						&& M2MString_append(buffer, M2MString_COLON)!=NULL
						&& M2MString_append(buffer, lineNumberString)!=NULL
						&& M2MString_append(buffer, (M2MString *)"l")!=NULL
						&& M2MString_append(buffer, M2MString_RIGHT_SQUARE_BRACKET)!=NULL

						&& M2MString_append(buffer, M2MString_LEFT_SQUARE_BRACKET)!=NULL
						&& M2MString_append(buffer, errnoMessage)!=NULL
						&& M2MString_append(buffer, M2MString_COLON)!=NULL
						&& M2MString_append(buffer, M2MString_SPACE)!=NULL
						&& M2MString_append(buffer, message)!=NULL
						&& M2MString_append(buffer, M2MString_RIGHT_SQUARE_BRACKET)!=NULL
						)
					{
					//===== Initialize error number =====
					this_initErrorNumber();
					//===== Return created log message string =====
					return (*buffer);
					}
				//===== Error handling =====
				else if ((*buffer)!=NULL)
					{
					//===== Release allocated memory =====
					M2MHeap_free((*buffer));
					//===== Initialize error number =====
					this_initErrorNumber();
					return NULL;
					}
				else
					{
					//===== Initialize error number =====
					this_initErrorNumber();
					return NULL;
					}
				}
			//===== In the case of not existing error number =====
			else
				{
				//===== Create new log message string =====
				if (M2MString_append(buffer, M2MString_LEFT_SQUARE_BRACKET)!=NULL
						&& M2MString_append(buffer, time)!=NULL
						&& M2MString_append(buffer, M2MString_RIGHT_SQUARE_BRACKET)!=NULL

						&& M2MString_append(buffer, M2MString_LEFT_SQUARE_BRACKET)!=NULL
						&& M2MString_append(buffer, logLevelString)!=NULL
						&& M2MString_append(buffer, M2MString_RIGHT_SQUARE_BRACKET)!=NULL

						&& M2MString_append(buffer, M2MString_LEFT_SQUARE_BRACKET)!=NULL
						&& M2MString_append(buffer, (M2MString *)"tid=")!=NULL
						&& M2MString_append(buffer, threadID)!=NULL
						&& M2MString_append(buffer, M2MString_RIGHT_SQUARE_BRACKET)!=NULL

						&& M2MString_append(buffer, M2MString_LEFT_SQUARE_BRACKET)!=NULL
						&& M2MString_append(buffer, functionName)!=NULL
						&& M2MString_append(buffer, M2MString_COLON)!=NULL
						&& M2MString_append(buffer, lineNumberString)!=NULL
						&& M2MString_append(buffer, (M2MString *)"l")!=NULL
						&& M2MString_append(buffer, M2MString_RIGHT_SQUARE_BRACKET)!=NULL

						&& M2MString_append(buffer, M2MString_LEFT_SQUARE_BRACKET)!=NULL
						&& M2MString_append(buffer, message)!=NULL
						&& M2MString_append(buffer, M2MString_RIGHT_SQUARE_BRACKET)!=NULL
						)
					{
					//===== Initialize error number =====
					this_initErrorNumber();
					//===== Return created log message string =====
					return (*buffer);
					}
				//===== Error handling =====
				else if ((*buffer)!=NULL)
					{
					//===== Release allocated memory =====
					M2MHeap_free((*buffer));
					//===== Initialize error number =====
					this_initErrorNumber();
					return NULL;
					}
				else
					{
					//===== Initialize error number =====
					this_initErrorNumber();
					return NULL;
					}
				}
			}
		//===== Error handling =====
		else
			{
			//===== Initialize error number =====
			this_initErrorNumber();
			return NULL;
			}
		}
	//===== Argument error =====
	else if (logLevelString==NULL)
		{
		//===== Initialize error number =====
		this_initErrorNumber();
		return NULL;
		}
	else if (functionName==NULL || functionNameLength<=0)
		{
		//===== Initialize error number =====
		this_initErrorNumber();
		return NULL;
		}
	else if (message==NULL || messageLength<=0)
		{
		//===== Initialize error number =====
		this_initErrorNumber();
		return NULL;
		}
	else
		{
		//===== Initialize error number =====
		this_initErrorNumber();
		return NULL;
		}
	}


/**
 * This method copies local time string into indicated "buffer" memory.<br>
 * Output string format is "yyyy/MM/dd HH:mm:ss.SSS";
 * This method doesn't allocation, so caller needs to prepare memory<br>
 * before call this method.<br>
 *
 * @param[out] buffer		memory buffer for copying local time string
 * @param[in] bufferLength	memory buffer length(max size)
 * @return					length of local time string or 0(means error)
 */
static size_t this_getLocalTimeString (M2MString *buffer, const size_t bufferLength)
	{
	//========== Variable ==========
	struct timeval currentTime;
	struct tm *localCalendar = NULL;
	size_t miliSecondLength = 0;
	M2MString *miliSecond = NULL;
	M2MString second[8];
	const M2MString *FORMAT = (M2MString *)"%Y-%m-%d %H:%M:%S.";

	//===== Check argument =====
	if (buffer!=NULL && bufferLength>0)
		{
		//===== Initialize buffer =====
		memset(buffer, 0, bufferLength);
		//===== Get current time =====
		if (gettimeofday(&currentTime, NULL)==0
				&& (localCalendar=localtime(&(currentTime.tv_sec)))!=NULL)
			{
			//===== Convert time to string =====
			strftime(buffer, bufferLength-1, FORMAT, localCalendar);
			//===== Convert millisecond to string =====
			if (M2MString_convertFromSignedLongToString((signed long)(currentTime.tv_usec/1000UL), &miliSecond)!=NULL
					&& (miliSecondLength=M2MString_length(miliSecond))>0)
				{
				//===== In the case of digit number of millisecond is 1 =====
				if (miliSecondLength==1)
					{
					memset(second, 0, sizeof(second));
					//===== Convert millisecond into second format =====
					M2MString_format(second, sizeof(second)-1, (M2MString *)"00%s", miliSecond);
					M2MHeap_free(miliSecond);
					}
				//===== In the case of digit number of millisecond is 2 =====
				else if (miliSecondLength==2)
					{
					memset(second, 0, sizeof(second));
					//===== Convert millisecond into second format =====
					M2MString_format(second, sizeof(second)-1, (M2MString *)"0%s", miliSecond);
					M2MHeap_free(miliSecond);
					}
				//===== In the case of digit number of millisecond is 3 =====
				else if (miliSecondLength==3)
					{
					memset(second, 0, sizeof(second));
					//===== Convert millisecond into second format =====
					M2MString_format(second, sizeof(second)-1, (M2MString *)"%s", miliSecond);
					M2MHeap_free(miliSecond);
					}
				//===== Error handling =====
				else
					{
					//===== Initialize buffer =====
					memset(buffer, 0, bufferLength);
					M2MHeap_free(miliSecond);
					return 0;
					}
				//===== Check buffer length for copying millisecond string =====
				if (M2MString_length(second)<(bufferLength-M2MString_length(buffer)-1))
					{
					//===== Copy millisecond string =====
					memcpy(&(buffer[M2MString_length(buffer)]), second, M2MString_length(second));
					//===== Release allocated memory =====
					M2MHeap_free(miliSecond);
					return M2MString_length(buffer);
					}
				//===== Error handling =====
				else
					{
					//===== Initialize buffer =====
					M2MHeap_free(miliSecond);
					memset(buffer, 0, bufferLength);
					return 0;
					}
				}
			//===== Error handling =====
			else
				{
				//===== Initialize buffer =====
				M2MHeap_free(miliSecond);
				memset(buffer, 0, bufferLength);
				return 0;
				}
			}
		//===== Error handling =====
		else
			{
			return 0;
			}
		}
	//===== Error handling =====
	else
		{
		return 0;
		}
	}


/**
 * Initialize "errorno" variable.<br>
 */
static void this_initErrorNumber ()
	{
	errno = 0;
	return;
	}


/**
 * Print out error message to standard error output.<br>
 *
 * @param[in] functionName	String indicating function name
 * @param[in] lineNumber	Line number in source file (can be embedded with "__LINE__")
 * @param[in] message		Message string indicating error content
 */
static void this_printErrorMessage (const M2MString *functionName, const uint32_t lineNumber, const M2MString *message)
	{
	//========== Variable ==========
	M2MString *logMessage = NULL;

	//===== Create new log message =====
	if (this_createNewLogMessage(functionName, lineNumber, message, &logMessage)!=NULL)
		{
		//===== Print out log =====
		M2MSystem_println(logMessage);
		//===== Release allocated memory =====
		M2MHeap_free(logMessage);
		}
	//===== Error handling =====
	else
		{
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
					this_printErrorMessage(__func__, __LINE__, (M2MString *)"Failed to get heap memory for copying string into pointer");
					return NULL;
					}
				}
			//===== Error handling =====
			else
				{
				this_printErrorMessage(__func__, __LINE__, (M2MString *)"Failed to get heap memory for copying string temporary");
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
				this_printErrorMessage(__func__, __LINE__, (M2MString *)"Failed to get heap memory for copying string into pointer");
				return NULL;
				}
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		this_printErrorMessage(__func__, __LINE__, (M2MString *)"Argument error! Indicated \"self\" pointer is NULL");
		return NULL;
		}
	else
		{
		this_printErrorMessage(__func__, __LINE__, (M2MString *)"Argument error! Indicated \"string\" string is NULL or vacant");
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
					this_printErrorMessage(__func__, __LINE__, (M2MString *)"Failed to get heap memory for copying string into pointer");
					return NULL;
					}
				}
			//===== Error handling =====
			else
				{
				this_printErrorMessage(__func__, __LINE__, (M2MString *)"Failed to get heap memory for copying string temporary");
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
				this_printErrorMessage(__func__, __LINE__, (M2MString *)"Failed to get heap memory for copying string into pointer");
				return NULL;
				}
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		this_printErrorMessage(__func__, __LINE__, (M2MString *)"Argument error! Indicated \"self\" pointer is NULL");
		return NULL;
		}
	else
		{
		this_printErrorMessage(__func__, __LINE__, (M2MString *)"Argument error! Indicated \"string\" string is NULL or \"stringLength\" is invalid");
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
int32_t M2MString_compareTo (const M2MString *self, const M2MString *string)
	{
	//========== Variable ==========
	size_t selfLength = 0;

	//===== Check argument =====
	if (self!=NULL && (selfLength=M2MString_length(self))>0
			&& string!=NULL)
		{
		return memcmp(self, string, selfLength);
		}
	//===== Argument error =====
	else if (self==NULL || selfLength<=0)
		{
		this_printErrorMessage(__func__, __LINE__, (M2MString *)"Argument error! Indicated \"self\" string is NULL or vacant");
		return -1;
		}
	else
		{
		this_printErrorMessage(__func__, __LINE__, (M2MString *)"Argument error! Indicated \"string\" string is NULL or vacant");
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
 * @param[in] boolean
 * @param[out] buffer
 * @param[in] bufferLength
 */
M2MString *M2MString_convertFromBooleanToString (const bool boolean, M2MString *buffer, const size_t bufferLength)
	{
	//========== Variable ==========
	const M2MString *TRUE = (M2MString *)"true";
	const M2MString *FALSE = (M2MString *)"false";

	//===== Check argument =====
	if (bufferLength>=M2MString_length(FALSE))
		{
		//===== In the case of "true" =====
		if (boolean==true)
			{
			//===== Copy "true" string into buffer =====
			memset(buffer, 0, bufferLength);
			M2MString_format(buffer, bufferLength-1, TRUE);
			return buffer;
			}
		//===== In the case of "false" =====
		else
			{
			//===== Copy "false" string into buffer =====
			memset(buffer, 0, bufferLength);
			M2MString_format(buffer, bufferLength-1, FALSE);
			return buffer;
			}
		}
	//===== Argument error =====
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
				this_printErrorMessage(__func__, __LINE__, (M2MString *)"Failed to get heap memory for copying string into pointer");
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			this_printErrorMessage(__func__, __LINE__, (M2MString *)"Failed to convert double number into string");
			return NULL;
			}
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(__func__, __LINE__, (M2MString *)"Argument error! Indicated \"string\" pointer is NULL");
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
				this_printErrorMessage(__func__, __LINE__, (M2MString *)"Failed to append string");
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
		this_printErrorMessage(__func__, __LINE__, (M2MString *)"Argument error! Indicated \"self\" string is NULL or vacant");
		return NULL;
		}
	else
		{
		this_printErrorMessage(__func__, __LINE__, (M2MString *)"Argument error! Indicated \"string\" pointer is NULL");
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
M2MString *M2MString_convertFromSignedIntegerToString (const int32_t number, M2MString **string)
	{
	//========== Variable ==========
	M2MString tmpBuffer[128];
	size_t stringLength = 0;

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
				this_printErrorMessage(__func__, __LINE__, (M2MString *)"Failed to get heap memory for copying string into pointer");
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			this_printErrorMessage(__func__, __LINE__, (M2MString *)"Failed to convert integer number into string");
			return NULL;
			}
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(__func__, __LINE__, (M2MString *)"Argument error! Indicated \"string\" pointer is NULL");
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
				this_printErrorMessage(__func__, __LINE__, (M2MString *)"Failed to get heap memory for copying string into pointer");
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			this_printErrorMessage(__func__, __LINE__, (M2MString *)"Failed to convert long integer number into string");
			return NULL;
			}
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(__func__, __LINE__, (M2MString *)"Argument error! Indicated \"string\" pointer is NULL");
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
		this_printErrorMessage(__func__, __LINE__, (M2MString *)"Argument error! Indicated \"string\" string is NULL or stringLength is invalid");
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
int32_t M2MString_convertFromStringToSignedLong (const M2MString *string, const size_t stringLength)
	{
	//========== Variable ==========
	M2MString STRING_ARRAY[stringLength+1];
	M2MString MESSAGE[1024];

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
		this_printErrorMessage(__func__, __LINE__, (M2MString *)"Argument error! Indicated \"string\" string is NULL");
		return (int)-1;
		}
	else
		{
		memset(MESSAGE, 0, sizeof(MESSAGE));
		snprintf(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Argument error! Length of indicated \"string\" string (=\"%s\") is shorter than stringLength(=\"%zu\")", string, stringLength);
		this_printErrorMessage(__func__, __LINE__, MESSAGE);
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
int32_t M2MString_convertFromStringToSignedInteger (const M2MString *string, const size_t stringLength)
	{
	//========== Variable ==========
	M2MString buffer[stringLength+1];

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
		this_printErrorMessage(__func__, __LINE__, (M2MString *)"Argument error! Indicated \"string\" string is NULL or stringLength is invalid");
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
		this_printErrorMessage(__func__, __LINE__, (M2MString *)"Argument error! Indicated \"string\" string is NULL");
		return (int64_t)0L;
		}
	else
		{
		this_printErrorMessage(__func__, __LINE__, (M2MString *)"Argument error! Indicated \"stringLength\" is bigger than length of \"string\"");
		return (int64_t)0L;
		}
	}



/**
 * This method convert from string to 32bit unsigned integer number.<br>
 *
 * @param[in] string		String indicating signed long
 * @param[in] stringLength	Size of string[Byte]
 * @return					Unsigned 32bit integer number converted from string
 */
uint32_t M2MString_convertFromStringToUnsignedInteger (const M2MString *string, const size_t stringLength)
	{
	//========== Variable ==========
	const uint32_t NUMBER = M2MString_convertFromStringToUnsignedLong(string, stringLength);

	//===== Check size of number =====
	if (NUMBER<=UINT_MAX)
		{
		return (uint32_t)NUMBER;
		}
	//===== Error handling =====
	else
		{
		return (uint32_t)0;
		}
	}


/**
 * This method convert from string to 32bit unsigned long number.<br>
 *
 * @param[in] string		String indicating signed long
 * @param[in] stringLength	Size of string[Byte]
 * @return					32bit unsigned long number converted from string
 */
uint32_t M2MString_convertFromStringToUnsignedLong (const M2MString *string, const size_t stringLength)
	{
	//========== Variable ==========
	M2MString STRING_ARRAY[stringLength+1];
	const uint32_t BASE = (uint32_t)10;

	//===== Check argument =====
	if (string!=NULL && 0<stringLength)
		{
		//===== Copy string into buffer =====
		memset(STRING_ARRAY, 0, sizeof(STRING_ARRAY));
		memcpy(STRING_ARRAY, string, stringLength);
		//===== Convert string into integer =====
		return strtoul(STRING_ARRAY, NULL, BASE);
		}
	//===== Argument error =====
	else
		{
		return (uint32_t)0L;
		}
	}


/**
 * This method converts from 32bit unsigned integer to string.<br>
 * Caller must provide enough buffers as "buffer" argument.<br>
 *
 * @param[in] number		conversion target number
 * @param[out] buffer		Buffer for copying unsigned integer string
 * @param[in] bufferLength	Length of Buffer[Byte]
 */
M2MString *M2MString_convertFromUnsignedIntegerToString (const uint32_t number, M2MString *buffer, const size_t bufferLength)
	{
	//===== Check argument =====
	if (bufferLength>0)
		{
		//===== Initialize temporary buffer =====
		memset(buffer, 0, bufferLength);
		//===== Convert from unsigned integer to string =====
		if (M2MString_format(buffer, bufferLength-1, "%u", number)>0)
			{
			return buffer;
			}
		//===== Error handling =====
		else
			{
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
 * This method converts from unsigned long to string.<br>
 * Caller must provide enough buffers as "buffer" argument.<br>
 *
 * @param[in] number		Conversion target number
 * @param[out] buffer		Array for copying integer string
 * @param[in] bufferLength	Length of array[Byte]
 * @return					Converted hexadecimal number string or NULL (in case of error)
 */
M2MString *M2MString_convertFromUnsignedLongToHexadecimalString (const uint32_t number, M2MString *buffer, const size_t bufferLength)
	{
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
			this_printErrorMessage(__func__, __LINE__, (M2MString *)"Failed to convert unsigned long to hexadecimal number");
			return NULL;
			}
		}
	//===== Argument error =====
	else if (buffer==NULL)
		{
		this_printErrorMessage(__func__, __LINE__, (M2MString *)"Argument error! Indicated \"buffer\" is NULL");
		return NULL;
		}
	else
		{
		this_printErrorMessage(__func__, __LINE__, (M2MString *)"Argument error! Indicated \"bufferLength\" isn't positive");
		return NULL;
		}
	}


/**
 * This method converts from unsigned long to string.<br>
 * Caller must provide enough buffers as "buffer" argument.<br>
 *
 * @param[in] number		Conversion target number
 * @param[out] buffer		Array for copying integer string
 * @param[in] bufferLength	Length of array[Byte]
 */
M2MString *M2MString_convertFromUnsignedLongToString (const uint32_t number, M2MString *buffer, const size_t bufferLength)
	{
	//===== Check argument =====
	if (bufferLength>0)
		{
		//===== Initialize temporary buffer =====
		memset(buffer, 0, bufferLength);
		//===== Convert from unsigned long to string =====
		if (M2MString_format(buffer, bufferLength-1, "%lu", number)>0)
			{
			return buffer;
			}
		//===== Error handling =====
		else
			{
			this_printErrorMessage(__func__, __LINE__, (M2MString *)"Failed to convert \"uint32_t\" number into string");
			return NULL;
			}
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(__func__, __LINE__, (M2MString *)"Argument error! Indicated \"bufferLength\" isn't positive");
		return NULL;
		}
	}


/**
 * Converts a UTF-16 string to UTF-8. Returns a new string that must be freed<br>
 * or NULL if no conversion was needed.<br>
 *
 * @param[in,out] string
 * @param[in] stringLength
 * @return
 */
M2MString *M2MString_convertFromUTF16ToUTF8 (M2MString **string, size_t *stringLength)
	{
	//========== Variable ==========
	M2MString *u = NULL;
	size_t l = 0;
	size_t sl = 0;
	size_t max = *stringLength;
	int32_t c = 0L;
	int32_t d = 0L;
	int32_t b = 0;
	int32_t be = (**string==(M2MString)'\xFE') ? 1 : (**string==(M2MString)'\xFF') ? 0 : -1;
	const uint32_t XMLParser_BUFSIZE = 1024;

	//=====  =====
	if (be == -1)
		{
		return NULL; // not UTF-16
		}
	//=====  =====
	u = (M2MString *)M2MHeap_malloc((*stringLength));
	//=====  =====
	for (sl=2; sl<*stringLength-1; sl+=2)
		{
		c = (be) ? (((*string)[sl] & 0xFF) << 8) | ((*string)[sl + 1] & 0xFF) // UTF-16BE
				: (((*string)[sl + 1] & 0xFF) << 8) | ((*string)[sl] & 0xFF); // UTF-16LE
		if (c>=0xD800 && c<=0xDFFF && (sl+=2)<*stringLength-1)
			{ // high-half
			d = (be) ? (((*string)[sl] & 0xFF) << 8) | ((*string)[sl + 1] & 0xFF) : (((*string)[sl + 1] & 0xFF) << 8) | ((*string)[sl] & 0xFF);
			c = (((c & 0x3FF) << 10) | (d & 0x3FF)) + 0x10000;
			}

		while (l + 6 > max)
			{
			u = (M2MString *)M2MHeap_realloc(u, max += XMLParser_BUFSIZE);
			}
		if (c < 0x80)
			{
			u[l++] = c; // US-ASCII subset
			}
		//===== multi-byte UTF-8 sequence =====
		else
			{
			for (b=0, d=c; d; d/=2)
				{
				b++; // bits in c
				}
			b = (b - 2) / 5; // bytes in payload
			u[l++] = (0xFF << (7 - b)) | (c >> (6 * b)); // head
			while (b)
				{
				u[l++] = 0x80 | ((c >> (6 * --b)) & 0x3F); // payload
				}
			}
		}
	return ((*string)=(M2MString *)M2MHeap_realloc(u, (*stringLength)=l));
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
		this_printErrorMessage(__func__, __LINE__, (M2MString *)"Argument error! Indicated \"one\" string is NULL");
		return false;
		}
	else if (another==NULL)
		{
		this_printErrorMessage(__func__, __LINE__, (M2MString *)"Argument error! Indicated \"another\" string is NULL");
		return false;
		}
	else
		{
		this_printErrorMessage(__func__, __LINE__, (M2MString *)"Argument error! Indicated \"length\" is invalid");
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
		this_printErrorMessage(__func__, __LINE__, (M2MString *)"Argument error! Indicated \"buffer\" is NULL");
		return -1;
		}
	else if (bufferLength<=0)
		{
		this_printErrorMessage(__func__, __LINE__, (M2MString *)"Argument error! Indicated \"bufferLength\" isn't positive");
		return -1;
		}
	else
		{
		this_printErrorMessage(__func__, __LINE__, (M2MString *)"Argument error! Indicated \"format\" string is NULL or vacant");
		return -1;
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
	//===== Check argument =====
	if (string!=NULL && keyword!=NULL)
		{
		//===== Return the result of search =====
		return (M2MString *)strstr(string, keyword);
		}
	//===== Argument error =====
	else if (string==NULL)
		{
		this_printErrorMessage(__func__, __LINE__, (M2MString *)"Argument error! Indicated \"string\" string is NULL");
		return NULL;
		}
	else
		{
		this_printErrorMessage(__func__, __LINE__, (M2MString *)"Argument error! Indicated \"keyword\" string is NULL");
		return NULL;
		}
	}


/**
 * @param character A character of number
 * @return			true : argument is number, false : argument isn't number
 */
bool M2MString_isNumber (const M2MString character)
	{
	if (isdigit(character)!=0)
		{
		return true;
		}
	else
		{
		return false;
		}
	}


/**
 * @param[in] self
 * @return
 */
bool M2MString_isSpace (const M2MString *self)
	{
	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Check head of string is space =====
		if ((M2MString_compareTo(M2MString_SPACE, self)==0
				|| M2MString_compareTo(M2MString_ZENKAKU_SPACE, self)==0))
			{
			return true;
			}
		//===== In the case of not space =====
		else
			{
			return false;
			}
		}
	//===== Argument error =====
	else
		{
		return false;
		}
	}


/**
 * @param[in] self
 * @return
 */
bool M2MString_isUTF (const M2MString *self)
	{
	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Check length of string =====
		if (M2MString_length(self)>=4)
			{
			//===== In the case of UTF =====
			if ((isxdigit(self[0]) && isxdigit(self[1]) && isxdigit(self[2]) && isxdigit(self[3]))!=0)
				{
				return true;
				}
			//===== In the case of not UTF =====
			else
				{
				return false;
				}
			}
		//===== In the case of short of length =====
		else
			{
			return false;
			}
		}
	//===== Argument error =====
	else
		{
		return false;
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
		this_printErrorMessage(__func__, __LINE__, (M2MString *)"Argument error! Indicated \"string\" string is NULL");
		return NULL;
		}
	else
		{
		this_printErrorMessage(__func__, __LINE__, (M2MString *)"Argument error! Indicated \"fromIndex\" string is NULL");
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
	//===== Check argument =====
	if (self!=NULL)
		{
		return strlen(self);
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(__func__, __LINE__, (M2MString *)"Argument error! Indicated \"self\" string is NULL");
		return 0;
		}
	}


/**
 * This method replaces each substring of this string.<br>
 * It matches the literal target sequence with the specified literal <br>
 * replacement sequence.<br>
 *
 * @param[in] self			original string
 * @param[in] target		sequence of character values to be replaced
 * @param[in] replacement	replacement sequence of character values
 * @param[out] buffer		buffer for copying replaced string
 * @param[in] bufferLength	length of buffer[Byte]
 * @return					replaced string or NULL(means error)
 */
M2MString *M2MString_replace (const M2MString *self, const M2MString *target, const M2MString *replacement, M2MString *buffer, const size_t bufferLength)
	{
	//========== Variable ==========
	M2MString *rest = NULL;
	M2MString *index = NULL;
	size_t currentLength = 0;
	size_t copyM2MStringLength = 0;
	const size_t TARGET_LENGTH = M2MString_length(target);
	const size_t REPLACEMENT_LENGTH = M2MString_length(replacement);

	//===== Check argument =====
	if (self!=NULL && M2MString_length(self)>0
			&& target!=NULL && M2MString_length(target)>0
			&& replacement!=NULL
			&& bufferLength>0)
		{
		//===== Initialize buffer =====
		memset(buffer, 0, bufferLength);
		rest = (M2MString *)self;
		//===== Detect position of target string =====
		while ((index=M2MString_indexOf(rest, target))!=NULL)
			{
			//===== Check buffer length =====
			if ((copyM2MStringLength=M2MString_length(rest)-M2MString_length(index))<(bufferLength-currentLength))
				{
				//===== Copy string into buffer =====
				memcpy(&(buffer[currentLength]), rest, copyM2MStringLength);
				//===== Update string length =====
				currentLength += copyM2MStringLength;
				//===== Check buffer length =====
				if (REPLACEMENT_LENGTH<(bufferLength-currentLength))
					{
					//===== Replace string =====
					memcpy(&(buffer[currentLength]), replacement, REPLACEMENT_LENGTH);
					//===== Update string length =====
					currentLength += REPLACEMENT_LENGTH;
					//===== Proceed pointer =====
					index += TARGET_LENGTH;
					rest = index;
					}
				//===== Error handling =====
				else
					{
					memset(buffer, 0, bufferLength);
					return NULL;
					}
				}
			//===== Error handling =====
			else
				{
				memset(buffer, 0, bufferLength);
				return NULL;
				}
			}
		//===== Check buffer length =====
		if (M2MString_length(rest)<(bufferLength-currentLength))
			{
			//===== Copy rest string into buffer =====
			memcpy(&(buffer[currentLength]), rest, M2MString_length(rest));
			return buffer;
			}
		//===== Error handling =====
		else
			{
			memset(buffer, 0, bufferLength);
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
 * If you want to repeat splitting same word, must use same savePoint<br>
 * variable.<br>
 *
 * @param self
 * @param delimiter
 * @param savePoint
 * @return
 */
M2MString *M2MString_split (M2MString *self, const M2MString *delimiter, M2MString **savePoint)
	{
	//===== Check argument =====
	if (delimiter!=NULL)
		{
		//===== Reentrant tokenization =====
		return (M2MString *)strtok_r(self, delimiter, (char **)savePoint);
		}
	//===== Argument error =====
	else
		{
		return self;
		}
	}


/**
 * @param self
 * @param buffer
 * @param bufferLength
 * @return
 */
M2MString *M2MString_toLowerCase (const M2MString *self, M2MString *buffer, const size_t bufferLength)
	{
	//========== Variable ==========
	size_t i = 0;
	size_t selfLength = 0;
	const unsigned int CHARACTER_LENGTH = 4;
	M2MString character[CHARACTER_LENGTH];

	//===== Check argument =====
	if (self!=NULL && bufferLength>0)
		{
		//===== Get length of string =====
		if ((selfLength=M2MString_length(self))>0 && selfLength<bufferLength)
			{
			//===== Initialize buffer =====
			memset(character, 0, CHARACTER_LENGTH);
			//===== Repeat transfer =====
			for (i=0; i<selfLength; i++)
				{
				//===== Transfer a character =====
				character[0] = tolower(self[i]);
				memcpy(&(buffer[i]), character, 1);
				//===== Initialize buffer =====
				memset(character, 0, CHARACTER_LENGTH);
				}
			return buffer;
			}
		//===== Error handling =====
		else
			{
			return NULL;
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		return NULL;
		}
	else
		{
		return NULL;
		}
	}


/**
 * @param[in] self
 * @param[out] upperCaseM2MString	buffer for upper case string(caller muse release this allocated memory)
 * @return
 */
M2MString *M2MString_toUpperCase (const M2MString *self, M2MString *buffer, const size_t bufferLength)
	{
	//========== Variable ==========
	size_t i = 0;
	size_t selfLength = 0;
	const unsigned int CHARACTER_LENGTH = 2;
	M2MString character[CHARACTER_LENGTH];

	//===== Check argument =====
	if (self!=NULL && bufferLength>0)
		{
		//===== Get length of string =====
		if ((selfLength=M2MString_length(self))>0 && selfLength<bufferLength)
			{
			//===== Initialize buffer =====
			memset(character, 0, CHARACTER_LENGTH);
			//===== Repeat transfer =====
			for (i=0; i<selfLength; i++)
				{
				//===== Transfer a character =====
				character[0] = toupper(self[i]);
				memcpy(&(buffer[i]), character, 1);
				//===== Initialize buffer =====
				memset(character, 0, CHARACTER_LENGTH);
				}
			return buffer;
			}
		//===== Error handling =====
		else
			{
			return NULL;
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		return NULL;
		}
	else
		{
		return NULL;
		}
	}


/* End Of File */
