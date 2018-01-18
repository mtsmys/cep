/*******************************************************************************
 * M2MSQLiteConfig.c
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

#include "m2m/db/M2MSQLiteConfig.h"


/*******************************************************************************
 * Public function
 ******************************************************************************/
/**
 * Close the connection of indicated SQLite3 database.<br>
 *
 * @param[in] database	SQLite3 database object to be closed
 */
void M2MSQLiteConfig_closeDatabase (sqlite3 *database)
	{
	//========== Variable ==========
	sqlite3_stmt *statement = NULL;
	sqlite3_stmt *next = NULL;
	M2MString message[256];
	const M2MString *METHOD_NAME = (M2MString *)"M2MSQLiteConfig_closeDatabase()";

	//===== Check argument =====
	if (database!=NULL)
		{
		//===== Close SQLite3 database =====
		if (sqlite3_close(database)==SQLITE_OK)
			{
#ifdef DEBUG
			M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, (M2MString *)"Closed SQLite3 database");
#endif // DEBUG
			}
		//===== In the case of failed to close the database =====
		else
			{
#ifdef DEBUG
			M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, (M2MString *)"Start to releasing heap memory for statements");
#endif // DEBUG
			//===== Get statement =====
			statement = sqlite3_next_stmt(database, NULL);
			//===== Repeat until existing statements =====
			while (statement!=NULL)
				{
				//===== Get next statement =====
				next = sqlite3_next_stmt(database, statement);
				//===== Release heap memory for statement =====
				sqlite3_finalize(statement);
				//===== Move to next statement =====
				statement = next;
				}
#ifdef DEBUG
			M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, (M2MString *)"Finished releasing heap memory for statements");
#endif // DEBUG
			//===== Close the SQLite3 database again =====
			if (sqlite3_close(database)==SQLITE_OK)
				{
#ifdef DEBUG
				M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, (M2MString *)"Closed SQLite3 database");
#endif // DEBUG
				}
			//===== Error handling =====
			else
				{
				memset(message, 0, sizeof(message));
				snprintf(message, sizeof(message)-1, (M2MString *)"Failed to close SQLite3 database because %s", (M2MString *)sqlite3_errmsg(database));
				M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, message, NULL);
				}
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"sqlite3\" object is NULL", NULL);
		}
	return;
	}


/**
 * Get a prepared statement object related with indicated SQL statement string.<br>
 *
 * @param database	SQLite3 database object
 * @param sql		String indicating SQL statement
 * @return			Prepared statement object or NULL (in case of error)
 */
sqlite3_stmt *M2MSQLiteConfig_getPreparedStatement (sqlite3 *database, const M2MString *sql)
	{
	//========== Variable ==========
	sqlite3_stmt *statement = NULL;
	int result = 0;
#ifdef DEBUG
	M2MString buffer[512];
#endif // DEBUG
	const M2MString *METHOD_NAME = (M2MString *)"M2MSQLiteConfig_getSQLitePreparedStatement()";

	//===== Check argument =====
	if (database!=NULL
			&& sql!=NULL && M2MString_length(sql)>0)
		{
		//===== Get prepared statement =====
		if ((result=sqlite3_prepare_v2(database, sql, -1, &statement, NULL))==SQLITE_OK
				&& (result=sqlite3_reset(statement))==SQLITE_OK
				&& (result=sqlite3_clear_bindings(statement))==SQLITE_OK)
			{
#ifdef DEBUG
			memset(buffer, 0, sizeof(buffer));
			snprintf(buffer, sizeof(buffer)-1, (M2MString *)"Converted a SQL statement (=\"%s\") into VBDE and created a new prepared statement", sql);
			M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, buffer);
#endif // DEBUG
			return statement;
			}
		//===== Error handling =====
		else
			{
			M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)sqlite3_errmsg(database), NULL);
			sqlite3_finalize(statement);
			return NULL;
			}
		}
	//===== Argument error =====
	else if (database==NULL)
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated SQLite3 database object is NULL", NULL);
		return NULL;
		}
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"sql\" string is NULL or vacant", NULL);
		return NULL;
		}
	}


/**
 * Create SQL statement for displaying table information, and copy to the pointer.<br>
 *
 * @param[in] tableName	String indicating table name
 * @param[out] sql		Pointer for copying the SQL statement (buffering is executed inside the function)
 * @return				String copied to pointer or NULL (in case of error)
 */
M2MString *M2MSQLiteConfig_getTableInfoSQL (const M2MString *tableName, M2MString **sql)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MSQLiteConfig_getTableInfoSQL()";

	//===== Check argument =====
	if (tableName!=NULL && M2MString_length(tableName)>0 && sql!=NULL)
		{
		//===== Copy SQL string =====
		if (M2MString_append(sql, (M2MString *)"PRAGMA table_info(")!=NULL
				&& M2MString_append(sql, tableName)!=NULL
				&& M2MString_append(sql, (M2MString *)")")!=NULL)
			{
			return (*sql);
			}
		//===== Error handling =====
		else
			{
			M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to get heap memory for copying SQL string", NULL);
			return NULL;
			}
		}
	//===== Argument error =====
	else if (tableName==NULL || M2MString_length(tableName)<=0)
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"tableName\" string is NULL or vacant", NULL);
		return NULL;
		}
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"sql\" is NULL", NULL);
		return NULL;
		}
	}


/**
 * Check the existence of indicated table and return the result.<br>
 *
 * @param[in] database	SQLite3 database object
 * @param[in] tableName	String indicating the table name
 * @return				true: In case of existing the table, false: In case of not existing the table
 */
bool M2MSQLiteConfig_isExistingTable (sqlite3 *database, const M2MString *tableName)
	{
	//========== Variable ==========
	size_t tableNameLength = 0;
	sqlite3_stmt *statement = NULL;
	int result = 0;
	int numberOfTable = -1;
	M2MString message[256];
	const M2MString *SQL = (M2MString *)"SELECT COUNT(DISTINCT name) AS result FROM sqlite_master WHERE type = 'table' AND name = ?";
	const M2MString *METHOD_NAME = (M2MString *)"M2MSQLiteConfig_isExistingTable()";

	//===== Check argument =====
	if (database!=NULL
			&& tableName!=NULL && (tableNameLength=M2MString_length(tableName))>0)
		{
		//===== Set SQL string =====
		if ((statement=M2MSQLiteConfig_getPreparedStatement(database, SQL))!=NULL)
			{
			//===== In case of existing the table =====
			if (M2MSQLiteConfig_setValueIntoPreparedStatement(M2MDataType_TEXT, 1, tableName, tableNameLength, statement)==true
					&& ((result=M2MSQLiteConfig_next(statement)==SQLITE_ROW) || result==SQLITE_DONE)
					&& (numberOfTable=sqlite3_column_int(statement, 0))==1)
				{
				//===== Release SQLite3 prepared statement =====
				sqlite3_finalize(statement);
#ifdef DEBUG
				memset(message, 0, sizeof(message));
				snprintf(message, sizeof(message)-1, (M2MString *)"The \"%s\" table already exists on the SQLite database", tableName);
				M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, message);
#endif /* DEBUG */
				return true;
				}
			//===== In case of not existing the table =====
			else if (numberOfTable==0)
				{
				//===== Release SQLite3 prepared statement =====
				sqlite3_finalize(statement);
#ifdef DEBUG
				memset(message, 0, sizeof(message));
				snprintf(message, sizeof(message)-1, (M2MString *)"The \"%s\" table hasn't exists on the SQLite database yet", tableName);
				M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, message);
#endif /* DEBUG */
				return false;
				}
			//===== Error handling =====
			else
				{
				//===== Release SQLite3 prepared statement =====
				sqlite3_finalize(statement);
				memset(message, 0, sizeof(message));
				snprintf(message, sizeof(message)-1, (M2MString *)"Failed to execute SQL statement because %s", (M2MString *)sqlite3_errmsg(database));
				M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, message, NULL);
				return false;
				}
			}
		//===== Error handling =====
		else
			{
			M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to get SQLite3 prepared statement object", NULL);
			return false;
			}
		}
	//===== Argument error =====
	else if (database==NULL)
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"sqlite3\" object is NULL", NULL);
		return false;
		}
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"tableName\" string is NULL or vacant", NULL);
		return false;
		}
	}


/**
 * Open the indicated SQLite3 database.<br>
 * If caller uses a database in memory, should set ":memory:" as <br>
 * "filename" argument.<br>
 * Otherwise, caller should SQLite3 file pathname string as "filename" <br>
 * argument.<br>
 *
 * @param[in] filename	String indicating database name
 * @return				Connection handler of opened SQLite3 database or NULL (in case of error)
 */
sqlite3 *M2MSQLiteConfig_openDatabase (const M2MString *filename)
	{
	//========== Variable ==========
	sqlite3 *database = NULL;
	M2MString message[256];
	const M2MString *METHOD_NAME = (M2MString *)"M2MSQLiteConfig_openDatabase()";

	//===== Check argument =====
	if (filename!=NULL && M2MString_length(filename)>0)
		{
		//===== Open the SQLite3 database =====
		if (sqlite3_open(filename, &database)==SQLITE_OK)
			{
#ifdef DEBUG
			memset(message, 0, sizeof(message));
			snprintf(message, sizeof(message)-1, (M2MString *)"Succeed to open a SQLite3 database(=\"%s\")", filename);
			M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, message);
#endif // DEBUG
			//===== Return the connection handler =====
			return database;
			}
		//===== Error handling =====
		else
			{
			//===== Close the SQLite3 database =====
			M2MSQLiteConfig_closeDatabase(database);
			memset(message, 0, sizeof(message));
			snprintf(message, sizeof(message)-1, (M2MString *)"Failed to open a SQLite3 database(=\"%s\")", filename);
			M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, message, NULL);
			return NULL;
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"filename\" string is NULL or vacant", NULL);
		return NULL;
		}
	}


/**
 * Set the auto-vacuum status in the SQLite3 database.<br>
 *
 * @param[in] database	SQLite3 database object to set automatic vacuum
 * @param[in] flag		true: Enable automatic vacuum, false: Disable automatic vacuum
 * @return				true: success, false: failure
 */
bool M2MSQLiteConfig_setAutoVacuum (sqlite3 *database, const bool flag)
	{
	//========== Variable ==========
	bool result = false;
	const M2MString *PRAGMA_AUTO_VACUUM_SQL = (M2MString *)"PRAGMA auto_vacuum = 1 ";
	const M2MString *PRAGMA_NOT_AUTO_VACUUM_SQL = (M2MString *)"PRAGMA auto_vacuum = 0 ";
	const M2MString *METHOD_NAME = (M2MString *)"M2MSQLiteConfig_setAutoVacuum()";

	//===== When automatic vacuum is enabled =====
	if (flag==true)
		{
		//===== Execute SQL statement to disable automatic vacuum =====
		if ((result=M2MSQLRunner_executeUpdate(database, PRAGMA_AUTO_VACUUM_SQL))==true)
			{
#ifdef DEBUG
			M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, (M2MString *)"Succeed to enable auto vacuum mode");
#endif // DEBUG
			}
		//===== Error handling =====
		else
			{
			M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to enable auto vacuum mode", NULL);
			}
		}
	//===== When automatic vacuum is disabled =====
	else
		{
		//===== Execute SQL statement to disable automatic vacuum =====
		if ((result=M2MSQLRunner_executeUpdate(database, PRAGMA_NOT_AUTO_VACUUM_SQL))==true)
			{
#ifdef DEBUG
			M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, (M2MString *)"Succeed to disable auto vacuum mode");
#endif // DEBUG
			}
		//===== Error handling =====
		else
			{
			M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to disable auto vacuum mode", NULL);
			}
		}
	return result;
	}


/**
 * Set the synchronous mode of the SQLite 3 database.<br>
 *
 * @param[in] database		SQLite3 database object to be set synchronous mode
 * @param[in] synchronous	true: Synchronous mode = NORMAL, false: Synchronous mode = OFF
 * @return					true: success, false: failure
 */
bool M2MSQLiteConfig_setSynchronous (sqlite3 *database, const bool synchronous)
	{
	//========== Variable ==========
	bool result = false;
	const M2MString *PRAGMA_SYNCHRONOUS_NORMAL_SQL = (M2MString *)"PRAGMA synchronous = NORMAL ";
	const M2MString *PRAGMA_SYNCHRONOUS_OFF_SQL = (M2MString *)"PRAGMA synchronous = OFF ";
	const M2MString *METHOD_NAME = (M2MString *)"M2MSQLiteConfig_setSynchronous()";

	//===== In the case of NORMAL mode =====
	if (synchronous==true)
		{
		//===== Set NORMAL synchronous mode to SQLite3 database =====
		if ((result=M2MSQLRunner_executeUpdate(database, PRAGMA_SYNCHRONOUS_NORMAL_SQL))==true)
			{
#ifdef DEBUG
			M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, (M2MString *)"Succeed to set the synchronous mode into \"NORMAL\"");
#endif // DEBUG
			}
		//===== Error handling =====
		else
			{
			M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to set the synchronous mode into \"NORMAL\"", NULL);
			}
		}
	//===== In the case of OFF mode =====
	else
		{
		//===== Set OFF synchronous mode to SQLite3 database =====
		if ((result=M2MSQLRunner_executeUpdate(database, PRAGMA_SYNCHRONOUS_OFF_SQL))==true)
			{
#ifdef DEBUG
			M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, (M2MString *)"Succeed to set the synchronous mode into \"OFF\"");
#endif // DEBUG
			}
		//===== Error handling =====
		else
			{
			M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to set the synchronous mode into \"OFF\"", NULL);
			}
		}
	return result;
	}


/**
 * Set the character code of the database to UTF-8.<br>
 *
 * @param[in] database	SQLite3 database object to set character code to UTF-8
 * @return				true: success, false: failure
 */
bool M2MSQLiteConfig_setUTF8 (sqlite3 *database)
	{
	//========== Variable ==========
	bool result = false;
	const M2MString *PRAGMA_ENCODING_SQL = (M2MString *)"PRAGMA encoding = 'UTF-8'";
	const M2MString *METHOD_NAME = (M2MString *)"M2MSQLiteConfig_setUTF8()";

	//===== Set UTF-8 to SQLite3 database =====
	if ((result=M2MSQLRunner_executeUpdate(database, PRAGMA_ENCODING_SQL))==true)
		{
#ifdef DEBUG
		M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, (M2MString *)"Succeed to set the encoding into \"UTF-8\"");
#endif // DEBUG
		}
	//===== Error handling =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to set the encoding into \"UTF-8\"", NULL);
		}
	return result;
	}


/**
 * Set the data in specified type into SQLite prepared statement.<br>
 *
 * @param[in] dataType		Data type of the SQLite3 database
 * @param[in] index			Index number of the field (>=1)
 * @param[in] value			String indicating the input data of the field
 * @param[in] valueLength	Length of string indicating field input data[Byte]
 * @param[in,out] statement	SQLite3 prepared statement object
 * @return					true: Succeed to set, false: Failed to set
 */
bool M2MSQLiteConfig_setValueIntoPreparedStatement (const M2MDataType dataType, unsigned int index, const M2MString *value, const size_t valueLength, sqlite3_stmt *statement)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MSQLiteConfig_setValueIntoPreparedStatement()";

	//===== Check argument =====
	if (index>=1 && statement!=NULL)
		{
		//===== In case of BLOB type =====
		if (dataType==M2MDataType_BLOB)
			{
			//===== Set value =====
			if (sqlite3_bind_blob(statement, index, value, valueLength, SQLITE_TRANSIENT)==SQLITE_OK)
				{
				return true;
				}
			//===== Error handling =====
			else
				{
				M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to set \"BLOB\" type value", NULL);
				return false;
				}
			}
		//===== In case of BOOL type =====
		else if (dataType==M2MDataType_BOOL)
			{
			if (M2MString_compareTo(value, (M2MString *)"true")==0 || M2MString_compareTo(value, (M2MString *)"TRUE")==0)
				{
				//===== Set value =====
				if (sqlite3_bind_int(statement, index, true)==SQLITE_OK)
					{
					return true;
					}
				//===== Error handling =====
				else
					{
					M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to set \"BOOL\" type value", NULL);
					return false;
					}
				}
			else
				{
				//===== Set value =====
				if (sqlite3_bind_int(statement, index, false)==SQLITE_OK)
					{
					return true;
					}
				//===== Error handling =====
				else
					{
					M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to set \"BOOL\" type value", NULL);
					return false;
					}
				}
			return false;
			}
		//===== In case of CHAR type =====
		else if (dataType==M2MDataType_CHAR)
			{
			//===== Set value =====
			if (sqlite3_bind_text(statement, index, value, -1, SQLITE_TRANSIENT)==SQLITE_OK)
				{
				return true;
				}
			//===== Error handling =====
			else
				{
				M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to set \"CHAR\" type value", NULL);
				return false;
				}
			}
		//===== In case of DATETIME type =====
		else if (dataType==M2MDataType_DATETIME)
			{
			//===== Set value =====
			if (sqlite3_bind_int64(statement, index, M2MString_convertFromStringToSignedLongLong(value, valueLength))==SQLITE_OK)
				{
				return true;
				}
			//===== Error handling =====
			else
				{
				M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to set \"DATETIME\" type value", NULL);
				return false;
				}
			}
		//===== In case of DOUBLE type =====
		else if (dataType==M2MDataType_DOUBLE)
			{
			//===== Set value =====
			if (sqlite3_bind_double(statement, index, M2MString_convertFromStringToDouble(value, valueLength))==SQLITE_OK)
				{
				return true;
				}
			//===== Error handling =====
			else
				{
				M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to set \"DOUBLE\" type value", NULL);
				return false;
				}
			}
		//===== In case of ERROR type =====
		else if (dataType==M2MDataType_ERROR)
			{
			return false;
			}
		//===== In case of FLOAT type =====
		else if (dataType==M2MDataType_FLOAT)
			{
			//===== Set value =====
			if (sqlite3_bind_double(statement, index, M2MString_convertFromStringToDouble(value, valueLength))==SQLITE_OK)
				{
				return true;
				}
			//===== Error handling =====
			else
				{
				M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to set \"FLOAT\" type value", NULL);
				return false;
				}
			}
		//===== In case of INTEGER type =====
		else if (dataType==M2MDataType_INTEGER)
			{
			//===== Set value =====
			if (sqlite3_bind_int(statement, index, M2MString_convertFromStringToSignedInteger(value, valueLength))==SQLITE_OK)
				{
				return true;
				}
			//===== Error handling =====
			else
				{
				M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to set \"INTEGER\" type value", NULL);
				return false;
				}
			}
		//===== In case of NULL type =====
		else if (dataType==M2MDataType_NULL)
			{
			//===== Set value =====
			if (sqlite3_bind_null(statement, index)==SQLITE_OK)
				{
				return true;
				}
			//===== Error handling =====
			else
				{
				M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to set \"NULL\" type value", NULL);
				return false;
				}
			}
		//===== In case of NUMERIC type =====
		else if (dataType==M2MDataType_NUMERIC)
			{
			//===== Set value =====
			if (sqlite3_bind_int64(statement, index, M2MString_convertFromStringToSignedLongLong(value, valueLength))==SQLITE_OK)
				{
				return true;
				}
			//===== Error handling =====
			else
				{
				M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to set \"NUMERIC\" type value", NULL);
				return false;
				}
			}
		//===== In case of REAL type =====
		else if (dataType==M2MDataType_REAL)
			{
			//===== Set value =====
			if (sqlite3_bind_double(statement, index, M2MString_convertFromStringToDouble(value, valueLength))==SQLITE_OK)
				{
				return true;
				}
			//===== Error handling =====
			else
				{
				M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to set \"REAL\" type value", NULL);
				return false;
				}
			}
		//===== In case of TEXT type =====
		else if (dataType==M2MDataType_TEXT)
			{
			//===== Set value =====
			if (sqlite3_bind_text(statement, index, value, -1, SQLITE_TRANSIENT)==SQLITE_OK)
				{
				return true;
				}
			//===== Error handling =====
			else
				{
				M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to set \"TEXT\" type value", NULL);
				return false;
				}
			}
		//===== In case of VARCHAR type =====
		else if (dataType==M2MDataType_VARCHAR)
			{
			//===== Set value =====
			if (sqlite3_bind_text(statement, index, value, valueLength, SQLITE_TRANSIENT)==SQLITE_OK)
				{
				return true;
				}
			//===== Error handling =====
			else
				{
				M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to set \"VARCHAR\" type value", NULL);
				return false;
				}
			}
		//===== Error handling =====
		else
			{
			M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"A data type matching the argument can't be found", NULL);
			return false;
			}
		}
	//===== Argument error =====
	else if (index<=0)
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"index\" number of position of the data value isn't positive", NULL);
		return false;
		}
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"sqlite3_stmt\" object is NULL", NULL);
		return false;
		}
	}


/**
 * Set the journal mode of the SQLite 3 database to WAL (Write Ahead Logging).<br>
 *
 * @param[in] database		SQLite3 database object to be set as WAL
 * @param[in] synchronous	true: Sync mode, false: Asynchronous mode
 * @return					true: success, false: failure
 */
bool M2MSQLiteConfig_setWAL (sqlite3 *database, const bool synchronous)
	{
	//========== Variable ==========
	bool result = false;
	const M2MString *PRAGMA_JOURNAL_MODE_SQL = (M2MString *)"PRAGMA journal_mode = WAL ";
	const M2MString *METHOD_NAME = (M2MString *)"M2MSQLiteConfig_setWAL()";

	//===== Set journal mode of SQLite3 database =====
	if ((result=M2MSQLRunner_executeUpdate(database, PRAGMA_JOURNAL_MODE_SQL))==true)
		{
#ifdef DEBUG
		M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, (M2MString *)"Succeed to change the journal mode into \"WAL\"");
#endif // DEBUG
		}
	//===== Error handling =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to change the journal mode into \"WAL\"", NULL);
		}
	return result;
	}


/**
 * Execute SQL on the prepared statement object and return result code.<br>
 *
 * @param[in] statement	SQLite3 prepared statement object
 * @return				Result code defined by SQLite3 API
 */
int M2MSQLiteConfig_next (sqlite3_stmt *statement)
	{
	//========== Variable ==========
	int result = 0;
	const M2MString *METHOD_NAME = (M2MString *)"M2MSQLiteConfig_next()";

	//===== Check argument =====
	if (statement!=NULL)
		{
		//===== Execute SQL statement =====
		while ((result=sqlite3_step(statement))==SQLITE_BUSY)
			{
#ifdef DEBUG
			M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, (M2MString *)"\"SQLITE_BUSY\" is returned as the SQL execution result ... continue SQL execution processing");
#endif // DEBUG
			}
		//===== Return result code =====
		return result;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"statement\" object is NULL", NULL);
		return SQLITE_ERROR;
		}
	}


/**
 * Execute the VACUUM process to the indicated SQLite3 database.<br>
 *
 * @param[in] database	SQLite3 database object to be vacuum
 * @return				true: success, false: failure
 */
bool M2MSQLiteConfig_vacuum (sqlite3 *database)
	{
	//========== Variable ==========
	bool result = false;
	const M2MString *SQL = "VACUUM ";
	const M2MString *METHOD_NAME = (M2MString *)"M2MSQLiteConfig_vacuum()";

	//===== Vacuum SQLite3 database =====
	if ((result=M2MSQLRunner_executeUpdate(database, SQL))==true)
		{
#ifdef DEBUG
		M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, (M2MString *)"Succeed to execute vacuum process");
#endif // DEBUG
		}
	//===== Error handling =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to execute vacuum process", NULL);
		}
	return result;
	}



// End Of File
