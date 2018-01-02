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
 * @return				true: success，false: failure
 */
bool M2MSQLiteConfig_setAutoVacuum (sqlite3 *database, const bool flag)
	{
	//========== Variable ==========
	bool result = false;
	const M2MString *PRAGMA_AUTO_VACUUM_SQL = (M2MString *)"PRAGMA auto_vacuum = 1 ";
	const M2MString *PRAGMA_NOT_AUTO_VACUUM_SQL = (M2MString *)"PRAGMA auto_vacuum = 0 ";
	const M2MString *METHOD_NAME = (M2MString *)"M2MSQLiteConfig_setAutoVacuum";

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
 * @return					true: success，false: failure
 */
bool M2MSQLiteConfig_setSynchronous (sqlite3 *database, const bool synchronous)
	{
	//========== Variable ==========
	bool result = false;
	const M2MString *PRAGMA_SYNCHRONOUS_NORMAL_SQL = (M2MString *)"PRAGMA synchronous = NORMAL ";
	const M2MString *PRAGMA_SYNCHRONOUS_OFF_SQL = (M2MString *)"PRAGMA synchronous = OFF ";
	const M2MString *METHOD_NAME = (M2MString *)"M2MSQLiteConfig_setSynchronous";

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
 * @return				true: success，false: failure
 */
bool M2MSQLiteConfig_setUTF8 (sqlite3 *database)
	{
	//========== Variable ==========
	bool result = false;
	const M2MString *PRAGMA_ENCODING_SQL = (M2MString *)"PRAGMA encoding = 'UTF-8'";
	const M2MString *METHOD_NAME = (M2MString *)"M2MSQLiteConfig_setUTF8";

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
 * @return					true: success，false: failure
 */
bool M2MSQLiteConfig_setWAL (sqlite3 *database, const bool synchronous)
	{
	//========== Variable ==========
	bool result = false;
	const M2MString *PRAGMA_JOURNAL_MODE_SQL = (M2MString *)"PRAGMA journal_mode = WAL ";
	const M2MString *METHOD_NAME = (M2MString *)"M2MSQLiteConfig_setWAL";

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
 * @return			true: success，false: failure
 */
bool M2MSQLiteConfig_vacuum (sqlite3 *database)
	{
	//========== Variable ==========
	bool result = false;
	const M2MString *SQL = "VACUUM ";
	const M2MString *METHOD_NAME = (M2MString *)"M2MSQLiteConfig_vacuum";

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
