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
	const M2MString *PRAGMA_AUTO_VACUUM_SQL = (M2MString *)"PRAGMA auto_vacuum = 1 ";
	const M2MString *PRAGMA_NOT_AUTO_VACUUM_SQL = (M2MString *)"PRAGMA auto_vacuum = 0 ";

	//===== When automatic vacuum is enabled =====
	if (flag==true)
		{
		//===== Execute SQL statement to disable automatic vacuum =====
		return M2MSQLRunner_executeUpdate(database, PRAGMA_AUTO_VACUUM_SQL);
		}
	//===== When automatic vacuum is disabled =====
	else
		{
		//===== Execute SQL statement to disable automatic vacuum =====
		return M2MSQLRunner_executeUpdate(database, PRAGMA_NOT_AUTO_VACUUM_SQL);
		}
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
	const M2MString *PRAGMA_SYNCHRONOUS_NORMAL_SQL = (M2MString *)"PRAGMA synchronous = NORMAL ";
	const M2MString *PRAGMA_SYNCHRONOUS_OFF_SQL = (M2MString *)"PRAGMA synchronous = OFF ";

	//===== In the case of NORMAL mode =====
	if (synchronous==true)
		{
		return M2MSQLRunner_executeUpdate(database, PRAGMA_SYNCHRONOUS_NORMAL_SQL);
		}
	//===== In the case of OFF mode =====
	else
		{
		return M2MSQLRunner_executeUpdate(database, PRAGMA_SYNCHRONOUS_OFF_SQL);
		}
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
	const M2MString *PRAGMA_ENCODING_SQL = (M2MString *)"PRAGMA encoding = 'UTF-8'";

	//===== Set UTF-8 to SQLite3 database =====
	return M2MSQLRunner_executeUpdate(database, PRAGMA_ENCODING_SQL);
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
	const M2MString *PRAGMA_JOURNAL_MODE_SQL = (M2MString *)"PRAGMA journal_mode = WAL ";

	//===== Set journal mode of SQLite3 database =====
	return M2MSQLRunner_executeUpdate(database, PRAGMA_JOURNAL_MODE_SQL);
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
	const M2MString *SQL = "VACUUM ";

	//===== Vacuum SQLite3 database =====
	return M2MSQLRunner_executeUpdate(database, SQL);
	}



// End Of File
