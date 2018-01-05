/*******************************************************************************
 * M2MSQLiteConfig.h
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

#pragma once

#ifndef M2M_DB_M2MSQLITECONFIG_H_
#define M2M_DB_M2MSQLITECONFIG_H_



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



#include "m2m/db/M2MSQLRunner.h"
#include "m2m/lang/M2MString.h"
#include "m2m/log/M2MLogger.h"
#include <stdbool.h>
#include <stdio.h>
#include <sqlite3.h>



/*******************************************************************************
 * Definition
 ******************************************************************************/
/**
 * String indicating the SQLite3 database file extension.
 */
#ifndef M2MSQLiteConfig_FILE_EXTENSION
#define M2MSQLiteConfig_FILE_EXTENSION (M2MString *)".sqlite"
#endif /* M2MSQLiteConfig_FILE_EXTENSION */



/*******************************************************************************
 * Public function
 ******************************************************************************/
/**
 * Close the connection of indicated SQLite3 database.<br>
 *
 * @param database	SQLite3 database to be closed
 */
void M2MSQLiteConfig_closeDatabase (sqlite3 *database);


/**
 * Create SQL statement for displaying table information, and copy to the pointer.<br>
 *
 * @param[in] tableName	String indicating table name
 * @param[out] sql		Pointer for copying the SQL statement (buffering is executed inside the function)
 * @return				String copied to pointer or NULL (in case of error)
 */
M2MString *M2MSQLiteConfig_getTableInfoSQL (const M2MString *tableName, M2MString **sql);


/**
 * Open the indicated SQLite3 database.<br>
 * If caller uses a database in memory, should set ":memory:" as <br>
 * "filename" argument.<br>
 *
 * @param filename	String indicating database name
 * @return			Connection handler of opened SQLite3 database or NULL (in case of error)
 */
sqlite3 *M2MSQLiteConfig_openDatabase (const M2MString *filename);


/**
 * Set the auto-vacuum status in the SQLite3 database.<br>
 *
 * @param[in] database	SQLite3 database object to set automatic vacuum
 * @param[in] flag		true: Enable automatic vacuum, false: Disable automatic vacuum
 * @return				true: success, false: failure
 */
bool M2MSQLiteConfig_setAutoVacuum (sqlite3 *database, const bool flag);


/**
 * Set the synchronous mode of the SQLite 3 database.<br>
 *
 * @param[in] database		SQLite3 database object to be set synchronous mode
 * @param[in] synchronous	true: Synchronous mode = NORMAL, false: Synchronous mode = OFF
 * @return					true: success, false: failure
 */
bool M2MSQLiteConfig_setSynchronous (sqlite3 *database, const bool synchronous);


/**
 * Set the character code of the database to UTF-8.<br>
 *
 * @param[in] database	SQLite3 database object to set character code to UTF-8
 * @return				true: success, false: failure
 */
bool M2MSQLiteConfig_setUTF8 (sqlite3 *database);


/**
 * Execute the VACUUM process to the indicated SQLite3 database.<br>
 *
 * @param database	SQLite3 database object to be vacuum
 * @return			true: success, false: failure
 */
bool M2MSQLiteConfig_vacuum (sqlite3 *database);



#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2M_DB_M2MSQLITECONFIG_H_ */
