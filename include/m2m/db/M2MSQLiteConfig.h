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



#include "m2m/db/M2MDataType.h"
#include "m2m/lang/M2MString.h"
#include "m2m/log/M2MFileAppender.h"
#include <stdbool.h>
#include <stdint.h>
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


/**
 * String indicating SQLite3 database directory permission(="0755")
 */
#ifndef M2MSQLiteConfig_DATABASE_DIRECTORY_PERMISSION
#define M2MSQLiteConfig_DATABASE_DIRECTORY_PERMISSION (M2MString *)"0755"
#endif /* M2MSQLiteConfig_DATABASE_DIRECTORY_PERMISSION */


/**
 * String indicating the use of SQLite3 memory database (=":memory:")
 */
#ifndef M2MSQLiteConfig_MEMORY_DATABASE
#define M2MSQLiteConfig_MEMORY_DATABASE (M2MString *)":memory:"
#endif /* M2MSQLiteConfig_MEMORY_DATABASE */



/*******************************************************************************
 * Public function
 ******************************************************************************/
/**
 * This function starts transaction with indicated SQLite3 database as argument.<br>
 *
 * @param[in] database	SQLite3 database manager object
 * @return				true: success, false: failure
 */
bool M2MSQLiteConfig_beginTransaction (sqlite3 *database);


/**
 * Close the connection of indicated SQLite3 database.<br>
 *
 * @param[in] database	SQLite3 database object to be closed
 */
void M2MSQLiteConfig_closeDatabase (sqlite3 *database);


/**
 * Commit the transaction of the specified SQLite3 database object.<br>
 *
 * @param[in] database	SQLite3 database manager object
 * @return				true: success, false: failure
 */
bool M2MSQLiteConfig_commitTransaction (sqlite3 *database);


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
bool M2MSQLiteConfig_executeUpdate (sqlite3 *database, const M2MString *sql);


/**
 * Get string indicating SQLite3 error message.<br>
 *
 * @param database	SQLite3 database management object
 * @return			String indicating SQLite3 error message which is handled by argument
 */
M2MString *M2MSQLiteConfig_getErrorMessage (sqlite3 *database);


/**
 * SQLite 3 Returns the maximum number of columns allowed in the table.<br>
 *
 * @param database	SQLite3 database management object
 * @return			Maximum number of columns in one table [pieces]
 */
int32_t M2MSQLiteConfig_getMaxColumnLength (sqlite3 *database);


/**
 * Get a prepared statement object related with indicated SQL statement string.<br>
 *
 * @param[in] database	SQLite3 database object
 * @param[in] sql		String indicating SQL statement
 * @return				Prepared statement object or NULL (in case of error)
 */
sqlite3_stmt *M2MSQLiteConfig_getPreparedStatement (sqlite3 *database, const M2MString *sql);


/**
 * Create SQL statement for displaying table information, and copy to the pointer.<br>
 *
 * @param[in] tableName	String indicating table name
 * @param[out] sql		Pointer for copying the SQL statement (buffering is executed inside the function)
 * @return				String copied to pointer or NULL (in case of error)
 */
M2MString *M2MSQLiteConfig_getTableInfoSQL (const M2MString *tableName, M2MString **sql);


/**
 * Check the existence of indicated table and return the result.<br>
 *
 * @param[in] database	SQLite3 database object
 * @param[in] tableName	String indicating the table name
 * @return				true: In case of existing the table, false: In case of not existing the table
 */
bool M2MSQLiteConfig_isExistingTable (sqlite3 *database, const M2MString *tableName);


/**
 * Open the indicated SQLite3 database.<br>
 * If caller uses a database in memory, should set ":memory:" as <br>
 * "filename" argument.<br>
 * Otherwise, caller should SQLite3 file pathname string as "filename" <br>
 * argument.<br>
 *
 * @param[in] sqliteFilePath	String indicating database file pathname(if you want to use in-memory database, please set ":memory:" string)
 * @return						Connection handler of opened SQLite3 database or NULL (in case of error)
 */
sqlite3 *M2MSQLiteConfig_openDatabase (const M2MString *sqliteFilePath);


/**
 * Execute SQL on the prepared statement object and return result code.<br>
 *
 * @param[in] statement	SQLite3 prepared statement object
 * @return				Result code defined by SQLite3 API
 */
int M2MSQLiteConfig_next (sqlite3_stmt *statement);


/**
 * Rollback the transaction of the specified SQLite3 database object.<br>
 *
 * @param[in] database	SQLite3 database manager object
 * @return				true: success, false: failure
 */
bool M2MSQLiteConfig_rollbackTransaction (sqlite3 *database);


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
 * Set the data in specified data type into SQLite prepared statement.<br>
 *
 * @param[in] dataType		Data type of the SQLite3 database
 * @param[in] index			Index number of the field (>=1)
 * @param[in] value			String indicating the input data of the field
 * @param[in] valueLength	Length of string indicating field input data[Byte]
 * @param[in,out] statement	SQLite3 prepared statement object
 * @return					true: Succeed to set, false: Failed to set
 */
bool M2MSQLiteConfig_setValueIntoPreparedStatement (const M2MDataType dataType, unsigned int index, const M2MString *value, const size_t valueLength, sqlite3_stmt *statement);


/**
 * Execute the VACUUM process to the indicated SQLite3 database.<br>
 *
 * @param[in] database	SQLite3 database object to be vacuum
 * @return				true: success, false: failure
 */
bool M2MSQLiteConfig_vacuum (sqlite3 *database);



#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2M_DB_M2MSQLITECONFIG_H_ */
