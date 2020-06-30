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

#include "m2m/lib/log/M2MFileAppender.h"


/*******************************************************************************
 * Definition
 ******************************************************************************/
/**
 * This method returns Logger object
 *
 * @param[in] self	M2MFileAppender object
 * @return			Logger object which is super(means base class) or NULL(means error)
 */
static M2MLogger *this_getSuper (const M2MFileAppender *self);


/**
 * This method translates max file size string to number.<br>
 *
 * @param[in] maxFileSizeString	max file size string
 * @return						number of max file size[Byte]
 */
static uint32_t this_translateMaxFileSize (const M2MString *maxFileSizeString);



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
 *
 * @param self
 */
static void this_deleteEncoding (M2MFileAppender *self)
	{
	//===== Check argument =====
	if (self!=NULL && self->encoding!=NULL)
		{
		M2MHeap_free(self->encoding);
		}
	else
		{
		}
	return;
	}


/**
 *
 * @param self
 */
static void this_deleteLogFile (M2MFileAppender *self)
	{
	//===== Check argument =====
	if (self!=NULL && self->logFile!=NULL)
		{
		M2MFile_delete(&(self->logFile));
		}
	//===== Argument error =====
	else
		{
		}
	return;
	}


/**
 *
 * @param[in,out] self
 */
static void this_deleteLogFilePath (M2MFileAppender *self)
	{
	//===== Check argument =====
	if (self!=NULL && self->logFilePath!=NULL)
		{
		M2MHeap_free(self->logFilePath);
		}
	//===== Argument error =====
	else
		{
		}
	return;
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
		return NULL;
		}
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
	//===== Check argument =====
	if (M2MString_compareTo(one, another)==0)
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

	//===== Check argument =====
	if (self!=NULL)
		{
		if ((encoding=M2MFileAppender_getEncoding(self))!=NULL
				&& (M2MString_compareTo(M2MSystem_UTF8, encoding)!=0
						|| M2MString_compareTo((M2MString *)"UTF8", encoding)!=0
						|| M2MString_compareTo((M2MString *)"utf-8", encoding)!=0
						|| M2MString_compareTo((M2MString *)"utf8", encoding)!=0))
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
			M2MLogger_error(NULL, (M2MString *)"M2MFileAppender.this_parseJSON()", __LINE__, (M2MString *)"Argument \"JSON\" includes error configuration");
			return NULL;
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_error(NULL, (M2MString *)"M2MFileAppender.this_parseJSON()", __LINE__, (M2MString *)"Argument \"M2MFileAppender\" is NULL");
		return NULL;
		}
	else
		{
		M2MLogger_error(NULL, (M2MString *)"M2MFileAppender.this_parseJSON()", __LINE__, (M2MString *)"Argument \"JSON\" is NULL");
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
				}
			//===== Release allocated memory =====
			M2MHeap_free(filePath);
			}
		//===== Error handling =====
		else
			{
			}
		}
	//===== Argument error =====
	else if (parentDirectoryPath==NULL)
		{
		}
	else
		{
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
				}
			//===== Release allocated memory =====
			M2MHeap_free(oldFilePath);
			}
		//===== Error handling =====
		else
			{
			}
		}
	//===== Argument error =====
	else if (parentDirectoryPath==NULL)
		{
		}
	else if (oldFileName==NULL)
		{
		}
	else
		{
		}
	return;
	}


/**
 * This method translates max file size string to number.<br>
 *
 * @param[in] maxFileSizeString	max file size string
 * @return						number of max file size[Byte]
 */
static uint32_t this_translateMaxFileSize (const M2MString *maxFileSizeString)
	{
	//========== Variable ==========
	size_t maxFileSizeStringLength = 0;
	M2MString *number = NULL;
	M2MString *unit = NULL;
	uint32_t index = 0;
	uint32_t maxFileSize = 0;
	const uint32_t KILO = 1024;
	const uint32_t MEGA = 1048576;
	const uint32_t GIGA = 1073741824;
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MFileAppender.this_translateMaxFileSize()";

	//===== Check argument =====
	if (maxFileSizeString!=NULL && (maxFileSizeStringLength=M2MString_length(maxFileSizeString))>0)
		{
		//===== Loop while existing string =====
		for (index=0; index<maxFileSizeStringLength; index++)
			{
			//===== In the case of number =====
			if (M2MString_isNumber(maxFileSizeString[index])==true)
				{
				//===== This means counting number string =====
				continue;
				}
			//===== In the case of no number =====
			else
				{
				break;
				}
			}
		//===== Allocate heap memory =====
		if ((number=(M2MString *)M2MHeap_malloc(index+1))!=NULL)
			{
			//===== Copy only "number" string =====
			memcpy(number, maxFileSizeString, index);
			//===== Convert number string into number =====
			maxFileSize = M2MString_convertFromStringToUnsignedInteger(number, index);
			//===== Release allocated heap memory =====
			M2MHeap_free(number);
			//===== In case of no existing "unit" =====
			if (maxFileSizeStringLength==index)
				{
				//===== Return only number =====
				return maxFileSize;
				}
			//===== In case of existing "unit" =====
			else
				{
				//===== Allocate heap memory =====
				if ((unit=(unsigned char *)M2MHeap_malloc(maxFileSizeStringLength-index+1))!=NULL)
					{
					//===== Copy only "unit" string =====
					memcpy(unit, &maxFileSizeString[index], maxFileSizeStringLength-index);
					//===== In the case of "K" or "k" =====
					if (this_isIncluding(unit, (M2MString *)"K")==true
							|| this_isIncluding(unit, (M2MString *)"k")==true)
						{
						//===== Release allocated heap memory =====
						M2MHeap_free(unit);
						return maxFileSize * KILO;
						}
					//===== In the case of "M" or "m" =====
					else if (this_isIncluding(unit, (M2MString *)"M")==true
							|| this_isIncluding(unit, (M2MString *)"m")==true)
						{
						//===== Release allocated heap memory =====
						M2MHeap_free(unit);
						return maxFileSize * MEGA;
						}
					//===== In the case of "G" or "g" =====
					else if (this_isIncluding(unit, (M2MString *)"G")==true
							|| this_isIncluding(unit, (M2MString *)"g")==true)
						{
						//===== Release allocated heap memory =====
						M2MHeap_free(unit);
						return maxFileSize * GIGA;
						}
					else
						{
						//===== Release allocated heap memory =====
						M2MHeap_free(unit);
						return maxFileSize;
						}
					}
				//===== Error handling =====
				else
					{
					M2MLogger_errorImpl(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to allocate heap memory for copying indicated \"unit\" string");
					return MEGA;
					}
				}
			}
		//===== Error handling =====
		else
			{
			M2MLogger_errorImpl(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to allocate heap memory for copying indicated \"number\" string");
			return MEGA;
			}
		}
	//===== Argument error =====
	else if (maxFileSizeString==NULL)
		{
		M2MLogger_errorImpl(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"maxFileSizeString\" string is NULL");
		return MEGA;
		}
	else
		{
		M2MLogger_errorImpl(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"maxFileSizeString\" string is vacant");
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
	const unsigned int MAX_BACKUP_INDEX = M2MFileAppender_getMaxBackupIndex(self);

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Get log file object =====
		if ((logFile=M2MFileAppender_getLogFile(self))!=NULL
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
							else if (M2MString_compareTo(fileName, logFileName)==0)
								{
								//===== Get index number of backup file =====
								backupIndexArray[numberOfArray] = 0;
								//===== Increment index number =====
								numberOfArray++;
								}
							//===== In the case of failed to detect backup file =====
							else
								{
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
						else if (M2MString_compareTo(fileName, logFileName)==0)
							{
							backupIndexArray[numberOfArray] = 0;
							numberOfArray++;
							}
						//===== In the case of failed to detect backup file =====
						else
							{
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
		if ((logFile=M2MFileAppender_getLogFile(self))!=NULL)
			{
			//===== In the case of need encoding =====
			if (this_needToEncoding(self)==true)
				{
				if (M2MString_convertCharacterSet(log, M2MSystem_UTF8, M2MFileAppender_getEncoding(self), &encodedMessage)!=NULL)
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
			if ((fileLength=M2MFile_length(logFile))<M2MFileAppender_getMaxFileSize(self))
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
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		}
	else
		{
		}
	return;
	}


/*******************************************************************************
 * Public function
 ******************************************************************************/
/**
 *
 * @param[in] self
 * @param[in] functionName
 * @param[in] lineNumber
 * @param[in] message
 */
void M2MLogger_infoImpl (M2MFileAppender *self, const M2MString *functionName, const unsigned int lineNumber, const M2MString *message)
	{
	//========== Variable ==========
	M2MString *log = NULL;

	//===== Check argument =====
	if (self!=NULL)
		{
		//=====  =====
		if (M2MFileAppender_getLogLevel(self)<=M2MLogLevel_INFO)
			{
			//===== Get log message =====
			if (M2MLogger_createNewLogMessage(M2MLogLevel_INFO, functionName, lineNumber, message, &log)!=NULL)
				{
				//===== Write log =====
				this_writeLog(self, log);
				M2MHeap_free(log);
				}
			//===== Error handling =====
			else
				{
				}
			}
		//=====  =====
		else
			{
			}
		}
	//===== Don't need to write =====
	else
		{
		}
	return;
	}


/**
 *
 * @param[in] self
 * @param[in] functionName
 * @param[in] lineNumber
 * @param[in] message
 */
void M2MLogger_debugImpl (M2MFileAppender *self, const M2MString *functionName, const unsigned int lineNumber, const M2MString *message)
	{
	//========== Variable ==========
	M2MString *log = NULL;

	//===== Check argument =====
	if (self!=NULL)
		{
		//=====  =====
		if (M2MFileAppender_getLogLevel(self)<=M2MLogLevel_DEBUG)
			{
			//===== Get log message =====
			if (M2MLogger_createNewLogMessage(M2MLogLevel_DEBUG, functionName, lineNumber, message, &log)!=NULL)
				{
				//===== Write log =====
				this_writeLog(self, log);
				M2MHeap_free(log);
				}
			//===== Error handling =====
			else
				{
				}
			}
		//=====  =====
		else
			{
			}
		}
	//===== Don't need to write =====
	else
		{
		}
	}


/**
 *
 * @param[in] self
 * @param[in] functionName
 * @param[in] lineNumber
 * @param[in] message
 */
void M2MLogger_traceImpl (M2MFileAppender *self, const M2MString *functionName, const unsigned int lineNumber, const M2MString *message)
	{
	//========== Variable ==========
	M2MString *log = NULL;

	//===== Check argument =====
	if (self!=NULL)
		{
		//=====  =====
		if (M2MFileAppender_getLogLevel(self)<=M2MLogLevel_TRACE)
			{
			//===== Get log message =====
			if (M2MLogger_createNewLogMessage(M2MLogLevel_TRACE, functionName, lineNumber, message, &log)!=NULL)
				{
				//===== Write log =====
				this_writeLog(self, log);
				M2MHeap_free(log);
				}
			//===== Error handling =====
			else
				{
				}
			}
		//=====  =====
		else
			{
			}
		}
	//===== Don't need to write =====
	else
		{
		}
	}


/**
 *
 * @param[in] self
 * @param[in] functionName
 * @param[in] lineNumber
 * @param[in] message
 */
void M2MLogger_warnImpl (M2MFileAppender *self, const M2MString *functionName, const unsigned int lineNumber, const M2MString *message)
	{
	//========== Variable ==========
	M2MString *log = NULL;

	//===== Check argument =====
	if (self!=NULL)
		{
		//=====  =====
		if (M2MFileAppender_getLogLevel(self)<=M2MLogLevel_WARN)
			{
			//===== Get log message =====
			if (M2MLogger_createNewLogMessage(M2MLogLevel_WARN, functionName, lineNumber, message, &log)!=NULL)
				{
				//===== Write log =====
				this_writeLog(self, log);
				M2MHeap_free(log);
				}
			//===== Error handling =====
			else
				{
				}
			}
		//=====  =====
		else
			{
			}
		}
	//===== Don't need to write =====
	else
		{
		}
	}


/**
 *
 * @param[in] self
 * @param[in] functionName
 * @param[in] lineNumber
 * @param[in] message
 */
void M2MLogger_fatalImpl (M2MFileAppender *self, const M2MString *functionName, const unsigned int lineNumber, const M2MString *message)
	{
	//========== Variable ==========
	M2MString *log = NULL;

	//===== Check argument =====
	if (self!=NULL)
		{
		//=====  =====
		if (M2MFileAppender_getLogLevel(self)<=M2MLogLevel_FATAL)
			{
			//===== Get log message =====
			if (M2MLogger_createNewLogMessage(M2MLogLevel_FATAL, functionName, lineNumber, message, &log)!=NULL)
				{
				//===== Write log =====
				this_writeLog(self, log);
				M2MHeap_free(log);
				}
			//===== Error handling =====
			else
				{
				}
			}
		//=====  =====
		else
			{
			}
		}
	//===== Don't need to write =====
	else
		{
		//===== Get log message =====
		if (M2MLogger_createNewLogMessage(M2MLogLevel_FATAL, functionName, lineNumber, message, &log)!=NULL)
			{
			//===== Write log =====
			M2MSystem_errPrintln(log);
			M2MHeap_free(log);
			}
		//===== Error handling =====
		else
			{
			}
		}
	}


/**
 *
 * @param[in] self
 * @param[in] functionName
 * @param[in] lineNumber
 * @param[in] message
 */
void M2MLogger_errorImpl (M2MFileAppender *self, const M2MString *functionName, const unsigned int lineNumber, const M2MString *message)
	{
	//========== Variable ==========
	M2MString *log = NULL;

	//===== Check argument =====
	if (self!=NULL)
		{
		//=====  =====
		if (M2MFileAppender_getLogLevel(self)<=M2MLogLevel_ERROR)
			{
			//===== Get log message =====
			if (M2MLogger_createNewLogMessage(M2MLogLevel_ERROR, functionName, lineNumber, message, &log)!=NULL)
				{
				//===== Write log =====
				this_writeLog(self, log);
				M2MHeap_free(log);
				}
			//===== Error handling =====
			else
				{
				}
			}
		//=====  =====
		else
			{
			}
		}
	//===== Don't need to write =====
	else
		{
		//===== Get log message =====
		if (M2MLogger_createNewLogMessage(M2MLogLevel_ERROR, functionName, lineNumber, message, &log)!=NULL)
			{
			//===== Write log =====
			M2MSystem_errPrintln(log);
			M2MHeap_free(log);
			}
		//===== Error handling =====
		else
			{
			}
		}
	}


/**
 * @param[in,out] self
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
 * @param[in] self	M2MFileAppender structure object
 * @return			right or wring to append to logging
 */
bool M2MFileAppender_getAppend (const M2MFileAppender *self)
	{
	//========== Variable ==========
	const bool DEFAULT_APPEND_CONFIG = true;
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MFileAppender_getAppend()";

	//===== Check argument =====
	if (self!=NULL)
		{
		return self->append;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_errorImpl(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MFileAppender\" structure object is NULL");
		return DEFAULT_APPEND_CONFIG;
		}
	}


/**
 * Create new default log file pathname string and copy it into the argument buffer.<br>
 *
 * @param[out] buffer		Buffer for copying default log file pathname string
 * @param[in] bufferLength	Length of prepared buffer
 * @return					The pointer of buffer copied log file pathname string or NULL (in case of error)
 */
M2MString *M2MFileAppender_getDefaultLogFilePath (M2MString *buffer, const size_t bufferLength)
	{
	//========== Variable ==========
	const M2MString *HOME_DIRECTORY = M2MDirectory_getHomeDirectoryPath();
	const M2MString *LOG_DIRECTORY = (M2MString *)"log";
	const size_t HOME_DIRECTORY_LENGTH = M2MString_length(HOME_DIRECTORY);
	const size_t SYSTEM_DIRECTORY_LENGTH = M2MString_length(M2MSystem_DIRECTORY);
	const size_t LOG_DIRECTORY_LENGTH = M2MString_length(LOG_DIRECTORY);
	const size_t LOGGER_LENGTH = M2MString_length(M2MLogger_DEFAULT_LOGGER_NAME);
	const size_t LOG_FILE_EXTENSION_LENGTH = M2MString_length(M2MFileAppender_LOG_FILE_EXTENTION);
	const size_t SEPARATOR_LENGTH = M2MString_length(M2MString_SLASH);

	//===== Check argument =====
	if (buffer!=NULL
			&& (HOME_DIRECTORY_LENGTH+SEPARATOR_LENGTH+SYSTEM_DIRECTORY_LENGTH+SEPARATOR_LENGTH+LOG_DIRECTORY_LENGTH+SEPARATOR_LENGTH+LOGGER_LENGTH+LOG_FILE_EXTENSION_LENGTH)<bufferLength)
		{
		//===== Initialize buffer =====
		memset(buffer, 0, bufferLength);
		//===== Copy pathname into buffer =====
		if (snprintf(&(buffer[0]), bufferLength-1, "%s%s%s%s%s%s%s%s", HOME_DIRECTORY, M2MString_SLASH, M2MSystem_DIRECTORY, M2MString_SLASH, LOG_DIRECTORY, M2MString_SLASH, M2MLogger_DEFAULT_LOGGER_NAME, M2MFileAppender_LOG_FILE_EXTENTION)>0)
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
 *
 * @param[in] self	File logging structure object
 * @return			 or NULL (in case of error)
 */
M2MString *M2MFileAppender_getEncoding (const M2MFileAppender *self)
	{
	//========== Variable ==========
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MFileAppender_getEncoding()";

	//===== Check argument =====
	if (self!=NULL)
		{
		return self->encoding;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_errorImpl(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MFileAppender\" structure object is NULL");
		return NULL;
		}
	}


/**
 * This method returns opened log file object.<br>
 *
 * @param[in] self	logging object
 * @return			opened log file or NULL(means error)
 */
M2MFile *M2MFileAppender_getLogFile (M2MFileAppender *self)
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
				if (M2MFileAppender_getAppend(self)==true)
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
					M2MFile_delete(&(self->logFile));
					return NULL;
					}
				}
			}
		//===== In the case of not existing file =====
		else
			{
			//===== Create new File object =====
			if ((self->logFile=M2MFile_new(M2MFileAppender_getLogFilePath(self)))!=NULL)
				{
				//===== In the case of existing indicated file =====
				if (M2MFile_exists(self->logFile)==true)
					{
					if (M2MFileAppender_getAppend(self)==true)
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
						M2MFile_delete(&(self->logFile));
						return NULL;
						}
					}
				}
			//===== Error handling =====
			else
				{
				return NULL;
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
 * Get log file pathname string.<br>
 *
 * @param[in] self	File logging structure object
 * @return			Log file pathname string or NULL (in case of error)
 */
M2MString *M2MFileAppender_getLogFilePath (const M2MFileAppender *self)
	{
	//========== Variable ==========
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MFileAppender_getLogFilePath()";

	//===== Check argument =====
	if (self!=NULL)
		{
		return self->logFilePath;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_errorImpl(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MFileAppender\" structure object is NULL");
		return NULL;
		}
	}



/**
 * @param[in] self			File logging structure object
 * @return
 */
M2MLogLevel M2MFileAppender_getLogLevel (const M2MFileAppender *self)
	{
	//========== Variable ==========
	const M2MLogLevel DEFAULT_LOG_LEVEL = M2MLogLevel_INFO;
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MFileAppender_getLogLevel()";

	//===== Check argument =====
	if (self!=NULL)
		{
		return M2MLogger_getLogLevel(this_getSuper(self));;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_errorImpl(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MFileAppender\" structure object is NULL");
		return DEFAULT_LOG_LEVEL;
		}
	}


/**
 * @param[in] self			File logging structure object
 * @return
 */
M2MString *M2MFileAppender_getLoggerName (const M2MFileAppender *self)
	{
	//========== Variable ==========
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MFileAppender_getLoggerName()";

	//===== Check argument =====
	if (self!=NULL)
		{
		return M2MLogger_getLoggerName(this_getSuper(self));
		}
	//===== Argument error =====
	else
		{
		M2MLogger_errorImpl(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MFileAppender\" structure object is NULL");
		return NULL;
		}
	}


/**
 * @param[in] self
 * @return
 */
uint32_t M2MFileAppender_getMaxBackupIndex (const M2MFileAppender *self)
	{
	//========== Variable ==========
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MFileAppender_setMaxBackupIndex()";

	//===== Check argument =====
	if (self!=NULL)
		{
		return self->maxBackupIndex;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_errorImpl(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MFileAppender\" structure object is NULL");
		return M2MFileAppender_DEFAULT_MAX_BACKUP_INDEX;
		}
	}


/**
 * @param[in] self
 * @return
 */
uint32_t M2MFileAppender_getMaxFileSize (const M2MFileAppender *self)
	{
	//========== Variable ==========
	const uint32_t DEFAULT_MAX_FILE_SIZE = 4194304;
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MFileAppender_getMaxFileSize()";

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
		M2MLogger_errorImpl(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MFileAppender\" structure object is NULL");
		return DEFAULT_MAX_FILE_SIZE;
		}
	}


/**
 * @return	new logger object (caller must release this memory with "M2MHeap_free()") or NULL(means error)
 */
M2MFileAppender *M2MFileAppender_new ()
	{
	//========== Variable ==========
	M2MFileAppender *self = NULL;

	//===== Allocate new memory from fileAppender =====
	if ((self=(M2MFileAppender *)M2MHeap_malloc(sizeof(M2MFileAppender)))!=NULL)
		{
		//===== Allocate new memory from super object =====
		if ((self->super=M2MLogger_new())!=NULL
				&& M2MFileAppender_setAppend(self, true)!=NULL
				&& M2MFileAppender_setEncoding(self, M2MSystem_UTF8)!=NULL
				&& M2MFileAppender_setMaxBackupIndex(self, M2MFileAppender_DEFAULT_MAX_BACKUP_INDEX)!=NULL
				&& M2MFileAppender_setMaxFileSize(self, M2MFileAppender_DEFAULT_MAX_LOG_FILE_SIZE)!=NULL
				)
			{
			self->logFile = NULL;
			self->logFilePath = NULL;
			return self;
			}
		//===== Error handling =====
		else
			{
			M2MFileAppender_delete(&self);
			return NULL;
			}
		}
	//===== Error handling =====
	else
		{
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
			if ((self=M2MFileAppender_new())!=NULL)
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
					M2MLogger_error(NULL, (M2MString *)"M2MFileAppender_parseJSONFile()", __LINE__, (M2MString *)"Failed to parse \"JSON\" object for configure \"M2MFileAppender\" object");
					M2MJSON_delete(&json);
					M2MFileAppender_delete(&self);
					return NULL;
					}
				}
			//===== Error handling =====
			else
				{
				M2MLogger_error(NULL, (M2MString *)"M2MFileAppender_parseJSONFile()", __LINE__, (M2MString *)"Failed to allocate new memory for creating \"M2MFileAppender\" object");
				M2MJSON_delete(&json);
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			M2MLogger_error(NULL, (M2MString *)"M2MFileAppender_parseJSONFile()", __LINE__, (M2MString *)"Failed to parse argument \"jsonFilePath\" configuration file");
			return NULL;
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(NULL, (M2MString *)"M2MFileAppender_parseJSONFile()", __LINE__, (M2MString *)"Argument \"jsonFilePath\" string is NULL");
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
			if ((self=M2MFileAppender_new())!=NULL)
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
					M2MLogger_error(NULL, (M2MString *)"M2MFileAppender_parseJSONString()", __LINE__, (M2MString *)"Failed to parse JSON object for configure \"M2MFileAppender\" object");
					M2MJSON_delete(&json);
					M2MFileAppender_delete(&self);
					return NULL;
					}
				}
			//===== Error handling =====
			else
				{
				M2MLogger_error(NULL, (M2MString *)"M2MFileAppender_parseJSONString()", __LINE__, (M2MString *)"Failed to allocate new memory for creating \"M2MFileAppender\" object");
				M2MJSON_delete(&json);
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			M2MLogger_error(NULL, (M2MString *)"M2MFileAppender_parseJSONString()", __LINE__, (M2MString *)"Failed to parse argument \"jsonString\"");
			return NULL;
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(NULL, (M2MString *)"M2MFileAppender_parseJSONString()", __LINE__, (M2MString *)"Argument \"jsonString\" is NULL");
		return NULL;
		}
	}


/**
 * @param[in,out] self			File logging structure object
 * @param[in] flag
 */
M2MFileAppender *M2MFileAppender_setAppend (M2MFileAppender *self, const bool flag)
	{
	//========== Variable ==========
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MFileAppender_setAppend()";

	//===== Check argument =====
	if (self!=NULL)
		{
		self->append = flag;
		return self;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_errorImpl(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MFileAppender\" structure object is NULL");
		return NULL;
		}
	}


/**
 * @param[in,out] self			File logging structure object
 * @param[in] encoding
 * @return
 */
M2MFileAppender *M2MFileAppender_setEncoding (M2MFileAppender *self, const M2MString *encoding)
	{
	//========== Variable ==========
	unsigned int encodingLength = 0;
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MFileAppender_setEncoding()";

	//===== Check argument =====
	if (self!=NULL && encoding!=NULL  && (encodingLength=M2MString_length(encoding))>0)
		{
		//===== Initialization =====
		this_deleteEncoding(self);
		//===== Allocate new memory =====
		if ((self->encoding=(M2MString *)M2MHeap_malloc(encodingLength+1))!=NULL)
			{
			memcpy(self->encoding, encoding, encodingLength);
			return self;
			}
		//===== Error handling =====
		else
			{
			M2MLogger_errorImpl(self, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to allocate heap memory for copying \"encoding\" string");
			return NULL;
			}
		}
	//===== Error handling =====
	else if (self==NULL)
		{
		M2MLogger_errorImpl(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MFileAppender\" structure object is NULL");
		return NULL;
		}
	else
		{
		M2MLogger_errorImpl(self, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"encoding\" string is NULL or vacant");
		return NULL;
		}
	}


/**
 * @param[in,out] self			File logging structure object
 * @param[in] filePath
 * @return
 */
M2MFileAppender *M2MFileAppender_setLogFilePath (M2MFileAppender *self, const M2MString *logFilePath)
	{
	//========== Variable ==========
	unsigned int logFilePathLength = 0;
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MFileAppender_setLogFilePath()";

	//===== Check argument =====
	if (self!=NULL && logFilePath!=NULL && (logFilePathLength=M2MString_length(logFilePath))>0)
		{
		//===== Initialize buffer =====
		this_deleteLogFilePath(self);
		//===== Allocate new memory for copying filePath =====
		if ((self->logFilePath=(M2MString *)M2MHeap_malloc(logFilePathLength+1))!=NULL)
			{
			memcpy(self->logFilePath, logFilePath, logFilePathLength);
			return self;
			}
		//===== Error handling =====
		else
			{
			M2MLogger_errorImpl(self, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to allocate heap memory for copying log file pathname string");
			return NULL;
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_errorImpl(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MFileAppender\" structure object is NULL");
		return NULL;
		}
	else
		{
		M2MLogger_errorImpl(self, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"logFilePath\" string is NULL or vacant");
		return NULL;
		}
	}


/**
 * @param[in,out] self			File logging structure object
 * @param[in] loggerName
 */
M2MFileAppender *M2MFileAppender_setLoggerName (M2MFileAppender *self, const M2MString *loggerName)
	{
	//========== Variable ==========
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MFileAppender_setLoggerName()";

	//===== Check argument =====
	if (self!=NULL && loggerName!=NULL && M2MString_length(loggerName)>0)
		{
		//===== Set logger name =====
		if (M2MLogger_setLoggerName(this_getSuper(self), loggerName)!=NULL)
			{
			return self;
			}
		//===== Error handling =====
		else
			{
			M2MLogger_errorImpl(self, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to set logger name");
			return NULL;
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_errorImpl(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MFileAppender\" structure object is NULL");
		return NULL;
		}
	else
		{
		M2MLogger_errorImpl(self, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"loggerName\" string is NULL or vacant");
		return NULL;
		}
	}


/**
 * @param[in,out] self			File logging structure object
 * @param[in] level
 */
M2MFileAppender *M2MFileAppender_setLogLevel (M2MFileAppender *self, const M2MLogLevel level)
	{
	//========== Variable ==========
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MFileAppender_setLogLevel()";

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
			M2MLogger_errorImpl(self, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to set log level");
			return NULL;
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_errorImpl(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MFileAppender\" structure object is NULL");
		return NULL;
		}
	}


/**
 * @param[in,out] self			File logging structure object
 * @param[in] maxBackupIndex
 */
M2MFileAppender *M2MFileAppender_setMaxBackupIndex (M2MFileAppender *self, const uint32_t maxBackupIndex)
	{
	//========== Variable ==========
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MFileAppender_setMaxBackupIndex()";

	//===== Check argument =====
	if (self!=NULL && maxBackupIndex>0)
		{
		self->maxBackupIndex = maxBackupIndex;
		return self;
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_errorImpl(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MFileAppender\" structure object is NULL");
		return NULL;
		}
	else
		{
		M2MLogger_errorImpl(self, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"maxBackupIndex\" is integer less than or equal to 0");
		self->maxBackupIndex = M2MFileAppender_DEFAULT_MAX_BACKUP_INDEX;
		return self;
		}
	}


/**
 * Set max file size into indicated File logging structure object.<br>
 *
 * @param[in,out] self			File logging structure object
 * @param[in] maxFileSizeString	Number string indicating max file size [Byte]
 */
M2MFileAppender *M2MFileAppender_setMaxFileSize (M2MFileAppender *self, const M2MString *maxFileSizeString)
	{
	//========== Variable ==========
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MFileAppender_setMaxFileSize()";

	//===== Check argument =====
	if (self!=NULL && maxFileSizeString!=NULL && M2MString_length(maxFileSizeString)>0)
		{
		//===== Set max log file size =====
		self->maxFileSize = this_translateMaxFileSize(maxFileSizeString);
		return self;
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_errorImpl(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MFileAppender\" structure object is NULL");
		return NULL;
		}
	else
		{
		M2MLogger_errorImpl(self, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"maxFileSizeString\" string is NULL or vacant");
		//===== Set default max log file size =====
		self->maxFileSize = this_translateMaxFileSize(M2MFileAppender_DEFAULT_MAX_LOG_FILE_SIZE);
		return self;
		}
	}



/* End Of File */
