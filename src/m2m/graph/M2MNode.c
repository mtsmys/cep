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
 * Private function
 ******************************************************************************/
/**
 * Get randomized 4[Byte] unsigned integer number.<br>
 *
 * @return	Randomized 4[Byte] unsigned integer number
 */
static uint32_t this_createNewNodeID ()
	{
	//========== Variable ==========
	static uint32_t x = 123456789;
	static uint32_t y = 362436069;
	static uint32_t z = 521288629;
	static uint32_t w = 88675123;
	uint32_t t = 0UL;

	//===== Get Xorshift number =====
	t = x ^ (x << 11);
	x = y;
	y = z;
	z = w;
	return (w = (w ^ (w >> 19)) ^ (t ^ (t >> 8)));
	}


/**
 * @param[in] database	SQLite3 database object
 * @return				true: , false:
 */
static bool this_createNewTable (sqlite3 *database)
	{
	//========== Variable ==========
	M2MString SQL[512];
	M2MString message[128];
	const M2MString *METHOD_NAME = (M2MString *)"M2MNode.this_createNewTable()";

	//===== Check argument =====
	if (database!=NULL)
		{
		//===== Check the existence of the table =====
		if (M2MSQLiteConfig_isExistingTable(database, M2MNode_TABLE_NAME)==true)
			{
			return true;
			}
		//===== In case of not existing the table =====
		else
			{
			//===== Create new SQL string =====
			memset(SQL, 0, sizeof(SQL));
			snprintf(SQL,
					sizeof(SQL)-1,
					(M2MString *)"CREATE TABLE %s (%s TEXT PRIMARY KEY  NOT NULL  UNIQUE, %s TEXT NOT NULL  UNIQUE, %s TEXT, %s NUMERIC, %s NUMERIC) ",
					M2MNode_TABLE_NAME,
					M2MNode_COLUMN_ID,
					M2MNode_COLUMN_NAME,
					M2MNode_COLUMN_PROPERTY,
					M2MNode_COLUMN_LEFT,
					M2MNode_COLUMN_RIGHT);
			//===== Startup transaction =====
			M2MSQLRunner_beginTransaction(database);
			//===== Execute SQL =====
			if (M2MSQLRunner_executeUpdate(database, SQL)==true)
				{
#ifdef DEBUG
				memset(message, 0, sizeof(message));
				snprintf(message, sizeof(message)-1, (M2MString *)"Succeed to create a table(=\"%s\") in SQLite3 database", M2MNode_TABLE_NAME);
				M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, message);
#endif // DEBUG
				}
			//===== Error handling =====
			else
				{
				memset(message, 0, sizeof(message));
				snprintf(message, sizeof(message)-1, (M2MString *)"Failed to create \"%s\" table in SQLite3 database because %s", M2MNode_TABLE_NAME, sqlite3_errmsg(database));
				M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, message, NULL);
				}
			//===== Shutdown transaction =====
			M2MSQLRunner_commitTransaction(database);
			return true;
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"sqlite3\" object is NULL", NULL);
		return false;
		}
	}


/**
 * @param[in] database	SQLite3 database object
 * @param[in] id		Number indicating node ID which is unique in "m2m_node" table
 * @param[in] name		String indicating node name
 * @param[in] property	String indicating property belonging to the node or NULL
 * @return				Number indicating node ID which is unique in "m2m_node" table or 0 (in case of error)
 */
static uint32_t this_setRecord (sqlite3 *database, const M2MString *name, const M2MString *property)
	{
	//========== Variable ==========
	size_t nameLength = 0;
	size_t propertyLength = 0;
	uint32_t id = 0;
	M2MString hexadecimalString[16];
	M2MString buffer[512];
	sqlite3_stmt *statement = NULL;
	int result = 0;
	const M2MString *METHOD_NAME = (M2MString *)"M2MNode.this_setRecord()";

	//===== Check argument =====
	if (database!=NULL
			&& name!=NULL && (nameLength=M2MString_length(name))>0)
		{
		memset(hexadecimalString, 0, sizeof(hexadecimalString));
		//===== Create new node ID =====
		if ((id=this_createNewNodeID())>0
				&& M2MString_convertFromUnsignedLongToHexadecimalString(id, hexadecimalString, sizeof(hexadecimalString))!=NULL)
			{
			memset(buffer, 0, sizeof(buffer));
			snprintf(buffer, sizeof(buffer)-1, (M2MString *)"INSERT INTO %s (%s,%s,%s) VALUES (?,?,?) ",
					M2MNode_TABLE_NAME,
					M2MNode_COLUMN_ID,
					M2MNode_COLUMN_NAME,
					M2MNode_COLUMN_PROPERTY);
			//=====  =====
			if ((statement=M2MSQLiteConfig_getPreparedStatement(database, buffer))!=NULL)
				{
				M2MSQLiteConfig_setValueIntoPreparedStatement(M2M_DATA_TYPE_TEXT, 1, hexadecimalString, M2MString_length(hexadecimalString), statement);
				M2MSQLiteConfig_setValueIntoPreparedStatement(M2M_DATA_TYPE_TEXT, 2, name, nameLength, statement);
				//===== In the case of existing node property information =====
				if (property!=NULL && (propertyLength=M2MString_length(property))>0)
					{
					//=====  =====
					M2MSQLiteConfig_setValueIntoPreparedStatement(M2M_DATA_TYPE_TEXT, 3, property, propertyLength, statement);
					}
				//=====  =====
				else
					{
					//=====  =====
					M2MSQLiteConfig_setValueIntoPreparedStatement(M2M_DATA_TYPE_NULL, 3, property, 0, statement);
					}
				//===== Set a new record indicating a node =====
				if ((result=M2MSQLiteConfig_next(statement))==SQLITE_ROW || result==SQLITE_DONE)
					{
					//===== Release prepared statement object =====
					sqlite3_finalize(statement);
#ifdef DEBUG
					M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, (M2MString *)"Succeed to insert a new record in node table");
#endif // DEBUG
					return id;
					}
				//===== Error handling =====
				else
					{
					//===== Release prepared statement object =====
					sqlite3_finalize(statement);
					memset(buffer, 0, sizeof(buffer));
					snprintf(buffer, sizeof(buffer)-1, (M2MString *)"Failed to insert a new record in node table because %s", sqlite3_errmsg(database));
					M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, buffer, NULL);
					return 0;
					}
				}
			//===== Error handling =====
			else
				{
				M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to get a prepared statement object", NULL);
				return 0;
				}
			}
		//===== Error handling =====
		else if (id<=0)
			{
			M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to create a node ID number", NULL);
			return 0;
			}
		else
			{
			M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to convert node ID number into hexadecimal string", NULL);
			return 0;
			}
		}
	//===== Argument error =====
	else if (database==NULL)
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"database\" object is NULL", NULL);
		return 0;
		}
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"name\" string is NULL or vacant", NULL);
		return 0;
		}
	}



/*******************************************************************************
 * Public function
 ******************************************************************************/
/**
 * Destructor.<br>
 * Delete a record indicated with ID in the "m2m_node" table.
 *
 * @param[in] database	SQLite3 database object
 * @param[in] id		Number indicating node ID which is unique in "m2m_node" table
 */
void M2MNode_delete (sqlite3 *database, const uint32_t id)
	{
	//========== Variable ==========
	M2MString hexadecimalString[16];
	M2MString buffer[512];
	sqlite3_stmt *statement = NULL;
	int result = 0;
	const M2MString *METHOD_NAME = (M2MString *)"M2MNode_delete";

	//===== Check argument =====
	if (database!=NULL)
		{
		memset(hexadecimalString, 0, sizeof(hexadecimalString));
		//===== Convert ID number into hexadecimal string =====
		if (M2MString_convertFromUnsignedLongToHexadecimalString(id, hexadecimalString, sizeof(hexadecimalString))!=NULL)
			{
			//===== Prepare SQL statement =====
			memset(buffer, 0, sizeof(buffer));
			snprintf(buffer, sizeof(buffer)-1, (M2MString *)"DELETE FROM %s WHERE %s = ? ", M2MNode_TABLE_NAME, M2MNode_COLUMN_ID);
			//===== Get prepared statement =====
			if ((statement=M2MSQLiteConfig_getPreparedStatement(database, buffer))!=NULL)
				{
				//===== Execute SQL statement =====
				if (M2MSQLiteConfig_setValueIntoPreparedStatement(M2M_DATA_TYPE_TEXT, 1, hexadecimalString, M2MString_length(hexadecimalString), statement)==true
						&& ((result=M2MSQLiteConfig_next(statement))==SQLITE_ROW || result==SQLITE_DONE))
					{
#ifdef DEBUG
					M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, (M2MString *)"Succeed to delete a record in node table");
#endif // DEBUG
					}
				//===== Error handling =====
				else
					{
					memset(buffer, 0, sizeof(buffer));
					snprintf(buffer, sizeof(buffer)-1, (M2MString *)"Failed to delete a record in node table because %s", sqlite3_errmsg(database));
					M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, buffer, NULL);
					}
				//===== Release prepared statement object =====
				sqlite3_finalize(statement);
				}
			//===== Error handling =====
			else
				{
				M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to get prepared statement object", NULL);
				}
			}
		//===== Error handling =====
		else
			{
			memset(buffer, 0, sizeof(buffer));
			snprintf(buffer, sizeof(buffer)-1, (M2MString *)"Failed to convert node id(=\"%"PRIu32"\") into hexadecimal string", id);
			M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, buffer, NULL);
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"database\" object is NULL", NULL);
		}
	return;
	}


/**
 * @param[in] database	SQLite3 database object
 * @param[in] name		String indicating node name
 * @return				Number indicating node ID which is unique in "m2m_node" table
 */
uint32_t M2MNode_getID (sqlite3 *database, const M2MString *name)
	{
	//========== Variable ==========
	size_t nameLength = 0;
	M2MString buffer[512];
	sqlite3_stmt *statement = NULL;
	uint32_t id = 0UL;
	int result = 0;
	M2MString *text = NULL;
	const M2MString *METHOD_NAME = (M2MString *)"M2MNode_getID";

	//===== Check argument =====
	if (database!=NULL
			&& name!=NULL && (nameLength=M2MString_length(name))>0)
		{
		//===== Prepare SQL statement =====
		memset(buffer, 0, sizeof(buffer));
		snprintf(buffer, sizeof(buffer)-1, (M2MString *)"SELECT %s FROM %s WHERE %s = ? ", M2MNode_COLUMN_ID, M2MNode_TABLE_NAME, M2MNode_COLUMN_NAME);
		//===== Get prepared statement =====
		if ((statement=M2MSQLiteConfig_getPreparedStatement(database, buffer))!=NULL)
			{
			//===== Execute SQL =====
			if (M2MSQLiteConfig_setValueIntoPreparedStatement(M2M_DATA_TYPE_TEXT, 1, name, nameLength, statement)==true
					&& ((result=M2MSQLiteConfig_next(statement))==SQLITE_ROW || result==SQLITE_DONE)
					&& (text=(M2MString *)sqlite3_column_text(statement, 0))!=NULL
					&& (id=M2MString_convertFromHexadecimalStringToUnsignedLong(text, M2MString_length(text)))>0UL)
				{
				//===== Release prepared statement object =====
				sqlite3_finalize(statement);
				return id;
				}
			//===== Error handling =====
			else
				{
				//===== Release prepared statement object =====
				sqlite3_finalize(statement);
				memset(buffer, 0, sizeof(buffer));
				snprintf(buffer, sizeof(buffer)-1, (M2MString *)"Failed to get node ID related to name because %s", sqlite3_errmsg(database));
				M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, buffer, NULL);
				return 0UL;
				}
			}
		//===== Error handling =====
		else
			{
			M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to get prepared statement object", NULL);
			return 0UL;
			}
		}
	//===== Argument error =====
	else if (database==NULL)
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"database\" object is NULL", NULL);
		return 0UL;
		}
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"name\" string is NULL or vacant", NULL);
		return 0UL;
		}
	}


/**
 * @param[in] database	SQLite3 database object
 * @param[in] id		Number indicating node ID which is unique in "m2m_node" table
 * @param[out] name		Pointer to copying the node name (buffering is executed inside this function)
 * @return				String indicating node name or NULL (in case of error)
 */
M2MString *M2MNode_getName (sqlite3 *database, const uint32_t id, M2MString **name)
	{
	//========== Variable ==========
	M2MString hexadecimalString[16];
	M2MString buffer[512];
	sqlite3_stmt *statement = NULL;
	int result = 0;
	const M2MString *METHOD_NAME = (M2MString *)"M2MNode_getName";

	//===== Check argument =====
	if (database!=NULL)
		{
		memset(hexadecimalString, 0, sizeof(hexadecimalString));
		//===== Convert ID number into hexadecimal string =====
		if (M2MString_convertFromUnsignedLongToHexadecimalString(id, hexadecimalString, sizeof(hexadecimalString))!=NULL)
			{
			memset(buffer, 0, sizeof(buffer));
			snprintf(buffer, sizeof(buffer)-1, (M2MString *)"SELECT %s FROM %s WHERE %s = ? ", M2MNode_COLUMN_NAME, M2MNode_TABLE_NAME, M2MNode_COLUMN_ID);
			//===== Get SQLite3 prepared statement =====
			if ((statement=M2MSQLiteConfig_getPreparedStatement(database, buffer))!=NULL)
				{
				//=====  =====
				if (M2MSQLiteConfig_setValueIntoPreparedStatement(M2M_DATA_TYPE_TEXT, 1, hexadecimalString, M2MString_length(hexadecimalString), statement)==true
						&& ((result=M2MSQLiteConfig_next(statement))==SQLITE_ROW || result==SQLITE_DONE)
						&& M2MString_append(name, sqlite3_column_text(statement, 0))!=NULL)
					{
					//===== Release prepared statement object =====
					sqlite3_finalize(statement);
					return (*name);
					}
				//===== Error handling =====
				else
					{
					//===== Release prepared statement object =====
					sqlite3_finalize(statement);
					memset(buffer, 0, sizeof(buffer));
					snprintf(buffer, sizeof(buffer)-1, (M2MString *)"Failed to get node name related to id because %s", sqlite3_errmsg(database));
					M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, buffer, NULL);
					return NULL;
					}
				}
			//===== Error handling =====
			else
				{
				M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to get prepared statement object", NULL);
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			memset(buffer, 0, sizeof(buffer));
			snprintf(buffer, sizeof(buffer)-1, (M2MString *)"Failed to convert node id(=\"%"PRIu32"\") into hexadecimal string", id);
			M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, buffer, NULL);
			return NULL;
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"database\" object is NULL", NULL);
		return NULL;
		}
	}


/**
 * @param[in] database	SQLite3 database object
 * @param[in] id		Number indicating node ID which is unique inside "m2m_node" table
 * @param[out] property	Pointer to copying property belonging to the node
 * @return				String indicating node name or NULL (in case of error)
 */
M2MString *M2MNode_getProperty (sqlite3 *database, const uint32_t id, M2MString **property)
	{
	//========== Variable ==========
	M2MString hexadecimalString[16];
	M2MString buffer[512];
	sqlite3_stmt *statement = NULL;
	int result = 0;
	const M2MString *METHOD_NAME = (M2MString *)"M2MNode_getProperty";

	//===== Check argument =====
	if (database!=NULL)
		{
		memset(hexadecimalString, 0, sizeof(hexadecimalString));
		//===== Convert ID number into hexadecimal string =====
		if (M2MString_convertFromUnsignedLongToHexadecimalString(id, hexadecimalString, sizeof(hexadecimalString))!=NULL)
			{
			memset(buffer, 0, sizeof(buffer));
			snprintf(buffer, sizeof(buffer)-1, (M2MString *)"SELECT %s FROM %s WHERE %s = ? ", M2MNode_COLUMN_PROPERTY, M2MNode_TABLE_NAME, M2MNode_COLUMN_ID);
			//===== Get SQLite3 prepared statement =====
			if ((statement=M2MSQLiteConfig_getPreparedStatement(database, buffer))!=NULL)
				{
				//=====  =====
				if (M2MSQLiteConfig_setValueIntoPreparedStatement(M2M_DATA_TYPE_TEXT, 1, hexadecimalString, M2MString_length(hexadecimalString), statement)==true
						&& ((result=M2MSQLiteConfig_next(statement))==SQLITE_ROW || result==SQLITE_DONE)
						&& M2MString_append(property, sqlite3_column_text(statement, 0))!=NULL)
					{
					//===== Release prepared statement object =====
					sqlite3_finalize(statement);
					return (*property);
					}
				//===== Error handling =====
				else
					{
					//===== Release prepared statement object =====
					sqlite3_finalize(statement);
					memset(buffer, 0, sizeof(buffer));
					snprintf(buffer, sizeof(buffer)-1, (M2MString *)"Failed to get node name related to id because %s", sqlite3_errmsg(database));
					M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, buffer, NULL);
					return NULL;
					}
				}
			//===== Error handling =====
			else
				{
				M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to get prepared statement object", NULL);
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			memset(buffer, 0, sizeof(buffer));
			snprintf(buffer, sizeof(buffer)-1, (M2MString *)"Failed to convert node id(=\"%"PRIu32"\") into hexadecimal string", id);
			M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, buffer, NULL);
			return NULL;
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"database\" object is NULL", NULL);
		return NULL;
		}
	}


/**
 * Constructor.<br>
 * Set a new record into the "m2m_node" table.<br>
 *
 * @param[in] database	SQLite3 database object
 * @param[in] name		String indicating unique node name in "m2m_node" table
 * @param[in] property	String indicating property belonging to the node or NULL
 * @return				Number indicating node ID which is unique in "m2m_node" table
 */
uint32_t M2MNode_new (sqlite3 *database, const M2MString *name, const M2MString *property)
	{
	//========== Variable ==========
	size_t nameLength = 0;
	const M2MString *METHOD_NAME = (M2MString *)"M2MNode_new()";

	//===== Check argument =====
	if (name!=NULL && (nameLength=M2MString_length(name))>0)
		{
		//===== Check the existence of table =====
		if (this_createNewTable(database)==true)
			{
			//===== Set a new record into table =====
			return this_setRecord(database, name, property);
			}
		//===== Error handling =====
		else
			{
			M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to check the existence of table in the SQLite3 database", NULL);
			return 0UL;
			}
		}
	//===== Argument error =====
	else if (database==NULL)
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! indicated \"database\" object is NULL", NULL);
		return 0UL;
		}
	else if (name==NULL || nameLength<=0)
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! indicated \"name\" string is NULL or vacant", NULL);
		return 0UL;
		}
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! indicated \"property\" string is NULL or vacant", NULL);
		return 0UL;
		}
	}



/* End Of File */
