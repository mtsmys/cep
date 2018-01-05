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
 * @param database	SQLite3 database to be closed
 */
void M2MSQLiteConfig_closeDatabase (sqlite3 *database)
	{
	//========== Variable ==========
	sqlite3_stmt *statement = NULL;
	sqlite3_stmt *next = NULL;
	M2MString MESSAGE[256];
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
				memset(MESSAGE, 0, sizeof(MESSAGE));
				snprintf(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Failed to close SQLite3 database because %s", (M2MString *)sqlite3_errmsg(database));
				M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, MESSAGE, NULL);
				}
			}
		}
	//===== Argument error =====
	else
		{
		snprintf(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Argument error! Indicated \"sqlite3\" object is NULL");
		}
	return;
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
	size_t tableNameLength = 0;
	const M2MString *FORMAT = (M2MString *)"PRAGMA table_info(%s)";
	const size_t FORMAT_LENGTH = M2MString_length(FORMAT);
	const M2MString *METHOD_NAME = (M2MString *)"M2MSQLiteConfig_getTableInfoSQL()";

	//===== Check argument =====
	if (tableName!=NULL && (tableNameLength=M2MString_length(tableName))>0
			&& sql!=NULL)
		{
		//===== Get heap memory for copying SQL string =====
		if (((*sql)=(M2MString *)M2MHeap_malloc(FORMAT_LENGTH+tableNameLength+1))!=NULL)
			{
			//===== Copy SQL string =====
			snprintf((*sql), FORMAT_LENGTH+tableNameLength, FORMAT, tableName);
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
	else if (tableName==NULL || tableNameLength<=0)
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
 * Open the indicated SQLite3 database.<br>
 * If caller uses a database in memory, should set ":memory:" as <br>
 * "filename" argument.<br>
 *
 * @param filename	String indicating database name
 * @return			Connection handler of opened SQLite3 database or NULL (in case of error)
 */
sqlite3 *M2MSQLiteConfig_openDatabase (const M2MString *filename)
	{
	//========== Variable ==========
	sqlite3 *database = NULL;
	M2MString MESSAGE[256];
	const M2MString *METHOD_NAME = (M2MString *)"M2MSQLiteConfig_openDatabase()";

	//===== Open the SQLite3 database =====
	if (sqlite3_open(filename, &database)==SQLITE_OK)
		{
#ifdef DEBUG
		memset(MESSAGE, 0, sizeof(MESSAGE));
		snprintf(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Succeed to open a SQLite3 database(=\"%s\")", filename);
		M2MLogger_printDebugMessage(MESSAGE, __LINE__, MESSAGE);
#endif // DEBUG
		//===== Return the connection handler =====
		return database;
		}
	//===== Error handling =====
	else
		{
		//===== Close the SQLite3 database =====
		M2MSQLiteConfig_closeDatabase(database);
		memset(MESSAGE, 0, sizeof(MESSAGE));
		snprintf(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Failed to open a SQLite3 database(=\"%s\")", filename);
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, MESSAGE, NULL);
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
 * Execute the VACUUM process to the indicated SQLite3 database.<br>
 *
 * @param database	SQLite3 database object to be vacuum
 * @return			true: success, false: failure
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
