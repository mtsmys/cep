/*******************************************************************************
 * M2MLogger.c : Logging library
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

#include "m2m/lib/log/M2MLogger.h"



/*******************************************************************************
 * Definition
 ******************************************************************************/
/**
 *
 * @param[in] functionName		String indicating function name
 * @param[in] lineNumber		Line number in source file (can be embedded with "__LINE__")
 * @param[in] message			Message string
 */
static void this_printErrorMessage (const M2MString *functionName, const uint32_t lineNumber, const M2MString *message);



/*******************************************************************************
 * Private function
 ******************************************************************************/
/**
 *
 * @param[in,out] self
 */
static void this_deleteLoggerName (M2MLogger *self)
	{
	//========== Variable ==========
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MLogger.this_deleteLoggerName()";

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== In case of existing logger name string =====
		if (M2MLogger_getLoggerName(self)!=NULL)
			{
			//===== Release allocated memory area =====
			M2MHeap_free(self->loggerName);
			}
		//===== In case of no existing logger name string =====
		else
			{
			}
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MLogger\" structure object is NULL");
		}
	return;
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



/*******************************************************************************
 * Public function
 ******************************************************************************/
/**
 * Copy the log message to the argument "buffer" pointer.<br>
 * Buffering of array for copying is executed inside the function.<br>
 * Therefore, it is necessary for caller to call the "M2MHeap_free()" function <br>
 * in order to prevent memory leak after using the variable.<br>
 *
 * @param[in] logLevel			Log level
 * @param[in] functionName		String indicating function name
 * @param[in] lineNumber		Line number in source file (can be embedded with "__LINE__")
 * @param[in] message			Message string
 * @param[out] buffer			Buffer to copy the created log message
 * @return						The pointer of "buffer" copied the created log message string or NULL (in case of error)
 */
M2MString *M2MLogger_createNewLogMessage (
		const M2MLogLevel logLevel,
		const M2MString *functionName,
		const uint32_t lineNumber,
		const M2MString *message,
		M2MString **buffer
		)
	{
	//========== Variable ==========
	M2MString *logLevelString = NULL;
	M2MString time[64];
	M2MString lineNumberString[16];
	M2MString errnoMessage[256];
	M2MString threadID[128];
	size_t functionNameLength = 0;
	size_t messageLength = 0;

	//===== Check argument =====
	if ((logLevelString=M2MLogLevel_toString (logLevel))!=NULL
			&& functionName!=NULL && (functionNameLength=M2MString_length(functionName))>0
			&& message!=NULL && (messageLength=M2MString_length(message))>0
			&& buffer!=NULL)
		{
		//===== Get line number string =====
		memset(lineNumberString, 0, sizeof(lineNumberString));
		snprintf(lineNumberString, sizeof(lineNumberString)-1, (M2MString *)"%d", lineNumber);
		//===== Initialize array =====
		memset(time, 0, sizeof(time));
		//===== Get current time string from local calendar ======
		if (M2MDate_getLocalTimeString(time, sizeof(time))>0
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
//						&& M2MString_append(buffer, errnoMessage)!=NULL
//						&& M2MString_append(buffer, M2MString_COLON)!=NULL
//						&& M2MString_append(buffer, M2MString_SPACE)!=NULL
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
 * Destructor.<br>
 * This function releases the heap memory area of logger structure object.<br>
 *
 * @param[in,out] self
 */
void M2MLogger_delete (M2MLogger **self)
	{
	//========== Variable ==========
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MLogger_delete()";

	//===== Check argument =====
	if (self!=NULL && (*self)!=NULL)
		{
		//===== Release heap memory area of logger name =====
		this_deleteLoggerName((*self));
		//===== Release heap memory area of logger object =====
		M2MHeap_free((*self));
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MLogger\" structure object is NULL");
		}
	return;
	}


/**
 *
 * @param[in] self
 * @return
 */
M2MString *M2MLogger_getLoggerName (const M2MLogger *self)
	{
	//========== Variable ==========
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MLogger_getLoggerName()";

	//===== Check argument =====
	if (self!=NULL)
		{
		return self->loggerName;
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MLogger\" structure object is NULL");
		return NULL;
		}
	}


/**
 * @param[in] self
 * @return
 */
M2MLogLevel M2MLogger_getLogLevel (const M2MLogger *self)
	{
	//========== Variable ==========
	M2MLogLevel DEFAULT_LOG_LEVE = M2MLogLevel_INFO;
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MLogger_getLogLevel()";

	//===== Check argument =====
	if (self!=NULL)
		{
		return self->level;
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MLogger\" structure object is NULL");
		return DEFAULT_LOG_LEVE;
		}
	}


/**
 * Constructor.<br>
 * This method allocates new memory for creating Logger structure object.<br>
 *
 * @return	created new Logger object or NULL (in case of error)
 */
M2MLogger *M2MLogger_new ()
	{
	//========== Variable ==========
	M2MLogger *self = NULL;
	const M2MLogLevel DEFAULT_LOG_LEVEL = M2MLogLevel_INFO;
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MLogger_new()";

	//===== Allocate new memory =====
	if ((self=(M2MLogger *)M2MHeap_malloc(sizeof(M2MLogger)))!=NULL)
		{
		//===== Set log level =====
		if (M2MLogger_setLogLevel(self, DEFAULT_LOG_LEVEL)!=NULL)
			{
			//=====  =====
			this_initErrorNumber();
			//=====  =====
			return self;
			}
		//===== Error handling =====
		else
			{
			this_printErrorMessage(FUNCTION_NAME, __LINE__, (M2MString *)"Failed to set default log level into created new \"M2MLogger\" structure object");
			//===== Release allocated memory area =====
			M2MLogger_delete(&self);
			return NULL;
			}
		}
	//===== Error handling =====
	else
		{
		this_printErrorMessage(FUNCTION_NAME, __LINE__, (M2MString *)"Failed to allocate new memory for creating \"M2MLogger\" structure object");
		return NULL;
		}
	}


/**
 * @param[in,out] self
 * @param[in] level
 * @return
 */
M2MLogger *M2MLogger_setLogLevel (M2MLogger *self, const M2MLogLevel level)
	{
	//========== Variable ==========
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MLogger_setLogLevel()";

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Set log level =====
		self->level = level;
		return self;
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MLogger\" structure object is NULL");
		return NULL;
		}
	}


/**
 * @param[in,out] self
 * @param[in] loggerName
 * @return
 */
M2MString *M2MLogger_setLoggerName (M2MLogger *self, const M2MString *loggerName)
	{
	//========== Variable ==========
	unsigned int loggerNameLength = 0;
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MLogger_setLoggerName()";

	//===== Check argument =====
	if (self!=NULL
			&& loggerName!=NULL
			&& (loggerNameLength=M2MString_length(loggerName))>0)
		{
		//===== Release allocated memory =====
		this_deleteLoggerName(self);
		//===== Allocate new memory =====
		if ((self->loggerName=(M2MString *)M2MHeap_malloc(loggerNameLength+1))!=NULL)
			{
			//===== Set logger name into appender =====
			memset(self->loggerName, 0, loggerNameLength+1);
			memcpy(self->loggerName, loggerName, loggerNameLength);
			return self->loggerName;
			}
		//===== Error handling =====
		else
			{
			this_printErrorMessage(FUNCTION_NAME, __LINE__, (M2MString *)"Failed to allocate new memory for copying \"loggerName\" string into \"M2MLogger\" structure object");
			return NULL;
			}
		}
	//===== Check argument =====
	else if (self==NULL)
		{
		this_printErrorMessage(FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MLogger\" structure object is NULL");
		return NULL;
		}
	else
		{
		this_printErrorMessage(FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"loggerName\" string is NULL or vacant");
		return NULL;
		}
	}


/* End Of File */
