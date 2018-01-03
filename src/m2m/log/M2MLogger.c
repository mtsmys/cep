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

#include "m2m/log/M2MLogger.h"


/*******************************************************************************
 * Declaration of private function
 ******************************************************************************/
/**
 * Initialize "errorno".<br>
 */
static void this_initErrorNumber ();


/**
 * Check whether the size of the argument file exceeds the default value. <br>
 *
 * @param[in] file	File object to be checked for size
 * @return			true: Exceeded default value, false: Within the default value
 */
static bool this_overMaxLogFileLength (const FILE *file);



/*******************************************************************************
 * Private function
 ******************************************************************************/
/**
 * Copy the debug message to the argument pointer.<br>
 * Buffering of array for copying is executed inside the function. <br>
 * Therefore, on the caller side, it is necessary to call the "M2MHeap_free()" <br>
 * function in order to prevent memory leak after using the variable.<br>
 *
 * @param[in] methodName		String indicating function name
 * @param[in] lineNumber		Line number in source file (can be embedded with "__LINE__")
 * @param[in] message			Message string indicating error content
 * @param[out] debugMessage		Buffer to copy the error message
 * @return						Error message copied to argument pointer or NULL (in case of error)
 */
#ifdef DEBUG
static M2MString *this_createNewDebugMessage (const M2MString *methodName, const unsigned int lineNumber, const M2MString *message, M2MString **debugMessage)
	{
	//========== Variable ==========
	M2MString time[128];
	M2MString lineNumberString[16];
	size_t errorMessageLength = 0;
	size_t timeLength = 0;
	size_t methodNameLength = 0;
	size_t lineNumberLength = 0;
	size_t messageLength = 0;
	const M2MString *METHOD_NAME = (M2MString *)"M2MLogger.this_createNewDebugMessage()";

	//===== Check argument =====
	if (methodName!=NULL && (methodNameLength=M2MString_length(methodName))>0
			&& (messageLength=M2MString_length(message))>0
			&& debugMessage!=NULL)
		{
		//===== Initialize array =====
		memset(time, 0, sizeof(time));
		memset(lineNumberString, 0, sizeof(lineNumberString));
		//===== Get line number string =====
		snprintf(lineNumberString, sizeof(lineNumberString)-1, (M2MString *)"%d", lineNumber);
		lineNumberLength = M2MString_length(lineNumberString);
		//===== Get current time string from local calendar ======
		if (M2MString_getLocalTime(time, sizeof(time))>0
				&& (timeLength=M2MString_length(time))>0)
			{
			//===== Get heap memory ======
			if ((errorMessageLength=M2MString_length((M2MString *)"[DEBUG]%s %s:%d[l]: %s\n")+timeLength+methodNameLength+lineNumberLength+messageLength)>0
					&& ((*debugMessage)=(M2MString *)M2MHeap_malloc(errorMessageLength+1))!=NULL)
				{
				snprintf((*debugMessage), errorMessageLength, (M2MString *)"[DEBUG]%s %s:%d[l]: %s\n", time, methodName, lineNumber, message);
				return (*debugMessage);
				}
			//===== Error handling ======
			else
				{
				M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to get heap memory for copying the debug message string", NULL);
				return NULL;
				}
			}
		//===== In the case of failed to get current time string ======
		else
			{
			//===== Get heap memory ======
			if ((errorMessageLength=M2MString_length((M2MString *)"[DEBUG]%s:%d[l]: %s\n")+methodNameLength+lineNumberLength+messageLength)>0
					&& ((*debugMessage)=(M2MString *)M2MHeap_malloc(errorMessageLength+1))!=NULL)
				{
				snprintf((*debugMessage), errorMessageLength, (M2MString *)"[DEBUG]%s:%d[l]: %s\n", methodName, lineNumber, message);
				return (*debugMessage);
				}
			//===== Error handling ======
			else
				{
				M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to get heap memory for copying the debug message string", NULL);
				return NULL;
				}
			}
		}
	//===== Argument error =====
	else if (methodName==NULL || methodNameLength<=0)
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"methodName\" is NULL", NULL);
		return NULL;
		}
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"message\" is NULL", NULL);
		return NULL;
		}
	}
#endif /* DEBUG */


/**
 * Copy the error message to the argument pointer.<br>
 * Buffering of array for copying is executed inside the function.<br>
 * Therefore, it is necessary for caller to call the "M2MHeap_free()" <br>
 * function in order to prevent memory leak after using the variable.<br>
 *
 * @param[in] methodName		String indicating function name
 * @param[in] lineNumber		Line number in source file (can be embedded with "__LINE__")
 * @param[in] message			Message string indicating error content
 * @param[out] errorMessage		Buffer to copy the error message
 * @return						Error message copied to argument pointer or NULL (in case of error)
 */
static M2MString *this_createNewErrorMessage (const M2MString *methodName, const unsigned int lineNumber, const M2MString *message, M2MString **errorMessage)
	{
	//========== Variable ==========
	M2MString time[128];
	M2MString lineNumberString[16];
	M2MString buffer[256];
	size_t errorMessageLength = 0;
	size_t timeLength = 0;
	size_t methodNameLength = 0;
	size_t lineNumberLength = 0;
	size_t messageLength = 0;
	size_t bufferLength = 0;

	//===== Check argument =====
	if (methodName!=NULL && (methodNameLength=M2MString_length(methodName))>0
			&& (messageLength=M2MString_length(message))>0
			&& errorMessage!=NULL)
		{
		//===== Initialize array =====
		memset(time, 0, sizeof(time));
		memset(lineNumberString, 0, sizeof(lineNumberString));
		//===== Get line number string =====
		snprintf(lineNumberString, sizeof(lineNumberString)-1, (M2MString *)"%d", lineNumber);
		lineNumberLength = M2MString_length(lineNumberString);
		//===== In the case of existing error number =====
		if (errno!=0
				&& strerror_r(errno, buffer, sizeof(buffer))==0
				&& (bufferLength=M2MString_length(buffer))>0)
			{
			//===== Get current time string from local calendar ======
			if (M2MString_getLocalTime(time, sizeof(time))>0
					&& (timeLength=M2MString_length(time))>0)
				{
				//===== Get heap memory ======
				if ((errorMessageLength=M2MString_length((M2MString *)"[ERROR]%s %s:%d[l]: %s: %s\r\n")+timeLength+methodNameLength+lineNumberLength+messageLength+bufferLength)>0
						&& ((*errorMessage)=(M2MString *)M2MHeap_malloc(errorMessageLength+1))!=NULL)
					{
					snprintf((*errorMessage), errorMessageLength, (M2MString *)"[ERROR]%s %s:%d[l]: %s: %s\r\n", time, methodName, lineNumber, message, buffer);
					this_initErrorNumber();
					return (*errorMessage);
					}
				//===== Error handling ======
				else
					{
					this_initErrorNumber();
					return NULL;
					}
				}
			//===== In the case of failed to get current time string ======
			else
				{
				//===== Get heap memory ======
				if ((errorMessageLength=M2MString_length((M2MString *)"[ERROR]%s:%d[l]: %s: %s\r\n")+methodNameLength+lineNumberLength+messageLength+bufferLength)>0
						&& ((*errorMessage)=(M2MString *)M2MHeap_malloc(errorMessageLength+1))!=NULL)
					{
					snprintf((*errorMessage), errorMessageLength, (M2MString *)"[ERROR]%s:%d[l]: %s: %s\r\n", methodName, lineNumber, message, buffer);
					this_initErrorNumber();
					return (*errorMessage);
					}
				//===== Error handling ======
				else
					{
					this_initErrorNumber();
					return NULL;
					}
				}
			}
		//===== In the case of not existing error number =====
		else
			{
			//===== Get current time string from local calendar ======
			if (M2MString_getLocalTime(time, sizeof(time))>0
					&& (timeLength=M2MString_length(time))>0)
				{
				//===== Get heap memory ======
				if ((errorMessageLength=M2MString_length((M2MString *)"[ERROR]%s %s:%d[l]: %s\r\n")+timeLength+methodNameLength+lineNumberLength+messageLength)>0
						&& ((*errorMessage)=(M2MString *)M2MHeap_malloc(errorMessageLength+1))!=NULL)
					{
					snprintf((*errorMessage), errorMessageLength, (M2MString *)"[ERROR]%s %s:%d[l]: %s\r\n", time, methodName, lineNumber, message);
					return (*errorMessage);
					}
				//===== Error handling ======
				else
					{
					return NULL;
					}
				}
			//===== In the case of failed to get current time string ======
			else
				{
				//===== Get heap memory ======
				if ((errorMessageLength=M2MString_length((M2MString *)"[ERROR]%s:%d[l]: %s\r\n")+methodNameLength+lineNumberLength+messageLength)>0
						&& ((*errorMessage)=(M2MString *)M2MHeap_malloc(errorMessageLength+1))!=NULL)
					{
					snprintf((*errorMessage), errorMessageLength, (M2MString *)"[ERROR]%s:%d[l]: %s\r\n", methodName, lineNumber, message);
					return (*errorMessage);
					}
				//===== Error handling ======
				else
					{
					return NULL;
					}
				}
			}
		}
	//===== Argument error =====
	else if (methodName==NULL || methodNameLength<=0)
		{
		return NULL;
		}
	else
		{
		return NULL;
		}
	}


/**
 * Open the file placed in the argument pathname.<br>
 *
 * @param[in] logFilePath	Absolute pathname of log file
 * @return					Opened FILE object or NULL (if copying is unnecessary)
 */
static FILE *this_getLogFile (const M2MString *logFileName)
	{
	//========== Variable ==========
	FILE *file = NULL;
	const M2MString *HOME_DIRECTORY = M2MDirectory_getHomeDirectoryPath();
	const size_t HOME_DIRECTORY_LENGTH = M2MString_length(HOME_DIRECTORY);
	const size_t SEPARATOR_LENGTH = M2MString_length(M2MDirectory_SEPARATOR);
	const size_t DIRECTORY_NAME_LENGTH = M2MString_length(M2MDirectory_DIRECTORY_NAME);
	const size_t LOG_FILE_NAME_LENGTH = M2MString_length(logFileName);
	M2MString LOG_FILE_DIRECTORY[HOME_DIRECTORY_LENGTH+SEPARATOR_LENGTH+DIRECTORY_NAME_LENGTH+2];
	M2MString LOG_FILE_PATH[HOME_DIRECTORY_LENGTH+SEPARATOR_LENGTH+DIRECTORY_NAME_LENGTH+SEPARATOR_LENGTH+LOG_FILE_NAME_LENGTH+2];

	//===== Get log file directory pathname =====
	memset(LOG_FILE_DIRECTORY, 0, sizeof(LOG_FILE_DIRECTORY));
	snprintf(LOG_FILE_DIRECTORY, sizeof(LOG_FILE_DIRECTORY), (M2MString *)"%s/%s", HOME_DIRECTORY, M2MDirectory_DIRECTORY_NAME);
	//===== Check the existence of directory =====
	if (M2MDirectory_exists(LOG_FILE_DIRECTORY)==true)
		{
		memset(LOG_FILE_PATH, 0, sizeof(LOG_FILE_PATH));
		snprintf(LOG_FILE_PATH, sizeof(LOG_FILE_PATH), (M2MString *)"%s/%s/%s", HOME_DIRECTORY, M2MDirectory_DIRECTORY_NAME, logFileName);
		//===== Open log file =====
		if ((file=M2MFile_open(LOG_FILE_PATH, true))!=NULL)
			{
			//===== In the case of the size of the log file exceeds the specified value =====
			if (this_overMaxLogFileLength(file)==true)
				{
				M2MFile_remove(LOG_FILE_PATH);
				}
			//===== In the case of the size of the log file is within the specified value =====
			else
				{
				// do nothing
				}
			return file;
			}
		//===== Error handling =====
		else
			{
			return NULL;
			}
		}
	//===== In the case of not existing log file =====
	else
		{
		//===== Create new log file =====
		if (M2MDirectory_mkdirs(LOG_FILE_DIRECTORY, M2MLogger_LOG_FILE_PERMISSION)==true)
			{
			memset(LOG_FILE_PATH, 0, sizeof(LOG_FILE_PATH));
			snprintf(LOG_FILE_PATH, sizeof(LOG_FILE_PATH), (M2MString *)"%s/%s/%s", HOME_DIRECTORY, M2MDirectory_DIRECTORY_NAME, logFileName);
			//===== Open log file =====
			if ((file=M2MFile_open(LOG_FILE_PATH, true))!=NULL)
				{
				//===== In the case of the size of the log file exceeds the specified value =====
				if (this_overMaxLogFileLength(file)==true)
					{
					M2MFile_remove(LOG_FILE_PATH);
					}
				//===== In the case of the size of the log file is within the specified value =====
				else
					{
					// do nothing
					}
				return file;
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
	}


/**
 * Return the default log file name string.<br>
 *
 * @return	Log file name string
 */
static M2MString *this_getLogFileName ()
	{
	return M2MLogger_DEFAULT_LOG_FILE_NAME;
	}


/**
 * Initialize "errorno".<br>
 */
static void this_initErrorNumber ()
	{
	errno = 0;
	return;
	}


/**
 * Check whether the size of the argument file exceeds the default value. <br>
 *
 * @param[in] file	File object to be checked for size
 * @return			true: Exceeded default value, false: Within the default value
 */
static bool this_overMaxLogFileLength (const FILE *file)
	{
	//===== In the case of the current file size exceeds the specified value =====
	if ((unsigned long)M2MLogger_MAX_LOG_FILE_LENGTH<M2MFile_length(file))
		{
		return true;
		}
	//===== In the case of the current file size is within the specified value =====
	else
		{
		return false;
		}
	}


/**
 * Output argument "message" into log file.<br>
 *
 * @param logFileName	Log file name string
 * @param message		Log message string
 */
static void this_outputLogMessage (const M2MString *logFileName, const M2MString *message)
	{
	//========== Variable ==========
	FILE *file = NULL;

	//===== Open log file =====
	if ((file=this_getLogFile(logFileName))!=NULL)
		{
		//===== Output debug message to log file =====
		M2MFile_write(file, message, M2MString_length(message));
		//===== Close log file =====
		M2MFile_close(file);
		}
	//===== Error handling =====
	else
		{
		//===== Output error message to standard error output =====
		fprintf(stdout, (M2MString *)"%s", message);
		}
	return;
	}


/**
 * Output debug message to file or standard error output.<br>
 *
 * @param[in] methodName		String indicating function name
 * @param[in] lineNumber		Line number in source file (can be embedded with "__LINE__")
 * @param[in] message			Message string indicating error content
 */
#ifdef DEBUG
static void this_printDebugMessage (const M2MString *methodName, const unsigned int lineNumber, const M2MString *message)
	{
	//========== Variable ==========
	M2MString *debugMessage = NULL;

	//===== Get formatted debug message =====
	if (this_createNewDebugMessage(methodName, lineNumber, message, &debugMessage)!=NULL)
		{
		//===== Output debug message into log file =====
		this_outputLogMessage(this_getLogFileName(), debugMessage);
		//===== Release heap memory =====
		M2MHeap_free(debugMessage);
		}
	//===== Error handling =====
	else
		{
		}
	return;
	}
#endif /* DEBUG */



/**
 * Output error message to file or standard error output.<br>
 *
 * @param[in] methodName		String indicating function name
 * @param[in] lineNumber		Line number in source file (can be embedded with "__LINE__")
 * @param[in] message			Message string indicating error content
 */
static void this_printErrorMessage (const M2MString *methodName, const unsigned int lineNumber, const M2MString *message)
	{
	//========== Variable ==========
	M2MString *errorMessage = NULL;

	//===== Get formatted error message =====
	if (this_createNewErrorMessage(methodName, lineNumber, message, &errorMessage)!=NULL)
		{
		//===== Output error message into log file =====
		this_outputLogMessage(this_getLogFileName(), errorMessage);
		//===== Release heap memory =====
		M2MHeap_free(errorMessage);
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
 * Output debug messages to standard output.<br>
 * Debug message is based on the function name, line number, and message.<br>
 *
 * @param[in] methodName	String indicating function name
 * @param[in] lineNumber	Line number in source file (can be embedded with "__LINE__")
 * @param[in] message		Message string indicating error content
 */
void M2MLogger_printDebugMessage (const M2MString *methodName, const unsigned int lineNumber, const M2MString *message)
	{
#ifdef DEBUG
	//===== Output debug message =====
	this_printDebugMessage(methodName, lineNumber, message);
#endif /* DEBUG */
	return;
	}


/**
 * Output error message to the standard error output.<br>
 * Error message is based on the function name, line number, and message.<br>
 * Also, when a pointer for copying error message is specified as argument, <br>
 * buffering is performed inside this function, and error message is copied.<br>
 *
 * @param[in] methodName					String indicating function name
 * @param[in] lineNumber					Line number in source file (can be embedded with "__LINE__")
 * @param[in] message						Message string indicating error content
 * @param[out] bufferForCopyingErrorMessage	Buffer to copy the error message or NULL (if copying is unnecessary)
 */
void M2MLogger_printErrorMessage (const M2MString *methodName, const unsigned int lineNumber, const M2MString *message, M2MString **bufferForCopyingErrorMessage)
	{
	//===== In the case of copying error message to argument =====
	if (bufferForCopyingErrorMessage!=NULL)
		{
		this_createNewErrorMessage(methodName, lineNumber, message, bufferForCopyingErrorMessage);
		}
	//===== In the case of not copying error message =====
	else
		{
		// do nothing
		}
	//===== Output error message into log file =====
	this_printErrorMessage(methodName, lineNumber, message);
	return;
	}



/* End Of File */
