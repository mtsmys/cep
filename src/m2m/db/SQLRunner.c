/*******************************************************************************
 * SQLRunner.c
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

#include "m2m/db/SQLRunner.h"


/*******************************************************************************
 * Public function
 ******************************************************************************/
/**
 * This function starts transaction with indicated SQLite3 database as argument.<br>
 *
 * @param[in] database	SQLite3 database manager object
 * @return				true: success, false: failure
 */
bool SQLRunner_beginTransaction (sqlite3 *database)
	{
	//========== Variable ==========
	const M2MString *BEGIN_SQL = (M2MString *)"BEGIN ";

	//===== Check argument =====
	if (database!=NULL)
		{
		//===== Execute SQL =====
		if (SQLRunner_executeUpdate(database, BEGIN_SQL)==true)
			{
#ifdef DEBUG
			M2MLogger_printDebugMessage((M2MString *)"SQLRunner_beginTransaction()", __LINE__, (M2MString *)"Transaction started for SQLite3 database");
#endif // DEBUG
			return true;
			}
		//===== Error handling =====
		else
			{
			M2MLogger_printErrorMessage((M2MString *)"SQLRunner_beginTransaction()", __LINE__, (M2MString *)"Failed to the start of transaction in SQLite3 database", NULL);
			return false;
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage((M2MString *)"SQLRunner_beginTransaction()", __LINE__, (M2MString *)"Argument error! Indicated \"sqlite3\" is NULL", NULL);
		return false;
		}
	}


/**
 * Commit the transaction of the specified SQLite3 database object.<br>
 *
 * @param[in] database	SQLite3 database manager object
 * @return				true: success, false: failure
 */
bool SQLRunner_commitTransaction (sqlite3 *database)
	{
	//========== Variable ==========
	const M2MString *COMMIT_SQL = (M2MString *)"COMMIT ";

	//===== Check argument =====
	if (database!=NULL)
		{
		//===== Commit =====
		if (SQLRunner_executeUpdate(database, COMMIT_SQL)==true)
			{
#ifdef DEBUG
			M2MLogger_printDebugMessage((M2MString *)"SQLRunner_commitTransaction()", __LINE__, (M2MString *)"Success to the commit in SQLite3 database");
#endif // DEBUG
			return true;
			}
		//===== Error handling =====
		else
			{
			M2MLogger_printErrorMessage((M2MString *)"SQLRunner_commitTransaction()", __LINE__, (M2MString *)"Failed to the commit in SQLite3 database", NULL);
			return false;
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage((M2MString *)"SQLRunner_commitTransaction()", __LINE__, (M2MString *)"Argument error! Indicated \"sqlite3\" is NULL", NULL);
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
bool SQLRunner_executeUpdate (sqlite3 *database, const M2MString *sql)
	{
	//========== Variable ==========
	sqlite3_stmt *statement = NULL;

	//===== Check argument =====
	if (database!=NULL && sql!=NULL)
		{
		//===== Convert SQL statement to VDBE (internal execution format) =====
		if (sqlite3_prepare(database, sql, -1, &statement, NULL)==SQLITE_OK)
			{
			//===== Execute SQL =====
			while (sqlite3_step(statement)==SQLITE_BUSY)
				{
				}
			if (sqlite3_finalize(statement)==SQLITE_OK)
				{
				return true;
				}
			//===== Error handling =====
			else
				{
				M2MLogger_printErrorMessage((M2MString *)"SQLRunner_executeUpdate()", __LINE__, (M2MString *)sqlite3_errmsg(database), NULL);
				return false;
				}
			}
		//===== Error handling =====
		else
			{
			M2MLogger_printErrorMessage((M2MString *)"SQLRunner_executeUpdate()", __LINE__, (M2MString *)sqlite3_errmsg(database), NULL);
			return false;
			}
		}
	//===== Argument error =====
	else if (database==NULL)
		{
		M2MLogger_printErrorMessage((M2MString *)"SQLRunner_executeUpdate()", __LINE__, (M2MString *)"Argument error! Indicated \"sqlite3\" is NULL", NULL);
		return false;
		}
	else
		{
		M2MLogger_printErrorMessage((M2MString *)"SQLRunner_executeUpdate()", __LINE__, (M2MString *)"Argument error! Indicated \"sql\" string is NULL", NULL);
		return false;
		}
	}



/* End Of File */
