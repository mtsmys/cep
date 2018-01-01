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
 * Set the auto-vacuum status in the SQLite3 database.<br>
 *
 * @param[in] database	SQLite3 database object to set automatic vacuum
 * @param[in] flag		true: Enable automatic vacuum, false: Disable automatic vacuum
 */
void M2MSQLiteConfig_setAutoVacuum (sqlite3 *database, const bool flag)
	{
	//========== Variable ==========
	const M2MString *PRAGMA_AUTO_VACUUM_SQL = (M2MString *)"PRAGMA auto_vacuum = 1 ";
	const M2MString *PRAGMA_NOT_AUTO_VACUUM_SQL = (M2MString *)"PRAGMA auto_vacuum = 0 ";
	const M2MString *METHOD_NAME = (M2MString *)"M2MSQLiteConfig_setAutoVacuum()";

	//===== Check argument =====
	if (database!=NULL)
		{
		//===== When automatic vacuum is enabled =====
		if (flag==true)
			{
			//===== Execute SQL statement to enable automatic vacuum =====
			if (M2MSQLRunner_executeUpdate(database, PRAGMA_AUTO_VACUUM_SQL)==true)
				{
				return;
				}
			//===== Error handling =====
			else
				{
				M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to automatic vacuum setting of SQLite 3 database", NULL);
				return;
				}
			}
		//===== When automatic vacuum is disabled =====
		else
			{
			//===== Execute SQL statement to disable automatic vacuum =====
			if (M2MSQLRunner_executeUpdate(database, PRAGMA_NOT_AUTO_VACUUM_SQL)==true)
				{
				return;
				}
			//===== Error handling =====
			else
				{
				M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to automatic vacuum off setting of SQLite 3 database", NULL);
				return;
				}
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"sqlite3\" structure object is NULL", NULL);
		return;
		}
	}


/**
 * Set the character code of the database to UTF-8.<br>
 *
 * @param[in] database	SQLite3 database object to set character code to UTF-8
 */
void M2MSQLiteConfig_setUTF8 (sqlite3 *database)
	{
	//========== Variable ==========
	const M2MString *PRAGMA_ENCODING_SQL = (M2MString *)"PRAGMA encoding = 'UTF-8'";
	const M2MString *METHOD_NAME = (M2MString *)"M2MSQLiteConfig_setUTF8()";

	//===== Check argument =====
	if (database!=NULL)
		{
		//===== Execute SQL statement =====
		if (M2MSQLRunner_executeUpdate(database, PRAGMA_ENCODING_SQL)==true)
			{
			return;
			}
		//===== Error handling =====
		else
			{
			M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to set UTF-8 character code of SQLite3 database", NULL);
			return;
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"sqlite3\" structure object is NULL", NULL);
		return;
		}
	}


/**
 * Set the journal mode of the SQLite 3 database to WAL (Write Ahead Logging).<br>
 *
 * @param[in] database		SQLite3 database object to be set as WAL
 * @param[in] synchronous	true: Sync mode, false: Asynchronous mode
 */
static void M2MSQLiteConfig_setWAL (sqlite3 *database, const bool synchronous)
	{
	//========== Variable ==========
	const M2MString *PRAGMA_JOURNAL_MODE_SQL = (M2MString *)"PRAGMA journal_mode = WAL ";
	const M2MString *PRAGMA_SYNCHRONOUS_SQL = (M2MString *)"PRAGMA synchronous = NORMAL ";
	const M2MString *PRAGMA_NOT_SYNCHRONOUS_SQL = (M2MString *)"PRAGMA synchronous = OFF ";
	const M2MString *METHOD_NAME = (M2MString *)"M2MSQLiteConfig_setWAL()";

	//===== Check argument =====
	if (database!=NULL)
		{
#ifdef DEBUG
		M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, (M2MString *)"Set the journal mode of SQLite3 database");
#endif // DEBUG
		//===== データベースのジャーナルモードを設定 =====
		if (M2MSQLRunner_executeUpdate(database, PRAGMA_JOURNAL_MODE_SQL)==true)
			{
			//===== 同期モードの場合 =====
			if (synchronous==true
					&& M2MSQLRunner_executeUpdate(database, PRAGMA_SYNCHRONOUS_SQL)==true)
				{
#ifdef DEBUG
				M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, (M2MString *)"Journal mode of SQLite 3 database set to WAL (synchronous mode)");
#endif // DEBUG
				return;
				}
			//===== 非同期モードの場合 =====
			else if (synchronous==false
					&& M2MSQLRunner_executeUpdate(database, PRAGMA_NOT_SYNCHRONOUS_SQL)==true)
				{
#ifdef DEBUG
				M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, (M2MString *)"Journal mode of SQLite 3 database set to WAL (asynchronous mode)");
#endif // DEBUG
				return;
				}
			//===== Error handling =====
			else
				{
				M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to set SQLite3 database synchronization mode", NULL);
				return;
				}
			}
		//===== Error handling =====
		else
			{
			M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to set WAL journal mode of SQLite 3 database", NULL);
			return;
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"sqlite3\" structure object is NULL", NULL);
		}
	return;
	}




// End Of File
