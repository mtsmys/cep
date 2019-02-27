/*******************************************************************************
 * M2MGraph.c
 *
 * Copyright (c) 2018, Akihisa Yasuda
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

#include "m2m/graph/M2MGraph.h"


/*******************************************************************************
 * Definition
 ******************************************************************************/
/**
 * Get logging object owned by the argument M2MGraph structure object.<br>
 *
 * @param[in] self	M2MGraph structure object
 * @return			Logging object owned by M2MGraph structure object
 */
static M2MFileAppender *this_getLogger (const M2MGraph *self);



/*******************************************************************************
 * Private function
 ******************************************************************************/
/**
 *
 * @return
 */
static M2MFileAppender *this_createNewLogger ()
	{
	//========== Variable ==========
	M2MFileAppender *logger = NULL;
	M2MString DEFAULT_LOG_FILE_PATH[1024];
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MGraph.this_createNewLogger()";

	//===== Create new logger object =====
	if ((logger=M2MFileAppender_new())!=NULL)
		{
		//===== Set-up logger object =====
		if (M2MFileAppender_setAppend(logger, true)!=NULL
				&& M2MFileAppender_setEncoding(logger, M2MSystem_UTF8)!=NULL
				&& M2MFileAppender_getDefaultLogFilePath(DEFAULT_LOG_FILE_PATH, sizeof(DEFAULT_LOG_FILE_PATH))!=NULL
				&& M2MFileAppender_setLogFilePath(logger, DEFAULT_LOG_FILE_PATH)!=NULL
				&& M2MFileAppender_setLogLevel(logger, M2MLogLevel_TRACE)!=NULL
				&& M2MFileAppender_setLoggerName(logger, M2MLogger_DEFAULT_LOGGER_NAME)!=NULL
				&& M2MFileAppender_setMaxBackupIndex(logger, 0)!=NULL
				&& M2MFileAppender_setMaxFileSize(logger, 0)!=NULL
				)
			{
			//===== Return created logger object =====
			return logger;
			}
		//===== Error handling =====
		else
			{
			M2MFileAppender_delete(&logger);
			M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to set-up \"M2MFileAppender\" structure object");
			return NULL;
			}
		}
	//===== Error handling =====
	else
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to allocate new memory for creating \"M2MFileAppender\" structure object");
		return NULL;
		}
	}


/**
 * Releases the heap memory of the database name possessed by the M2MGraph object.<br>
 *
 * @param[in,out] self	M2MGraph structure object
 */
static void this_deleteDatabaseName (M2MGraph *self)
	{
	//========== Variable ==========
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MGraph.this_deleteDatabaseName()";

	//===== Check argument =====
	if (self!=NULL && M2MGraph_getDatabaseName(self)!=NULL)
		{
		M2MHeap_free(self->databaseName);
		M2MLogger_debug(this_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Deleted string indicating SQLite3 database file name");
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MGraph\" structure object is NULL");
		}
	else
		{
		M2MLogger_debug(this_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"SQLite3 database file name hasn't been set yet");
		}
	return;
	}


/**
 * Releases the heap memory of the SQLite3 database object possessed by the M2MGraph object.<br>
 *
 * @param[in,out] self	M2MGraph structure object
 */
static void this_deleteFileDatabase (M2MGraph *self)
	{
	//========== Variable ==========
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MGraph.this_deleteFileDatabase()";

	//===== Check argument =====
	if (self!=NULL && self->fileDatabase!=NULL)
		{
		M2MSQLiteConfig_closeDatabase(self->fileDatabase);
		M2MLogger_debug(this_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Closed the SQLite3 database object");
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MGraph\" structure object is NULL");
		}
	else
		{
		M2MLogger_debug(this_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"SQLite3 database object hasn't been set yet");
		}
	return;
	}


/**
 * Releases the heap memory of the logging object possessed by the M2MGraph object.<br>
 *
 * @param[in,out] self	M2MGraph structure object
 */
static void this_deleteLogger (M2MGraph *self)
	{
	//========== Variable ==========
	M2MFileAppender *logger = NULL;
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MGraph.this_deleteLogger()";

	//===== Check argument =====
	if (self!=NULL && (logger=this_getLogger(self))!=NULL)
		{
		//===== Release allocated memory =====
		M2MFileAppender_delete(&logger);
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MGraph\" structure object is NULL");
		}
	else
		{
		}
	return;
	}


/**
 * Releases the heap memory of the SQLite3 database object possessed by the M2MGraph object.<br>
 *
 * @param[in,out] self	M2MGraph structure object
 */
static void this_deleteMemoryDatabase (M2MGraph *self)
	{
	//========== Variable ==========
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MGraph.this_deleteMemoryDatabase()";

	//===== Check argument =====
	if (self!=NULL && self->memoryDatabase!=NULL)
		{
		M2MSQLiteConfig_closeDatabase(self->memoryDatabase);
		M2MLogger_debug(this_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Closed the SQLite3 database object");
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MGraph\" structure object is NULL");
		}
	else
		{
		M2MLogger_debug(this_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"SQLite3 database object hasn't been set yet");
		}
	return;
	}


/**
 * Get logging object owned by the argument M2MGraph structure object.<br>
 *
 * @param[in] self	M2MGraph structure object
 * @return			Logging object owned by M2MGraph structure object
 */
static M2MFileAppender *this_getLogger (const M2MGraph *self)
	{
	//========== Variable ==========
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MGraph.this_getLogger()";

	//===== Check argument =====
	if (self!=NULL)
		{
		return self->logger;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MGraph\" structure object is NULL");
		return NULL;
		}
	}


/**
 * @param[in] fileDatabase	SQLite3 database object
 * @return					true: , false:
 */
static bool this_isClosed (sqlite3 *fileDatabase)
	{
	//========== Variable ==========
	M2MString fileDatabaseName[1024];
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MGraph.this_isClosed()";

	//===== Check argument =====
	if (fileDatabase!=NULL)
		{
		//===== Check the connection with SQLite3 database =====
		if (sqlite3_db_filename(fileDatabase, fileDatabaseName)!=NULL)
			{
			return false;
			}
		//===== In case of not existing the connection with SQLite3 database =====
		else
			{
			return true;
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"fileDatabase\" structure object is NULL");
		return true;
		}
	}


/**
 * Set SQLite3 database file name of member of the M2MGraph structure object.<br>
 *
 * @param[in,out] self		M2MGraph structure object to be set SQLite3 database file name
 * @param[in] databaseName	String indicating SQLite3 database file name
 * @return					M2MGraph structure object with member variable updated or NULL (in case of error)
 */
static M2MGraph *this_setDatabaseName (M2MGraph *self, const M2MString *databaseName)
	{
	//========== Variable ==========
	M2MString MESSAGE[1024];
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MGraph.this_setDatabaseName()";

	//===== Check argument =====
	if (self!=NULL
			&& databaseName!=NULL && M2MString_length(databaseName)>0)
		{
		//===== Initialize SQLite3 file database name =====
		this_deleteDatabaseName(self);
		//===== Copy SQLite3 database file name =====
		if (M2MString_append(&(self->databaseName), databaseName)!=NULL)
			{
			//===== In the case of extension isn't given in name =====
			if (M2MString_lastIndexOf(databaseName, M2MSQLiteConfig_FILE_EXTENSION)==NULL)
				{
				//===== Copy SQLite3 file extension =====
				if (M2MString_append(&(self->databaseName), M2MSQLiteConfig_FILE_EXTENSION)!=NULL)
					{
					// do nothing
					}
				//===== Error handling =====
				else
					{
					this_deleteDatabaseName(self);
					M2MLogger_error(this_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Failed to set the SQLite3 database name in the M2MGraph structure object");
					return NULL;
					}
				}
			//===== In the case of extension is given in name =====
			else
				{
				// do nothing
				}
			memset(MESSAGE, 0, sizeof(MESSAGE));
			snprintf(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Set the SQLite3 database file name (=\"%s\") in the M2MGraph structure object", M2MGraph_getDatabaseName(self));
			M2MLogger_debug(this_getLogger(self), FUNCTION_NAME, __LINE__, MESSAGE);
			return self;
			}
		//===== Error handling =====
		else
			{
			M2MLogger_error(this_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Failed to get heap memory for copying SQLite3 database name");
			return NULL;
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MGraph\" structure object is NULL");
		return NULL;
		}
	else
		{
		M2MLogger_error(this_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"databaseName\" string is NULL or vacant");
		return NULL;
		}
	}


/**
 * Set logging object as member variable of the M2MGraph structure object.<br>
 *
 * @param[in,out] self		M2MGraph structure object to be set SQLite3 file database
 * @param[in] logger		Logging structure object
 * @return					M2MGraph structure object with member variable updated or NULL (in case of error)
 */
static M2MGraph *this_setLogger (M2MGraph *self, M2MFileAppender *logger)
	{
	//========== Variable ==========
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MGraph.this_setLogger()";

	//===== Check argument =====
	if (self!=NULL && logger!=NULL)
		{
		//===== Set logging object =====
		self->logger = logger;
		return self;
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MGraph\" structure object is NULL");
		return NULL;
		}
	else
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"logger\" structure object is NULL");
		return NULL;
		}
	}


/**
 * Set SQLite3 database object as member variable of the M2MGraph structure object.<br>
 *
 * @param[in,out] self		M2MGraph structure object to be set SQLite3 file database
 * @param[in] fileDatabase	SQLite3 file database object
 * @return					M2MGraph structure object with member variable updated or NULL (in case of error)
 */
static M2MGraph *this_setFileDatabase (M2MGraph *self, sqlite3 *fileDatabase)
	{
	//========== Variable ==========
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MGraph.this_setFileDatabase()";

	//===== Check argument =====
	if (self!=NULL
			&& fileDatabase!=NULL && this_isClosed(fileDatabase)==false)
		{
		//===== In case of existing the connection with SQLite3 database =====
		if (self->fileDatabase!=NULL)
			{
			//===== Initialize the connection with SQLite3 database =====
			M2MSQLiteConfig_closeDatabase(self->fileDatabase);
			self->fileDatabase = NULL;
			}
		//===== In case of not existing the connection with SQLite3 database =====
		else
			{
			// do nothing
			}
		//===== Set the connection with SQLite3 database =====
		self->fileDatabase = fileDatabase;
		M2MLogger_debug(this_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Set the SQLite3 file database into indicated \"M2MGraph\" object");
		return self;
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MGraph\" object is NULL");
		return NULL;
		}
	else
		{
		M2MLogger_error(this_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"sqlite3\" object is NULL");
		return NULL;
		}
	}


/**
 * Set SQLite3 database object as member variable of the M2MGraph structure object.<br>
 *
 * @param[in,out] self			M2MGraph structure object to be set SQLite3 file database
 * @param[in] memoryDatabase	SQLite3 in-memory database object
 * @return						M2MGraph structure object with member variable updated or NULL (in case of error)
 */
static M2MGraph *this_setMemoryDatabase (M2MGraph *self, sqlite3 *memoryDatabase)
	{
	//========== Variable ==========
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MGraph.this_setMemoryDatabase()";

	//===== Check argument =====
	if (self!=NULL
			&& memoryDatabase!=NULL && this_isClosed(memoryDatabase)==false)
		{
		//===== In case of existing the connection with SQLite3 database =====
		if (self->memoryDatabase!=NULL)
			{
			//===== Initialize the connection with SQLite3 database =====
			M2MSQLiteConfig_closeDatabase(self->memoryDatabase);
			self->memoryDatabase = NULL;
			}
		//===== In case of not existing the connection with SQLite3 database =====
		else
			{
			// do nothing
			}
		//===== Set the connection with SQLite3 database =====
		self->memoryDatabase = memoryDatabase;
		M2MLogger_debug(this_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Set the SQLite3 file database into indicated \"M2MGraph\" object");
		return self;
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MGraph\" object is NULL");
		return NULL;
		}
	else
		{
		M2MLogger_error(this_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"sqlite3\" object is NULL");
		return NULL;
		}
	}



/*******************************************************************************
 * Public function
 ******************************************************************************/
/**
 * Connect nodes specified by arguments with an edge.<br>
 * Actually maintain relationships in a "Nested Sets Model" in the table <br>
 * of SQLite 3 database.<br>
 *
 * @param[in] self			M2MGraph structure object
 * @param[in] nodeID		Node ID of the connection source to be connected at edge
 * @param[in] anotherNodeID	Node ID of connection destination to be connected by edge
 * @return					M2MGraph structure object relationships updated or NULL (in case of error)
 */
M2MGraph *M2MGraph_connect (const M2MGraph *self, const uint32_t nodeID, const uint32_t anotherNodeID)
	{
	//========== Variable ==========
	sqlite3 *fileDatabase = NULL;
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MGraph_connect()";

	//===== Check argument =====
	if (self!=NULL && nodeID>0 && anotherNodeID>0)
		{
		//===== Get SQLite3 object =====
		if ((fileDatabase=M2MGraph_getFileDatabase((M2MGraph *)self))!=NULL)
			{
			return (M2MGraph *)self;
			}
		//===== Error handling =====
		else
			{
			M2MLogger_error(this_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Failed to get \"sqlite3\" object possessed by \"M2MGraph\" object");
			return NULL;
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"sqlite3\" object is NULL");
		return NULL;
		}
	else if (nodeID<=0)
		{
		M2MLogger_error(this_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"oneNodeID\" isn't positive");
		return NULL;
		}
	else
		{
		M2MLogger_error(this_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"anotherNodeID\" isn't positive");
		return NULL;
		}
	}


/**
 * Destructor.<br>
 * Release the heap memory for M2MGraph structure object.<br>
 *
 * @param[in,out] self	M2MGraph structure object which is to be released memory
 */
void M2MGraph_delete (M2MGraph **self)
	{
	//===== Check argument =====
	if (self!=NULL && (*self)!=NULL)
		{
		//===== Release allocated memory =====
		this_deleteDatabaseName((*self));
		this_deleteFileDatabase((*self));
		this_deleteMemoryDatabase((*self));
		this_deleteLogger((*self));
		M2MHeap_free((*self));
		}
	//===== Argument error =====
	else
		{
		}
	return;
	}


/**
 * Get string indicating the database name possessed by the M2MGraph object.<br>
 *
 * @param[in] self	M2MGraph structure object
 * @return			String indicating database file name or NULL (in case of error)
 */
M2MString *M2MGraph_getDatabaseName (const M2MGraph *self)
	{
	//========== Variable ==========
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MGraph_getDatabaseName()";

	//===== Check argument =====
	if (self!=NULL)
		{
		return self->databaseName;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MGraph\" object is NULL");
		return NULL;
		}
	}


/**
 * Get the SQLite connection object on file possessed by the M2MGraph object.<br>
 *
 * @param[in] self		M2MGraph structure object
 * @return				SQLite3 database object
 */
sqlite3 *M2MGraph_getFileDatabase (const M2MGraph *self)
	{
	//========== Variable ==========
	M2MString *databaseName = NULL;
	sqlite3 *fileDatabase = NULL;
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MGraph_getFileDatabase()";

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== In case of existing SQLite3 database object =====
		if ((self->fileDatabase)!=NULL)
			{
			//===== Check the connection with SQLite3 database =====
			if (this_isClosed(self->fileDatabase)==false)
				{
				return self->fileDatabase;
				}
			//===== Error handling =====
			else
				{
				M2MLogger_error(this_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Failed to get the SQLite3 database object whose connection is alive");
				return NULL;
				}
			}
		//===== In case of not existing SQLite3 database object =====
		else
			{
			//===== Get the connection with SQLite3 database =====
			if ((databaseName=M2MGraph_getDatabaseName(self))!=NULL
					&& (fileDatabase=M2MSQLiteConfig_openDatabase(databaseName))!=NULL
					&& this_setFileDatabase((M2MGraph *)self, fileDatabase)!=NULL)
				{
				return self->fileDatabase;
				}
			//===== Error handling =====
			else
				{
				M2MSQLiteConfig_closeDatabase(fileDatabase);
				M2MLogger_error(this_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Failed to connect with SQLite3 database");
				return NULL;
				}
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MGraph\" structure object is NULL");
		return NULL;
		}
	}


/**
 * Get the SQLite connection object on memory possessed by the M2MGraph object.<br>
 *
 * @param[in] self		M2MGraph structure object
 * @return				SQLite3 database object
 */
sqlite3 *M2MGraph_getMemoryDatabase (const M2MGraph *self)
	{
	//========== Variable ==========
	sqlite3 *memoryDatabase = NULL;
	const M2MString *DATA_SOURCE_NAME = (M2MString *)":memory:";
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MGraph_getMemoryDatabase()";

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== In case of existing SQLite3 database object =====
		if ((self->memoryDatabase)!=NULL)
			{
			//===== Check the connection with SQLite3 database =====
			if (this_isClosed(self->memoryDatabase)==false)
				{
				return self->memoryDatabase;
				}
			//===== Error handling =====
			else
				{
				M2MLogger_error(this_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Failed to get the SQLite3 database object whose connection is alive");
				return NULL;
				}
			}
		//===== In case of not existing SQLite3 database object =====
		else
			{
			//===== Get the connection with SQLite3 database =====
			if ((memoryDatabase=M2MSQLiteConfig_openDatabase(DATA_SOURCE_NAME))!=NULL
					&& this_setMemoryDatabase((M2MGraph *)self, memoryDatabase)!=NULL)
				{
				return self->memoryDatabase;
				}
			//===== Error handling =====
			else
				{
				M2MSQLiteConfig_closeDatabase(memoryDatabase);
				M2MLogger_error(this_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Failed to connect with SQLite3 database");
				return NULL;
				}
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MGraph\" structure object is NULL");
		return NULL;
		}
	}


/**
 * Constructor.<br>
 * This function creates new M2MGraph structure object.<br>
 * When creating, this function executes memory allocation, so caller must <br>
 * release the memory for avoiding memory leak.<br>
 *
 * @param[in] databaseName	String indicating SQLite3 file database name
 * @return					Created new M2MGraph structure object or NULL (in case of error)
 */
M2MGraph *M2MGraph_new (const M2MString *databaseName)
	{
	//========== Variable ==========
	M2MGraph *self = NULL;
	M2MFileAppender *logger = NULL;
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MGraph_new()";

	//===== Check argument =====
	if (databaseName!=NULL && M2MString_length(databaseName)>0)
		{
		//===== Get heap memory =====
		if ((self=(M2MGraph *)M2MHeap_malloc(sizeof(M2MGraph)))!=NULL)
			{
			//===== Initialize M2MGraph object =====
			if ((logger=this_createNewLogger())!=NULL
					&& this_setLogger(self, logger)!=NULL
					&& this_setDatabaseName(self, databaseName)!=NULL)
				{
				M2MLogger_info(this_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Succeed to create a M2MGraph structure object");
				//===== Return created M2MGraph object =====
				return self;
				}
			//===== Error handling =====
			else
				{
				//===== Release heap memory =====
				M2MGraph_delete(&self);
				M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to initialize M2MGraph structure object");
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to allocate heap memory for creating M2MGraph structure object");
			return NULL;
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"databaseName\" string is NULL or vacant");
		return NULL;
		}
	}



/* End Of File */
