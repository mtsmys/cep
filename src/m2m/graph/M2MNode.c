/*******************************************************************************
 * M2MNode.c
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

#include "m2m/graph/M2MNode.h"



/*******************************************************************************
 * Definition
 ******************************************************************************/
/**
 * Get randomized 32[bit] unsigned integer number.<br>
 *
 * @return	Randomized 32[bit] unsigned integer number
 */
static uint32_t this_getRandomNumber ();



/*******************************************************************************
 * Private function
 ******************************************************************************/
/**
 * @param[in] logger		Logging object
 * @param[out] buffer		Buffer for storing 8[Byte] node ID string which is unique in "m2m_node" table
 * @param[in] bufferLength	Size of buffer for storing 8[Byte] node ID
 * @return					Pointer of stored node ID or NULL (in case of error)
 */
static M2MString *this_createNewNodeID (const M2MFileAppender *logger, M2MString buffer[], const size_t bufferLength)
	{
	//========== Variable ==========
	M2MString message[64];
	const uint32_t RANDOM_NUMBER = this_getRandomNumber();
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MNode.this_createNewNodeID()";

	//===== Check argument =====
	if (buffer!=NULL && bufferLength>(sizeof(uint32_t)*2))
		{
		//===== Initialize buffer =====
		memset(buffer, 0, bufferLength);
		//===== Convert from 32bit integer to hexadecimal string =====
		if (M2MString_convertFromUnsignedLongToHexadecimalString(RANDOM_NUMBER, buffer, bufferLength)!=NULL)
			{
			memset(message, 0, sizeof(message));
			snprintf(message, sizeof(message)-1, (M2MString *)"Created new M2M node ID (=\"%s\")", buffer);
			M2MLogger_info(logger, FUNCTION_NAME, __LINE__, message);
			return buffer;
			}
		//===== Error handling =====
		else
			{
			M2MLogger_error(logger, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to convert from 32bit unsigned integer number to hexadecimal string");
			return NULL;
			}
		}
	//===== Argument error =====
	else if (buffer==NULL)
		{
		M2MLogger_error(logger, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"buffer\" array is NULL");
		return NULL;
		}
	else
		{
		M2MLogger_error(logger, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"bufferLength\" is too small to store hexadecimal string");
		return NULL;
		}
	}


/**
 * @param[in] database	SQLite3 database object
 * @param[in] logger	Logging object
 * @return				true: , false:
 */
static bool this_createNewTable (sqlite3 *database, const M2MFileAppender *logger)
	{
	//========== Variable ==========
	M2MString buffer[512];
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MNode.this_createNewTable()";
	const M2MString *SQL_FORMAT = (M2MString *)"CREATE TABLE %s (%s TEXT PRIMARY KEY NOT NULL UNIQUE, %s TEXT, %s TEXT, %s NUMERIC NOT NULL, %s NUMERIC NOT NULL) ";

	//===== Check argument =====
	if (database!=NULL)
		{
		//===== Check the existence of the table =====
		if (M2MSQLite_isExistingTable(database, M2MNode_TABLE_NAME)==true)
			{
			M2MLogger_debug(logger, FUNCTION_NAME, __LINE__, (M2MString *)"There is the table for storing records already");
			return true;
			}
		//===== In case of not existing the table =====
		else
			{
			//===== Create new SQL string =====
			memset(buffer, 0, sizeof(buffer));
			snprintf(buffer,
					sizeof(buffer)-1,
					SQL_FORMAT,
					M2MNode_TABLE_NAME,
					M2MNode_COLUMN_ID,
					M2MNode_COLUMN_NAME,
					M2MNode_COLUMN_PROPERTY,
					M2MNode_COLUMN_LEFT,
					M2MNode_COLUMN_RIGHT);

			//===== Startup transaction =====
			if (M2MSQLite_beginTransaction(database)==true)
				{
				//===== Execute SQL =====
				if (M2MSQLite_executeUpdate(database, buffer)==true)
					{
					//===== Shutdown transaction =====
					M2MSQLite_commitTransaction(database);
					M2MLogger_info(logger, FUNCTION_NAME, __LINE__, (M2MString *)"Created new table for storing records as nodes");
					return true;
					}
				//===== Error handling =====
				else
					{
					M2MLogger_error(logger, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to create a table in the SQLite3 database");
					//===== Shutdown transaction =====
					M2MSQLite_rollbackTransaction(database);
					return false;
					}
				}
			//===== Error handling =====
			else
				{
				M2MLogger_error(logger, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to startup transaction for SQL execution");
				return false;
				}
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(logger, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"sqlite3\" structure object is NULL");
		return false;
		}
	}


/**
 * Get randomized 32[bit] unsigned integer number.<br>
 *
 * @return	Randomized 32[bit] unsigned integer number
 */
static uint32_t this_getRandomNumber ()
	{
	//========== Variable ==========
	TinyMT32 tinyMT;
	uint32_t randomNumber = 0UL;
	const uint32_t CURRENT_TIME = M2MDate_getCurrentTimeMillis();

	//===== Initialize with seed =====
	TinyMT32_init(&tinyMT, CURRENT_TIME);
	//===== Get random number =====
	randomNumber = TinyMT32_generate_uint32(&tinyMT);
	return randomNumber;
	}


/**
 * Insert a new record into indicated database as an expression of one node in a graph.<br>
 * 
 * @param[in] database	SQLite3 database object
 * @param[in] name		String indicating node name
 * @param[in] property	String indicating property belonging to the node or NULL
 * @param[in] logger	Logging object
 * @param[out] id		Buffer for storing 8[Byte] node ID string which is unique in "m2m_node" table
 * @param[in] idLength	Size of buffer for storing 8[Byte] node ID
 * @return				Pointer of stored node ID or NULL (in case of error)
 */
static M2MString *this_setRecord (sqlite3 *database, const M2MString *name, const M2MString *property, const M2MFileAppender *logger, M2MString id[], const size_t idLength)
	{
	//========== Variable ==========
	size_t nameLength = 0;
	size_t propertyLength = 0;
	M2MString *sql = NULL;
	size_t sqlLength = 0;
	M2MString message[1024];
	sqlite3_stmt *statement = NULL;
	int result = 0;
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MNode.this_setRecord()";
	const M2MString *SQL_FORMAT = (M2MString *)"INSERT INTO %s (%s,%s,%s,%s,%s) VALUES (?,?,?,0,0) ";

	//===== Check argument =====
	if (database!=NULL
			&& name!=NULL && (nameLength=M2MString_length(name))>0 
			&& idLength>0)
		{
		//===== Get length of property string =====
		propertyLength = M2MString_length(property);
		//===== Check the existence of table =====
		if (M2MSQLite_isExistingTable(database, M2MNode_TABLE_NAME)==false)
			{
			//===== Create new table =====
			if (this_createNewTable(database, logger)==true)
				{
				}
			//===== Error handling =====
			else
				{
				M2MLogger_error(logger, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to create the table for setting a record");
				return NULL;
				}
			}
		//===== In case of existing the table =====
		else
			{
			}
		//===== Create new node ID =====
		if (this_createNewNodeID(logger, id, idLength)!=NULL)
			{
			}
		//===== Error handling =====
		else
			{
			M2MLogger_error(logger, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to create new node ID string");
			return NULL;
			}
		//===== Allocate heap memory for SQL string =====
		if ((sqlLength=M2MString_length(SQL_FORMAT) 
				+ M2MString_length(M2MNode_TABLE_NAME) 
				+ M2MString_length(M2MNode_COLUMN_ID) 
				+ M2MString_length(M2MNode_COLUMN_NAME) 
				+ M2MString_length(M2MNode_COLUMN_PROPERTY) 
				+ M2MString_length(M2MNode_COLUMN_LEFT) 
				+ M2MString_length(M2MNode_COLUMN_RIGHT) 
				+ M2MString_length(id) 
				+ nameLength
				+ propertyLength 
				+ 1)>0 
				&& (sql=(M2MString *)M2MHeap_malloc(sqlLength))!=NULL)
			{
			//===== Initialize buffer =====
			memset(sql, 0, sqlLength);
			//===== Prepare INSERT SQL =====
			snprintf(sql,
					sqlLength-1,
					SQL_FORMAT,
					M2MNode_TABLE_NAME,
					M2MNode_COLUMN_ID,
					M2MNode_COLUMN_NAME,
					M2MNode_COLUMN_PROPERTY,
					M2MNode_COLUMN_LEFT,
					M2MNode_COLUMN_RIGHT
					);
			//===== Startup transaction =====
			if (M2MSQLite_beginTransaction(database)==true)
				{
				//===== Create new prepared statement =====
				if ((statement=M2MSQLite_getPreparedStatement(database, sql))!=NULL)
					{
					//===== Set value into statement =====
					M2MSQLite_setValueIntoPreparedStatement(M2MSQLiteDataType_TEXT, 1, id, M2MString_length(id), statement);
					M2MSQLite_setValueIntoPreparedStatement(M2MSQLiteDataType_TEXT, 2, name, nameLength, statement);
					//===== In the case of existing node property information =====
					if (property!=NULL && propertyLength>0)
						{
						//===== Set value into statement =====
						M2MSQLite_setValueIntoPreparedStatement(M2MSQLiteDataType_TEXT, 3, property, propertyLength, statement);
						}
					//===== In the case of no existing node property information =====
					else
						{
						//===== Set value into statement =====
						M2MSQLite_setValueIntoPreparedStatement(M2MSQLiteDataType_NULL, 3, property, 0, statement);
						}

					//========== Execute INSERT SQL ==========
					if ((result=M2MSQLite_next(statement))==SQLITE_ROW || result==SQLITE_DONE)
						{
						//===== Finish transaction =====
						M2MSQLite_commitTransaction(database);
						//===== Release prepared statement object =====
						M2MSQLite_closeStatement(statement);
						//===== Release heap memory area of SQL string =====
						M2MHeap_free(sql);
						return id;
						}
					//===== Error handling =====
					else
						{
						memset(message, 0, sizeof(message));
						snprintf(message, sizeof(message)-1, (M2MString *)"Failed to set a record because of \"%s\"", M2MSQLite_getErrorMessage(database));
						M2MLogger_error(logger, FUNCTION_NAME, __LINE__, message);
						//===== Finish transaction =====
						M2MSQLite_rollbackTransaction(database);
						//===== Release prepared statement object =====
						M2MSQLite_closeStatement(statement);
						//===== Release heap memory area of SQL string =====
						M2MHeap_free(sql);
						return NULL;
						}
					}
				//===== Error handling =====
				else
					{
					M2MLogger_error(logger, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to create the prepared statement object");
					//===== Release heap memory area of SQL string =====
					M2MHeap_free(sql);
					return NULL;
					}
				}
			//===== Error handling =====
			else
				{
				M2MLogger_error(logger, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to startup transaction for SQL execution");
				//===== Release heap memory area of SQL string =====
				M2MHeap_free(sql);
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			M2MLogger_error(logger, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to allocate heap memory for creating INSERT SQL string");
			return NULL;
			}
		}
	//===== Argument error =====
	else if (database==NULL)
		{
		M2MLogger_error(logger, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"database\" structure object is NULL");
		return NULL;
		}
	else
		{
		M2MLogger_error(logger, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"name\" string is NULL or vacant");
		return NULL;
		}
	}


/**
 * @param[in] database	SQLite3 database object
 * @param[in] id		Hexadecimal string indicating node ID number
 * @param[in] left
 * @param[in] right
 * @param[in] logger	Logging object
 * @return
 */
static bool this_updateNestedSetsModel (sqlite3 *database, const M2MString *id, const M2MString *left, const M2MString *right, const M2MFileAppender *logger)
	{
	//========== Variable ==========
	size_t idLength = 0;
	size_t leftLength = 0;
	size_t rightLength = 0;
	M2MString sql[512];
	sqlite3_stmt *statement = NULL;
	int result = 0;
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MNode.this_updateNestedSetsModel()";
	const M2MString *SQL_FORMAT = (M2MString *)"UPDATE %s SET %s = ?, %s = ? WHERE %s = ? ";

	//===== Check argument =====
	if (database!=NULL && id!=NULL && (idLength=M2MString_length(id))>0)
		{
		//===== Initialize buffer for SQL =====
		memset(sql, 0, sizeof(sql));
		//===== Prepare UPDATE SQL string =====
		snprintf(sql,
				sizeof(sql)-1,
				SQL_FORMAT,
				M2MNode_TABLE_NAME,
				M2MNode_COLUMN_LEFT,
				M2MNode_COLUMN_RIGHT,
				M2MNode_COLUMN_ID);
		//=====  =====
		if ((statement=M2MSQLite_getPreparedStatement(database, sql))!=NULL)
			{
			//===== In case of setting Nested Sets Model values =====
			if (left!=NULL && (leftLength=M2MString_length(left))>0
					&& right!=NULL && (rightLength=M2MString_length(right))>0)
				{
				//===== Set values into statement =====
				if (M2MSQLite_setValueIntoPreparedStatement(M2MSQLiteDataType_NUMERIC, 1, left, leftLength, statement)==true
						&& M2MSQLite_setValueIntoPreparedStatement(M2MSQLiteDataType_NUMERIC, 2, right, rightLength, statement)==true
						&& M2MSQLite_setValueIntoPreparedStatement(M2MSQLiteDataType_TEXT, 3, id, idLength, statement)==true
						&& ((result=M2MSQLite_next(statement))==SQLITE_ROW || result==SQLITE_DONE))
					{
					M2MSQLite_closeStatement(statement);
					return true;
					}
				//===== Error handling =====
				else
					{
					M2MSQLite_closeStatement(statement);
					return false;
					}
				}
			//===== In case of initializing values =====
			else if (left==NULL && right==NULL)
				{
				//===== Set values into statement =====
				if (M2MSQLite_setValueIntoPreparedStatement(M2MSQLiteDataType_NULL, 1, NULL, 0, statement)==true
						&& M2MSQLite_setValueIntoPreparedStatement(M2MSQLiteDataType_NULL, 2, NULL, 0, statement)==true
						&& M2MSQLite_setValueIntoPreparedStatement(M2MSQLiteDataType_TEXT, 3, id, idLength, statement)==true
						&& ((result=M2MSQLite_next(statement))==SQLITE_ROW || result==SQLITE_DONE))
					{
					M2MSQLite_closeStatement(statement);
					return true;
					}
				//===== Error handling =====
				else
					{
					M2MSQLite_closeStatement(statement);
					return false;
					}
				}
			//===== Error handling =====
			else
				{
				M2MSQLite_closeStatement(statement);
				return false;
				}
			}
		//===== Error handling =====
		else
			{
			M2MLogger_error(logger, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to get a prepared statement object");
			return false;
			}
		}
	//===== Argument error =====
	else if (database==NULL)
		{
		M2MLogger_error(logger, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"sqlite3\" structure object is NULL");
		return false;
		}
	else
		{
		M2MLogger_error(logger, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"id\" string is NULL or vacant");
		return false;
		}
	}



/*******************************************************************************
 * Public function
 ******************************************************************************/
/**
 * Constructor.<br>
 * Set a new record as 1 node into the "m2m_node" table.<br>
 *
 * @param[in] database	SQLite3 database object
 * @param[in] name		String indicating unique node name in "m2m_node" table
 * @param[in] property	String indicating property belonging to the node or NULL
 * @param[in] logger	Logging object
 * @param[out] id		Buffer for storing 8[Byte] node ID string which is unique in "m2m_node" table
 * @param[in] idLength	Size of buffer for storing 8[Byte] node ID
 * @return				Pointer of stored node ID or NULL (in case of error)
 */
M2MString *M2MNode_add (sqlite3 *database, const M2MString *name, const M2MString *property, const M2MFileAppender *logger, M2MString id[], const size_t idLength)
	{
	//========== Variable ==========
	size_t nameLength = 0;
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MNode_add()";

	//===== Check argument =====
	if (database!=NULL 
			&& name!=NULL && (nameLength=M2MString_length(name))>0)
		{
		//===== Set a new record into table =====
		return this_setRecord(database, name, property, logger, id, idLength);
		}
	//===== Argument error =====
	else if (database==NULL)
		{
		M2MLogger_error(logger, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"sqlite3\" structure object is NULL");
		return NULL;
		}
	else
		{
		M2MLogger_error(logger, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"name\" string is NULL");
		return NULL;
		}
	}


/**
 * Destructor.<br>
 * Delete a record indicated with ID in the "m2m_node" table.
 *
 * @param[in] database	SQLite3 database object
 * @param[in] id		Node ID string which is unique in "m2m_node" table
 * @param[in] logger	Logging object
 */
void M2MNode_delete (sqlite3 *database, const M2MString *id, const M2MFileAppender *logger)
	{
	//========== Variable ==========
	size_t idLength = 0;
	M2MString sql[128];
	sqlite3_stmt *statement = NULL;
	int result = 0;
	M2MString message[1024];
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MNode_delete()";
	const M2MString *SQL_FORMAT = (M2MString *)"DELETE FROM %s WHERE %s = ? ";

	//===== Check argument =====
	if (database!=NULL && id!=NULL && (idLength=M2MString_length(id))>0)
		{
		//===== Create DELETE SQL =====
		memset(sql, 0, sizeof(sql));
		snprintf(sql,
				sizeof(sql)-1,
				SQL_FORMAT,
				M2MNode_TABLE_NAME,
				M2MNode_COLUMN_ID);
		//===== Get prepared statement =====
		if ((statement=M2MSQLite_getPreparedStatement(database, sql))!=NULL)
			{
			//===== Execute SQL statement =====
			if (M2MSQLite_setValueIntoPreparedStatement(M2MSQLiteDataType_TEXT, 1, id, idLength, statement)==true
					&& ((result=M2MSQLite_next(statement))==SQLITE_ROW || result==SQLITE_DONE))
				{
				}
			//===== Error handling =====
			else
				{
				memset(message, 0, sizeof(message));
				snprintf(message, sizeof(message)-1, (M2MString *)"Failed to delete a record because of \"%s\"", M2MSQLite_getErrorMessage(database));
				M2MLogger_error(logger, FUNCTION_NAME, __LINE__, message);
				}
			//===== Release prepared statement object =====
			M2MSQLite_closeStatement(statement);
			}
		//===== Error handling =====
		else
			{
			M2MLogger_error(logger, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to get a prepared statement object");
			}
		}
	//===== Argument error =====
	else if (database==NULL)
		{
		M2MLogger_error(logger, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"sqlite3\" structure object is NULL");
		}
	else
		{
		M2MLogger_error(logger, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"id\" string is NULL or vacant");
		}
	return;
	}


/**
 * @param[in] database	SQLite3 database object
 * @param[in] name		String indicating node name
 * @param[in] logger	Logging object
 * @param[out] id		Buffer for storing node ID string
 * @param[in] idLength	Size of buffer for storing 8[Byte] node ID
 * @return				Pointer of stored node ID or NULL (in case of error)
 */
M2MString *M2MNode_getID (sqlite3 *database, const M2MString *name, const M2MFileAppender *logger, M2MString id[], const size_t idLength)
	{
	//========== Variable ==========
	size_t nameLength = 0;
	M2MString sql[1024];
	sqlite3_stmt *statement = NULL;
	int result = 0;
	M2MString *value = NULL;
	size_t valueLength = 0;
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MNode_getID()";
	const M2MString *SQL_FORMAT = (M2MString *)"SELECT %s FROM %s WHERE %s = ? ";

	//===== Check argument =====
	if (database!=NULL
			&& name!=NULL && (nameLength=M2MString_length(name))>0)
		{
		//===== Prepare SQL statement =====
		memset(sql, 0, sizeof(sql));
		snprintf(sql,
				sizeof(sql)-1,
				SQL_FORMAT,
				M2MNode_COLUMN_ID,
				M2MNode_TABLE_NAME,
				M2MNode_COLUMN_NAME
				);
		//===== Get prepared statement =====
		if ((statement=M2MSQLite_getPreparedStatement(database, sql))!=NULL)
			{
			//===== Execute SQL =====
			if (M2MSQLite_setValueIntoPreparedStatement(M2MSQLiteDataType_TEXT, 1, name, nameLength, statement)==true
					&& ((result=M2MSQLite_next(statement))==SQLITE_ROW || result==SQLITE_DONE)
					&& (value=(M2MString *)sqlite3_column_text(statement, 0))!=NULL
					&& (valueLength=M2MString_length(value))<idLength)
				{
				//===== Copy ID string into buffer =====
				memset(id, 0, idLength);
				memcpy(id, value, valueLength);
				//===== Release prepared statement object =====
				M2MSQLite_closeStatement(statement);
				return id;
				}
			//===== Error handling =====
			else if (valueLength>=idLength)
				{
				//===== Release prepared statement object =====
				M2MSQLite_closeStatement(statement);
				M2MLogger_error(logger, FUNCTION_NAME, __LINE__, (M2MString *)"");
				return NULL;
				}
			else
				{
				//===== Release prepared statement object =====
				M2MSQLite_closeStatement(statement);
				M2MLogger_error(logger, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to get node ID string from SQLite table");
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			M2MLogger_error(logger, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to get a prepared statement object");
			return NULL;
			}
		}
	//===== Argument error =====
	else if (database==NULL)
		{
		M2MLogger_error(logger, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"sqlite3\" structure object is NULL");
		return NULL;
		}
	else
		{
		M2MLogger_error(logger, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"name\" string is NULL or vacant");
		return NULL;
		}
	}


/**
 * @param[in] database	SQLite3 database object
 * @param[in] logger	Logging object
 * @param[out] idList	List object for storing ID number strings (allocation is executed in this function, so caller must release this memory)
 * @return				List object stored strings indicating node IDs which are unique in "m2m_node" table
 */
M2MList *M2MNode_getIDList (sqlite3 *database, const M2MFileAppender *logger, M2MList **idList)
	{
	//========== Variable ==========
	M2MString sql[128];
	sqlite3_stmt *statement = NULL;
	int32_t resultCode = 0;
	int32_t columnLength = -1;
	int32_t i = 0;
	M2MString *id = NULL;
	size_t idLength = 0;
	int32_t sqliteColumnType = 0;
//	const bool CHUNK = false;
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MNode_getIDList()";
	const M2MString *SQL_FORMAT = (M2MString *)"SELECT %s FROM %s ORDER BY %s ";

	//===== Check argument =====
	if (database!=NULL && idList!=NULL)
		{
		//===== Prepare buffer for storing ID numbers =====
		if (((*idList)=M2MList_new())!=NULL)
			{
			//===== Prepare SQL statement =====
			memset(sql, 0, sizeof(sql));
			snprintf(
					sql, 
					sizeof(sql)-1, 
					SQL_FORMAT, 
					M2MNode_COLUMN_ID, 
					M2MNode_TABLE_NAME, 
					M2MNode_COLUMN_NAME
					);
			//===== Get prepared statement object =====
			if ((statement=M2MSQLite_getPreparedStatement(database, sql))!=NULL)
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
					//===== Repeat the output of SELECT result data by the number of columns =====
					for (i=0; i<columnLength; i++)
						{
						//===== When the data type of the SELECT result column is an integer =====
						if ((sqliteColumnType=sqlite3_column_type(statement, i))==SQLITE_INTEGER)
							{
/*
							if (M2MString_convertFromSignedIntegerToString(sqlite3_column_int(statement, i), &value)!=NULL)
								{
								M2MList_add((*idList), value, M2MString_length(value));
								M2MHeap_free(value);
								}
							//===== Error handling =====
							else
								{
								M2MLogger_error(logger, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to convert Integer data of SELECT result to character string");
								}
*/
							}
						//===== When the data type of the SELECT result column is a floating point number =====
						else if (sqliteColumnType==SQLITE_FLOAT)
							{
/*
							if (M2MString_convertFromDoubleToString(sqlite3_column_double(statement, i), &value)!=NULL)
								{
								M2MList_add((*idList), value, M2MString_length(value));
								M2MHeap_free(value);
								}
							//===== Error handling =====
							else
								{
								M2MLogger_error(logger, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to convert Floating point number data of SELECT result to character string");
								}
*/
							}
						//===== When the data type of the SELECT result column is a character string =====
						else if (sqliteColumnType==SQLITE_TEXT)
							{
							if ((idLength=(size_t)sqlite3_column_bytes(statement, i))>0
									&& (id=(M2MString *)M2MHeap_malloc(idLength+1))!=NULL
									&& memset(id, 0, idLength+1)!=NULL
									&& memcpy(id, sqlite3_column_text(statement, i), idLength)!=NULL)
								{
								M2MList_add((*idList), id, idLength);
								M2MHeap_free(id);
								}
							//===== Error handling =====
							else
								{
								M2MLogger_error(logger, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to get character string data of SELECT result");
								}
							}
						//===== When the data type of the SELECT result column is byte data =====
						else if (sqliteColumnType==SQLITE_BLOB)
							{
/*
							if ((valueLength=(size_t)sqlite3_column_bytes(statement, i))>0
									&& M2MBase64_encode(
											(unsigned char *)sqlite3_column_blob(statement, i),
											valueLength,
											&value,
											CHUNK)!=NULL)
								{
								M2MList_add((*idList), value, M2MString_length(value));
								M2MHeap_free(value);
								}
							//===== Error handling =====
							else if (valueLength<=0)
								{
								M2MLogger_error(logger, FUNCTION_NAME, __LINE__, M2MSQLite_getErrorMessage(database));
								}
							else
								{
								M2MLogger_error(logger, FUNCTION_NAME, __LINE__, M2MSQLite_getErrorMessage(database));
								}
*/
							}
						//===== When the data type of the SELECT result column is NULL =====
						else if (sqliteColumnType==SQLITE_NULL)
							{
							}
						//===== In other cases =====
						else
							{
							M2MLogger_error(logger, FUNCTION_NAME, __LINE__, (M2MString *)"SQLite internal error! (The data type of the SELECT result differs from the value of the rule)");
							}
						}
					}
				//===== Close SQLite3 statement object =====
				M2MSQLite_closeStatement(statement);
				return (*idList);
				}
			//===== Error handling =====
			else
				{
				M2MLogger_error(logger, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to get prepared statement object depending on indicated \"sql\" string");
				//===== Release allocated memory =====
				M2MList_delete((*idList));
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			M2MLogger_error(logger, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to allocate heap memory for constructing \"M2MList\" structure object");
			return NULL;
			}
		}
	//===== Argument error =====
	else if (database==NULL)
		{
		M2MLogger_error(logger, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"sqlite3\" structure object is NULL");
		return NULL;
		}
	else
		{
		M2MLogger_error(logger, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MList\" object pointer is NULL");
		return NULL;
		}
	}


/**
 * @param[in] database	SQLite3 database object
 * @param[in] id		Number indicating node ID which is unique in "m2m_node" table
 * @param[in] logger	Logging object
 * @param[out] name		Pointer to copying the node name (buffering is executed inside this function)
 * @return				String indicating node name or NULL (in case of error)
 */
M2MString *M2MNode_getName (sqlite3 *database, const M2MString *id, const M2MFileAppender *logger, M2MString **name)
	{
	//========== Variable ==========
	size_t idLength = 0;
	M2MString sql[128];
	sqlite3_stmt *statement = NULL;
	int result = 0;
	M2MString message[1024];
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MNode_getName()";
	const M2MString *SQL_FORMAT = (M2MString *)"SELECT %s FROM %s WHERE %s = ? ";

	//===== Check argument =====
	if (database!=NULL && id!=NULL && (idLength=M2MString_length(id))>0)
		{
		//===== Create SQL =====
		memset(sql, 0, sizeof(sql));
		snprintf(sql,
				sizeof(sql)-1,
				SQL_FORMAT,
				M2MNode_COLUMN_NAME,
				M2MNode_TABLE_NAME,
				M2MNode_COLUMN_ID);
		//===== Get SQLite3 prepared statement =====
		if ((statement=M2MSQLite_getPreparedStatement(database, sql))!=NULL)
			{
			//===== Get node name string =====
			if (M2MSQLite_setValueIntoPreparedStatement(M2MSQLiteDataType_TEXT, 1, id, idLength, statement)==true
					&& ((result=M2MSQLite_next(statement))==SQLITE_ROW || result==SQLITE_DONE)
					&& M2MString_append(name, sqlite3_column_text(statement, 0))!=NULL)
				{
				//===== Release prepared statement object =====
				M2MSQLite_closeStatement(statement);
				return (*name);
				}
			//===== Error handling =====
			else
				{
				memset(message, 0, sizeof(message));
				snprintf(message, sizeof(message)-1, (M2MString *)"Failed to get node name string because of \"%s\"", M2MSQLite_getErrorMessage(database));
				M2MLogger_error(logger, FUNCTION_NAME, __LINE__, message);
				//===== Release prepared statement object =====
				M2MSQLite_closeStatement(statement);
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			M2MLogger_error(logger, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to get a prepared statement object");
			return NULL;
			}
		}
	//===== Argument error =====
	else if (database==NULL)
		{
		M2MLogger_error(logger, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"sqlite3\" structure object is NULL");
		return NULL;
		}
	else
		{
		M2MLogger_error(logger, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"id\" string is NULL or vacant");
		return NULL;
		}
	}


/**
 * @param[in] database	SQLite3 database object
 * @param[in] id		Number indicating node ID which is unique inside "m2m_node" table
 * @param[in] logger	Logging object
 * @param[out] property	Pointer to copying property belonging to the node
 * @return				Pointer of stored node property string
 */
M2MString *M2MNode_getProperty (sqlite3 *database, const M2MString *id, const M2MFileAppender *logger, M2MString **property)
	{
	//========== Variable ==========
	size_t idLength = 0;
	M2MString sql[1024];
	sqlite3_stmt *statement = NULL;
	int result = 0;
	M2MString message[128];
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MNode_getProperty()";
	const M2MString *SQL_FORMAT = (M2MString *)"SELECT %s FROM %s WHERE %s = ? ";

	//===== Check argument =====
	if (database!=NULL && id!=NULL && (idLength=M2MString_length(id))>0)
		{
		//===== Create SQL =====
		memset(sql, 0, sizeof(sql));
		snprintf(sql,
				sizeof(sql)-1,
				SQL_FORMAT,
				M2MNode_COLUMN_PROPERTY,
				M2MNode_TABLE_NAME,
				M2MNode_COLUMN_ID);
		//===== Get SQLite3 prepared statement =====
		if ((statement=M2MSQLite_getPreparedStatement(database, sql))!=NULL)
			{
			//===== Get node property string =====
			if (M2MSQLite_setValueIntoPreparedStatement(M2MSQLiteDataType_TEXT, 1, id, idLength, statement)==true
					&& ((result=M2MSQLite_next(statement))==SQLITE_ROW || result==SQLITE_DONE)
					&& M2MString_append(property, sqlite3_column_text(statement, 0))!=NULL)
				{
				//===== Release prepared statement object =====
				M2MSQLite_closeStatement(statement);
				return (*property);
				}
			//===== Error handling =====
			else
				{
				memset(message, 0, sizeof(message));
				snprintf(message, sizeof(message)-1, (M2MString *)"Failed to get node property string because of \"%s\"", M2MSQLite_getErrorMessage(database));
				M2MLogger_error(logger, FUNCTION_NAME, __LINE__, message);
				//===== Release prepared statement object =====
				M2MSQLite_closeStatement(statement);
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			M2MLogger_error(logger, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to get a prepared statement object");
			return NULL;
			}
		}
	//===== Argument error =====
	else if (database==NULL)
		{
		M2MLogger_error(logger, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"sqlite3\" structure object is NULL");
		return NULL;
		}
	else
		{
		M2MLogger_error(logger, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"id\" string is NULL or vacant");
		return NULL;
		}
	}


/**
 * Set left & right parameters for Nested Sets Model into a node.<br>
 * If initialize them, set 0 as their arguments in the same time.<br>
 *
 * @param[in] database	SQLite3 database object
 * @param[in] id		Node ID string which is unique in "m2m_node" table
 * @param[in] left		Nested Sets Model parameter (>=1) or 0 (in case of initialization)
 * @param[in] right		Nested Sets Model parameter (>=1) or 0 (in case of initialization)
 * @param[in] logger	Logging object
 * @return				String indicating node ID which was set Nested Sets Model parameter or NULL (in case of error)
 */
M2MString *M2MNode_setNestedSetsModel (sqlite3 *database, M2MString *id, const uint32_t left, const uint32_t right, const M2MFileAppender *logger)
	{
	//========== Variable ==========
	M2MString leftString[16];
	M2MString rightString[16];
	M2MString message[1024];
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MNode_setNestedSetsModel()";

	//===== Check argument =====
	if (database!=NULL && id!=NULL && M2MString_length(id)>0)
		{
		//===== In case of setting parameters =====
		if (left>0 && right>0 && left!=right
				&& M2MString_convertFromUnsignedLongToString(left, leftString, sizeof(leftString))!=NULL
				&& M2MString_convertFromUnsignedLongToString(right, rightString, sizeof(rightString))!=NULL)
			{
			//===== Update a record =====
			if (this_updateNestedSetsModel(database, id, leftString, rightString, logger)==true)
				{
				return id;
				}
			//===== Error handling =====
			else
				{
				memset(message, 0, sizeof(message));
				snprintf(message, sizeof(message)-1, (M2MString *)"Failed to update a record into (id, left, right) = (%s, %u, %u)", id, left, right);
				M2MLogger_error(logger, FUNCTION_NAME, __LINE__, message);
				return NULL;
				}
			}
		//===== In case of initialize parameters =====
		else if (left==0 && right==0)
			{
			//===== Update a record =====
			if (this_updateNestedSetsModel(database, id, NULL, NULL, logger)==true)
				{
				return id;
				}
			//===== Error handling =====
			else
				{
				memset(message, 0, sizeof(message));
				snprintf(message, sizeof(message)-1, (M2MString *)"Failed to update a record into (id, left, right) = (%s, %u, %u)", id, left, right);
				M2MLogger_error(logger, FUNCTION_NAME, __LINE__, message);
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			M2MLogger_error(logger, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"left\" and \"right\" number are invalid");
			return NULL;
			}
		}
	//===== Argument error =====
	else if (database==NULL)
		{
		M2MLogger_error(logger, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"sqlite3\" structure object is NULL");
		return NULL;
		}
	else
		{
		M2MLogger_error(logger, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"id\" string is NULL or vacant");
		return NULL;
		}
	}



/* End Of File */
