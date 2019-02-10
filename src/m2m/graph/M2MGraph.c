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
 * Private function
 ******************************************************************************/
/**
 * Releases the heap memory of the database name possessed by the M2MGraph object.<br>
 *
 * @param[in,out] self	M2MGraph structure object
 */
static void this_deleteDatabaseName (M2MGraph *self)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MGraph.this_deleteDatabaseName()";

	//===== Check argument =====
	if (self!=NULL && M2MGraph_getDatabaseName(self)!=NULL)
		{
		M2MHeap_free(self->databaseName);
#ifdef DEBUG
		M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, (M2MString *)"Deleted string indicating SQLite3 database file name");
#endif // DEBUG
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MGraph\" structure object is NULL", NULL);
		}
	else
		{
#ifdef DEBUG
		M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, (M2MString *)"SQLite3 database file name hasn't been set yet");
#endif // DEBUG
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
	const M2MString *METHOD_NAME = (M2MString *)"M2MGraph.this_deleteFileDatabase()";

	//===== Check argument =====
	if (self!=NULL && self->fileDatabase!=NULL)
		{
		M2MSQLiteConfig_closeDatabase(self->fileDatabase);
#ifdef DEBUG
		M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, (M2MString *)"Closed the SQLite3 database object");
#endif // DEBUG
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MGraph\" structure object is NULL", NULL);
		}
	else
		{
#ifdef DEBUG
		M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, (M2MString *)"SQLite3 database object hasn't been set yet");
#endif // DEBUG
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
	const M2MString *METHOD_NAME = (M2MString *)"M2MGraph.this_deleteMemoryDatabase()";

	//===== Check argument =====
	if (self!=NULL && self->memoryDatabase!=NULL)
		{
		M2MSQLiteConfig_closeDatabase(self->memoryDatabase);
#ifdef DEBUG
		M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, (M2MString *)"Closed the SQLite3 database object");
#endif // DEBUG
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MGraph\" structure object is NULL", NULL);
		}
	else
		{
#ifdef DEBUG
		M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, (M2MString *)"SQLite3 database object hasn't been set yet");
#endif // DEBUG
		}
	return;
	}


/**
 * @param[in] fileDatabase	SQLite3 database object
 * @return					true: , false:
 */
static bool this_isClosed (sqlite3 *fileDatabase)
	{
	//========== Variable ==========
	M2MString fileDatabaseName[1024];

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
#ifdef DEBUG
	M2MString MESSAGE[256];
#endif // DEBUG
	const M2MString *METHOD_NAME = (M2MString *)"M2MGraph.this_setDatabaseName()";

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
					M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to set the SQLite3 database name in the M2MGraph structure object", NULL);
					return NULL;
					}
				}
			//===== In the case of extension is given in name =====
			else
				{
				// do nothing
				}
#ifdef DEBUG
			memset(MESSAGE, 0, sizeof(MESSAGE));
			snprintf(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Set the SQLite3 database file name (=\"%s\") in the M2MGraph structure object", M2MGraph_getDatabaseName(self));
			M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, MESSAGE);
#endif // DEBUG
			return self;
			}
		//===== Error handling =====
		else
			{
			M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to get heap memory for copying SQLite3 database name", NULL);
			return NULL;
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MGraph\" structure object is NULL", NULL);
		return NULL;
		}
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"databaseName\" string is NULL or vacant", NULL);
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
	const M2MString *METHOD_NAME = (M2MString *)"M2MGraph.this_setFileDatabase()";

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
#ifdef DEBUG
		M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, (M2MString *)"Set the SQLite3 file database into indicated \"M2MGraph\" object");
#endif // DEBUG
		return self;
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MGraph\" object is NULL", NULL);
		return NULL;
		}
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"sqlite3\" object is NULL", NULL);
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
	const M2MString *METHOD_NAME = (M2MString *)"M2MGraph.this_setMemoryDatabase()";

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
#ifdef DEBUG
		M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, (M2MString *)"Set the SQLite3 file database into indicated \"M2MGraph\" object");
#endif // DEBUG
		return self;
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MGraph\" object is NULL", NULL);
		return NULL;
		}
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"sqlite3\" object is NULL", NULL);
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
	const M2MString *METHOD_NAME = (M2MString *)"M2MGraph_connect()";

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
			M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to get \"sqlite3\" object possessed by \"M2MGraph\" object", NULL);
			return NULL;
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"sqlite3\" object is NULL", NULL);
		return NULL;
		}
	else if (nodeID<=0)
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"oneNodeID\" isn't positive", NULL);
		return NULL;
		}
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"anotherNodeID\" isn't positive", NULL);
		return NULL;
		}
	}


/**
 * Destructor.<br>
 * Release the heap memory for M2MGraph structure object.<br>
 *
 * @param[in,out] self	M2MGraph structure object to be freed of memory area
 */
void M2MGraph_delete (M2MGraph **self)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MGraph_delete()";

	//===== Check argument =====
	if (self!=NULL && (*self)!=NULL)
		{
		//===== Release allocated memory =====
		this_deleteDatabaseName((*self));
		this_deleteFileDatabase((*self));
		this_deleteMemoryDatabase((*self));
		M2MHeap_free((*self));
#ifdef DEBUG
		M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, (M2MString *)"Release the heap memory for \"M2MGraph\" object");
#endif // DEBUG
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MGraph\" object is NULL", NULL);
		}
	return;
	}


/**
 * Return string indicating the database name possessed by the M2MGraph object.<br>
 *
 * @param[in] self	M2MGraph structure object
 * @return			String indicating database file name or NULL (in case of error)
 */
M2MString *M2MGraph_getDatabaseName (const M2MGraph *self)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MGraph_getDatabaseName()";

	//===== Check argument =====
	if (self!=NULL)
		{
		return self->databaseName;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MGraph\" object is NULL", NULL);
		return NULL;
		}
	}


/**
 * @param[in] self		M2MGraph structure object
 * @return				SQLite3 database object
 */
sqlite3 *M2MGraph_getFileDatabase (const M2MGraph *self)
	{
	//========== Variable ==========
	M2MString *databaseName = NULL;
	sqlite3 *fileDatabase = NULL;
	const M2MString *METHOD_NAME = (M2MString *)"M2MGraph_getFileDatabase()";

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
				M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to get the SQLite3 database object whose connection is alive", NULL);
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
				M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to connect with SQLite3 database", NULL);
				return NULL;
				}
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MGraph\" structure object is NULL", NULL);
		return NULL;
		}
	}


/**
 * @param[in] self		M2MGraph structure object
 * @return				SQLite3 database object
 */
sqlite3 *M2MGraph_getMemoryDatabase (const M2MGraph *self)
	{
	//========== Variable ==========
	sqlite3 *memoryDatabase = NULL;
	const M2MString *DATA_SOURCE_NAME = (M2MString *)":memory:";
	const M2MString *METHOD_NAME = (M2MString *)"M2MGraph_getMemoryDatabase()";

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
				M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to get the SQLite3 database object whose connection is alive", NULL);
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
				M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to connect with SQLite3 database", NULL);
				return NULL;
				}
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MGraph\" structure object is NULL", NULL);
		return NULL;
		}
	}


/**
 * Constructor.<br>
 *
 * @param[in] databaseName	String indicating SQLite3 file database name
 * @return					Created new M2MGraph structure object or NULL (in case of error)
 */
M2MGraph *M2MGraph_new (const M2MString *databaseName)
	{
	//========== Variable ==========
	M2MGraph *self = NULL;
	const M2MString *METHOD_NAME = (M2MString *)"M2MGraph_new()";

	//===== Check argument =====
	if (databaseName!=NULL && M2MString_length(databaseName)>0)
		{
		//===== Get heap memory =====
		if ((self=(M2MGraph *)M2MHeap_malloc(sizeof(M2MGraph)))!=NULL)
			{
			//===== Set file database name =====
			if (this_setDatabaseName(self, databaseName)!=NULL)
				{
#ifdef DEBUG
				M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, (M2MString *)"Succeed to create a M2MGraph structure object");
#endif // DEBUG
				return self;
				}
			//===== Error handling =====
			else
				{
				//===== Release heap memory =====
				M2MGraph_delete(&self);
				M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to initialize M2MGraph structure object", NULL);
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to get heap memory for creating M2MGraph structure object", NULL);
			return NULL;
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"databaseName\" string is NULL or vacant", NULL);
		return NULL;
		}
	}



/* End Of File */
