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
 * @param[in] functionName		String indicating function name
 * @param[in] lineNumber		Line number in source file (can be embedded with "__LINE__")
 * @param[in] message			Message string indicating error content
 * @param[out] debugMessage		Buffer to copy the error message
 * @return						Error message copied to argument pointer or NULL (in case of error)
 */
#ifdef DEBUG
static M2MString *this_createNewDebugMessage (const M2MString *functionName, const unsigned int lineNumber, const M2MString *message, M2MString **debugMessage)
	{
	//========== Variable ==========
	M2MString time[128];
	M2MString lineNumberString[16];
	size_t errorMessageLength = 0;
	size_t timeLength = 0;
	size_t functionNameLength = 0;
	size_t lineNumberLength = 0;
	size_t messageLength = 0;
	const M2MString *METHOD_NAME = (M2MString *)"M2MLogger.this_createNewDebugMessage()";

	//===== Check argument =====
	if (functionName!=NULL && (functionNameLength=M2MString_length(functionName))>0
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
			if ((errorMessageLength=M2MString_length((M2MString *)"[DEBUG]%s %s:%d[l]: %s\n")+timeLength+functionNameLength+lineNumberLength+messageLength)>0
					&& ((*debugMessage)=(M2MString *)M2MHeap_malloc(errorMessageLength+1))!=NULL)
				{
				snprintf((*debugMessage), errorMessageLength, (M2MString *)"[DEBUG]%s %s:%d[l]: %s\n", time, functionName, lineNumber, message);
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
			if ((errorMessageLength=M2MString_length((M2MString *)"[DEBUG]%s:%d[l]: %s\n")+functionNameLength+lineNumberLength+messageLength)>0
					&& ((*debugMessage)=(M2MString *)M2MHeap_malloc(errorMessageLength+1))!=NULL)
				{
				snprintf((*debugMessage), errorMessageLength, (M2MString *)"[DEBUG]%s:%d[l]: %s\n", functionName, lineNumber, message);
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
	else if (functionName==NULL || functionNameLength<=0)
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"functionName\" is NULL", NULL);
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
 * @param[in] functionName		String indicating function name
 * @param[in] lineNumber		Line number in source file (can be embedded with "__LINE__")
 * @param[in] message			Message string indicating error content
 * @param[out] errorMessage		Buffer to copy the error message
 * @return						Error message copied to argument pointer or NULL (in case of error)
 */
static M2MString *this_createNewErrorMessage (const M2MString *functionName, const unsigned int lineNumber, const M2MString *message, M2MString **errorMessage)
	{
	//========== Variable ==========
	M2MString time[128];
	M2MString lineNumberString[16];
	M2MString buffer[256];
	size_t errorMessageLength = 0;
	size_t timeLength = 0;
	size_t functionNameLength = 0;
	size_t lineNumberLength = 0;
	size_t messageLength = 0;
	size_t bufferLength = 0;

	//===== Check argument =====
	if (functionName!=NULL && (functionNameLength=M2MString_length(functionName))>0
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
				if ((errorMessageLength=M2MString_length((M2MString *)"[ERROR]%s %s:%d[l]: %s: %s\r\n")+timeLength+functionNameLength+lineNumberLength+messageLength+bufferLength)>0
						&& ((*errorMessage)=(M2MString *)M2MHeap_malloc(errorMessageLength+1))!=NULL)
					{
					snprintf((*errorMessage), errorMessageLength, (M2MString *)"[ERROR]%s %s:%d[l]: %s: %s\r\n", time, functionName, lineNumber, message, buffer);
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
				if ((errorMessageLength=M2MString_length((M2MString *)"[ERROR]%s:%d[l]: %s: %s\r\n")+functionNameLength+lineNumberLength+messageLength+bufferLength)>0
						&& ((*errorMessage)=(M2MString *)M2MHeap_malloc(errorMessageLength+1))!=NULL)
					{
					snprintf((*errorMessage), errorMessageLength, (M2MString *)"[ERROR]%s:%d[l]: %s: %s\r\n", functionName, lineNumber, message, buffer);
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
				if ((errorMessageLength=M2MString_length((M2MString *)"[ERROR]%s %s:%d[l]: %s\r\n")+timeLength+functionNameLength+lineNumberLength+messageLength)>0
						&& ((*errorMessage)=(M2MString *)M2MHeap_malloc(errorMessageLength+1))!=NULL)
					{
					snprintf((*errorMessage), errorMessageLength, (M2MString *)"[ERROR]%s %s:%d[l]: %s\r\n", time, functionName, lineNumber, message);
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
				if ((errorMessageLength=M2MString_length((M2MString *)"[ERROR]%s:%d[l]: %s\r\n")+functionNameLength+lineNumberLength+messageLength)>0
						&& ((*errorMessage)=(M2MString *)M2MHeap_malloc(errorMessageLength+1))!=NULL)
					{
					snprintf((*errorMessage), errorMessageLength, (M2MString *)"[ERROR]%s:%d[l]: %s\r\n", functionName, lineNumber, message);
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
	else if (functionName==NULL || functionNameLength<=0)
		{
		return NULL;
		}
	else
		{
		return NULL;
		}
	}


/**
 *
 * @param[in,out] self
 */
static void this_deleteLoggerName (M2MLogger *self)
	{
	//===== Check argument =====
	if (self!=NULL)
		{
		//=====  =====
		if (M2MLogger_getLoggerName(self)!=NULL)
			{
			M2MHeap_free(self->loggerName);
			}
		//=====  =====
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
 * Initialize "errorno" variable.<br>
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
 * @param[in] functionName		String indicating function name
 * @param[in] lineNumber		Line number in source file (can be embedded with "__LINE__")
 * @param[in] message			Message string indicating error content
 */
#ifdef DEBUG
static void this_printDebugMessage (const M2MString *functionName, const unsigned int lineNumber, const M2MString *message)
	{
	//========== Variable ==========
	M2MString *debugMessage = NULL;

	//===== Get formatted debug message =====
	if (this_createNewDebugMessage(functionName, lineNumber, message, &debugMessage)!=NULL)
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
 * @param[in] functionName		String indicating function name
 * @param[in] lineNumber		Line number in source file (can be embedded with "__LINE__")
 * @param[in] message			Message string indicating error content
 */
static void this_printErrorMessage (const M2MString *functionName, const unsigned int lineNumber, const M2MString *message)
	{
	//========== Variable ==========
	M2MString *errorMessage = NULL;

	//===== Get formatted error message =====
	if (this_createNewErrorMessage(functionName, lineNumber, message, &errorMessage)!=NULL)
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
 * Destructor.<br>
 * This function releases the heap memory area of logger structure object.<br>
 *
 * @param[in,out] self
 */
void M2MLogger_delete (M2MLogger **self)
	{
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
	//===== Check argument =====
	if (self!=NULL)
		{
		return self->loggerName;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(__func__, __LINE__, (M2MString *)"Argument error! Indicated \"M2MLogger\" structure object is NULL");
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

	//===== Check argument =====
	if (self!=NULL)
		{
		return self->level;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(__func__, __LINE__, (M2MString *)"Argument error! Indicated \"M2MLogger\" structure object is NULL");
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

	//===== Allocate new memory =====
	if ((self=(M2MLogger *)M2MHeap_malloc(sizeof(M2MLogger)))!=NULL)
		{
		//===== Set log level =====
		if (M2MLogger_setLogLevel(self, DEFAULT_LOG_LEVEL)!=NULL)
			{
			return self;
			}
		//===== Error handling =====
		else
			{
			M2MLogger_printErrorMessage(__func__, __LINE__, (M2MString *)"Failed to set log level into \"M2MLogger\" structure object");
			//===== Release allocated memory area =====
			M2MLogger_delete(&self);
			return NULL;
			}
		}
	//===== Error handling =====
	else
		{
		M2MLogger_printErrorMessage(__func__, __LINE__, (M2MString *)"Failed to allocate new memory for creating \"M2MLogger\" structure object");
		return NULL;
		}
	}


/**
 * Output debug messages to standard output.<br>
 * Debug message is based on the function name, line number, and message.<br>
 *
 * @param[in] functionName	String indicating function name
 * @param[in] lineNumber	Line number in source file (can be embedded with "__LINE__")
 * @param[in] message		Message string indicating error content
 */
void M2MLogger_printDebugMessage (const M2MString *functionName, const unsigned int lineNumber, const M2MString *message)
	{
#ifdef DEBUG
	//===== Output debug message =====
	this_printDebugMessage(functionName, lineNumber, message);
#endif /* DEBUG */
	return;
	}


/**
 * This method prints out debug message if "DEBUG" symbol is set.<br>
 *
 * @param[in] functionName	method name string
 * @param[in] lineNumber	the line for registering
 * @param[in] message		debug information string
 */
void M2MLogger_printDebugMessage (const M2MString *functionName, const unsigned int lineNumber, const M2MString *message)
	{
#ifdef DEBUG
	//========== Variable ==========
	const size_t TIME_LENGTH = 128;
	M2MString TIME[TIME_LENGTH];

	//===== Check argument =====
	if (functionName!=NULL && message!=NULL)
		{
		//===== Initialize buffer =====
		memset(TIME, 0, TIME_LENGTH);
		//===== Get local time string =====
		if (M2MDate_getLocalTimeString(TIME, TIME_LENGTH)>0)
			{
			fprintf(stdout, (M2MString *)"[DEBUG]%s %s:%d[l]: %s\n", TIME, functionName, lineNumber, message);
			}
		else
			{
			}
		}
	//===== Argument error =====
	else if (functionName==NULL)
		{
		M2MLogger_printErrorMessage(__func__, __LINE__, (M2MString *)"Argument error! Indicated \"functionName\" string is NULL");
		}
	else
		{
		M2MLogger_printErrorMessage(__func__, __LINE__, (M2MString *)"Argument error! Indicated \"message\" string is NULL");
		}
#endif // DEBUG
	return;
	}


/**
 * Output error message to the standard error output.<br>
 * Error message is based on the function name, line number, and message.<br>
 * Also, when a pointer for copying error message is specified as argument, <br>
 * buffering is performed inside this function, and error message is copied.<br>
 *
 * @param[in] functionName					String indicating function name
 * @param[in] lineNumber					Line number in source file (can be embedded with "__LINE__")
 * @param[in] message						Message string indicating error content
 * @param[out] bufferForCopyingErrorMessage	Buffer to copy the error message or NULL (if copying is unnecessary)
 */
void M2MLogger_printErrorMessage (const M2MString *functionName, const unsigned int lineNumber, const M2MString *message, M2MString **bufferForCopyingErrorMessage)
	{
	//===== In the case of copying error message to argument =====
	if (bufferForCopyingErrorMessage!=NULL)
		{
		this_createNewErrorMessage(functionName, lineNumber, message, bufferForCopyingErrorMessage);
		}
	//===== In the case of not copying error message =====
	else
		{
		// do nothing
		}
	//===== Output error message into log file =====
	this_printErrorMessage(functionName, lineNumber, message);
	return;
	}


/**
 * This method print out error message and if the variable "errorMessage"<br>
 * isn't NULL, copy the message into it.<br>
 * If "errorMessage" isn't NULL, caller must release the memory out of<br>
 * this method.<br>
 * <br>
 * if defined "NOERROR" symbol, doesn't output message.<br>
 *
 * @param[in] functionName	method name string
 * @param[in] lineNumber	the line detected error
 * @param[in] message		error information string
 */
void M2MLogger_printErrorMessage (const M2MString *functionName, const unsigned int lineNumber, const M2MString *message)
	{
#ifndef NOERROR
	//========== Variable ==========
	const size_t TIME_LENGTH = 128;
	M2MString TIME[TIME_LENGTH];
	const size_t ERROR_MESSAGE_LENGTH = 256;
	M2MString errorMessage[ERROR_MESSAGE_LENGTH];

	//===== Check argument =====
	if (functionName!=NULL && message!=NULL)
		{
		//===== Initialize buffer =====
		memset(TIME, 0, TIME_LENGTH);
		memset(errorMessage, 0, ERROR_MESSAGE_LENGTH);
		//===== Get local time string =====
		if (M2MDate_getLocalTimeString(TIME, TIME_LENGTH)>0)
			{
			//===== Check existence of system error message =====
			if (errno!=0
					&& strerror_r(errno, errorMessage, ERROR_MESSAGE_LENGTH)==0)
				{
				fprintf(stderr, (M2MString *)"[ERROR]%s %s:%d[l]: %s: %s\n", TIME, functionName, lineNumber, message, errorMessage);
				//===== Initialize error number =====
				this_initErrorNumber();
				}
			//===== In the case of not existing error number and message =====
			else
				{
				fprintf(stderr, (M2MString *)"[ERROR]%s %s:%d[l]: %s\n", TIME, functionName, lineNumber, message);
				}
			}
		//===== Error handling =====
		else
			{
			}
		}
	//===== Argument error =====
	else if (functionName==NULL)
		{
		}
	else
		{
		}
#endif // NOERROR
	return;
	}


/**
 * This method prints out info level message if "DEBUG" symbol is set.<br>
 *
 * @param[in] functionName	method name string
 * @param[in] lineNumber	the line detected error
 * @param[in] message		error information string
 */
void M2MLogger_printInfoMessage (const M2MString *functionName, const unsigned int lineNumber, const M2MString *message)
	{
#ifdef DEBUG
	//========== Variable ==========
	const size_t TIME_LENGTH = 128;
	M2MString TIME[TIME_LENGTH];

	//===== Check argument =====
	if (functionName!=NULL && message!=NULL)
		{
		//===== Initialize buffer =====
		memset(TIME, 0, TIME_LENGTH);
		//===== Get local time string =====
		if (M2MDate_getLocalTimeString(TIME, TIME_LENGTH)>0)
			{
			fprintf(stdout, (M2MString *)"[INFO]%s %s:%d[l]: %s\n", TIME, functionName, lineNumber, message);
			}
		else
			{
			}
		}
	//===== Argument error =====
	else if (functionName==NULL)
		{
		M2MLogger_printErrorMessage(__func__, __LINE__, (M2MString *)"Argument error! Indicated \"functionName\" string is NULL");
		}
	else
		{
		M2MLogger_printErrorMessage(__func__, __LINE__, (M2MString *)"Argument error! Indicated \"message\" string is NULL");
		}
#endif // DEBUG
	return;
	}


/**
 * @param[in,out] self
 * @param[in] level
 * @return
 */
M2MLogger *M2MLogger_setLogLevel (M2MLogger *self, const M2MLogLevel level)
	{
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
		M2MLogger_printErrorMessage(__func__, __LINE__, (M2MString *)"Argument error! Indicated \"M2MLogger\" structure object is NULL");
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
			M2MLogger_printErrorMessage(__func__, __LINE__, (M2MString *)"Failed to allocate new memory for copying logger name string");
			return NULL;
			}
		}
	//===== Check argument =====
	else if (self==NULL)
		{
		M2MLogger_printErrorMessage(__func__, __LINE__, (M2MString *)"Argument error! Indicated \"M2MLogger\" structure object is NULL");
		return NULL;
		}
	else if (loggerName==NULL)
		{
		M2MLogger_printErrorMessage(__func__, __LINE__, (M2MString *)"Argument error! Indicated \"loggerName\" string is NULL");
		return NULL;
		}
	else
		{
		M2MLogger_printErrorMessage(__func__, __LINE__, (M2MString *)"Argument error! Length of indicated \"loggerName\" string isn't positive");
		return NULL;
		}
	}


/* End Of File */
