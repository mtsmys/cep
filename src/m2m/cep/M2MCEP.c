/*******************************************************************************
 * M2MCEP.c
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

#include "m2m/cep/M2MCEP.h"


/*******************************************************************************
 * Declaration of private function
 ******************************************************************************/
/**
 * Create string indicating the database file path and copy it to the buffer. <br>
 * If processing fails, NULL is returned as a return value.<br>
 *
 * @param[in] databaseName			String indicating SQLite3 database file name
 * @param[out] databaseFilePath		Pointer for storing string indicating the database file path
 * @return							Pointer of string indicating database file path or NULL (in case of error)
 */
static M2MString *this_getDatabaseFilePath (const M2MString *databaseName, M2MString **databaseFilePath);


/**
 * Get the CEP record information object managed by the argument CEP object.<br>
 *
 * @param[in] self	CEP structure object
 * @return			CEP record information object
 */
static M2MDataFrame *this_getDataFrame (const M2MCEP *self);


/**
 * Get the upper limit of the number of CEP records held by the argument CEP object.<br>
 *
 * @param[in] self	CEP structure object
 * @return			The upper limit of the number of CEP records
 */
static unsigned int this_getMaxRecord (const M2MCEP *self);


/**
 * Get the number of inserted records held by the argument CEP object.<br>
 *
 * @param self	CEP structure object
 * @return		Number of inserted records
 */
static unsigned int this_getRecordCounter (const M2MCEP *self);


/**
 * Get the number of records for which SQLite3 database performs vacuum processing.<br>
 *
 * @param[in] self	CEP structure object
 * @return			Number of records for which SQLite 3 database performs vacuum processing
 */
static unsigned int this_getVacuumRecord (const M2MCEP *self);


/**
 * Get a flag indicating persistence availability from the argument CEP object.<br>
 *
 * @param[in,out] self		CEP structure object
 * @return					Flag indicating permanence
 */
static bool this_getPersistence (const M2MCEP *self);


/**
 * Get the table construction object held by the CEP object as a member variable.<br>
 *
 * @param[in] self	CEP structure object
 * @return			Table construction object or NULL (in case of error)
 */
static M2MTableManager *this_getTableManager (const M2MCEP *self);


/**
 * Bulk insert records into SQLite 3 database specified by argument.<br>
 *
 * @param[in] database		SQLite3 database management object
 * @param[in] record		Record management object
 * @param[in] tableManager	Table construction object
 * @param[in] recordList	Record information object
 * @return					Number of inserted records or -1 (in case of error)
 */
static int this_insertRecordList (sqlite3 *database, const M2MDataFrame *record, M2MTableManager *tableManager, M2MList *recordList);


/**
 * If the specified maximum number of records is exceeded for the record of <br>
 * "already inserted in SQLite3 database in memory" of the CEP record management <br>
 * object held by the CEP execution object specified by the argument, the record <br>
 * of excess Into the SQLite 3 database on the file and make it persistent.<br>
 *
 * @param[in,out] self	CEP structure object
 * @return				Successful CEP execution object or NULL (in case of insertion error)
 */
static M2MCEP *this_insertRecordListToFileDatabase (M2MCEP *self);


/**
 * Insert all records (= CEPRecord-> newRecordList) held by CEP object variables <br>
 * into SQLite3 database in memory.<br>
 *
 * @param[in] self	CEP structure object
 * @return			Number of records where insert processing or -1 (in case of error)
 */
static int this_insertRecordListToMemoryDatabase (M2MCEP *self);


/**
 * Create a SQLite 3 database file under the specified directory.<br>
 * If creation fails, NULL is returned.<br>
 *
 * @param[in] databaseName	String indicating database name
 * @param[in] tableManager	Table information management object
 * @param[in] synchronous	true: Synchronous mode = NORMAL, false: Synchronous mode = OFF
 * @param[in] vacuumRecord	Number of records vacuum processing is to be executed (if this is 0, set automatic vacuum)
 * @return					SQLite3 database file object or NULL (in case of error)
 */
static sqlite3 *this_openFileDatabase (const M2MString *databaseName, const M2MTableManager *tableManager, const bool synchronous, const unsigned int vacuumRecord);


/**
 * Create a SQLite3 database on memory.<br>
 * If creation fails, NULL is returned.<br>
 *
 * @param[in] tableManager	Table information management object
 * @param[in] synchronous	true: Synchronous mode = NORMAL, false: Synchronous mode = OFF
 * @param[in] vacuumRecord	Number of records vacuum processing is to be executed (if this is 0, set automatic vacuum)
 * @return					SQLite3 database object or NULL (in case of error)
 */
static sqlite3 *this_openMemoryDatabase (const M2MTableManager *tableManager, const bool synchronous, const unsigned int vacuumRecord);


/**
 * Set SQLite3 database file name of member of the CEP structure object.<br>
 *
 * @param[in,out] self		CEP structure object to be set SQLite3 database file name
 * @param[in] databaseName	String indicating SQLite3 database file name
 * @return					CEP structure object with member variable updated or NULL (in case of error)
 */
static M2MCEP *this_setDatabaseName (M2MCEP *self, const M2MString *databaseName);


/**
 * Set indicated M2MDataFrame object into CEP structure object.<br>
 *
 * @param[in,out] self		CEP structure object to be set M2MDataFrame object
 * @param[in] dataFrame		M2MDataFrame object
 * @return					CEP structure object with member variable updated or NULL (in case of error)
 */
static M2MCEP *this_setDataFrame (M2MCEP *self, M2MDataFrame *dataFrame);


/**
 * Set the SQLite3 file database management object as the member variable held <br>
 * by the CEP execution object specified by the argument.<br>
 *
 * @param[in,out] self		CEP structure object
 * @param[in] fileDatabase	SQLite3 file database management object
 * @return					CEP structure object with member variable updated or NULL (in case of error)
 */
static M2MCEP *this_setFileDatabase (M2MCEP *self, sqlite3 *fileDatabase);


/**
 * Set the SQLite 3 database management object in memory as the member variable <br>
 * possessed by the CEP execution object specified by the argument.<br>
 *
 * @param[in,out] self			CEP structure object
 * @param[in] memoryDatabase	SQLite 3 database management object on memory
 * @return						CEP structure object with member variable updated or NULL (in case of error)
 */
static M2MCEP *this_setMemoryDatabase (M2MCEP *self, sqlite3 *memoryDatabase);


/**
 * Set a table construction object as a member variable for the argument CEP object.<br>
 *
 * @param[in,out] self		CEP structure object
 * @param[in] tableManager	Table construction object
 * @return					CEP structure object with member variable updated or NULL (in case of error)
 */
static M2MCEP *this_setTableManager (M2MCEP *self, const M2MTableManager *tableManager);


/**
 * Update the number of records for executing the vacuum on the SQLite 3 database.<br>
 *
 * @param[in] self		CEP structure object
 * @param[in] count		Number of records added to the counter (> 0) or 0 (for initializing the counters)
 */
static void this_updateRecordCounter (M2MCEP *self, const unsigned int count);



/*******************************************************************************
 * Private function
 ******************************************************************************/
/**
 * For the SQLite 3 database in memory, check that the number of records in the <br>
 * table specified by the argument does not exceed the upper limit value.<br>
 * If it exceeds the upper limit value, records are deleted in the order of the <br>
 * inserted records until the number of records matches the upper limit value.<br>
 *
 * @param[in] self		CEP structure object
 * @param[in] tableName	String indicating table name
 */
static void this_adjustMemoryDatabaseRecord (M2MCEP *self, const M2MString *tableName)
	{
	//========== Variable ==========
	int excess = 0;
	M2MString *sql = NULL;
	M2MString MESSAGE[256];
	const unsigned int MAX_RECORD = this_getMaxRecord(self);
	const M2MDataFrame *RECORD = M2MDataFrame_begin(this_getDataFrame(self));
	const M2MList *OLD_RECORD_LIST = M2MList_begin(M2MDataFrame_getOldRecordList(RECORD));
	const unsigned int OLD_RECORD_LIST_LENGTH = M2MList_length((M2MList *)OLD_RECORD_LIST);
	const M2MString *DELETE_SQL = (M2MString *)"DELETE FROM %s WHERE rowid IN (SELECT rowid FROM %s ORDER BY rowid LIMIT %u) ";
	const size_t TABLE_NAME_LENGTH = M2MString_length(tableName);
	const size_t DELETE_SQL_LENGTH = M2MString_length(DELETE_SQL) + TABLE_NAME_LENGTH + TABLE_NAME_LENGTH + 64;
	const M2MString *METHOD_NAME = (M2MString *)"M2MCEP.this_adjustMemoryDatabaseRecord()";

	//===== Check argument =====
	if (self!=NULL && tableName!=NULL && TABLE_NAME_LENGTH>0)
		{
		//===== Get record information =====
		if (MAX_RECORD>0 && OLD_RECORD_LIST!=NULL && OLD_RECORD_LIST_LENGTH>0)
			{
			//===== When the number of records exceeds the upper limit =====
			if ((excess=OLD_RECORD_LIST_LENGTH-MAX_RECORD)>0)
				{
				memset(MESSAGE, 0, sizeof(MESSAGE));
				snprintf(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"The number of records in the \"%s\" table of the SQLite 3 database in memory exceeds the upper limit by \"%d\"", tableName, excess);
				M2MLogger_debug(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, MESSAGE);
				//===== Get SQLite 3 database on memory =====
				if ((sql=(M2MString *)M2MHeap_malloc(DELETE_SQL_LENGTH+1))!=NULL)
					{
					//===== Create a DELETE statement =====
					snprintf(sql, DELETE_SQL_LENGTH, DELETE_SQL, tableName, tableName, excess);
					//===== Execute DELETE statement =====
					if (M2MSQLite_executeUpdate(M2MCEP_getMemoryDatabase(self), sql)==true)
						{
						memset(MESSAGE, 0, sizeof(MESSAGE));
						snprintf(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Deleted \"%s\" of records in \"%d\" table of SQLite3 database in memory", tableName, excess);
						M2MLogger_debug(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, MESSAGE);
						//===== Release heap memory area of SQL string =====
						M2MHeap_free(sql);
						return;
						}
					//===== Error handling =====
					else
						{
						M2MLogger_error(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Failed to delete excess records of SQLite 3 database in memory");
						//===== Release heap memory area of SQL string =====
						M2MHeap_free(sql);
						return;
						}
					}
				//===== Error handling =====
				else
					{
					M2MLogger_error(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Failed to get the heap memory area for creating the DELETE statement for deleting the excess record");
					return;
					}
				}
			//===== When the number of records is within the upper limit value =====
			else
				{
				memset(MESSAGE, 0, sizeof(MESSAGE));
				snprintf(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"The current number of records in the \"%s\" table of the SQLite 3 database in memory is \"%u\"", tableName, OLD_RECORD_LIST_LENGTH);
				M2MLogger_debug(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, MESSAGE);
				return;
				}
			}
		//===== Error handling =====
		else if (MAX_RECORD<=0)
			{
			M2MLogger_error(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"The upper limit value of the number of records in the SQLite 3 database on the memory set by the CEP execution object specified by the argument is 0");
			return;
			}
		else if (OLD_RECORD_LIST==NULL)
			{
			M2MLogger_error(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"The past record information object possessed by the CEP execution object specified by the argument is NULL");
			return;
			}
		else
			{
			M2MLogger_error(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"The number of past record information objects possessed by the CEP execution object specified by the argument is 0");
			return;
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"Argument error! The CEP execution object specified by argument is NULL");
		return;
		}
	else if (tableName==NULL)
		{
		M2MLogger_error(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Argument error! The string indicating the table name specified by the argument is NULL");
		return;
		}
	else
		{
		M2MLogger_error(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Argument error! The number of character strings in the character string indicating the table name specified by the argument is 0");
		return;
		}
	}


/**
 * The number of inserted records is checked, and when the default value is <br>
 * reached, the vacuum process is executed.<br>
 * For the vacuum process, execute for both SQLite 3 database on memory and <br>
 * SQLite 3 database on file.<br>
 *
 * @param[in,out] self	CEP structure object
 */
static void this_checkRecordCounterForVacuum (M2MCEP *self)
	{
	//========== Variable ==========
	unsigned int vacuumRecord = 0;
	const M2MString *METHOD_NAME = (M2MString *)"M2MCEP.this_checkRecordCounterForVacuum()";

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Confirm the number of records to execute vacuum processing =====
		if ((vacuumRecord=this_getVacuumRecord(self))>0)
			{
			//===== Vacuum processing is performed, when the number of records of regulations is reached =====
			if (vacuumRecord<this_getRecordCounter(self))
				{
				M2MLogger_debug(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Start to vacuum processing on the SQLite3 database in memory");
				//===== Vacuum the SQLite 3 database on memory =====
				M2MSQLite_vacuum(M2MCEP_getMemoryDatabase(self));
				M2MLogger_debug(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Vacuum processing was executed on the SQLite3 database in memory");
				//===== In the case of record persistence =====
				if (this_getPersistence(self)==true)
					{
					M2MLogger_debug(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Start to vacuum processing on the SQLite3 database file");
					//===== Vacuum the SQLite3 database on the file =====
					M2MSQLite_vacuum(M2MCEP_getFileDatabase(self));
					M2MLogger_debug(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Vacuum processing was executed on the SQLite3 database file");
					}
				//===== In case of record non-persistence =====
				else
					{
					// do nothing
					}
				//===== Initialize counter =====
				this_updateRecordCounter(self, 0);
				}
			//===== When the record number of the regulations has not been reached =====
			else
				{
				// do nothing
				}
			return;
			}
		//===== When vacuum processing is not executed (automatic vacuum) setting =====
		else
			{
			return;
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MCEP\" structure object is NULL");
		return;
		}
	}


/**
 * Close the SQLite3 file database managed by the CEP execution object specified <br>
 * by the argument and release the heap memory area.<br>
 *
 * @param[in,out] self	CEP structure object
 */
static void this_closeFileDatabase (M2MCEP *self)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MCEP.this_closeFileDatabase()";

	//===== Check argument =====
	if (self!=NULL)
		{
		M2MSQLite_closeDatabase(M2MCEP_getFileDatabase(self));
		M2MLogger_debug(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Closed SQLite3 file database");
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MCEP\" object is NULL");
		}
	return;
	}


/**
 * Exit the SQLite 3 memory database managed by the CEP execution object <br>
 * specified by the argument and release the heap memory area.<br>
 *
 * @param[in,out] self	CEP structure object
 */
static void this_closeMemoryDatabase (M2MCEP *self)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MCEP.this_closeMemoryDatabase()";

	//===== Check argument =====
	if (self!=NULL)
		{
		M2MSQLite_closeDatabase(M2MCEP_getMemoryDatabase(self));
		M2MLogger_debug(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Closed SQLite3 memory database");
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MCEP\" object is NULL");
		}
	return;
	}


/**
 * Create a new INSERT statement for the prepared statement.<br>
 *
 * @param[in] tableName			String indicating table name
 * @param[in] columnNameCSV		CSV format string indicating column name
 * @param[in] columnListLength	An integer indicating the number of columns
 * @param[out] sql				Pointer to copy the newly created INSERT statement (buffering is executed inside the function)
 * @return						Pointer of newly created INSERT statement or NULL (in case of error)
 */
static M2MString *this_createInsertSQL (const M2MString *tableName, const M2MString *columnNameCSV, unsigned int columnListLength, M2MString **sql)
	{
	//========== Variable ==========
	unsigned int i = 0;
	unsigned int index = 0;
	size_t sqlLength = 0;
	const M2MString *INSERT_SQL = (M2MString *)"INSERT INTO %s (%s) VALUES (%s) ";
	const size_t QUESTION_MARK_LENGTH = M2MString_length((M2MString *)M2MString_QUESTION_MARK);
	const size_t COMMA_LENGTH = M2MString_length((M2MString *)M2MString_COMMA);
	const size_t PARAMETER_LENGTH = columnListLength * (QUESTION_MARK_LENGTH + COMMA_LENGTH);
	M2MString PARAMETER[PARAMETER_LENGTH];
	const M2MString *METHOD_NAME = (M2MString *)"M2MCEP.this_createInsertSQL()";

	//===== Check argument =====
	if (tableName!=NULL && columnNameCSV!=NULL && columnListLength>0 && sql!=NULL)
		{
		//===== Initialize array =====
		memset(PARAMETER, 0, PARAMETER_LENGTH);
		//===== Repeat as many columns =====
		for (i=0; i<columnListLength; i++)
			{
			//===== In the first case =====
			if (i==0)
				{
				//===== Add "?" character =====
				memcpy(&(PARAMETER[index]), (M2MString *)M2MString_QUESTION_MARK, QUESTION_MARK_LENGTH);
				index++;
				}
			//===== In the case of 2nd and subsequent cases =====
			else
				{
				//===== Add "," character =====
				memcpy(&(PARAMETER[index]), (M2MString *)M2MString_COMMA, COMMA_LENGTH);
				index++;
				//===== Add "?" character =====
				memcpy(&(PARAMETER[index]), (M2MString *)M2MString_QUESTION_MARK, QUESTION_MARK_LENGTH);
				index++;
				}
			}
		//===== Get a heap memory area for creating a new INSERT statement =====
		if ((sqlLength=M2MString_length(INSERT_SQL)+M2MString_length(tableName)+M2MString_length(columnNameCSV)+M2MString_length(PARAMETER))>0
				&& ((*sql)=(M2MString *)M2MHeap_malloc(sqlLength+1))!=NULL)
			{
			//===== Create a new INSERT statement =====
			if (snprintf((*sql), sqlLength, INSERT_SQL, tableName, columnNameCSV, PARAMETER)>0)
				{
				//===== Return INSERT string =====
				return (*sql);
				}
			//===== Error handling =====
			else
				{
				M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"Failed to create new INSERT statement");
				//===== When the heap memory area of the INSERT statement still remains =====
				if ((*sql)!=NULL)
					{
					//===== Release heap memory area =====
					M2MHeap_free((*sql));
					}
				//===== When the heap memory area of the INSERT statement does not exist =====
				else
					{
					// do nothing
					}
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"Failed to get heap memory area to create new INSERT statement");
			return NULL;
			}
		}
	//===== Argument error =====
	else if (tableName==NULL)
		{
		M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"Argument error! The string indicating the table name specified by the argument is NULL");
		return NULL;
		}
	else if (columnNameCSV==NULL)
		{
		M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"Argument error! The CSV format string indicating the column name specified by the argument is NULL");
		return NULL;
		}
	else if (columnListLength<=0)
		{
		M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"Argument error! An integer indicating the number of columns specified by the argument is 0 or less");
		return NULL;
		}
	else
		{
		M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"Argument error! The pointer to copy the INSERT statement specified by the argument is NULL");
		return NULL;
		}
	}


/**
 * Releases the heap memory of the database name possessed by the CEP object.<br>
 *
 * @param[in,out] self	CEP structure object
 */
static void this_deleteDatabaseName (M2MCEP *self)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MCEP.this_deleteDatabaseName()";

	//===== Check argument =====
	if (self!=NULL && M2MCEP_getDatabaseName(self)!=NULL)
		{
		M2MHeap_free(self->databaseName);
		M2MLogger_debug(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Deleted string indicating SQLite3 database file name");
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_error(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MCEP\" structure object is NULL");
		}
	else
		{
		M2MLogger_debug(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"SQLite3 database file name hasn't been set yet");
		}
	return;
	}


/**
 * Releases the heap memory area of the logging object held by the argument CEP.<br>
 *
 * @param[in,out] self	CEP structure object
 */
static void this_deleteLogger (M2MCEP *self)
	{
	//========== Variable ==========
	M2MFileAppender *logger = NULL;

	//===== Check argument =====
	if (self!=NULL && (logger=M2MCEP_getLogger(self))!=NULL)
		{
		M2MFileAppender_delete(&logger);
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


/**
 * Releases the heap memory area of the table building object held by the CEP <br>
 * execution object specified by the argument as a member variable.<br>
 * Be aware that after invoking the method, you can not access the table <br>
 * building object.<br>
 *
 * @param[in,out] self	CEP structure object
 */
static void this_deleteTableManager (M2MCEP *self)
	{
	//========== Variable ==========
	M2MTableManager *tableManager = NULL;
	const M2MString *METHOD_NAME = (M2MString *)"M2MCEP.this_deleteTableManager()";

	//===== Check argument =====
	if (self!=NULL && (tableManager=this_getTableManager(self))!=NULL)
		{
		M2MTableManager_delete(&tableManager);
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_error(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MCEP\" structure object is NULL");
		}
	else
		{
		}
	return;
	}


/**
 * Store all persistent unprocessed records in the SQLite 3 database on the file.<br>
 *
 * @param[in] self	CEP structure object
 */
static void this_flushCEPRecord (M2MCEP *self)
	{
	//========== Variable ==========
	sqlite3 *fileDatabase = NULL;
	M2MTableManager *tableManager = NULL;
	M2MDataFrame *dataFrame = NULL;
	bool persistence = false;
	const M2MString *METHOD_NAME = (M2MString *)"M2MCEP.this_flushCEPRecord()";

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Retrieve (the first node of) the record management object =====
		if ((tableManager=this_getTableManager(self))!=NULL
				&& (dataFrame=M2MDataFrame_begin(this_getDataFrame(self)))!=NULL)
			{
			//===== In the case of record persistence =====
			if ((persistence=this_getPersistence(self))==true
					&& (fileDatabase=M2MCEP_getFileDatabase(self))!=NULL)
				{
				M2MLogger_debug(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Initiates transaction processing to bulk insert record information into SQLite database on file");
				//===== Start Transaction =====
				M2MSQLite_beginTransaction(fileDatabase);
				//===== Repeat until record management object reaches the end =====
				while (M2MDataFrame_next(dataFrame)!=NULL)
					{
					//===== Insert / delete record management object =====
					this_insertRecordList(fileDatabase, dataFrame, tableManager, M2MDataFrame_getOldRecordList(dataFrame));
					//===== Move to the next record management object =====
					dataFrame = M2MDataFrame_next(dataFrame);
					}
				//===== Insert / delete end record management object =====
				this_insertRecordList(fileDatabase, dataFrame, tableManager, M2MDataFrame_getOldRecordList(dataFrame));
				//===== End Transaction =====
				M2MSQLite_commitTransaction(M2MCEP_getFileDatabase(self));
				M2MLogger_debug(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Finished transaction processing to bulk insert records not inserted into SQLite database on file");
				}
			//===== In case of record non-persistence =====
			else
				{
				}
			//===== Successful completion =====
			return;
			}
		//===== Error handling =====
		else if (tableManager==NULL)
			{
			M2MLogger_error(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"The table construction object obtained from the CEP execution object specified by the argument is NULL");
			return;
			}
		else
			{
			M2MLogger_error(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"The record management object obtained from the CEP execution object specified by the argument is NULL");
			return;
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MCEP\" structure object is NULL");
		return;
		}
	}


/**
 * Get the CEP record information object managed by the argument CEP object.<br>
 *
 * @param[in] self	CEP structure object
 * @return			CEP record information object
 */
static M2MDataFrame *this_getDataFrame (const M2MCEP *self)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MCEP.this_getDataFrame()";

	//===== Check argument =====
	if (self!=NULL)
		{
		return self->dataFrame;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MCEP\" structure object is NULL");
		return NULL;
		}
	}


/**
 * Copy SQLite3 database working directory pathname (="~/.m2m/cep").<br>
 * Because the heap memory allocation process is executed inside this function, <br>
 * caller must release the memory to prevent memory leak.<br>
 *
 * @param[out] directoryPath	Pointer for storing string indicating the directory SQLite 3 database file is set
 * @return						SQLite3 working directory pathname or NULL (in case of error)
 */
static M2MString *this_getDatabaseDirectoryPath (M2MString **directoryPath)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MCEP.this_getDatabaseDirectoryPath()";

	//===== Check argument =====
	if (directoryPath!=NULL)
		{
		//===== Get CEP working directory pathname =====
		if (M2MString_append(directoryPath, M2MDirectory_getHomeDirectoryPath())!=NULL
				&& M2MString_append(directoryPath, M2MDirectory_SEPARATOR)!=NULL
				&& M2MString_append(directoryPath, M2MCEP_DIRECTORY)!=NULL)
			{
			return (*directoryPath);
			}
		//===== Error handling =====
		else
			{
			M2MHeap_free((*directoryPath))
			M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"Failed to copy the string indicating the home directory path");
			return NULL;
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"directoryPath\" pointer is NULL");
		return NULL;
		}
	}


/**
 * Create string indicating the database file path and copy it to the buffer. <br>
 * If processing fails, NULL is returned as a return value.<br>
 *
 * @param[in] databaseName			String indicating SQLite3 database file name
 * @param[out] databaseFilePath		Pointer for storing string indicating the database file path
 * @return							Pointer of string indicating database file path or NULL (in case of error)
 */
static M2MString *this_getDatabaseFilePath (const M2MString *databaseName, M2MString **databaseFilePath)
	{
	//========== Variable ==========
	M2MString *directoryPath = NULL;
	const M2MString *METHOD_NAME = (M2MString *)"M2MCEP.this_getDatabaseFilePath()";

	//===== Check argument =====
	if (databaseName!=NULL && M2MString_length(databaseName)>0
			&& databaseFilePath!=NULL)
		{
		//===== Copy database file installation directory pathname =====
		if (this_getDatabaseDirectoryPath(&directoryPath)!=NULL
				&& M2MDirectory_mkdirs(directoryPath, M2MSQLite_DATABASE_DIRECTORY_PERMISSION)==true
				&& M2MString_append(databaseFilePath, directoryPath)!=NULL
				&& M2MString_append(databaseFilePath, M2MDirectory_SEPARATOR)!=NULL
				&& M2MString_append(databaseFilePath, databaseName)!=NULL)
			{
			//===== Release heap memory of directory path string =====
			M2MHeap_free(directoryPath);
			//=====  =====
			return (*databaseFilePath);
			}
		//===== Error handling =====
		else
			{
			//===== In the case of directory path string exists =====
			if (directoryPath!=NULL)
				{
				//===== Release heap memory of directory path string =====
				M2MHeap_free(directoryPath);
				}
			else
				{
				}
			return NULL;
			}
		}
	//===== Argument error =====
	else if (databaseName==NULL || M2MString_length(databaseName)<=0)
		{
		M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"databaseName\" string is NULL or vacant");
		return NULL;
		}
	else
		{
		M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"databaseFilePath\" is NULL");
		return NULL;
		}
	}


/**
 * @param columnList
 * @param columnNameCSV
 * @param dataTypeArray
 * @param dataTypeArrayLength
 * @return
 */
static unsigned int this_getDataTypeArray (M2MColumnList *columnList, const M2MString *columnNameCSV, M2MDataType dataTypeArray[], const size_t dataTypeArrayLength)
	{
	//========== Variable ==========
	M2MString *columnName = NULL;
	M2MString *index = NULL;
	unsigned int dataTypeIndex = 0;
	M2MColumn *column = NULL;
	M2MDataType dataType;
	const size_t COMMA_LENGTH = M2MString_length((M2MString *)M2MString_COMMA);

	//===== Check argument =====
	if (columnList!=NULL
			&& columnNameCSV!=NULL && M2MString_length(columnNameCSV)>0
			&& dataTypeArray!=NULL
			&& dataTypeArrayLength>0)
		{
		//===== Get head pointer of column name in CSV format =====
		columnName = (M2MString *)columnNameCSV;
		//===== Create a column data type storage array =====
		memset(dataTypeArray, 0, dataTypeArrayLength);
		dataTypeIndex = 0;
		//===== Separate column names in CSV format with commas =====
		while (columnName!=NULL
				&& (index=M2MString_indexOf(columnName, (M2MString *)M2MString_COMMA))!=NULL)
			{
			//===== Search objects with the same column name =====
			if ((column=M2MColumnList_search(columnList, columnName, M2MString_length(columnName) - M2MString_length(index)))!=NULL)
				{
				//===== Get column data type =====
				dataType = M2MColumn_getDataType(column);
				//===== Store column data type in array =====
				dataTypeArray[dataTypeIndex] = dataType;
				}
			//===== Error handling =====
			else
				{
				dataTypeArray[dataTypeIndex] = M2MDataType_ERROR;
				}
			dataTypeIndex++;
			//===== Move the pointer by comma =====
			index += COMMA_LENGTH;
			//===== Move the start pointer to the next column name =====
			columnName = index;
			}
		//===== Search objects with the same column name =====
		if ((column=M2MColumnList_search(columnList, columnName, M2MString_length(columnName)))!=NULL)
			{
			//===== Get column data type =====
			dataType = M2MColumn_getDataType(column);
			//===== Store column data type in array =====
			dataTypeArray[dataTypeIndex] = dataType;
			dataTypeIndex++;
			}
		//===== Error handling =====
		else
			{
			dataTypeArray[dataTypeIndex] = M2MDataType_ERROR;
			}
		return dataTypeIndex;
		}
	//===== Argument error =====
	else
		{
		return 0;
		}
	}


/**
 * Get the upper limit of the number of CEP records held by the argument CEP object.<br>
 *
 * @param[in] self	CEP structure object
 * @return			The upper limit of the number of CEP records
 */
static unsigned int this_getMaxRecord (const M2MCEP *self)
	{
	//========== Variable ==========
	const unsigned int DEFAULT_MAX_RECORD = 50;
	const M2MString *METHOD_NAME = (M2MString *)"M2MCEP.this_getMaxRecord()";

	//===== Check argument =====
	if (self!=NULL)
		{
		return self->maxRecord;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MCEP\" structure object is NULL");
		return DEFAULT_MAX_RECORD;
		}
	}


/**
 * Get a flag indicating persistence availability from the argument CEP object.<br>
 *
 * @param[in,out] self		CEP structure object
 * @return					Flag indicating permanence
 */
static bool this_getPersistence (const M2MCEP *self)
	{
	//========== Variable ==========
	const bool PERSISTENCE = true;
	const M2MString *METHOD_NAME = (M2MString *)"M2MCEP.this_getPersistence()";

	//===== Check argument =====
	if (self!=NULL)
		{
		return self->persistence;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MCEP\" structure object is NULL");
		return PERSISTENCE;
		}
	}


/**
 * Get the number of inserted records held by the argument CEP object.<br>
 *
 * @param self	CEP structure object
 * @return		Number of inserted records
 */
static unsigned int this_getRecordCounter (const M2MCEP *self)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MCEP.this_getRecordCounter()";

	//===== Check argument =====
	if (self!=NULL)
		{
		return self->recordCounter;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MCEP\" structure object is NULL");
		return 0;
		}
	}


/**
 * Get the table construction object held by the CEP object as a member variable.<br>
 *
 * @param[in] self	CEP structure object
 * @return			Table construction object or NULL (in case of error)
 */
static M2MTableManager *this_getTableManager (const M2MCEP *self)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MCEP.this_getTableManager()";

	//===== Check argument =====
	if (self!=NULL)
		{
		return self->tableManager;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MCEP\" structure object is NULL");
		return NULL;
		}
	}


/**
 * Get the number of records for which SQLite3 database performs vacuum processing.<br>
 *
 * @param[in] self	CEP structure object
 * @return			Number of records for which SQLite 3 database performs vacuum processing
 */
static unsigned int this_getVacuumRecord (const M2MCEP *self)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MCEP.this_getVacuumRecord()";

	//===== Check argument =====
	if (self!=NULL)
		{
		return self->vacuumRecord;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MCEP\" structure object is NULL");
		return 0;
		}
	}


/**
 * @param[in] csv
 * @return				true : , false :
 */
static bool this_includesData (const M2MString *csv)
	{
	//========== Variable ==========
	M2MString *index = NULL;
	const size_t CRLF_LENGTH = M2MString_length((M2MString *)M2MString_CRLF);

	//===== Check argument =====
	if (csv!=NULL)
		{
		//===== Confirm header line feed code =====
		if ((index=M2MString_indexOf(csv, (M2MString *)M2MString_CRLF))!=NULL)
			{
			//===== Advance the pointer by the number of line feed code characters =====
			index += CRLF_LENGTH;
			//===== Confirm existence of data =====
			if (M2MString_length(index)>0)
				{
				return true;
				}
			//===== When data was not included =====
			else
				{
				return false;
				}
			}
		//===== When the header is not included =====
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
 * Initializes the CEP object specified by the argument.<br>
 *
 * @param[in,out] self		CEP structure object
 * @param[in] databaseName	SQLite3 database name on file
 * @param[in] tableManager	Object for building SQLite3 database table
 * @return					CEP structure object or NULL (in case of error)
 */
static M2MCEP *this_init (M2MCEP *self, const M2MString *databaseName, const M2MTableManager *tableManager)
	{
	//========== Variable ==========
	M2MFileAppender *logger = NULL;
	const unsigned int MAX_RECORD = 50;
	const bool PERSISTENCE = true;
	const M2MString *METHOD_NAME = (M2MString *)"M2MCEP.this_init()";

	//===== Check argument =====
	if (self!=NULL
			&& databaseName!=NULL && M2MString_length(databaseName)>0
			&& tableManager!=NULL)
		{
		//===== Initialize CEP object =====
		if (this_setDatabaseName(self, databaseName)!=NULL
				&& M2MCEP_setMaxRecord(self, MAX_RECORD)!=NULL
				&& this_setDataFrame(self, M2MDataFrame_new())!=NULL
				&& this_setTableManager(self, tableManager)!=NULL
				&& M2MCEP_setPersistence(self, PERSISTENCE)!=NULL
				&& (logger=M2MFileAppender_new())!=NULL
				&& M2MCEP_setLogger(self, logger)!=NULL
				)
			{
			return self;
			}
		//===== Error handling =====
		else
			{
			M2MCEP_delete(&self);
			M2MLogger_error(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Failed to initialize \"M2MCEP\" object");
			return NULL;
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MCEP\" structure object is NULL");
		return NULL;
		}
	else if (databaseName==NULL || M2MString_length(databaseName)<=0)
		{
		M2MLogger_error(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"databaseName\" string is NULL or vacant");
		return NULL;
		}
	else
		{
		M2MLogger_error(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MTableManager\" structure object is NULL");
		return NULL;
		}
	}


/**
 * @param[in] fileDatabase	SQLite3 managed object on file
 * @param[in] tableManager	Table construction object
 * @param[in] tableRecord	Table record management object
 * @param[in] maxRecord		Maximum number of records to keep in SQLite3 database table in memory (Execution of persistence when it becomes larger)
 * @param[in] persistence	Flag indicating whether to perform persistence
 * @return
 */
static void this_insertOldRecordList (sqlite3 *fileDatabase, M2MDataFrame *tableRecord, M2MTableManager *tableManager, const unsigned int maxRecord, const bool persistence)
	{
	//========== Variable ==========
	sqlite3_stmt* statement = NULL;
	M2MColumnList *columnList = NULL;
	M2MList *oldRecordList = NULL;
	unsigned int oldRecordListLength = 0;
	M2MDataType DATA_TYPE_ARRAY[M2MSQLite_getMaxColumnLength(fileDatabase)];
	M2MString *value = NULL;
	M2MString *valueIndex = NULL;
	M2MString *insertSQL = NULL;
	M2MString *tableName = NULL;
	M2MString *columnNameCSV = NULL;
	unsigned int columnIndex = 0;
	unsigned int dataTypeArrayLength = 0;
	int resultCode = 0;
	int rest = 0;
	unsigned int i = 0;
	const size_t COMMA_LENGTH = M2MString_length((M2MString *)M2MString_COMMA);
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MCEP.this_insertOldRecordList()";

	//===== Check argument =====
	if (tableRecord!=NULL
			&& (tableName=M2MDataFrame_getTableName(tableRecord))!=NULL
			&& (oldRecordList=M2MList_begin(M2MDataFrame_getOldRecordList(tableRecord)))!=NULL
			&& (oldRecordListLength=M2MList_length(oldRecordList))>0)
		{
		//===== When the number of records in which SQLite 3 database has been inserted in memory exceeds the specified maximum number of records =====
		if ((rest=oldRecordListLength-maxRecord)>0)
			{
			//===== In case of past insert record persistence =====
			if (persistence==true)
				{
				//===== Preparation for past record insertion into SQLite 3 database on file =====
				if ((columnNameCSV=M2MDataFrame_getColumnName(tableRecord))!=NULL
						&& (columnList=M2MColumnList_begin(M2MTableManager_getColumnList(tableManager, tableName)))!=NULL
						&& (dataTypeArrayLength=this_getDataTypeArray(columnList, columnNameCSV, DATA_TYPE_ARRAY, sizeof(DATA_TYPE_ARRAY)))>0
						&& this_createInsertSQL(tableName, columnNameCSV, M2MColumnList_length(columnList), &insertSQL)!=NULL
						&& (statement=M2MSQLite_getPreparedStatement(fileDatabase, insertSQL))!=NULL)
					{
					}
				//===== Error handling =====
				else if (columnNameCSV==NULL)
					{
					M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"The CSV format string indicating the column name obtained from the table record management object is NULL");
					return;
					}
				else if (columnList==NULL)
					{
					M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"The column structure object of the table obtained from the table building object is NULL");
					return;
					}
				else if (dataTypeArrayLength<=0)
					{
					M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Column data type array obtained from table building object is NULL");
					return;
					}
				else if (insertSQL==NULL)
					{
					M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"An attempt to create an INSERT statement to insert a record into the SQLite 3 database on the file failed");
					return;
					}
				else
					{
					M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, M2MSQLite_getErrorMessage(fileDatabase));
					return;
					}
				}
			//===== In case of past insertion record non-persistence =====
			else
				{
				// do nothing
				}
			//===== Repeat as many records as the excess =====
			for (i=0; i<rest; i++)
				{
				//===== When to make it persistent =====
				if (persistence==true)
					{
					//===== Create an INSERT statement =====
					if ((value=(M2MString *)M2MList_getValue(oldRecordList))!=NULL)
						{
						//===== Initialize field position =====
						columnIndex = 0;
						//===== Repeat with column name in CSV format =====
						while (columnIndex<(dataTypeArrayLength-1)
								&& value!=NULL
								&& (valueIndex=M2MString_indexOf(value, (M2MString *)M2MString_COMMA))!=NULL)
							{
							//===== Set value =====
							M2MSQLite_setValueIntoPreparedStatement(DATA_TYPE_ARRAY[columnIndex], columnIndex+1, value, M2MString_length(value)-M2MString_length(valueIndex), statement);
							//===== Move the field position to the next =====
							columnIndex++;
							//===== Move the position of the value to the next =====
							valueIndex += COMMA_LENGTH;
							value = valueIndex;
							}
						//===== Check the consistency of the number of columns and the number of inserted data =====
						if (columnIndex==(dataTypeArrayLength-1))
							{
							//===== Set the last insert data =====
							M2MSQLite_setValueIntoPreparedStatement(DATA_TYPE_ARRAY[columnIndex], columnIndex+1, value, M2MString_length(value), statement);
							//===== Execute INSERT =====
							while ((resultCode=sqlite3_step(statement))==SQLITE_BUSY)
								{
								}
							//===== When INSERT succeeds =====
							if (resultCode==SQLITE_DONE)
								{
								}
							//===== Error handling =====
							else
								{
								M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, M2MSQLite_getErrorMessage(fileDatabase));
								}
							}
						//===== Reset parameters of INSERT statement =====
						sqlite3_reset(statement);
						}
					//===== Error creating INSERT statement =====
					else
						{
						M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Failed in preparing INSERT statement for inserting record into SQLite 3 database on file");
						}
					}
				//===== When not to perpetuate =====
				else
					{
					// do nothing
					}
				//===== Delete the record inserted in the database and move to the next record information object =====
				oldRecordList = M2MList_remove(oldRecordList);
				}
			//===== When to make it persistent =====
			if (persistence==true)
				{
				//===== Execution of SQL execution object (releasing heap memory area) =====
				M2MSQLite_closeStatement(statement);
				}
			//===== When not to perpetuate =====
			else
				{
				// do nothing
				}
			//===== Update record information object =====
			tableRecord->oldRecordList = oldRecordList;
			//===== Release heap memory area of INSERT statement =====
			M2MHeap_free(insertSQL);
			return;
			}
		//===== When the number of records in which SQLite 3 database has been inserted in memory is within the specified maximum number of records =====
		else
			{
			return;
			}
		}
	//===== Error handling =====
	else if (tableRecord==NULL)
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"The record management object specified by the argument is NULL");
		return;
		}
	else if (tableName==NULL)
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"String indicating table name obtained from table record management object is NULL");
		return;
		}
	else if (oldRecordList==NULL)
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Inserted record information object obtained from table record management object is NULL");
		return;
		}
	else
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Number of non-inserted records acquired from table record management object is 0 or less");
		return;
		}
	}


/**
 * Bulk insert records into SQLite 3 database specified by argument.<br>
 *
 * @param[in] database		SQLite3 database management object
 * @param[in] record		Record management object
 * @param[in] tableManager	Table construction object
 * @param[in] recordList	Record information object
 * @return					Number of inserted records or -1 (in case of error)
 */
static int this_insertRecordList (sqlite3 *database, const M2MDataFrame *record, M2MTableManager *tableManager, M2MList *recordList)
	{
	//========== Variable ==========
	sqlite3_stmt* statement = NULL;
	M2MColumnList *columnList = NULL;
	M2MDataType DATA_TYPE_ARRAY[M2MSQLite_getMaxColumnLength(database)];
	M2MString *value = NULL;
	M2MString *valueIndex = NULL;
	M2MString *insertSQL = NULL;
	M2MString *tableName = NULL;
	M2MString *columnNameCSV = NULL;
	int numberOfRecord = 0;
	unsigned int columnIndex = 0;
	unsigned int dataTypeArrayLength = 0;
	int resultCode = 0;
	const size_t COMMA_LENGTH = M2MString_length((M2MString *)M2MString_COMMA);
	const M2MString *METHOD_NAME = (M2MString *)"M2MCEP.this_insertRecordList()";

	//===== Check argument =====
	if (database!=NULL && tableManager!=NULL && record!=NULL && (recordList=M2MList_begin(recordList))!=NULL)
		{
		//===== Get table name, column name CSV and new record information object to be inserted =====
		if ((tableName=M2MDataFrame_getTableName(record))!=NULL
				&& (columnNameCSV=M2MDataFrame_getColumnName(record))!=NULL
				&& (columnList=M2MTableManager_getColumnList(tableManager, tableName))!=NULL
				&& (columnList=M2MColumnList_begin(columnList))!=NULL
				&& this_createInsertSQL(tableName, columnNameCSV, M2MColumnList_length(columnList), &insertSQL)!=NULL
				&& (statement=M2MSQLite_getPreparedStatement(database, insertSQL))!=NULL
				&& (dataTypeArrayLength=this_getDataTypeArray(columnList, columnNameCSV, DATA_TYPE_ARRAY, sizeof(DATA_TYPE_ARRAY)))>0)
			{
			//===== Reset parameters of SQL statement =====
			sqlite3_reset(statement);
			//===== Repeat with uninserted record information object =====
			while (M2MList_next(recordList)!=NULL)
				{
				//===== Create an INSERT statement =====
				if ((value=(M2MString *)M2MList_getValue(recordList))!=NULL)
					{
					//===== Initialize field position =====
					columnIndex = 0;
					//===== Repeat with column name in CSV format =====
					while (columnIndex<(dataTypeArrayLength-1)
							&& value!=NULL
							&& (valueIndex=M2MString_indexOf(value, (M2MString *)M2MString_COMMA))!=NULL)
						{
						//===== Set value to INSERT statement =====
						M2MSQLite_setValueIntoPreparedStatement(DATA_TYPE_ARRAY[columnIndex], columnIndex+1, value, M2MString_length(value)-M2MString_length(valueIndex), statement);
						//===== Move the field position to the next =====
						columnIndex++;
						//===== Move the position of the value to the next =====
						valueIndex += COMMA_LENGTH;
						value = valueIndex;
						}
					//===== Check the consistency of the number of columns and the number of inserted data =====
					if (columnIndex==(dataTypeArrayLength-1))
						{
						//===== Set the last data to INSERT statement =====
						M2MSQLite_setValueIntoPreparedStatement(DATA_TYPE_ARRAY[columnIndex], columnIndex+1, value, M2MString_length(value), statement);
						//===== Execute INSERT statement =====
						while ((resultCode=sqlite3_step(statement))==SQLITE_BUSY)
							{
							}
						//===== When INSERT succeeds =====
						if (resultCode==SQLITE_DONE)
							{
							}
						//===== Error handling =====
						else
							{
							M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"An error occurred in INSERT processing of SQLite3 database");
							}
						}
					//===== Reset parameters of INSERT statement =====
					sqlite3_reset(statement);
					//===== Count the number of inserted records =====
					numberOfRecord++;
					}
				//===== Error creating INSERT statement =====
				else
					{
					M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"Failed in preparing INSERT statement for inserting records into SQLite 3 database");
					}
				//===== Proceed to next uninserted record =====
				recordList = M2MList_next(recordList);
				}
			//===== Create an INSERT statement from the (last) uninserted record =====
			if ((value=(M2MString *)M2MList_getValue(recordList))!=NULL)
				{
				columnIndex = 0;
				//===== Repeat with column name in CSV format =====
				while (columnIndex<dataTypeArrayLength
						&& value!=NULL
						&& (valueIndex=M2MString_indexOf(value, (M2MString *)M2MString_COMMA))!=NULL)
					{
					//===== Set value to INSERT statement =====
					M2MSQLite_setValueIntoPreparedStatement(DATA_TYPE_ARRAY[columnIndex], columnIndex+1, value, M2MString_length(value)-M2MString_length(valueIndex), statement);
					//===== Move the field position to the next =====
					columnIndex++;
					//===== Move the position of the value to the next =====
					valueIndex += COMMA_LENGTH;
					value = valueIndex;
					}
				//===== Set the last data to INSERT statement =====
				M2MSQLite_setValueIntoPreparedStatement(DATA_TYPE_ARRAY[columnIndex], columnIndex+1, value, M2MString_length(value), statement);
				//===== Execute INSERT statement =====
				while ((resultCode=sqlite3_step(statement))==SQLITE_BUSY)
					{
					}
				//===== When INSERT succeeds =====
				if (resultCode==SQLITE_DONE)
					{
					}
				//===== Error handling =====
				else
					{
					M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"An error occurred in INSERT processing of SQLite3 database");
					}
				//===== Reset parameters of INSERT statement =====
				sqlite3_reset(statement);
				//===== Count the number of inserted records =====
				numberOfRecord++;
				}
			//===== Error creating INSERT statement =====
			else
				{
				M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"Failed in preparing INSERT statement for inserting records into SQLite 3 database");
				}
			//===== Finalize SQL execution (releasing heap memory area) =====
			M2MSQLite_closeStatement(statement);
			//===== Release heap memory area of INSERT statement =====
			M2MHeap_free(insertSQL);
			//===== Returns the number of inserted records =====
			return numberOfRecord;
			}
		//===== Error handling =====
		else if (tableName==NULL)
			{
			M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"String indicating table name obtained from record management object is NULL");
			return -1;
			}
		else if (columnNameCSV==NULL)
			{
			M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"The CSV format string indicating the column name obtained from the record management object is NULL");
			return -1;
			}
		else if (columnList==NULL)
			{
			M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"The column structure object of the table obtained from the table building object is NULL");
			return -1;
			}
		else if (statement==NULL)
			{
			M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"The prepared statement object obtained from the SQLite3 database management object specified by the argument is NULL");
			return -1;
			}
		else
			{
			M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"Column data type array obtained from table building object is NULL");
			return -1;
			}
		}
	//===== Argument error =====
	else if (database==NULL)
		{
		M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"The SQLite3 database management object specified by the argument is NULL");
		return -1;
		}
	else if (tableManager==NULL)
		{
		M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"The table construction object specified by the argument is NULL");
		return -1;
		}
	else if (record==NULL)
		{
		M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"The record management object specified by the argument is NULL");
		return -1;
		}
	else
		{
		M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"The uninserted record information object obtained from the record management object is NULL");
		return -1;
		}
	}


/**
 * If the specified maximum number of records is exceeded for the record of <br>
 * "already inserted in SQLite3 database in memory" of the CEP record management <br>
 * object held by the CEP execution object specified by the argument, the record <br>
 * of excess Into the SQLite 3 database on the file and make it persistent.<br>
 *
 * @param[in,out] self	CEP structure object
 * @return				Successful CEP execution object or NULL (in case of insertion error)
 */
static M2MCEP *this_insertRecordListToFileDatabase (M2MCEP *self)
	{
	//========== Variable ==========
	M2MTableManager *tableManager = NULL;
	M2MDataFrame *tableRecord = NULL;
	unsigned int maxRecord = 0;
	bool persistence = false;
	const M2MString *METHOD_NAME = (M2MString *)"M2MCEP.this_insertRecordListToFileDatabase()";

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Get table record management object =====
		if ((tableManager=this_getTableManager(self))!=NULL
				&& (tableRecord=M2MDataFrame_begin(this_getDataFrame(self)))!=NULL
				&& (maxRecord=this_getMaxRecord(self))>0)
			{
			//===== In the case of record persistence =====
			if ((persistence=this_getPersistence(self))==true)
				{
				M2MLogger_debug(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Start transaction processing to insert records into SQLite database on file");
				//===== Start Transaction =====
				M2MSQLite_beginTransaction(M2MCEP_getFileDatabase(self));
				}
			//===== In case of record non-persistence =====
			else
				{
				// do nothing
				}
			//===== Repeat with record management object =====
			while (M2MDataFrame_next(tableRecord)!=NULL)
				{
				//===== Execute past record processing held by record management object =====
				this_insertOldRecordList(M2MCEP_getFileDatabase(self), tableRecord, tableManager, maxRecord, persistence);
				//===== Move to the next table record management object =====
				tableRecord = M2MDataFrame_next(tableRecord);
				}
			//===== Execute past record processing held by the last record management object =====
			this_insertOldRecordList(M2MCEP_getFileDatabase(self), tableRecord, tableManager, maxRecord, persistence);
			//===== In the case of record persistence =====
			if (this_getPersistence(self)==true)
				{
				//===== End transaction =====
				M2MSQLite_commitTransaction(M2MCEP_getFileDatabase(self));
				M2MLogger_debug(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Transaction processing for inserting records into SQLite database on file has ended");
				}
			//===== In case of record non-persistence =====
			else
				{
				// do nothing
				}
			//===== Successful completion =====
			return self;
			}
		//===== Error handling =====
		else if (tableManager==NULL)
			{
			M2MLogger_error(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"The table building object obtained from argument \"CEP\" object is NULL");
			return NULL;
			}
		else if (tableRecord==NULL)
			{
			M2MLogger_error(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Table record management object obtained from argument \"CEP\" object is NULL");
			return NULL;
			}
		else
			{
			M2MLogger_error(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"The upper limit of the number of CEP records per table obtained from argument \"CEP\" object is less than or equal to 0");
			return NULL;
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MCEP\" structure object is NULL");
		return NULL;
		}
	}


/**
 * Insert all records (= CEPRecord-> newRecordList) held by CEP object variables <br>
 * into SQLite3 database in memory.<br>
 *
 * @param[in] self	CEP structure object
 * @return			Number of records where insert processing or -1 (in case of error)
 */
static int this_insertRecordListToMemoryDatabase (M2MCEP *self)
	{
	//========== Variable ==========
	sqlite3 *memoryDatabase = NULL;
	M2MTableManager *tableManager = NULL;
	M2MDataFrame *record = NULL;
	int numberOfRecord = 0;
	int result = 0;
	const M2MString *METHOD_NAME = (M2MString *)"M2MCEP.this_insertRecordListToMemoryDatabase()";

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Get database and record management object =====
		if ((memoryDatabase=M2MCEP_getMemoryDatabase(self))!=NULL
				&& (tableManager=this_getTableManager(self))!=NULL
				&& (record=M2MDataFrame_begin(this_getDataFrame(self)))!=NULL)
			{
			M2MLogger_debug(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Insert records into SQLite database in memory");
			//===== Start Transaction =====
			M2MSQLite_beginTransaction(memoryDatabase);
			//===== Repeat until record management object reaches the end =====
			while (M2MDataFrame_next(record)!=NULL)
				{
				//===== Batch insertion of records in the same table =====
				if ((result=this_insertRecordList(memoryDatabase, record, tableManager, M2MDataFrame_getNewRecordList(record)))>0)
					{
					//===== Move new inserted record to past record =====
					M2MDataFrame_moveFromNewRecordListToOldRecordList(record);
					//===== Count the number of inserted records =====
					numberOfRecord += result;
					}
				//===== Error handling =====
				else
					{
					M2MLogger_error(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Failed to insert record into SQLite database in memory");
					}
				//===== Proceed to the next record information object =====
				record = M2MDataFrame_next(record);
				}
			//===== Batch insertion of (final) table into table =====
			if ((result=this_insertRecordList(memoryDatabase, record, tableManager, M2MDataFrame_getNewRecordList(record)))>0)
				{
				//===== Move new inserted record to past record =====
				M2MDataFrame_moveFromNewRecordListToOldRecordList(record);
				//===== Count the number of inserted records =====
				numberOfRecord += result;
				}
			//===== Error handling =====
			else
				{
				M2MLogger_error(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Failed to insert record into SQLite database in memory");
				}
			//===== Commit =====
			M2MSQLite_commitTransaction(memoryDatabase);
			M2MLogger_debug(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Finished inserting records into SQLite database in memory");
			//===== Returns the number of inserted records =====
			return numberOfRecord;
			}
		//===== Error handling =====
		else if (memoryDatabase==NULL)
			{
			M2MLogger_error(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"The SQLite3 memory object obtained from the argument CEP object is NULL");
			return -1;
			}
		else if (tableManager==NULL)
			{
			M2MLogger_error(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"The table construction object obtained from the argument CEP object is NULL");
			return -1;
			}
		else
			{
			M2MLogger_error(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"The record management object obtained from the argument CEP object is NULL");
			return -1;
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MCEP\" structure object is NULL");
		return -1;
		}
	}


/**
 * Create a SQLite 3 database file under the specified directory.<br>
 * If creation fails, NULL is returned.<br>
 *
 * @param[in] databaseName	String indicating database name
 * @param[in] tableManager	Table information management object
 * @param[in] synchronous	true: Synchronous mode = NORMAL, false: Synchronous mode = OFF
 * @param[in] vacuumRecord	Number of records vacuum processing is to be executed (if this is 0, set automatic vacuum)
 * @return					SQLite3 database file object or NULL (in case of error)
 */
static sqlite3 *this_openFileDatabase (const M2MString *databaseName, const M2MTableManager *tableManager, const bool synchronous, const unsigned int vacuumRecord)
	{
	//========== Variable ==========
	sqlite3 *fileDatabase = NULL;
	M2MString *databaseFilePath = NULL;
	const M2MString *METHOD_NAME = (M2MString *)"M2MCEP.this_openFileDatabase()";

	//===== Check argument =====
	if (databaseName!=NULL && tableManager!=NULL)
		{
		//===== Get database file path stringl =====
		if (this_getDatabaseFilePath(databaseName, &databaseFilePath)!=NULL)
			{
			//===== Open SQLite3 file database =====
			if ((fileDatabase=M2MSQLite_openDatabase(databaseFilePath))!=NULL)
				{
				//===== Release memory space of database file path string =====
				M2MHeap_free(databaseFilePath);
				//===== Set character code to UTF-8 =====
				M2MSQLite_setUTF8(fileDatabase);
				//===== When automatic vacuum OFF =====
				if (vacuumRecord>0)
					{
					M2MSQLite_setAutoVacuum(fileDatabase, false);
					}
				//===== When automatic vacuum ON =====
				else
					{
					M2MSQLite_setAutoVacuum(fileDatabase, true);
					}
				//===== Set synchronous mode =====
				M2MSQLite_setSynchronous (fileDatabase, synchronous);
				//===== Construct a table =====
				M2MTableManager_createTable((M2MTableManager *)tableManager, fileDatabase);
				//===== Return SQLite3 database object =====
				return fileDatabase;
				}
			//===== Error handling =====
			else
				{
				M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"Failed to open SQLite3 file database");
				//===== Release memory space of database file path string =====
				M2MHeap_free(databaseFilePath);
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"Failed to get string indicating the file path of the SQLite database");
			return NULL;
			}
		}
	//===== Error handling =====
	else if (databaseName==NULL)
		{
		M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"The string indicating the SQLite database name on the file is NULL");
		return NULL;
		}
	else
		{
		M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"The table construction object specified by the argument is NULL");
		return NULL;
		}
	}


/**
 * Create a SQLite3 database on memory.<br>
 * If creation fails, NULL is returned.<br>
 *
 * @param[in] tableManager	Table information management object
 * @param[in] synchronous	true: Synchronous mode = NORMAL, false: Synchronous mode = OFF
 * @param[in] vacuumRecord	Number of records vacuum processing is to be executed (if this is 0, set automatic vacuum)
 * @return					SQLite3 database object or NULL (in case of error)
 */
static sqlite3 *this_openMemoryDatabase (const M2MTableManager *tableManager, const bool synchronous, const unsigned int vacuumRecord)
	{
	//========== Variable ==========
	sqlite3 *memoryDatabase = NULL;
	const M2MString *MEMORY_DATABASE = (M2MString *)":memory:";
	const M2MString *METHOD_NAME = (M2MString *)"M2MCEP.this_openMemoryDatabase()";

	//===== Open SQLite3 memory database =====
	if ((memoryDatabase=M2MSQLite_openDatabase(MEMORY_DATABASE))!=NULL)
		{
		//===== Set character code to UTF-8 =====
		M2MSQLite_setUTF8(memoryDatabase);
		//===== When automatic vacuum OFF =====
		if (vacuumRecord>0)
			{
			//===== Set the vacuum setting =====
			M2MSQLite_setAutoVacuum(memoryDatabase, false);
			}
		//===== When automatic vacuum ON =====
		else
			{
			//===== Set the vacuum setting =====
			M2MSQLite_setAutoVacuum(memoryDatabase, true);
			}
		//===== Set synchronous mode =====
		M2MSQLite_setSynchronous (memoryDatabase, synchronous);
		//===== Construct a table =====
		M2MTableManager_createTable((M2MTableManager *)tableManager, memoryDatabase);
		//===== Return SQLite3 database object in memory =====
		return memoryDatabase;
		}
	//===== Error handling =====
	else
		{
		M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"Failed to open SQLite database on memory");
		return NULL;
		}
	}


/**
 * Set SQLite3 database file name of member of the CEP structure object.<br>
 *
 * @param[in,out] self		CEP structure object to be set SQLite3 database file name
 * @param[in] databaseName	String indicating SQLite3 database file name
 * @return					CEP structure object with member variable updated or NULL (in case of error)
 */
static M2MCEP *this_setDatabaseName (M2MCEP *self, const M2MString *databaseName)
	{
	//========== Variable ==========
	M2MString MESSAGE[256];
	const M2MString *METHOD_NAME = (M2MString *)"M2MCEP.this_setDatabaseName()";

	//===== Check argument =====
	if (self!=NULL
			&& databaseName!=NULL && M2MString_length(databaseName)>0)
		{
		//===== Initialize SQLite3 fiel database name =====
		this_deleteDatabaseName(self);
		//===== Copy SQLite3 database file name =====
		if (M2MString_append(&(self->databaseName), databaseName)!=NULL)
			{
			//===== In the case of extension isn't given in name =====
			if (M2MString_lastIndexOf(databaseName, M2MSQLite_FILE_EXTENSION)==NULL)
				{
				//===== Copy SQLite3 file extension =====
				if (M2MString_append(&(self->databaseName), M2MSQLite_FILE_EXTENSION)!=NULL)
					{
					// do nothing
					}
				//===== Error handling =====
				else
					{
					this_deleteDatabaseName(self);
					M2MLogger_error(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Failed to set the SQLite3 database name in the CEP structure object");
					return NULL;
					}
				}
			//===== In the case of extension is given in name =====
			else
				{
				// do nothing
				}
			memset(MESSAGE, 0, sizeof(MESSAGE));
			snprintf(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Set the SQLite3 database file name (=\"%s\") in the CEP structure object", M2MCEP_getDatabaseName(self));
			M2MLogger_debug(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, MESSAGE);
			return self;
			}
		//===== Error handling =====
		else
			{
			M2MLogger_error(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Failed to get heap memory for copying SQLite3 database name");
			return NULL;
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_error(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MCEP\" structure object is NULL");
		return NULL;
		}
	else if (databaseName==NULL)
		{
		M2MLogger_error(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"databaseName\" string is NULL");
		return NULL;
		}
	else
		{
		M2MLogger_error(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"databaseName\" string is vacant");
		return NULL;
		}
	}


/**
 * Set indicated M2MDataFrame object into CEP structure object.<br>
 *
 * @param[in,out] self		CEP structure object to be set M2MDataFrame object
 * @param[in] dataFrame		M2MDataFrame object
 * @return					CEP structure object with member variable updated or NULL (in case of error)
 */
static M2MCEP *this_setDataFrame (M2MCEP *self, M2MDataFrame *dataFrame)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MDataFrame.this_setDataFrame()";

	//===== Check argument =====
	if (self!=NULL && dataFrame!=NULL)
		{
		self->dataFrame = dataFrame;
		M2MLogger_debug(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Set M2MDataFrame object into CEP object");
		return self;
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_error(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MCEP\" structure object is NULL");
		return NULL;
		}
	else
		{
		M2MLogger_error(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MDataFrame\" structure object is NULL");
		return NULL;
		}
	}


/**
 * Set the SQLite3 file database management object as the member variable held <br>
 * by the CEP execution object specified by the argument.<br>
 *
 * @param[in,out] self		CEP structure object
 * @param[in] fileDatabase	SQLite3 file database management object
 * @return					CEP structure object with member variable updated or NULL (in case of error)
 */
static M2MCEP *this_setFileDatabase (M2MCEP *self, sqlite3 *fileDatabase)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MCEP.this_setFileDatabase()";

	//===== Check argument =====
	if (self!=NULL && fileDatabase!=NULL)
		{
		self->fileDatabase=fileDatabase;
		M2MLogger_debug(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Set SQLite3 database management object on file to CEP execution object");
		return self;
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_error(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MCEP\" structure object is NULL");
		return NULL;
		}
	else
		{
		M2MLogger_error(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"SQLite3 execution object specified by argument is NULL");
		return NULL;
		}
	}


/**
 * Set the SQLite 3 database management object in memory as the member variable <br>
 * possessed by the CEP execution object specified by the argument.<br>
 *
 * @param[in,out] self			CEP structure object
 * @param[in] memoryDatabase	SQLite 3 database management object on memory
 * @return						CEP structure object with member variable updated or NULL (in case of error)
 */
static M2MCEP *this_setMemoryDatabase (M2MCEP *self, sqlite3 *memoryDatabase)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MCEP.this_setMemoryDatabase()";

	//===== Check argument =====
	if (self!=NULL && memoryDatabase!=NULL)
		{
		self->memoryDatabase = memoryDatabase;
		M2MLogger_debug(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Set SQLite3 database management object on memory in CEP object");
		return self;
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_error(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MCEP\" structure object is NULL");
		return NULL;
		}
	else
		{
		M2MLogger_error(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"SQLite3 execution object specified by argument is NULL");
		return NULL;
		}
	}


/**
 * Set a table construction object as a member variable for the argument CEP object.<br>
 *
 * @param[in,out] self		CEP structure object
 * @param[in] tableManager	Table construction object
 * @return					CEP structure object with member variable updated or NULL (in case of error)
 */
static M2MCEP *this_setTableManager (M2MCEP *self, const M2MTableManager *tableManager)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MCEP.this_setTableManager()";

	//===== Check argument =====
	if (self!=NULL && tableManager!=NULL)
		{
		//===== Delete an existing table building object =====
		this_deleteTableManager(self);
		//===== Set table construction object =====
		self->tableManager = (M2MTableManager *)tableManager;
		M2MLogger_debug(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Set a table construction object in the CEP object");
		return self;
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_error(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MCEP\" structure object is NULL");
		return NULL;
		}
	else
		{
		M2MLogger_error(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"The table construction object specified by the argument is NULL");
		return NULL;
		}
	}


/**
 * Update the number of records for executing the vacuum on the SQLite 3 database.<br>
 *
 * @param[in] self		CEP structure object
 * @param[in] count		Number of records added to the counter (> 0) or 0 (for initializing the counters)
 */
static void this_updateRecordCounter (M2MCEP *self, const unsigned int count)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MCEP.this_updateRecordCounter()";

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== When the automatic vacuum setting is ON =====
		if (this_getVacuumRecord(self)==0)
			{
			return;
			}
		//===== When automatic vacuum setting is OFF =====
		else
			{
			//===== When the counter is an integer of 1 or more =====
			if (count>=1)
				{
				//===== Add counter =====
				self->recordCounter = self->recordCounter + count;
				M2MLogger_debug(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Update record counters for vacuum processing");
				return;
				}
			//===== When the counter is 0 =====
			else
				{
				//===== Initialize counter =====
				self->recordCounter = 0;
				M2MLogger_debug(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Vacuum processing record counter has been initialized");
				return;
				}
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MCEP\" structure object is NULL");
		return;
		}
	}


/*******************************************************************************
 * Public function
 ******************************************************************************/
/**
 * Release the heap memory of CEP structure object. <br>
 * Note that after calling this function, caller will not be able to access <br>
 * the CEP structure object appointed by argument.<br>
 *
 * @param[in,out] self	CEP structure object to be freed of memory area
 */
void M2MCEP_delete (M2MCEP **self)
	{
	//========== Variable ==========
	M2MDataFrame *record = NULL;
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MCEP_delete()";

	//===== Check argument =====
	if (self!=NULL && (*self)!=NULL)
		{
		//===== Release heap memory for database name string =====
		this_deleteDatabaseName((*self));
		//===== Close memory database =====
		this_closeMemoryDatabase((*self));
		//===== Insert CEP record information object (not inserted) into file database =====
		this_flushCEPRecord((*self));
		//===== Close file database =====
		this_closeFileDatabase((*self));
		//=====  =====
		this_deleteLogger((*self));
		//===== Release the heap memory area of the CEP record information object =====
		if ((record=this_getDataFrame((*self)))!=NULL)
			{
			M2MDataFrame_delete(&record);
			}
		else
			{
			}
		//===== Release heap memory area of table construction object =====
		this_deleteTableManager((*self));
		//===== Release CEP object's heap memory area =====
		M2MHeap_free((*self));
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MCEP\" object is NULL");
		}
	return;
	}


/**
 * Return string indicating the database name possessed by the CEP object.<br>
 *
 * @param[in] self	CEP structure object
 * @return			String indicating database file name or NULL (in case of error)
 */
M2MString *M2MCEP_getDatabaseName (const M2MCEP *self)
	{
	//========== Variable ==========
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MCEP_getDatabaseName()";

	//===== Check argument =====
	if (self!=NULL)
		{
		return self->databaseName;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MCEP\" structure object is NULL");
		return NULL;
		}
	}


/**
 * Get the SQLite3 database object on the file managed by the CEP object. <br>
 * If the CEP object does not hold the SQLite 3 database object on the file <br>
 * and the record persistence flag is set up, it will acquire and return <br>
 * the SQLite 3 database object newly created.<br>
 *
 * @param[in] self	CEP structure object
 * @return			SQLite3 database object on file or NULL (in case of error)
 */
sqlite3 *M2MCEP_getFileDatabase (M2MCEP *self)
	{
	//========== Variable ==========
	M2MString *databaseName = NULL;
	M2MTableManager *tableManager = NULL;
	sqlite3 *fileDatabase = NULL;
	const bool SYNCHRONOUS_MODE = false;
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MCEP_getFileDatabase()";

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Confirm the flag for record persistence =====
		if (this_getPersistence(self)==true)
			{
			//===== Confirm SQLite 3 database management object on file =====
			if (self->fileDatabase!=NULL)
				{
				return self->fileDatabase;
				}
			//===== When not owning the SQLite 3 database management object on the file =====
			else
				{
				//===== Get SQLite3 database management object on file =====
				if ((databaseName=M2MCEP_getDatabaseName(self))!=NULL
						&& (tableManager=this_getTableManager(self))!=NULL
						&& (fileDatabase=this_openFileDatabase(databaseName, tableManager, SYNCHRONOUS_MODE, this_getVacuumRecord(self)))!=NULL)
					{
					//===== Set SQLite3 database management object on file =====
					this_setFileDatabase(self, fileDatabase);
					//===== Return SQLite3 database management object on file =====
					return fileDatabase;
					}
				//===== Error handling =====
				else if (databaseName==NULL)
					{
					M2MLogger_error(M2MCEP_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Failed to get database file name necessary for creating SQLite 3 database management object on file");
					return NULL;
					}
				//===== Error handling =====
				else if (tableManager==NULL)
					{
					M2MLogger_error(M2MCEP_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Failed to get the table construction object necessary for creating the SQLite 3 database management object on the file");
					return NULL;
					}
				//===== Error handling =====
				else
					{
					M2MLogger_error(M2MCEP_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Failed to create SQLite3 database management object on file");
					return NULL;
					}
				}
			}
		//===== When the record persistence flag is not set =====
		else
			{
			M2MLogger_debug(M2MCEP_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Because record persistence is invalid, we don't get the SQLite3 database object on the file");
			return NULL;
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MCEP\" structure object is NULL");
		return NULL;
		}
	}


/**
 * Return the logging object owned by the argument CEP object.<br>
 *
 * @param[in] self	CEP structure object
 * @return			Logging object
 */
M2MFileAppender *M2MCEP_getLogger (const M2MCEP *self)
	{
	//========== Variable ==========
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MCEP_getLogger()";

	//===== Check argument =====
	if (self!=NULL)
		{
		return self->logger;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MCEP\" structure object is NULL");
		return NULL;
		}
	}


/**
 * Return the SQLite 3 database object on the memory managed by the CEP object.<br>
 *
 * @param[in] self	CEP structure object
 * @return			SQLite3 database object on memory or NULL (in case of error)
 */
sqlite3 *M2MCEP_getMemoryDatabase (M2MCEP *self)
	{
	//========== Variable ==========
	sqlite3 *memoryDatabase = NULL;
	const bool SYNCHRONOUS_MODE = false;
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MCEP_getMemoryDatabase()";

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Confirm SQLite 3 database management object in memory =====
		if (self->memoryDatabase!=NULL)
			{
			return self->memoryDatabase;
			}
		//===== When the SQLite 3 database management object in the memory doesn't exist =====
		else
			{
			//===== Open SQLite 3 database management object in memory =====
			if ((memoryDatabase=this_openMemoryDatabase(this_getTableManager(self), SYNCHRONOUS_MODE, this_getVacuumRecord(self)))!=NULL)
				{
				//===== Set SQLite3 database management object on memory =====
				this_setMemoryDatabase(self, memoryDatabase);
				//===== Return SQLite3 database management object in memory =====
				return memoryDatabase;
				}
			//===== Error handling =====
			else
				{
				M2MLogger_error(M2MCEP_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Failed to create SQLite3 database object in memory");
				return NULL;
				}
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MCEP\" structure object is NULL");
		return NULL;
		}
	}


/**
 * Return CEP library version number string defined in "M2MCEP.h" file.
 *
 * @return	CEP library version number string
 */
M2MString *M2MCEP_getVersion ()
	{
	return M2MCEP_VERSION;
	}


/**
 * Insert string in CSV format into the table of SQLite 3 database for CEP. <br>
 * The internal processing procedure is as follows.<br>
 * <br>
 * 1) Parse CSV string and import into a CEPRecord structure object. <br>
 * 2) Insert record data into the table of SQLite3 memory database. <br>
 * 3) Check the current record number of the table of SQLite3 memory database, <br>
 *    and delete it in the oldest order if it exceeds the specified <br>
 *    maximum value. <br>
 * 4) Fetch the same data as the deleted record from the CEPRecord <br>
 *    structure object and insert it into the SQLite3 database file for <br>
 *    persistence. <br>
 * 5) Delete excess from record data of CEPRecord structure object. <br>
 *
 * @param[in,out] self	CEP structure object
 * @param[in] tableName	String indicating the table name to be inserted record
 * @param[in] csv		String in CSV format as insert data (the first line specifies the column name as a header)
 * @return				Number of inserted records[row] or -1 (in case of error)
 */
int M2MCEP_insertCSV (M2MCEP *self, const M2MString *tableName, const M2MString *csv)
	{
	//========== Variable ==========
	int numberOfRecord = 0;
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MCEP_insertCSV()";

	//===== Check argument =====
	if (self!=NULL && tableName!=NULL && csv!=NULL)
		{
		//===== Set CSV string to M2MDataFrame object =====
		if (M2MDataFrame_setCSV(this_getDataFrame(self), tableName, csv)>0)
			{
			//===== Insert record into SQLite3 memory database =====
			if ((numberOfRecord=this_insertRecordListToMemoryDatabase(self))>=0)
				{
				//===== Update record counters for vacuum processing =====
				this_updateRecordCounter(self, numberOfRecord);
				//===== Adjust the number of records in SQLite3 database in memory =====
				this_adjustMemoryDatabaseRecord(self, tableName);
				//===== Insert record into SQLite3 database file for persistence =====
				if (this_insertRecordListToFileDatabase(self)!=NULL)
					{
					//===== Execute vacuum processing to SQLite3 databases =====
					this_checkRecordCounterForVacuum(self);
					//===== Return number of records =====
					return numberOfRecord;
					}
				//===== Error handling =====
				else
					{
					M2MLogger_error(M2MCEP_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Failed to insert record into SQLite3 database on file");
					return -1;
					}
				}
			//===== Error handling =====
			else
				{
				M2MLogger_error(M2MCEP_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Failed to insert record into SQLite 3 database in memory");
				return -1;
				}
			}
		//===== Error handling =====
		else
			{
			M2MLogger_error(M2MCEP_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Failed to set CSV format string to record management object");
			return -1;
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MCEP\" structure object is NULL");
		return -1;
		}
	else if (tableName==NULL)
		{
		M2MLogger_error(M2MCEP_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! The string indicating the table name specified by the argument is NULL");
		return -1;
		}
	else
		{
		M2MLogger_error(M2MCEP_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! The string indicating the CSV format record specified by the argument is NULL");
		return -1;
		}
	}


/**
 * Constructor.<br>
 * Create new CEP structure object and prepare for SQLite3 database processing. <br>
 * For the table, be sure to specify the SQLite3 database on memory as it <br>
 * needs to be built every time "M2MCEP" object is created. <br>
 *
 * @param[in] databaseName	String indicating SQLite3 database name
 * @param[in] tableBuilder	Structure object for building SQLite3 database table
 * @return					Created CEP structure object or NULL (in case of error)
 */
M2MCEP *M2MCEP_new (const M2MString *databaseName, const M2MTableManager *tableManager)
	{
	//========== Variable ==========
	M2MCEP *self = NULL;
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MCEP_new()";

	//===== Check argument =====
	if (databaseName!=NULL && tableManager!=NULL)
		{
		//===== Get heap memory of CEP execution object =====
		if ((self=(M2MCEP *)M2MHeap_malloc(sizeof(M2MCEP)))!=NULL)
			{
			//===== Initialize member variable of CEP object =====
			if (this_init(self, databaseName, tableManager)!=NULL)
				{
				//===== Return new created CEP object =====
				return self;
				}
			//===== Error handling =====
			else
				{
				M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to initialize CEP object");
				//===== Release heap memory area for CEP object =====
				M2MCEP_delete(&self);
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to get the heap memory area for creating a new CEP object");
			return NULL;
			}
		}
	//===== Argument error =====
	else if (databaseName==NULL)
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"databaseName\" string is NULL");
		return NULL;
		}
	else
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"tableManager\" object is NULL");
		return NULL;
		}
	}


/**
 * Executes the SQL statement specified by the argument on the SQLite 3 database <br>
 * in memory and returns the result as a CSV format string.<br>
 * For the character string of the CSV format as the execution result, the <br>
 * column name character string is the first line, and the data after the second <br>
 * line is the data.<br>
 * <br>
 * [Attention!]<br>
 * In order to execute this function, "SQLITE_ENABLE_COLUMN_METADATA" must be <br>
 * enabled as option setting at compile time of SQLite 3.<br>
 * This is because this function can not create a character string of CSV format <br>
 * to be output as a result unless table information is acquired from the SELECT <br>
 * result.<br>
 * If "SQLITE_ENABLE_COLUMN_METADATA" is not valid, keep in mind that this <br>
 * function will cause a compile error (because undefined functions are called).<br>
 *
 * @param[in] self		CEP structure object (used for database management)
 * @param[in] sql		String indicating SQL statement
 * @param[out] result	CSV character string of execution result or NULL (in case of error)
 * @return
 */
M2MString *M2MCEP_select (M2MCEP *self, const M2MString *sql, M2MString **result)
	{
	//========== Variable ==========
	sqlite3 *memoryDatabase = NULL;
	sqlite3_stmt* statement = NULL;
	int resultCode = 0;
	int columnLength = -1;
	int i = 0;
	bool thisIsHeader = true;
	M2MString *columnName = NULL;
	size_t columnNameLength = 0;
	M2MString *data = NULL;
	size_t dataLength = 0;
	int sqliteColumnType = 0;
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MCEP_select()";

	//===== Check argument =====
	if (self!=NULL && sql!=NULL && result!=NULL)
		{
		//===== Convert SQL statement to VDBE (SQLite internal execution format) =====
		if ((memoryDatabase=M2MCEP_getMemoryDatabase(self))!=NULL
				&& (statement=M2MSQLite_getPreparedStatement(memoryDatabase, sql))!=NULL
				)
			{
			//===== Execute SQL (repeat until the result row reaches the end) =====
			while ((resultCode=sqlite3_step(statement))==SQLITE_ROW)
				{
				//===== When the number of columns of the SELECT result has not been acquired yet =====
				if (columnLength<0)
					{
					//===== Get number of columns of SELECT result =====
					columnLength = sqlite3_column_count(statement);
					}
				//===== When acquiring the number of columns of the SELECT result =====
				else
					{
					// do nothing
					}
				//===== When header line is not output =====
				if (thisIsHeader==true)
					{
					//===== Repeat by number of columns =====
					for (i=0; i<columnLength; i++)
						{
						//===== (Reference code :) Get the original table name (UTF-8) which is not an alias =====
//						sqlite3_column_table_name(statement, 1);
						//===== Get the original column name (UTF-8) which is not an alias =====
						if ((columnName=(M2MString *)sqlite3_column_origin_name(statement, i))!=NULL
								&& (columnNameLength=M2MString_length(columnName))>0)
							{
							//===== In case of 1st time =====
							if (i==0)
								{
								// do nothing
								}
							//===== In case of the 2nd and subsequent times =====
							else
								{
								//===== Add comma character =====
								M2MString_append(result, M2MString_COMMA);
								}
							//===== Add column name =====
							M2MString_append(result, columnName);
							}
						//===== Error handling =====
						else
							{
							M2MLogger_error(M2MCEP_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Failed to get character string indicating column name from SELECT result");
							}
						}
					//===== Add line feed code =====
					M2MString_append(result, M2MString_CRLF);
					//===== Reduce header line output flag =====
					thisIsHeader = false;
					}
				//===== When header line has already been output =====
				else
					{
					// do nothing
					}
				//===== Repeat the output of SELECT result data by the number of columns =====
				for (i=0; i<columnLength; i++)
					{
					//===== In case of 1st time =====
					if (i==0)
						{
						// do nothing
						}
					//===== In case of the 2nd and subsequent times =====
					else
						{
						//===== Add comma character =====
						M2MString_append(result, M2MString_COMMA);
						}
					//===== When the data type of the SELECT result column is an integer =====
					if ((sqliteColumnType=sqlite3_column_type(statement, i))==SQLITE_INTEGER)
						{
						if (M2MString_convertFromSignedIntegerToString(sqlite3_column_int(statement, i), &data)!=NULL)
							{
							//===== Append integer data string =====
							M2MString_append(result, data);
							//===== Release heap memory area of data string =====
							M2MHeap_free(data);
							}
						//===== Error handling =====
						else
							{
							M2MLogger_error(M2MCEP_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Failed to convert Integer data of SELECT result to character string");
							}
						}
					//===== When the data type of the SELECT result column is a floating point number =====
					else if (sqliteColumnType==SQLITE_FLOAT)
						{
						if (M2MString_convertFromDoubleToString(sqlite3_column_double(statement, i), &data)!=NULL)
							{
							//===== Append real number data string =====
							M2MString_append(result, data);
							//===== Release heap memory area of data string =====
							M2MHeap_free(data);
							}
						//===== Error handling =====
						else
							{
							M2MLogger_error(M2MCEP_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Failed to convert Floating point number data of SELECT result to character string");
							}
						}
					//===== When the data type of the SELECT result column is a character string =====
					else if (sqliteColumnType==SQLITE_TEXT)
						{
						if ((data=(M2MString *)sqlite3_column_text(statement, i))!=NULL)
							{
							//===== Add character string =====
							M2MString_append(result, data);
							}
						//===== Error handling =====
						else
							{
							M2MLogger_error(M2MCEP_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Failed to get character string data of SELECT result");
							}
						}
					//===== When the data type of the SELECT result column is byte data =====
					else if (sqliteColumnType==SQLITE_BLOB)
						{
						if ((dataLength=(size_t)sqlite3_column_bytes(statement, i))>0
								&& M2MBase64_encode(sqlite3_column_blob(statement, i), dataLength, &data, false)!=NULL)
							{
							//===== Added character string obtained by converting byte data to base64 =====
							M2MString_append(result, data);
							//===== Release heap memory area of data string =====
							M2MHeap_free(data);
							}
						//===== Error handling =====
						else if (dataLength<=0)
							{
							M2MLogger_error(M2MCEP_getLogger(self), FUNCTION_NAME, __LINE__, M2MSQLite_getErrorMessage(memoryDatabase));
							}
						else
							{
							M2MLogger_error(M2MCEP_getLogger(self), FUNCTION_NAME, __LINE__, M2MSQLite_getErrorMessage(memoryDatabase));
							}
						}
					//===== When the data type of the SELECT result column is NULL =====
					else if (sqliteColumnType==SQLITE_NULL)
						{
						M2MString_append(result, (M2MString *)"NULL");
						}
					//===== In other cases =====
					else
						{
						M2MLogger_error(M2MCEP_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"SQLite internal error! (The data type of the SELECT result differs from the value of the rule)");
						}
					}
				//===== Add line feed code =====
				M2MString_append(result, M2MString_CRLF);
				}
			//===== Close SQLite3 statement object =====
			M2MSQLite_closeStatement(statement);
			//===== When data is included in the execution result =====
			if (this_includesData((*result))==true)
				{
				//===== Return the execution result as it is =====
				return (*result);
				}
			//===== When data is not included in the execution result =====
			else
				{
				//===== Release heap memory area of result =====
				M2MHeap_free((*result));
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			M2MLogger_error(M2MCEP_getLogger(self), FUNCTION_NAME, __LINE__, M2MSQLite_getErrorMessage(memoryDatabase));
			return NULL;
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MCEP\" structure object is NULL");
		return NULL;
		}
	else if (sql==NULL)
		{
		M2MLogger_error(M2MCEP_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"sql\" string is NULL");
		return NULL;
		}
	else
		{
		M2MLogger_error(M2MCEP_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"result\" pointer is NULL");
		return NULL;
		}
	}


/**
 * Set logging structure object in column information object.<br>
 *
 * @param[in,out] self	Column information object
 * @param[in] logger	Logging structure object
 * @return				Column information object with logger set or NULL (in case of error)
 */
M2MCEP *M2MCEP_setLogger (M2MCEP *self, const M2MFileAppender *logger)
	{
	//========== Variable ==========
	M2MFileAppender *oldLogger = NULL;
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MCEP_setLogger()";

	//===== Check argument =====
	if (self!=NULL && logger!=NULL)
		{
		//===== In case of existing logger object yet =====
		if ((oldLogger=self->logger)!=NULL)
			{
			//===== Release allocated memory =====
			M2MFileAppender_delete(&oldLogger);
			}
		//===== In case of no existing logger object =====
		else
			{
			}
		//===== Set logger object =====
		self->logger = (M2MFileAppender *)logger;
		//=====  =====
		return self;
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MColumn\" object is NULL");
		return NULL;
		}
	else
		{
		M2MLogger_error(M2MCEP_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MFileAppender\" object is NULL");
		return NULL;
		}
	}


/**
 * Change the maximum number of records that can be stored in each table in the <br>
 * SQLite 3 database existing in memory to the integer value specified by the <br>
 * argument.<br>
 * Note that it is not possible to set an integer number of 500 or more to the <br>
 * record number upper limit value.<br>
 *
 * @param[in,out] self		Maximum number of records CEP execution object to be changed
 * @param[in] maxRecord		Integer indicating the maximum number of records in one table or 0 (in case of error)
 * @return					CEP execution object or 0 (maximum number of records set)
 */
M2MCEP *M2MCEP_setMaxRecord (M2MCEP *self, const unsigned int maxRecord)
	{
	//========== Variable ==========
	M2MString MESSAGE[256];
	const unsigned int MAX_RECORD_MAX = 500;
	const M2MString *METHOD_NAME = (M2MString *)"M2MCEP_setMaxRecord()";

	//===== Check argument =====
	if (self!=NULL && 0<maxRecord && maxRecord<MAX_RECORD_MAX)
		{
		//===== Set number of records upper limit =====
		self->maxRecord = maxRecord;
		memset(MESSAGE, 0, sizeof(MESSAGE));
		snprintf(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"The upper limit number of records (=\"%u\") in the SQLite database in memory has been set in the CEP object", self->maxRecord);
		M2MLogger_debug(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, MESSAGE);
		return self;
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_error(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MCEP\" structure object is NULL");
		return NULL;
		}
	else if (maxRecord<=0)
		{
		M2MLogger_error(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Argument error! The upper limit of the number of records specified by argument is an integer less than or equal to 0 which is the minimum setting value.");
		return NULL;
		}
	else
		{
		M2MLogger_error(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Argument error! The number of records specified by the argument The upper limit value is an integer of 500 or more which is the maximum setting value.");
		return NULL;
		}
	}


/**
 * Set a flag indicating permanence (recording in the SQLite 3 database on the <br>
 * file) to the CEP structure object specified by the argument.<br>
 * When set to true, all records inserted by M2MCEP_insertCSV () are recorded in <br>
 * the SQLite 3 database on the file.<br>
 * If set to "false", do not record to the SQLite 3 database on the file.<br>
 *
 * @param[in,out] self		CEP structure object
 * @param[in] persistence	Flag indicating permanence (record in SQLite 3 database on file)
 * @return					CEP execution object with a flag indicating permanence availability or NULL (in case of error)
 */
M2MCEP *M2MCEP_setPersistence (M2MCEP *self, const bool persistence)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MCEP_setPersistence()";

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== In the case of persistence =====
		if (persistence==true)
			{
			self->persistence = true;
			M2MLogger_debug(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Set flag for insert record persistence to \"ON\"");
			return self;
			}
		//===== In case of non-persistence =====
		else if (persistence==false)
			{
			self->persistence = false;
			M2MLogger_debug(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Set flag for insert record persistence to \"OFF\"");
			return self;
			}
		//===== Error handling =====
		else
			{
			M2MLogger_error(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"The permanence flag indicated by argument is neither \"true\" nor \"false\"");
			return NULL;
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(M2MCEP_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MCEP\" structure object is NULL");
		return NULL;
		}
	}


/**
 * When forcibly terminating when using the shared library (libcep.so), set the <br>
 * event handler to acquire the signal and perform the termination processing.<br>
 *
 * @param[in] self	CEP structure object
 */
void M2MCEP_setSignalHandler (const M2MCEP *self)
	{
	//========== Variable ==========
	struct sigaction newAction;
	struct sigaction oldAction;

	//=====  =====
//	newAction.sa_handler = termination_handler;
	//=====  =====
	sigemptyset(&newAction.sa_mask);
	//=====  =====
	newAction.sa_flags = 0;
	//=====  =====
	sigaction(SIGINT, NULL, &oldAction);
	//=====  =====
	if (oldAction.sa_handler!=SIG_IGN)
		{
		sigaction (SIGINT, &newAction, NULL);
		}
	//=====  =====
	sigaction(SIGHUP, NULL, &oldAction);
	//=====  =====
	if (oldAction.sa_handler!=SIG_IGN)
		{
		sigaction(SIGHUP, &newAction, NULL);
		}
	//=====  =====
	sigaction(SIGTERM, NULL, &oldAction);
	//=====  =====
	if (oldAction.sa_handler!=SIG_IGN)
		{
		sigaction(SIGTERM, &newAction, NULL);
		}
	//=====  =====
	sigaction(SIGKILL, NULL, &oldAction);
	//=====  =====
	if (oldAction.sa_handler!=SIG_IGN)
		{
		sigaction(SIGKILL, &newAction, NULL);
		}
	return;
	}


/**
 * SQLite 3 Sets the number of records for vacuuming the database.<br>
 * For this vacuum processing setting, it is a setting of sharing SQLite 3 <br>
 * database on memory and SQLite 3 database on file.<br>
 *
 * @param self			Number of records for vacuum processing CEP execution object to be set
 * @param vacuumRecord	An integer value indicating the number of records of vacuum processing (automatic vacuum setting when 0 is specified)
 * @return				CEP execution object or number of records to be vacuum processed or NULL (in case of error)
 */
M2MCEP *M2MCEP_setVacuumRecord (M2MCEP *self, const unsigned int vacuumRecord)
	{
	//========== Variable ==========
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MCEP_setVacuumRecord()";

	//===== Check argument =====
	if (self!=NULL)
		{
		self->vacuumRecord = vacuumRecord;
		return self;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MCEP\" structure object is NULL");
		return NULL;
		}
	}



/* End Of File */
