/*******************************************************************************
 * M2MFileAppender.c :
 *
 * Copyright (c) 2019, Akihisa Yasuda
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

#include "m2m/log/M2MFileAppender.h"


/*******************************************************************************
 * Definition
 ******************************************************************************/
/**
 * @param[in] self	M2MFileAppender structure object
 * @return			character set name string
 */
static unsigned char *this_getEncoding (const M2MFileAppender *self);


/**
 * This method returns log file path string.<br>
 *
 * @param[in] self	M2MFileAppender object
 * @return			log file path string
 */
static unsigned char *this_getLogFilePath (const M2MFileAppender *self);


/**
 * This method returns Logger object
 *
 * @param[in] self	M2MFileAppender object
 * @return			Logger object which is super(means base class) or NULL(means error)
 */
static M2MLogger *this_getSuper (const M2MFileAppender *self);


/**
 * This method initializes "errno".<br>
 */
static void this_initErrorNumber ();


/**
 * This method translates max file size string to number.<br>
 *
 * @param[in] maxFileSizeString	max file size string
 * @return						number of max file size[Byte]
 */
static unsigned int this_translateMaxFileSize (const M2MString *maxFileSizeString);



/*******************************************************************************
 * Private method
 ******************************************************************************/
/**
 * @param[in] a
 * @param[in] b
 * @return
 */
static int this_compareForQuickSort (const void *a, const void *b)
	{
	if ((*(int *)a)<(*(int *)b))
		{
		return 1;
		}
	else if ((*(int *)a)==(*(int *)b))
		{
		return 0;
		}
	else
		{
		return -1;
		}
	}


/**
 * This method creates new log message string.<br>
 *
 * @param[in] level			log leve
 * @param[in] methodName	method name string
 * @param[in] lineNumber	the line for registering
 * @param[in] message		debug information string
 * @param[out] log			pointer for copying created log message
 * @return					created log message or NULL(means error)
 */
static unsigned char *this_createLogMessage (const M2MLogLevel level, const M2MString *methodName, const unsigned int lineNumber, const M2MString *message, M2MString **log)
	{
	//========== Variable ==========
	M2MString *logLevel = NULL;
	size_t logLevelLength = 0;
	size_t methodNameLength = 0;
	M2MString LINE_NUMBER[64];
	size_t lineNumberLength = 0;
	size_t messageLength = 0;
	size_t logLength = 0;
	M2MString TIME[128];
	size_t timeLength = 0;
	M2MString ERROR_MESSAGE[256];
	size_t errorMessageLength = 0;

	//===== Check argument =====
	if ((logLevel=M2MLogLevel_toString(level))!=NULL && (logLevelLength=M2MString_length(logLevel))>0
			&& methodName!=NULL && (methodNameLength=M2MString_length(methodName))>0
			&& message!=NULL && (messageLength=M2MString_length(message))>0
			&& log!=NULL)
		{
		//===== Initialize buffer =====
		memset(TIME, 0, sizeof(TIME));
		memset(ERROR_MESSAGE, 0, sizeof(ERROR_MESSAGE));
		memset(LINE_NUMBER, 0, sizeof(LINE_NUMBER));
		//===== Convert from double to string =====
		if (M2MDate_getLocalTimeString(TIME, sizeof(TIME))>0
				&& (timeLength=M2MString_length(TIME))>0
				&& M2MString_format(LINE_NUMBER, sizeof(LINE_NUMBER)-1, "%d", lineNumber)>=0
				&& (lineNumberLength=M2MString_length(LINE_NUMBER))>0)
			{
			//===== Check existence of system error message =====
			if (errno!=0
					&& strerror_r(errno, ERROR_MESSAGE, sizeof(ERROR_MESSAGE))==0
					&& (errorMessageLength=M2MString_length(ERROR_MESSAGE))>0)
				{
				//===== Allocate new memory =====
				if ((logLength=logLevelLength+timeLength+methodNameLength+lineNumberLength+messageLength+errorMessageLength+12)>0
						&& ((*log)=(M2MString *)M2MHeap_malloc(logLength+1))!=NULL)
					{
					M2MString_format((*log), logLength, (M2MString *)"[%s]%s %s:%s[l]: %s: %s", logLevel, TIME, methodName, LINE_NUMBER, message, ERROR_MESSAGE);
					//===== Initialize error number =====
					this_initErrorNumber();
					return (*log);
					}
				//===== Error handling =====
				else
					{
					M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender.this_createLogMessage()", __LINE__, (M2MString *)"Failed to allocate new memory for copying log message string");
					//===== Initialize error number =====
					this_initErrorNumber();
					return NULL;
					}
				}
			//===== In the case of not existing error number and message =====
			else
				{
				//===== Allocate new memory =====
				if ((logLength=logLevelLength+timeLength+methodNameLength+lineNumberLength+messageLength+errorMessageLength+10)>0
						&& ((*log)=(M2MString *)M2MHeap_malloc(logLength+1))!=NULL)
					{
					M2MString_format((*log), logLength, (M2MString *)"[%s]%s %s:%s[l]: %s", logLevel, TIME, methodName, LINE_NUMBER, message);
					return (*log);
					}
				//===== Error handling =====
				else
					{
					M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender.this_createLogMessage()", __LINE__, (M2MString *)"Failed to allocate new memory for copying log message string");
					return NULL;
					}
				}
			}
		//===== Error handling =====
		else if (timeLength<=0)
			{
			M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender.this_createLogMessage()", __LINE__, (M2MString *)"Failed to get current time from local calendar");
			return NULL;
			}
		else
			{
			M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender.this_createLogMessage()", __LINE__, (M2MString *)"Failed to convert line number into string");
			return NULL;
			}
		}
	//===== Argument error =====
	else if (logLevel==NULL || logLevelLength<=0)
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender.this_createLogMessage()", __LINE__, (M2MString *)"Argument \"level\" is invalid");
		return NULL;
		}
	else if (methodName==NULL || methodNameLength<=0)
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender.this_createLogMessage()", __LINE__, (M2MString *)"Argument \"methodName\" is NULL or vacant");
		return NULL;
		}
	else if (message==NULL || messageLength<=0)
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender.this_createLogMessage()", __LINE__, (M2MString *)"Argument \"message\" is NULL or vacant");
		return NULL;
		}
	else
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender.this_createLogMessage()", __LINE__, (M2MString *)"Argument \"log\" is NULL");
		return NULL;
		}
	}


/**
 *
 * @param self
 */
static void this_deleteEncoding (M2MFileAppender *self)
	{
	if (self!=NULL)
		{
		if (this_getEncoding(self)!=NULL)
			{
			M2MHeap_free(self->encoding);
			}
		else
			{
			}
		}
	else
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender.this_deleteEncoding()", __LINE__, (M2MString *)"Argument \"M2MFileAppender\" is NULL");
		}
	return;
	}


/**
 *
 * @param self
 */
static void this_deleteLogFile (M2MFileAppender *self)
	{
	if (self!=NULL)
		{
		if (self->logFile!=NULL)
			{
			M2MFile_delete(&(self->logFile));
			}
		else
			{
			}
		}
	else
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender.this_deleteLogFile()", __LINE__, (M2MString *)"Argument \"M2MFileAppender\" is NULL");
		}
	return;
	}


/**
 *
 * @param[in,out] self
 */
static void this_deleteLogFilePath (M2MFileAppender *self)
	{
	if (self!=NULL)
		{
		if (this_getLogFilePath(self)!=NULL)
			{
			M2MHeap_free(self->logFilePath);
			}
		else
			{
			}
		}
	else
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender.this_deleteLogFilePath()", __LINE__, (M2MString *)"Argument \"M2MFileAppender\" is NULL");
		}
	return;
	}


/**
 * @param[in] self	M2MFileAppender structure object
 * @return			right or wring to append to logging
 */
static bool this_getAppend (const M2MFileAppender *self)
	{
	if (self!=NULL)
		{
		return self->append;
		}
	else
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender.this_getAppend()", __LINE__, (M2MString *)"Argument \"M2MFileAppender\" is NULL");
		return false;
		}
	}


/**
 * @param[in] self	M2MFileAppender structure object
 * @return			character set name string
 */
static unsigned char *this_getEncoding (const M2MFileAppender *self)
	{
	if (self!=NULL)
		{
		return self->encoding;
		}
	else
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender.this_getEncoding()", __LINE__, (M2MString *)"Argument \"M2MFileAppender\" is NULL");
		return NULL;
		}
	}


/**
 * This method returns opened log file object.<br>
 *
 * @param[in] self	logging object
 * @return			opened log file or NULL(means error)
 */
static M2MFile *this_getLogFile (M2MFileAppender *self)
	{
	//========== Variable ==========
	const M2MString *PERMISSION = (M2MString *)"0755";

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Check existence of file object =====
		if (self->logFile!=NULL)
			{
			//===== Check existence of opened file =====
			if (M2MFile_isClosed(self->logFile)==false)
				{
				return self->logFile;
				}
			else
				{
				if (this_getAppend(self)==true)
					{
					}
				else
					{
					M2MFile_remove(self->logFile);
					}
				//===== Open file =====
				if (M2MFile_open(self->logFile)!=NULL)
					{
					return self->logFile;
					}
				//===== Error handling =====
				else
					{
					M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender.this_getLogFile()", __LINE__, (M2MString *)"Failed to open log file");
					M2MFile_delete(&(self->logFile));
					return NULL;
					}
				}
			}
		//===== In the case of not existing file =====
		else
			{
			//===== Create new File object =====
			if ((self->logFile=M2MFile_new(this_getLogFilePath(self)))!=NULL)
				{
				//===== In the case of existing indicated file =====
				if (M2MFile_exists(self->logFile)==true)
					{
					if (this_getAppend(self)==true)
						{
						}
					else
						{
						M2MFile_remove(self->logFile);
						}
					//===== Open file =====
					if (M2MFile_open(self->logFile)!=NULL)
						{
						return self->logFile;
						}
					//===== Error handling =====
					else
						{
						M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender.this_getLogFile()", __LINE__, (M2MString *)"Failed to open log file");
						M2MFile_delete(&(self->logFile));
						return NULL;
						}
					}
				//===== In the case of not existing indicated file =====
				else
					{
					//===== Create and open file =====
					if (M2MFile_createNewFile(self->logFile, PERMISSION)!=NULL
							&& M2MFile_open(self->logFile)!=NULL)
						{
						return self->logFile;
						}
					//===== Error handling =====
					else
						{
						M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender.this_getLogFile()", __LINE__, (M2MString *)"Failed to open log file");
						M2MFile_delete(&(self->logFile));
						return NULL;
						}
					}
				}
			//===== Error handling =====
			else
				{
				M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender.this_getLogFile()", __LINE__, (M2MString *)"Failed to create new \"File\" object(which isn't a file)");
				return NULL;
				}
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender.this_getLogFile()", __LINE__, (M2MString *)"Argument \"M2MFileAppender\" is NULL");
		return NULL;
		}
	}


/**
 * This method returns log file path string.<br>
 *
 * @param[in] self	M2MFileAppender object
 * @return			log file path string
 */
static unsigned char *this_getLogFilePath (const M2MFileAppender *self)
	{
	//===== Check argument =====
	if (self!=NULL)
		{
		return self->logFilePath;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender.this_getLogFilePath()", __LINE__, (M2MString *)"Argument \"M2MFileAppender\" is NULL");
		return NULL;
		}
	}


/**
 * @param[in] self
 * @return
 */
static M2MLogLevel this_getLogLevel (const M2MFileAppender *self)
	{
	return M2MLogger_getLogLevel(this_getSuper(self));
	}


/**
 * @param[in] self
 * @return
 */
static unsigned int this_getMaxBackupIndex (const M2MFileAppender *self)
	{
	if (self!=NULL)
		{
		return self->maxBackupIndex;
		}
	else
		{
		return 0;
		}
	}


/**
 * @param[in] self
 * @return
 */
static unsigned int this_getMaxFileSize (const M2MFileAppender *self)
	{
	//========== Variable ==========
	const unsigned int DEFAULT_MAX_FILE_SIZE = 4194304;

	//===== Check argument =====
	if (self!=NULL)
		{
		//=====  =====
		if (self->maxFileSize>0)
			{
			return self->maxFileSize;
			}
		//=====  =====
		else
			{
			return DEFAULT_MAX_FILE_SIZE;
			}
		}
	//===== Argument error =====
	else
		{
		return DEFAULT_MAX_FILE_SIZE;
		}
	}


/**
 * This method returns Logger object
 *
 * @param[in] self	M2MFileAppender object
 * @return			Logger object which is super(means base class) or NULL(means error)
 */
static M2MLogger *this_getSuper (const M2MFileAppender *self)
	{
	if (self!=NULL)
		{
		return self->super;
		}
	else
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender.this_getSuper()", __LINE__, (M2MString *)"Argument \"M2MFileAppender\" is NULL");
		return NULL;
		}
	}


/**
 * This method initializes "errno".<br>
 */
static void this_initErrorNumber ()
	{
	errno = 0;
	return;
	}


/**
 * This method confirm "one" string including "another" string.<br>
 *
 * @param[in] one		base string which is longer
 * @param[in] another	confirm string which is shorter
 * @return				true : including, false : not including
 */
static bool this_isIncluding (const M2MString *one, const M2MString *another)
	{
	//========== Variable ==========
	size_t oneLength = 0;
	size_t anotherLength = 0;

	//===== Check argument =====
	if (one!=NULL && (oneLength=M2MString_length(one))>0
			&& another!=NULL && (anotherLength=M2MString_length(another))>0
			&& anotherLength<=oneLength
			&& M2MString_compare(one, another, anotherLength)==0)
		{
		return true;
		}
	//===== Argument error =====
	else
		{
		return false;
		}
	}


/**
 *
 * @param self
 * @return
 */
static bool this_needToEncoding (const M2MFileAppender *self)
	{
	//========== Variable ==========
	M2MString *encoding = NULL;
	size_t encodingLength = 0;

	//===== Check argument =====
	if (self!=NULL)
		{
		if ((encoding=this_getEncoding(self))!=NULL
				&& (encodingLength=M2MString_length(encoding))>0
				&& (M2MString_compare(M2MCharacterEncoding_UTF8, encoding, encodingLength)!=0
						|| M2MString_compare((M2MString *)"UTF8", encoding, encodingLength)!=0
						|| M2MString_compare((M2MString *)"utf-8", encoding, encodingLength)!=0
						|| M2MString_compare((M2MString *)"utf8", encoding, encodingLength)!=0))
			{
			return true;
			}
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
 * @param[in,out] self
 * @param[in] json
 * @return
 */
static M2MFileAppender *this_parseJSON (M2MFileAppender *self, M2MJSON *json)
	{
	//========== Variable ==========
	M2MString *maxBackupIndex = NULL;

	//===== Check argument =====
	if (self!=NULL && json!=NULL)
		{
		//===== Set-up configuration =====
		if (M2MFileAppender_setLoggerName(self, M2MJSON_getString(M2MJSONPointer_evaluate(json, (unsigned char *)"\"/Logger/Name\"")))!=NULL
				&& M2MFileAppender_setEncoding(self, M2MJSON_getString(M2MJSONPointer_evaluate(json, (unsigned char *)"\"/FileAppender/Encoding\"")))!=NULL
				&& M2MFileAppender_setLogFilePath(self, M2MJSON_getString(M2MJSONPointer_evaluate(json, (unsigned char *)"\"/FileAppender/File\"")))!=NULL
				&& M2MFileAppender_setAppend(self, M2MString_convertFromStringToBoolean(M2MJSON_getString(M2MJSONPointer_evaluate(json, (unsigned char *)"\"/FileAppender/Append\""))))!=NULL
				&& M2MFileAppender_setLogLevel(self, M2MLogLevel_translateString(M2MJSON_getString(M2MJSONPointer_evaluate(json, (unsigned char *)"\"/Logger/Level\""))))!=NULL
				&& (maxBackupIndex = M2MJSON_getString(M2MJSONPointer_evaluate(json, (unsigned char *)"\"/FileAppender/MaxBackupIndex\"")))!=NULL
				&& M2MFileAppender_setMaxBackupIndex(self, M2MString_convertFromStringToUnsignedInteger(maxBackupIndex, M2MString_length(maxBackupIndex)))!=NULL
				&& M2MFileAppender_setMaxFileSize(self, this_translateMaxFileSize(M2MJSON_getString(M2MJSONPointer_evaluate(json, (unsigned char *)"\"/FileAppender/MaxFileSize\""))))!=NULL)
			{
			return self;
			}
		//===== Error handling =====
		else
			{
			M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender.this_parseJSON()", __LINE__, (M2MString *)"Argument \"JSON\" includes error configuration");
			return NULL;
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender.this_parseJSON()", __LINE__, (M2MString *)"Argument \"M2MFileAppender\" is NULL");
		return NULL;
		}
	else
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender.this_parseJSON()", __LINE__, (M2MString *)"Argument \"JSON\" is NULL");
		return NULL;
		}
	}


/**
 * @param[in] parentDirectoryPath
 * @param[in] fileName
 */
static void this_removeLogFile (const M2MString *parentDirectoryPath, const M2MString *fileName)
	{
	//========== Variable ==========
	M2MString *filePath = NULL;
	M2MFile *file = NULL;

	//===== Check argument =====
	if (parentDirectoryPath!=NULL && fileName!=NULL)
		{
		//===== Construct file pathname string =====
		if (M2MString_append(&filePath, parentDirectoryPath)!=NULL
				&& M2MString_append(&filePath, M2MDirectory_SEPARATOR)!=NULL
				&& M2MString_append(&filePath, fileName)!=NULL)
			{
			//===== Create new File object =====
			if ((file=M2MFile_new(filePath))!=NULL)
				{
				//===== Remove the file =====
				M2MFile_remove(file);
				//===== Delete File object =====
				M2MFile_delete(&file);
				}
			//===== Error handling =====
			else
				{
				M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender.this_removeLogFile()", __LINE__, (M2MString *)"Failed to create new \"File\" object for removing");
				}
			//===== Release allocated memory =====
			M2MHeap_free(filePath);
			}
		//===== Error handling =====
		else
			{
			M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender.this_removeLogFile()", __LINE__, (M2MString *)"Failed to construct file pathname string for removing");
			}
		}
	//===== Argument error =====
	else if (parentDirectoryPath==NULL)
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender.this_removeLogFile()", __LINE__, (M2MString *)"Argument \"parentDirectoryPath\" is NULL");
		}
	else
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender.this_removeLogFile()", __LINE__, (M2MString *)"Argument \"fileName\" is NULL");
		}
	return;
	}


/**
 * @param parentDirectoryPath
 * @param olfFileName
 * @param newFileName
 */
static void this_renameLogFile (const M2MString *parentDirectoryPath, const M2MString *oldFileName, const M2MString *newFileName)
	{
	//========== Variable ==========
	M2MString *oldFilePath = NULL;
	M2MFile *file = NULL;

	//===== Check argument =====
	if (parentDirectoryPath!=NULL && oldFileName!=NULL && newFileName!=NULL)
		{
		//===== Construct file pathname string =====
		if (M2MString_append(&oldFilePath, parentDirectoryPath)!=NULL
				&& M2MString_append(&oldFilePath, M2MDirectory_SEPARATOR)!=NULL
				&& M2MString_append(&oldFilePath, oldFileName)!=NULL)
			{
			//===== Rename the file =====
			if ((file=M2MFile_new(oldFilePath))!=NULL
					&& (file=M2MFile_rename(&file, newFileName))!=NULL)
				{
				M2MFile_delete(&file);
				}
			//===== Error handling =====
			else
				{
				M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender.this_renameLogFile()", __LINE__, (M2MString *)"Failed to rename \"File\" object");
				}
			//===== Release allocated memory =====
			M2MHeap_free(oldFilePath);
			}
		//===== Error handling =====
		else
			{
			M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender.this_renameLogFile()", __LINE__, (M2MString *)"Failed to construct file pathname string for renaming");
			}
		}
	//===== Argument error =====
	else if (parentDirectoryPath==NULL)
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender.this_renameLogFile()", __LINE__, (M2MString *)"Argument \"parentDirectoryPath\" is NULL");
		}
	else if (oldFileName==NULL)
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender.this_renameLogFile()", __LINE__, (M2MString *)"Argument \"oldFileName\" is NULL");
		}
	else
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender.this_renameLogFile()", __LINE__, (M2MString *)"Argument \"newFileName\" is NULL");
		}
	return;
	}


/**
 * This method translates max file size string to number.<br>
 *
 * @param[in] maxFileSizeString	max file size string
 * @return						number of max file size[Byte]
 */
static unsigned int this_translateMaxFileSize (const M2MString *maxFileSizeString)
	{
	//========== Variable ==========
	size_t maxFileSizeLength = 0;
	unsigned char *number = NULL;
	unsigned char *unit = NULL;
	unsigned int index = 0;
	unsigned int maxFileSize = 0;
	const unsigned int KILO = 1024;
	const unsigned int MEGA = 1048576;
	const unsigned int GIGA = 1073741824;

	//===== Check argument =====
	if (maxFileSizeString!=NULL && (maxFileSizeLength=M2MString_length(maxFileSizeString))>0)
		{
		//===== Loop while existing string =====
		for (index=0; index<maxFileSizeLength; index++)
			{
			//===== In the case of number =====
			if (M2MString_isNumber(maxFileSizeString[index])==true)
				{
				continue;
				}
			else
				{
				break;
				}
			}
		if ((number=(unsigned char *)M2MHeap_malloc(index+1))!=NULL)
			{
			memcpy(number, maxFileSizeString, index);
			maxFileSize = M2MString_convertFromStringToUnsignedInteger(number, index);
			M2MHeap_free(number);
			if (maxFileSizeLength==index)
				{
				return maxFileSize;
				}
			else
				{
				if ((unit=(unsigned char *)M2MHeap_malloc(maxFileSizeLength-index+1))!=NULL)
					{
					memcpy(unit, &maxFileSizeString[index], maxFileSizeLength-index);
					//===== In the case of "K" or "k" =====
					if (this_isIncluding(unit, (unsigned char *)"K")==true
							|| this_isIncluding(unit, (unsigned char *)"k")==true)
						{
						M2MHeap_free(unit);
						return maxFileSize * KILO;
						}
					//===== In the case of "M" or "m" =====
					else if (this_isIncluding(unit, (unsigned char *)"M")==true
							|| this_isIncluding(unit, (unsigned char *)"m")==true)
						{
						M2MHeap_free(unit);
						return maxFileSize * MEGA;
						}
					//===== In the case of "G" or "g" =====
					else if (this_isIncluding(unit, (unsigned char *)"G")==true
							|| this_isIncluding(unit, (unsigned char *)"g")==true)
						{
						M2MHeap_free(unit);
						return maxFileSize * GIGA;
						}
					else
						{
						M2MHeap_free(unit);
						return maxFileSize;
						}
					}
				else
					{
					return MEGA;
					}
				}
			}
		else
			{
			return MEGA;
			}
		}
	//===== Argument error =====
	else if (maxFileSizeString==NULL)
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender_translateMaxFileSize()", __LINE__, (M2MString *)"Argument \"maxFileSizeString\" is NULL");
		return MEGA;
		}
	else
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender_translateMaxFileSize()", __LINE__, (M2MString *)"Length of argument \"maxFileSizeString\" isn't positive");
		return MEGA;
		}
	}


/**
 * @param[in] self	logging object
 */
static void this_updateExistingLogFiles (M2MFileAppender *self)
	{
	//========== Variable ==========
	M2MFile *logFile = NULL;
	M2MString *logFileName = NULL;
	size_t logFileNameLength = 0;
	M2MString *parentDirectoryPath = NULL;
	M2MList *fileNameList = NULL;
	M2MString *fileName = NULL;
	M2MString *oldFileName = NULL;
	M2MString *newFileName = NULL;
	M2MString numberString[64];
	unsigned int *backupIndexArray = NULL;
	unsigned int numberOfArray = 0;
	unsigned int i = 0;
	unsigned char *index = NULL;
	unsigned int number = 0;
	const unsigned int MAX_BACKUP_INDEX = this_getMaxBackupIndex(self);

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Get log file object =====
		if ((logFile=this_getLogFile(self))!=NULL
				&& (logFileName=M2MFile_getName(logFile))!=NULL
				&& (logFileNameLength=M2MString_length(logFileName))>0)
			{
			//===== Get pathname string of parent directory =====
			if (M2MFile_getParentDirectoryPath(logFile, &parentDirectoryPath)!=NULL)
				{
				//===== Get file list =====
				if ((fileNameList=M2MList_begin(M2MDirectory_listFiles(parentDirectoryPath)))!=NULL
						&& (backupIndexArray=(unsigned int *)M2MHeap_malloc(sizeof(unsigned int) * (M2MList_length(fileNameList) + 1)))!=NULL)
					{
					numberOfArray = 0;
					//===== Loop while existing files =====
					while (M2MList_next(fileNameList)!=NULL)
						{
						//===== In the case of detected target file name =====
						if ((fileName=(M2MString *)M2MList_getValue(fileNameList))!=NULL
								&& this_isIncluding(fileName, logFileName)==true)
							{
							//===== In the case of including "_" in file name =====
							if ((index=M2MString_indexOf(fileName, M2MString_UNDER_LINE))!=NULL
									&& (index+=M2MString_length(M2MString_UNDER_LINE))!=NULL)
								{
								//===== Get index number of backup file =====
								backupIndexArray[numberOfArray] = (unsigned int)M2MString_convertFromStringToUnsignedInteger(index, M2MString_length(index));
								//===== Increment index number =====
								numberOfArray++;
								}
							//===== In the case of existing only 1 backup file =====
							else if (M2MString_compare(fileName, logFileName, logFileNameLength)==0)
								{
								//===== Get index number of backup file =====
								backupIndexArray[numberOfArray] = 0;
								//===== Increment index number =====
								numberOfArray++;
								}
							//===== In the case of failed to detect backup file =====
							else
								{
								M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender.this_updateExistingLogFiles()", __LINE__, (M2MString *)"Failed to detect backup file name");
								}
							}
						//===== Error handling =====
						else
							{
							}
						//=====  =====
						fileNameList = M2MList_next(fileNameList);
						}
					//===== In the case of detected target file name =====
					if ((fileName=(M2MString *)M2MList_getValue(fileNameList))!=NULL
							&& this_isIncluding(fileName, logFileName)==true)
						{
						//===== In the case of including "_" in file name =====
						if ((index=M2MString_indexOf(fileName, M2MString_UNDER_LINE))!=NULL
								&& (index+=M2MString_length(M2MString_UNDER_LINE))!=NULL)
							{
							backupIndexArray[numberOfArray] = (unsigned int)M2MString_convertFromStringToUnsignedInteger(index, M2MString_length(index));
							numberOfArray++;
							}
						//===== In the case of existing only 1 backup file =====
						else if (M2MString_compare(fileName, logFileName, logFileNameLength)==0)
							{
							backupIndexArray[numberOfArray] = 0;
							numberOfArray++;
							}
						//===== In the case of failed to detect backup file =====
						else
							{
							M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender.this_updateExistingLogFiles()", __LINE__, (M2MString *)"Failed to detect backup file name");
							}
						}
					//===== Error handling =====
					else
						{
						}
					//===== Release allocated memory for List object =====
					M2MList_delete(fileNameList);
					//===== Sort array with number =====
					qsort(backupIndexArray, numberOfArray, sizeof(unsigned int), this_compareForQuickSort);
					//=====  =====
					M2MFile_close(logFile);
					//=====  =====
					if (numberOfArray<=MAX_BACKUP_INDEX)
						{
						for (i=0; i<numberOfArray; i++)
							{
							if (i==numberOfArray-1)
								{
								number = backupIndexArray[i];
								number++;
								M2MString_append(&newFileName, logFileName);
								M2MString_append(&newFileName, M2MString_UNDER_LINE);
								M2MString_append(&newFileName, M2MString_convertFromUnsignedIntegerToString(number, numberString, sizeof(numberString)));
								this_renameLogFile(parentDirectoryPath, logFileName, newFileName);
								M2MHeap_free(newFileName);
								}
							else
								{
								number = backupIndexArray[i];
								M2MString_append(&oldFileName, logFileName);
								M2MString_append(&oldFileName, M2MString_UNDER_LINE);
								M2MString_append(&oldFileName, M2MString_convertFromUnsignedIntegerToString(number, numberString, sizeof(numberString)));
								number++;
								M2MString_append(&newFileName, logFileName);
								M2MString_append(&newFileName, M2MString_UNDER_LINE);
								M2MString_append(&newFileName, M2MString_convertFromUnsignedIntegerToString(number, numberString, sizeof(numberString)));
								this_renameLogFile(parentDirectoryPath, oldFileName, newFileName);
								M2MHeap_free(oldFileName);
								M2MHeap_free(newFileName);
								}
							}
						}
					else
						{
						for (i=0; i<numberOfArray; i++)
							{
							if (i<(numberOfArray-MAX_BACKUP_INDEX))
								{
								number = backupIndexArray[i];
								M2MString_append(&oldFileName, logFileName);
								M2MString_append(&oldFileName, M2MString_UNDER_LINE);
								M2MString_append(&oldFileName, M2MString_convertFromUnsignedIntegerToString(number, numberString, sizeof(numberString)));
								this_removeLogFile(parentDirectoryPath, oldFileName);
								M2MHeap_free(oldFileName);
								}
							else if (i==(numberOfArray-1))
								{
								number = backupIndexArray[i];
								number++;
								M2MString_append(&newFileName, logFileName);
								M2MString_append(&newFileName, M2MString_UNDER_LINE);
								M2MString_append(&newFileName, M2MString_convertFromUnsignedIntegerToString(number, numberString, sizeof(numberString)));
								this_renameLogFile(parentDirectoryPath, logFileName, newFileName);
								M2MHeap_free(newFileName);
								}
							else
								{
								number = backupIndexArray[i];
								M2MString_append(&oldFileName, logFileName);
								M2MString_append(&oldFileName, M2MString_UNDER_LINE);
								M2MString_append(&oldFileName, M2MString_convertFromUnsignedIntegerToString(number, numberString, sizeof(numberString)));
								number++;
								M2MString_append(&newFileName, logFileName);
								M2MString_append(&newFileName, M2MString_UNDER_LINE);
								M2MString_append(&newFileName, M2MString_convertFromUnsignedIntegerToString(number, numberString, sizeof(numberString)));
								this_renameLogFile(parentDirectoryPath, oldFileName, newFileName);
								M2MHeap_free(oldFileName);
								M2MHeap_free(newFileName);
								}
							}
						}
					M2MHeap_free(backupIndexArray);
					}
				//=====  =====
				else
					{
					}
				M2MHeap_free(parentDirectoryPath);
				}
			//=====  =====
			else
				{
				}
			}
		//=====  =====
		else
			{
			}
		}
	//=====  =====
	else
		{
		}
	return;
	}


/**
 * This method write log message into log file.<br>
 *
 * @param[in] self		logging object
 * @param[in] log		log message string
 */
static void this_writeLog (M2MFileAppender *self, const M2MString *log)
	{
	//========== Variable ==========
	M2MFile *logFile = NULL;
	M2MString *encodedMessage = NULL;
	unsigned long fileLength = 0;

	//===== Check argument =====
	if (self!=NULL && log!=NULL)
		{
		//===== Get file =====
		if ((logFile=this_getLogFile(self))!=NULL)
			{
			//===== In the case of need encoding =====
			if (this_needToEncoding(self)==true)
				{
				if (M2MString_convertCharacterSet(log, M2MCharacterEncoding_UTF8, this_getEncoding(self), &encodedMessage)!=NULL)
					{
					//===== Write log message =====
					if (M2MFile_write(logFile, encodedMessage, M2MString_length(encodedMessage))>0)
						{
						//===== Write line feed =====
						M2MFile_write(logFile, M2MString_CRLF, M2MString_length(M2MString_CRLF));
						}
					else
						{
						}
					M2MHeap_free(encodedMessage);
					}
				else
					{
					//===== Write log message =====
					if (M2MFile_write(logFile, log, M2MString_length(log))>0)
						{
						//===== Write line feed =====
						M2MFile_write(logFile, M2MString_CRLF, M2MString_length(M2MString_CRLF));
						}
					else
						{
						}
					}
				}
			//===== In the case of not need encoding =====
			else
				{
				//===== Write log message =====
				if (M2MFile_write(logFile, log, M2MString_length(log))>0)
					{
					//===== Write line feed =====
					M2MFile_write(logFile, M2MString_CRLF, M2MString_length(M2MString_CRLF));
					}
				else
					{
					}
				}
			//===== Check file size =====
			if ((fileLength=M2MFile_length(logFile))<this_getMaxFileSize(self))
				{
				// do nothing
				}
			//===== In case of exceeding max file size =====
			else
				{
				//===== Rename existing log files =====
				this_updateExistingLogFiles(self);
				}
			}
		//===== Error handling =====
		else
			{
			M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender.this_writeLog()", __LINE__, (M2MString *)"Failed to open log file");
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender.this_writeLog()", __LINE__, (M2MString *)"Argument \"M2MFileAppender\" is NULL");
		}
	else
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender.this_writeLog()", __LINE__, (M2MString *)"Argument \"message\" is NULL or vacant");
		}
	return;
	}


/*******************************************************************************
 * Public method
 ******************************************************************************/
/**
 *
 * @param[in] self
 * @param[in] methodName
 * @param[in] lineNumber
 * @param[in] message
 */
void M2MLogger_infoImpl (M2MFileAppender *self, const M2MString *methodName, const unsigned int lineNumber, const M2MString *message)
	{
	//========== Variable ==========
	M2MString *log = NULL;

	//===== Check argument =====
	if (self!=NULL && this_getLogLevel(self)<=M2MLogLevel_INFO)
		{
		//===== Get log message =====
		if (this_createLogMessage(M2MLogLevel_INFO, methodName, lineNumber, message, &log)!=NULL)
			{
			//===== Write log =====
			this_writeLog(self, log);
			M2MHeap_free(log);
			}
		//===== Error handling =====
		else
			{
			this_writeLog(self, message);
			}
		}
	//===== Don't need to write =====
	else
		{
		}
	return;
	}


/**
 * @param[in] self
 * @param[in] message
 */
void M2MLogger_debugImpl (M2MFileAppender *self, const M2MString *methodName, const unsigned int lineNumber, const M2MString *message)
	{
	//========== Variable ==========
	M2MString *log = NULL;

	//===== Check argument =====
	if (self!=NULL && this_getLogLevel(self)<=M2MLogLevel_DEBUG)
		{
		//===== Get log message =====
		if (this_createLogMessage(M2MLogLevel_DEBUG, methodName, lineNumber, message, &log)!=NULL)
			{
			//===== Write log =====
			this_writeLog(self, log);
			M2MHeap_free(log);
			}
		//===== Error handling =====
		else
			{
			this_writeLog(self, message);
			}
		}
	//===== Don't need to write =====
	else
		{
		}
	}


/**
 * @param[in] self
 * @param[in] message
 */
void M2MLogger_traceImpl (M2MFileAppender *self, const M2MString *methodName, const unsigned int lineNumber, const M2MString *message)
	{
	//========== Variable ==========
	M2MString *log = NULL;

	//===== Check argument =====
	if (self!=NULL && this_getLogLevel(self)<=M2MLogLevel_TRACE)
		{
		//===== Get log message =====
		if (this_createLogMessage(M2MLogLevel_TRACE, methodName, lineNumber, message, &log)!=NULL)
			{
			//===== Write log =====
			this_writeLog(self, log);
			M2MHeap_free(log);
			}
		//===== Error handling =====
		else
			{
			this_writeLog(self, message);
			}
		}
	//===== Don't need to write =====
	else
		{
		}
	}


/**
 * @param[in] self
 * @param[in] message
 */
void M2MLogger_warnImpl (M2MFileAppender *self, const M2MString *methodName, const unsigned int lineNumber, const M2MString *message)
	{
	//========== Variable ==========
	M2MString *log = NULL;

	//===== Check argument =====
	if (self!=NULL && this_getLogLevel(self)<=M2MLogLevel_WARN)
		{
		//===== Get log message =====
		if (this_createLogMessage(M2MLogLevel_WARN, methodName, lineNumber, message, &log)!=NULL)
			{
			//===== Write log =====
			this_writeLog(self, log);
			M2MHeap_free(log);
			}
		//===== Error handling =====
		else
			{
			this_writeLog(self, message);
			}
		}
	//===== Don't need to write =====
	else
		{
		}
	}


/**
 * @param[in] self
 * @param[in] message
 */
void M2MLogger_fatalImpl (M2MFileAppender *self, const M2MString *methodName, const unsigned int lineNumber, const M2MString *message)
	{
	//========== Variable ==========
	M2MString *log = NULL;

	//===== Check argument =====
	if (self!=NULL && this_getLogLevel(self)<=M2MLogLevel_FATAL)
		{
		//===== Get log message =====
		if (this_createLogMessage(M2MLogLevel_FATAL, methodName, lineNumber, message, &log)!=NULL)
			{
			//===== Write log =====
			this_writeLog(self, log);
			M2MHeap_free(log);
			}
		//===== Error handling =====
		else
			{
			this_writeLog(self, message);
			}
		}
	//===== Don't need to write =====
	else
		{
		}
	}


/**
 * @param[in] self
 * @param[in] message
 */
void M2MLogger_errorImpl (M2MFileAppender *self, const M2MString *methodName, const unsigned int lineNumber, const M2MString *message)
	{
	//========== Variable ==========
	M2MString *log = NULL;

	//===== Check argument =====
	if (self!=NULL && this_getLogLevel(self)<=M2MLogLevel_ERROR)
		{
		//===== Get log message =====
		if (this_createLogMessage(M2MLogLevel_ERROR, methodName, lineNumber, message, &log)!=NULL)
			{
			//===== Write log =====
			this_writeLog(self, log);
			M2MHeap_free(log);
			}
		//===== Error handling =====
		else
			{
			this_writeLog(self, message);
			}
		}
	//===== Don't need to write =====
	else
		{
		}
	}


/**
 * @return	new logger object(caller must release this memory with "M2MHeap_free()") or NULL(means error)
 */
M2MFileAppender *M2MFileAppender_createNewFileAppender ()
	{
	//========== Variable ==========
	M2MFileAppender *self = NULL;

	//===== Allocate new memory from fileAppender =====
	if ((self=(M2MFileAppender *)M2MHeap_malloc(sizeof(M2MFileAppender)))!=NULL)
		{
		//===== Allocate new memory from super object =====
		if ((self->super=M2MLogger_new())!=NULL
				&& M2MFileAppender_setAppend(self, false)!=NULL
				&& M2MFileAppender_setEncoding(self, M2MCharacterEncoding_UTF8)!=NULL
				&& M2MFileAppender_setMaxFileSize(self, 0)!=NULL
				&& M2MFileAppender_setMaxBackupIndex(self, 0)!=NULL)
			{
			self->logFile = NULL;
			self->logFilePath = NULL;
			return self;
			}
		//===== Error handling =====
		else
			{
			M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender_createNewM2MFileAppender()", __LINE__, (M2MString *)"Failed to allocate new memory for creating \"Logger\" structure object");
			M2MFileAppender_delete(&self);
			return NULL;
			}
		}
	//===== Error handling =====
	else
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender_createNewM2MFileAppender()", __LINE__, (M2MString *)"Failed to allocate new memory for creating \"M2MFileAppender\" structure object");
		return NULL;
		}
	}


/**
 * @param[in] this
 */
void M2MFileAppender_delete (M2MFileAppender **self)
	{
	//===== Check argument =====
	if (self!=NULL && (*self)!=NULL)
		{
		//===== Delete Logger object =====
		M2MLogger_delete(&((*self)->super));
		//===== Delete encoding string =====
		this_deleteEncoding((*self));
		//===== Delete File object =====
		this_deleteLogFile((*self));
		//===== Delete log file path string =====
		this_deleteLogFilePath((*self));
		//===== Delete M2MFileAppender object =====
		M2MHeap_free(*self);
		return;
		}
	//===== Argument error =====
	else
		{
		}
	return;
	}


/**
 * This method copies log file pathname string into indicated buffer.<br>
 * The pathname independent with backbeat library, so if caller won't<br>
 * use the pathname, it isn't suitable for calling this method.<br>
 * The pathname is follow.<br>
 * <br>
 * ~/.backbeat/log/(loggerName).log<br>
 *
 * @param[in] loggerName	logger name string
 * @param[out] buffer		buffer for copying log file pathname string
 * @param[in] bufferLength	lengh of buffer[Byte]
 * @return
 */
M2MString *M2MFileAppender_getLogFilePath (const M2MString *loggerName, M2MString *buffer, const size_t bufferLength)
	{
	//===== Check argument =====
	if (loggerName!=NULL && bufferLength>0)
		{
		memset(buffer, 0, bufferLength);
		M2MString_format(buffer, bufferLength-1, (unsigned char *)"%s%s.backbeat%slog%s%s.log", M2MDirectory_getHomeDirectoryPath(), M2MDirectory_SEPARATOR, M2MDirectory_SEPARATOR, M2MDirectory_SEPARATOR, loggerName);
		return buffer;
		}
	//===== Argument error =====
	else if (loggerName==NULL)
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender_getLogFilePath()", __LINE__, (M2MString *)"Argument \"loggerName\" is NULL");
		return NULL;
		}
	else
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender_getLogFilePath()", __LINE__, (M2MString *)"Argument \"bufferLength\" isn't positive");
		return NULL;
		}
	}


/**
 * @param[in] self
 * @return
 */
M2MLogLevel M2MFileAppender_getLogLevel (const M2MFileAppender *self)
	{
	if (self!=NULL)
		{
		return this_getLogLevel(self);
		}
	else
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender_getLogLevel()", __LINE__, (M2MString *)"Argument \"M2MFileAppender\" is NULL");
		return M2MLogLevel_INFO;
		}
	}


/**
 * @param[in] self
 * @return
 */
M2MString *M2MFileAppender_getLoggerName (const M2MFileAppender *self)
	{
	if (self!=NULL)
		{
		return M2MLogger_getLoggerName(this_getSuper(self));
		}
	else
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender_getLoggerName()", __LINE__, (M2MString *)"Argument \"M2MFileAppender\" is NULL");
		return NULL;
		}
	}


/**
 * @param[in] jsonFilePath	configuration file path
 * @return					logging object or NULL(error happened)
 */
M2MFileAppender *M2MFileAppender_parseJSONFile (const M2MString *jsonFilePath)
	{
	//========== Variable ==========
	M2MJSON *json = NULL;
	M2MFileAppender *self = NULL;

	//===== Check argument =====
	if (jsonFilePath!=NULL)
		{
		//===== Parse JSON file =====
		if ((json=M2MJSONParser_parseFile(jsonFilePath))!=NULL)
			{
			//===== Create new logging object =====
			if ((self=M2MFileAppender_createNewFileAppender())!=NULL)
				{
				//===== Parse JSON object for configuration =====
				if (this_parseJSON(self, json)!=NULL)
					{
					M2MJSON_delete(&json);
					return self;
					}
				//===== Error handling =====
				else
					{
					M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender_parseJSONFile()", __LINE__, (M2MString *)"Failed to parse \"JSON\" object for configure \"M2MFileAppender\" object");
					M2MJSON_delete(&json);
					M2MFileAppender_delete(&self);
					return NULL;
					}
				}
			//===== Error handling =====
			else
				{
				M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender_parseJSONFile()", __LINE__, (M2MString *)"Failed to allocate new memory for creating \"M2MFileAppender\" object");
				M2MJSON_delete(&json);
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender_parseJSONFile()", __LINE__, (M2MString *)"Failed to parse argument \"jsonFilePath\" configuration file");
			return NULL;
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender_parseJSONFile()", __LINE__, (M2MString *)"Argument \"jsonFilePath\" string is NULL");
		return NULL;
		}
	}


/**
 * This method parses JSON format string for constructing FileAppendar object.<br>
 *
 * @param[in] jsonString	JSON format configuration string
 * @return					new created logging object or NULL(means error)
 */
M2MFileAppender *M2MFileAppender_parseJSONString (const M2MString *jsonString)
	{
	//========== Variable ==========
	M2MFileAppender *self = NULL;
	M2MJSON *json = NULL;

	//===== Check argument =====
	if (jsonString!=NULL)
		{
		//===== Parse JSON format string =====
		if ((json=M2MJSONParser_parseString(jsonString))!=NULL)
			{
			//===== Create new logging object =====
			if ((self=M2MFileAppender_createNewFileAppender())!=NULL)
				{
				//===== Parse JSON object for configuration =====
				if (this_parseJSON(self, json)!=NULL)
					{
					M2MJSON_delete(&json);
					return self;
					}
				//===== Error handling =====
				else
					{
					M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender_parseJSONString()", __LINE__, (M2MString *)"Failed to parse JSON object for configure \"M2MFileAppender\" object");
					M2MJSON_delete(&json);
					M2MFileAppender_delete(&self);
					return NULL;
					}
				}
			//===== Error handling =====
			else
				{
				M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender_parseJSONString()", __LINE__, (M2MString *)"Failed to allocate new memory for creating \"M2MFileAppender\" object");
				M2MJSON_delete(&json);
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender_parseJSONString()", __LINE__, (M2MString *)"Failed to parse argument \"jsonString\"");
			return NULL;
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender_parseJSONString()", __LINE__, (M2MString *)"Argument \"jsonString\" is NULL");
		return NULL;
		}
	}


/**
 * @param[in] appender
 * @param[in] flag
 */
M2MFileAppender *M2MFileAppender_setAppend (M2MFileAppender *self, const bool flag)
	{
	//===== Check argument =====
	if (self!=NULL)
		{
		self->append = flag;
		return self;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender_setAppend()", __LINE__, (M2MString *)"Argument \"M2MFileAppender\" is NULL");
		return NULL;
		}
	}


/**
 * @param[in] this
 * @param[in] encoding
 * @return
 */
M2MString *M2MFileAppender_setEncoding (M2MFileAppender *self, const M2MString *encoding)
	{
	//========== Variable ==========
	unsigned int encodingLength = 0;

	//===== Check argument =====
	if (self!=NULL && encoding!=NULL  && (encodingLength=M2MString_length(encoding))>0)
		{
		//===== Initialization =====
		this_deleteEncoding(self);
		//===== Allocate new memory =====
		if ((self->encoding=(M2MString *)M2MHeap_malloc(encodingLength+1))!=NULL)
			{
			memcpy(self->encoding, encoding, encodingLength);
			return self->encoding;
			}
		//===== Error handling =====
		else
			{
			M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender_setEncoding()", __LINE__, (M2MString *)"Failed to allocate new memory for copying encoding character set string");
			return NULL;
			}
		}
	//===== Error handling =====
	else if (self==NULL)
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender_setEncoding()", __LINE__, (M2MString *)"Argument \"M2MFileAppender\" is NULL");
		return NULL;
		}
	else if (encoding==NULL)
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender_setEncoding()", __LINE__, (M2MString *)"Argument \"encoding\" is NULL");
		return NULL;
		}
	else
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender_setEncoding()", __LINE__, (M2MString *)"Length of argument \"encoding\" isn't positive");
		return NULL;
		}
	}


/**
 * @param[in] self
 * @param[in] filePath
 * @return
 */
unsigned char *M2MFileAppender_setLogFilePath (M2MFileAppender *self, const M2MString *logFilePath)
	{
	//========== Variable ==========
	unsigned int logFilePathLength = 0;

	//===== Check argument =====
	if (self!=NULL && logFilePath!=NULL && (logFilePathLength=M2MString_length(logFilePath))>0)
		{
		//===== Initialize buffer =====
		this_deleteLogFilePath(self);
		//===== Allocate new memory for copying filePath =====
		if ((self->logFilePath=(M2MString *)M2MHeap_malloc(logFilePathLength+1))!=NULL)
			{
			memcpy(self->logFilePath, logFilePath, logFilePathLength);
			return self->logFilePath;
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
		M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender_setLogFilePath()", __LINE__, (M2MString *)"Argument \"M2MFileAppender\" is NULL");
		return NULL;
		}
	else if (logFilePath==NULL)
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender_setLogFilePath()", __LINE__, (M2MString *)"Argument \"logFilePath\" is NULL");
		return NULL;
		}
	else
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender_setLogFilePath()", __LINE__, (M2MString *)"Length of argument \"logFilePath\" isn't positive");
		return NULL;
		}
	}


/**
 * @param[in] self
 * @param[in] level
 */
M2MFileAppender *M2MFileAppender_setLogLevel (M2MFileAppender *self, const M2MLogLevel level)
	{
	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Set log level =====
		if (M2MLogger_setLogLevel(this_getSuper(self), level)!=NULL)
			{
			return self;
			}
		//===== Error handling =====
		else
			{
			M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender_setLevel()", __LINE__, (M2MString *)"Failed to set log level into \"Logger\" object");
			return NULL;
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender_setLevel()", __LINE__, (M2MString *)"Argument \"M2MFileAppender\" is NULL");
		return NULL;
		}
	}


/**
 * @param[in] self
 * @param[in] loggerName
 */
unsigned char *M2MFileAppender_setLoggerName (const M2MFileAppender *self, const M2MString *loggerName)
	{
	return M2MLogger_setLoggerName(this_getSuper(self), loggerName);
	}


/**
 * @param[in] self
 * @param[in] maxBackupIndex
 */
M2MFileAppender *M2MFileAppender_setMaxBackupIndex (M2MFileAppender *self, const unsigned int maxBackupIndex)
	{
	//===== Check argument =====
	if (self!=NULL)
		{
		self->maxBackupIndex = maxBackupIndex;
		return self;
		}
	//===== Error handling =====
	else
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender_setMaxBackupIndex()", __LINE__, (M2MString *)"Argument \"M2MFileAppender\" is NULL");
		return NULL;
		}
	}


/**
 * @param[in] self
 * @param[in] maxFileSize
 */
M2MFileAppender *M2MFileAppender_setMaxFileSize (M2MFileAppender *self, unsigned int maxFileSize)
	{
	//========== Variable ==========
	const unsigned int DEFAULT_MAX_FILE_SIZE = 1048576;

	//===== Check argument =====
	if (self!=NULL && maxFileSize>0)
		{
		self->maxFileSize = maxFileSize;
		return self;
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MFileAppender_setMaxFileSize()", __LINE__, (M2MString *)"Argument \"M2MFileAppender\" is NULL");
		return NULL;
		}
	else
		{
		self->maxFileSize = DEFAULT_MAX_FILE_SIZE;
		return self;
		}
	}



/* End Of File */
