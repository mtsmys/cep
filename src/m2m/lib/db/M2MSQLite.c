/*******************************************************************************
 * M2MSQLite.c
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

#include "m2m/lib/db/M2MSQLite.h"


/*******************************************************************************
 * Private function
 ******************************************************************************/
/**
 * This function returns SQL statement object got from indicated database & sql.<br>
 *
 * @param[in] database	SQLite3 database manager object
 * @param[in] sql		SQL string
 * @return				SQL statement object
 */
static sqlite3_stmt *this_getStatement (sqlite3 *database, const M2MString *sql)
	{
	//========== Variable ==========
	sqlite3_stmt *statement = NULL;
	M2MString message[2048];
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MSQLite_beginTransaction()";

	//===== Check argument =====
	if (database!=NULL && sql!=NULL && M2MString_length(sql)>0)
		{
		//===== Get SQLite3 statement object =====
		if (sqlite3_prepare_v2(database, sql, -1, &statement, NULL)==SQLITE_OK)
			{
			return statement;
			}
		//===== Error handling =====
		else
			{
			memset(message, 0, sizeof(message));
			snprintf(message, sizeof(message)-1, (M2MString *)"Failed to convert from SQL into VDBE because of \"%s\"", M2MSQLite_getErrorMessage(database));
			M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, message);
			return NULL;
			}
		}
	//===== Argument error =====
	else if (database==NULL)
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"sqlite3\" object is NULL");
		return NULL;
		}
	else
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"sql\" string is NULL or vacant");
		return NULL;
		}
	}



/*******************************************************************************
 * Public function
 ******************************************************************************/
/**
 * This function starts transaction with indicated SQLite3 database as argument.<br>
 *
 * @param[in] database	SQLite3 database manager object
 * @return				true: success, false: failure
 */
bool M2MSQLite_beginTransaction (sqlite3 *database)
	{
	//========== Variable ==========
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MSQLite_beginTransaction()";
	const M2MString *BEGIN_SQL = (M2MString *)"BEGIN ";

	//===== Check argument =====
	if (database!=NULL)
		{
		//===== Execute SQL =====
		if (M2MSQLite_executeUpdate(database, BEGIN_SQL)==true)
			{
			return true;
			}
		//===== Error handling =====
		else
			{
			M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to start transaction in indicated SQLite3 database");
			return false;
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"sqlite3\" object is NULL");
		return false;
		}
	}


/**
 * Close the connection of indicated SQLite3 database.<br>
 *
 * @param[in] database	SQLite3 database object to be closed
 */
void M2MSQLite_closeDatabase (sqlite3 *database)
	{
	//========== Variable ==========
	sqlite3_stmt *statement = NULL;
	sqlite3_stmt *next = NULL;
	M2MString message[256];
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MSQLite_closeDatabase()";

	//===== Check argument =====
	if (database!=NULL)
		{
		//===== Close SQLite3 database =====
		if (sqlite3_close(database)==SQLITE_OK)
			{
			}
		//===== In the case of failed to close the database =====
		else
			{
			//===== Get statement =====
			statement = sqlite3_next_stmt(database, NULL);
			//===== Repeat until existing statements =====
			while (statement!=NULL)
				{
				//===== Get next statement =====
				next = sqlite3_next_stmt(database, statement);
				//===== Release heap memory for statement =====
				M2MSQLite_closeStatement(statement);
				//===== Move to next statement =====
				statement = next;
				}
			//===== Close the SQLite3 database again =====
			if (sqlite3_close(database)==SQLITE_OK)
				{
				}
			//===== Error handling =====
			else
				{
				memset(message, 0, sizeof(message));
				snprintf(message, sizeof(message)-1, (M2MString *)"Failed to close SQLite3 database because %s", M2MSQLite_getErrorMessage(database));
				M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, message);
				}
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"sqlite3\" object is NULL");
		}
	return;
	}


/**
 * Close & release allocated memory of indicated SQL statement object.<br>
 *
 * @param[in,out] statement	SQL statement object to be closed
 */
void M2MSQLite_closeStatement (sqlite3_stmt *statement)
	{
	//========== Variable ==========
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MSQLite_closeStatement()";

	//===== Check argument =====
	if (statement!=NULL)
		{
		//===== Close statement =====
		if (sqlite3_finalize(statement)==SQLITE_OK)
			{
			// do nothing
			}
		//===== Error handling =====
		else
			{
//			M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to close indicated \"sqlite3_stmt\" object");
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"statement\" object is NULLt");
		}
	return;
	}


/**
 * Commit the transaction of the specified SQLite3 database object.<br>
 *
 * @param[in] database	SQLite3 database manager object
 * @return				true: success, false: failure
 */
bool M2MSQLite_commitTransaction (sqlite3 *database)
	{
	//========== Variable ==========
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MSQLite_commitTransaction()";
	const M2MString *COMMIT_SQL = (M2MString *)"COMMIT ";

	//===== Check argument =====
	if (database!=NULL)
		{
		//===== Commit =====
		if (M2MSQLite_executeUpdate(database, COMMIT_SQL)==true)
			{
			return true;
			}
		//===== Error handling =====
		else
			{
			M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to commit transaction in indicated SQLite3 database");
			return false;
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"sqlite3\" object is NULL");
		return false;
		}
	}


/**
 * Execute the SQL statement in the SQLite3 database object.<br>
 * <br>
 * [Caution!]<br>
 * This function needs many times, so don't use this for "INSERT" SQL or <br>
 * "UPDATE" SQL.<br>
 *
 * @param database	SQLite3 database manager object
 * @param sql		SQL string
 * @return			true: success, false: failuer
 */
bool M2MSQLite_executeUpdate (sqlite3 *database, const M2MString *sql)
	{
	//========== Variable ==========
	sqlite3_stmt *statement = NULL;
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MSQLite_executeUpdate()";

	//===== Check argument =====
	if (database!=NULL
			&& sql!=NULL && M2MString_length(sql)>0)
		{
		//===== Convert SQL statement to VDBE (internal execution format) =====
		if ((statement=this_getStatement(database, sql))!=NULL)
			{
			//===== Execute INSERT/UPDATE =====
			if (M2MSQLite_next(statement)==SQLITE_DONE)
				{
				//===== Check result status =====
				M2MSQLite_closeStatement(statement);
				return true;
				}
			//===== Error handling =====
			else
				{
				//===== Check result status =====
				M2MSQLite_closeStatement(statement);
				return false;
				}
			}
		//===== Error handling =====
		else
			{
			M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to convert from SQL into VDBE for executing update");
			return false;
			}
		}
	//===== Argument error =====
	else if (database==NULL)
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"sqlite3\" object is NULL");
		return false;
		}
	else
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"sql\" string is NULL or vacant");
		return false;
		}
	}


/**
 * Get string indicating SQLite3 error message.<br>
 *
 * @param database	SQLite3 database management object
 * @return			String indicating SQLite3 error message which is handled by argument
 */
M2MString *M2MSQLite_getErrorMessage (sqlite3 *database)
	{
	return (M2MString *)sqlite3_errmsg(database);
	}


/**
 * SQLite 3 Returns the maximum number of columns allowed in the table.<br>
 *
 * @param database	SQLite3 database management object
 * @return			Maximum number of columns in one table [pieces]
 */
int32_t M2MSQLite_getMaxColumnLength (sqlite3 *database)
	{
	//========== Variable ==========
	const int32_t DEFAULT_MAX_COLUMN_LENGTH = 2000;

	//===== Check argument =====
	if (database!=NULL)
		{
		return (int32_t)sqlite3_limit(database, SQLITE_LIMIT_COLUMN, -1);
		}
	//===== Argument error =====
	else
		{
		return DEFAULT_MAX_COLUMN_LENGTH;
		}
	}


/**
 * Get a prepared statement object related with indicated SQL statement string.<br>
 *
 * @param database	SQLite3 database object
 * @param sql		String indicating SQL statement
 * @return			Prepared statement object or NULL (in case of error)
 */
sqlite3_stmt *M2MSQLite_getPreparedStatement (sqlite3 *database, const M2MString *sql)
	{
	//========== Variable ==========
	sqlite3_stmt *statement = NULL;
	int result = 0;
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MSQLite_getPreparedStatement()";

	//===== Check argument =====
	if (database!=NULL
			&& sql!=NULL && M2MString_length(sql)>0)
		{
		//===== Get prepared statement =====
		if ((statement=this_getStatement(database, sql))!=NULL
				&& (result=sqlite3_reset(statement))==SQLITE_OK
				&& (result=sqlite3_clear_bindings(statement))==SQLITE_OK)
			{
			return statement;
			}
		//===== Error handling =====
		else
			{
			M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to get prepared statement object");
			M2MSQLite_closeStatement(statement);
			return NULL;
			}
		}
	//===== Argument error =====
	else if (database==NULL)
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated SQLite3 database object is NULL");
		return NULL;
		}
	else
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"sql\" string is NULL or vacant");
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
M2MString *M2MSQLite_getTableInfoSQL (const M2MString *tableName, M2MString **sql)
	{
	//========== Variable ==========
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MSQLite_getTableInfoSQL()";

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
			M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to get heap memory for copying SQL string");
			return NULL;
			}
		}
	//===== Argument error =====
	else if (tableName==NULL || M2MString_length(tableName)<=0)
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"tableName\" string is NULL or vacant");
		return NULL;
		}
	else
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"sql\" is NULL");
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
bool M2MSQLite_isExistingTable (sqlite3 *database, const M2MString *tableName)
	{
	//========== Variable ==========
	size_t tableNameLength = 0;
	sqlite3_stmt *statement = NULL;
	int result = 0;
	int numberOfTable = -1;
	M2MString message[1024];
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MSQLite_isExistingTable()";
	const M2MString *SQL = (M2MString *)"SELECT COUNT(DISTINCT name) AS result FROM sqlite_master WHERE type = 'table' AND name = ?";

	//===== Check argument =====
	if (database!=NULL
			&& tableName!=NULL && (tableNameLength=M2MString_length(tableName))>0)
		{
		//===== Set SQL string =====
		if ((statement=M2MSQLite_getPreparedStatement(database, SQL))!=NULL)
			{
			//===== In case of existing the table =====
			if (M2MSQLite_setValueIntoPreparedStatement(M2MSQLiteDataType_TEXT, 1, tableName, tableNameLength, statement)==true
					&& ((result=M2MSQLite_next(statement)==SQLITE_ROW) || result==SQLITE_DONE)
					&& (numberOfTable=sqlite3_column_int(statement, 0))==1)
				{
				//===== Release SQLite3 prepared statement =====
				M2MSQLite_closeStatement(statement);
				return true;
				}
			//===== In case of not existing the table =====
			else if (numberOfTable==0)
				{
				//===== Release SQLite3 prepared statement =====
				M2MSQLite_closeStatement(statement);
				return false;
				}
			//===== Error handling =====
			else
				{
				//===== Release SQLite3 prepared statement =====
				M2MSQLite_closeStatement(statement);
				memset(message, 0, sizeof(message));
				snprintf(message, sizeof(message)-1, (M2MString *)"Failed to execute SQL statement because %s", M2MSQLite_getErrorMessage(database));
				M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, message);
				return false;
				}
			}
		//===== Error handling =====
		else
			{
			M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to get SQLite3 prepared statement object");
			return false;
			}
		}
	//===== Argument error =====
	else if (database==NULL)
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"sqlite3\" object is NULL");
		return false;
		}
	else
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"tableName\" string is NULL or vacant");
		return false;
		}
	}


/**
 * Execute SQL on the prepared statement object and return result code.<br>
 *
 * @param[in] statement	SQLite3 prepared statement object
 * @return				Result code defined by SQLite3 API
 */
int M2MSQLite_next (sqlite3_stmt *statement)
	{
	//========== Variable ==========
	int result = 0;
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MSQLite_next()";

	//===== Check argument =====
	if (statement!=NULL)
		{
		//===== Execute SQL statement =====
		while ((result=sqlite3_step(statement))==SQLITE_BUSY)
			{
			}
		//===== Return result code =====
		return result;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"statement\" object is NULL");
		return SQLITE_ERROR;
		}
	}


/**
 * Open the indicated SQLite3 database.<br>
 * If caller uses a database in memory, should set ":memory:" as <br>
 * "filename" argument.<br>
 * Otherwise, caller should SQLite3 file pathname string as "filename" <br>
 * argument.<br>
 *
 * @param[in] sqliteFilePath	String indicating database file pathname (if you want to use in-memory database, please set ":memory:" string)
 * @return						Connection handler of opened SQLite3 database or NULL (in case of error)
 */
sqlite3 *M2MSQLite_openDatabase (const M2MString *sqliteFilePath)
	{
	//========== Variable ==========
	sqlite3 *database = NULL;
	M2MString message[1024];
	const int32_t ENABLE_EXTENDED_RESULT_CODE = 1;
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MSQLite_openDatabase()";

	//===== Check argument =====
	if (sqliteFilePath!=NULL && M2MString_length(sqliteFilePath)>0)
		{
		//===== Open the SQLite3 database =====
		if (sqlite3_open(sqliteFilePath, &database)==SQLITE_OK
				&& sqlite3_extended_result_codes(database, ENABLE_EXTENDED_RESULT_CODE)==SQLITE_OK)
			{
			//===== Return the connection handler =====
			return database;
			}
		//===== Error handling =====
		else
			{
			//===== Close the SQLite3 database =====
			M2MSQLite_closeDatabase(database);
			memset(message, 0, sizeof(message));
			snprintf(message, sizeof(message)-1, (M2MString *)"Failed to open a SQLite3 database(=\"%s\")", sqliteFilePath);
			M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, message);
			return NULL;
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"sqliteFilePath\" string is NULL or vacant");
		return NULL;
		}
	}


/**
 * Rollback the transaction of the specified SQLite3 database object.<br>
 *
 * @param[in] database	SQLite3 database manager object
 * @return				true: success, false: failure
 */
bool M2MSQLite_rollbackTransaction (sqlite3 *database)
	{
	//========== Variable ==========
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MSQLite_commitTransaction()";
	const M2MString *ROLLBACK_SQL = (M2MString *)"ROLLBACK ";

	//===== Check argument =====
	if (database!=NULL)
		{
		//===== Commit =====
		if (M2MSQLite_executeUpdate(database, ROLLBACK_SQL)==true)
			{
			return true;
			}
		//===== Error handling =====
		else
			{
			M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to rollback transaction in indicated SQLite3 database");
			return false;
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"sqlite3\" object is NULL");
		return false;
		}
	}


/**
 * Set the auto-vacuum status in the SQLite3 database.<br>
 *
 * @param[in] database	SQLite3 database object to set automatic vacuum
 * @param[in] flag		true: Enable automatic vacuum, false: Disable automatic vacuum
 * @return				true: success, false: failure
 */
bool M2MSQLite_setAutoVacuum (sqlite3 *database, const bool flag)
	{
	//========== Variable ==========
	bool result = false;
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MSQLite_setAutoVacuum()";
	const M2MString *PRAGMA_AUTO_VACUUM_SQL = (M2MString *)"PRAGMA auto_vacuum = 1 ";
	const M2MString *PRAGMA_NOT_AUTO_VACUUM_SQL = (M2MString *)"PRAGMA auto_vacuum = 0 ";

	//===== When automatic vacuum is enabled =====
	if (flag==true)
		{
		//===== Execute SQL statement to disable automatic vacuum =====
		if ((result=M2MSQLite_executeUpdate(database, PRAGMA_AUTO_VACUUM_SQL))==true)
			{
			}
		//===== Error handling =====
		else
			{
			M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to enable auto vacuum mode");
			}
		}
	//===== When automatic vacuum is disabled =====
	else
		{
		//===== Execute SQL statement to disable automatic vacuum =====
		if ((result=M2MSQLite_executeUpdate(database, PRAGMA_NOT_AUTO_VACUUM_SQL))==true)
			{
			}
		//===== Error handling =====
		else
			{
			M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to disable auto vacuum mode");
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
bool M2MSQLite_setSynchronous (sqlite3 *database, const bool synchronous)
	{
	//========== Variable ==========
	bool result = false;
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MSQLite_setSynchronous()";
	const M2MString *PRAGMA_SYNCHRONOUS_NORMAL_SQL = (M2MString *)"PRAGMA synchronous = NORMAL ";
	const M2MString *PRAGMA_SYNCHRONOUS_OFF_SQL = (M2MString *)"PRAGMA synchronous = OFF ";

	//===== In the case of NORMAL mode =====
	if (synchronous==true)
		{
		//===== Set NORMAL synchronous mode to SQLite3 database =====
		if ((result=M2MSQLite_executeUpdate(database, PRAGMA_SYNCHRONOUS_NORMAL_SQL))==true)
			{
			}
		//===== Error handling =====
		else
			{
			M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to set the synchronous mode into \"NORMAL\"");
			}
		}
	//===== In the case of OFF mode =====
	else
		{
		//===== Set OFF synchronous mode to SQLite3 database =====
		if ((result=M2MSQLite_executeUpdate(database, PRAGMA_SYNCHRONOUS_OFF_SQL))==true)
			{
			}
		//===== Error handling =====
		else
			{
			M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to set the synchronous mode into \"OFF\"");
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
bool M2MSQLite_setUTF8 (sqlite3 *database)
	{
	//========== Variable ==========
	bool result = false;
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MSQLite_setUTF8()";
	const M2MString *PRAGMA_ENCODING_SQL = (M2MString *)"PRAGMA encoding = 'UTF-8'";

	//===== Set UTF-8 to SQLite3 database =====
	if ((result=M2MSQLite_executeUpdate(database, PRAGMA_ENCODING_SQL))==true)
		{
		}
	//===== Error handling =====
	else
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to set the encoding into \"UTF-8\"");
		}
	return result;
	}


/**
 * Set the data in specified data type into SQLite prepared statement.<br>
 *
 * @param[in] dataType		Data type of the SQLite3 database
 * @param[in] index			Index number of the field (>=1)
 * @param[in] value			String indicating the input data of the field
 * @param[in] valueLength	Length of string indicating field input data[Byte]
 * @param[in,out] statement	SQLite3 prepared statement object
 * @return					true: Succeed to set, false: Failed to set
 */
bool M2MSQLite_setValueIntoPreparedStatement (const M2MSQLiteDataType dataType, unsigned int index, const M2MString *value, const size_t valueLength, sqlite3_stmt *statement)
	{
	//========== Variable ==========
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MSQLite_setValueIntoPreparedStatement()";

	//===== Check argument =====
	if (index>=1 && statement!=NULL)
		{
		//===== In case of BLOB type =====
		if (dataType==M2MSQLiteDataType_BLOB)
			{
			//===== Set value =====
			if (sqlite3_bind_blob(statement, index, value, valueLength, SQLITE_TRANSIENT)==SQLITE_OK)
				{
				return true;
				}
			//===== Error handling =====
			else
				{
				M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to set \"BLOB\" type value");
				return false;
				}
			}
		//===== In case of BOOL type =====
		else if (dataType==M2MSQLiteDataType_BOOL)
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
					M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to set \"BOOL\" type value");
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
					M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to set \"BOOL\" type value");
					return false;
					}
				}
			return false;
			}
		//===== In case of CHAR type =====
		else if (dataType==M2MSQLiteDataType_CHAR)
			{
			//===== Set value =====
			if (sqlite3_bind_text(statement, index, value, -1, SQLITE_TRANSIENT)==SQLITE_OK)
				{
				return true;
				}
			//===== Error handling =====
			else
				{
				M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to set \"CHAR\" type value");
				return false;
				}
			}
		//===== In case of DATETIME type =====
		else if (dataType==M2MSQLiteDataType_DATETIME)
			{
			//===== Set value =====
			if (sqlite3_bind_int64(statement, index, M2MString_convertFromStringToSignedLongLong(value, valueLength))==SQLITE_OK)
				{
				return true;
				}
			//===== Error handling =====
			else
				{
				M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to set \"DATETIME\" type value");
				return false;
				}
			}
		//===== In case of DOUBLE type =====
		else if (dataType==M2MSQLiteDataType_DOUBLE)
			{
			//===== Set value =====
			if (sqlite3_bind_double(statement, index, M2MString_convertFromStringToDouble(value, valueLength))==SQLITE_OK)
				{
				return true;
				}
			//===== Error handling =====
			else
				{
				M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to set \"DOUBLE\" type value");
				return false;
				}
			}
		//===== In case of ERROR type =====
		else if (dataType==M2MSQLiteDataType_ERROR)
			{
			return false;
			}
		//===== In case of FLOAT type =====
		else if (dataType==M2MSQLiteDataType_FLOAT)
			{
			//===== Set value =====
			if (sqlite3_bind_double(statement, index, M2MString_convertFromStringToDouble(value, valueLength))==SQLITE_OK)
				{
				return true;
				}
			//===== Error handling =====
			else
				{
				M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to set \"FLOAT\" type value");
				return false;
				}
			}
		//===== In case of INTEGER type =====
		else if (dataType==M2MSQLiteDataType_INTEGER)
			{
			//===== Set value =====
			if (sqlite3_bind_int(statement, index, M2MString_convertFromStringToSignedInteger(value, valueLength))==SQLITE_OK)
				{
				return true;
				}
			//===== Error handling =====
			else
				{
				M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to set \"INTEGER\" type value");
				return false;
				}
			}
		//===== In case of NULL type =====
		else if (dataType==M2MSQLiteDataType_NULL)
			{
			//===== Set value =====
			if (sqlite3_bind_null(statement, index)==SQLITE_OK)
				{
				return true;
				}
			//===== Error handling =====
			else
				{
				M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to set \"NULL\" type value");
				return false;
				}
			}
		//===== In case of NUMERIC type =====
		else if (dataType==M2MSQLiteDataType_NUMERIC)
			{
			//===== Set value =====
			if (sqlite3_bind_int64(statement, index, M2MString_convertFromStringToSignedLongLong(value, valueLength))==SQLITE_OK)
				{
				return true;
				}
			//===== Error handling =====
			else
				{
				M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to set \"NUMERIC\" type value");
				return false;
				}
			}
		//===== In case of REAL type =====
		else if (dataType==M2MSQLiteDataType_REAL)
			{
			//===== Set value =====
			if (sqlite3_bind_double(statement, index, M2MString_convertFromStringToDouble(value, valueLength))==SQLITE_OK)
				{
				return true;
				}
			//===== Error handling =====
			else
				{
				M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to set \"REAL\" type value");
				return false;
				}
			}
		//===== In case of TEXT type =====
		else if (dataType==M2MSQLiteDataType_TEXT)
			{
			//===== Set value =====
			if (sqlite3_bind_text(statement, index, value, -1, SQLITE_TRANSIENT)==SQLITE_OK)
				{
				return true;
				}
			//===== Error handling =====
			else
				{
				M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to set \"TEXT\" type value");
				return false;
				}
			}
		//===== In case of VARCHAR type =====
		else if (dataType==M2MSQLiteDataType_VARCHAR)
			{
			//===== Set value =====
			if (sqlite3_bind_text(statement, index, value, valueLength, SQLITE_TRANSIENT)==SQLITE_OK)
				{
				return true;
				}
			//===== Error handling =====
			else
				{
				M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to set \"VARCHAR\" type value");
				return false;
				}
			}
		//===== Error handling =====
		else
			{
			M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"A data type matching the argument can't be found");
			return false;
			}
		}
	//===== Argument error =====
	else if (index<=0)
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"index\" number of position of the data value isn't positive");
		return false;
		}
	else
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"sqlite3_stmt\" object is NULL");
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
bool M2MSQLite_setWAL (sqlite3 *database, const bool synchronous)
	{
	//========== Variable ==========
	bool result = false;
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MSQLite_setWAL()";
	const M2MString *PRAGMA_JOURNAL_MODE_SQL = (M2MString *)"PRAGMA journal_mode = WAL ";

	//===== Set journal mode of SQLite3 database =====
	if ((result=M2MSQLite_executeUpdate(database, PRAGMA_JOURNAL_MODE_SQL))==true)
		{
		}
	//===== Error handling =====
	else
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to change the journal mode into \"WAL\"");
		}
	return result;
	}


/**
 * Execute the VACUUM process to the indicated SQLite3 database.<br>
 *
 * @param[in] database	SQLite3 database object to be vacuum
 * @return				true: success, false: failure
 */
bool M2MSQLite_vacuum (sqlite3 *database)
	{
	//========== Variable ==========
	bool result = false;
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MSQLite_vacuum()";
	const M2MString *SQL = "VACUUM ";

	//===== Vacuum SQLite3 database =====
	if ((result=M2MSQLite_executeUpdate(database, SQL))==true)
		{
		}
	//===== Error handling =====
	else
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Failed to execute vacuum process");
		}
	return result;
	}



// End Of File
