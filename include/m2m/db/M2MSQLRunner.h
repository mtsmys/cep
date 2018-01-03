/*******************************************************************************
 * M2MSQLRunner.h
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

#pragma once

#ifndef M2M_DB_M2MSQLRUNNER_H_
#define M2M_DB_M2MSQLRUNNER_H_



#include "m2m/lang/M2MString.h"
#include "m2m/log/M2MLogger.h"
#include <sqlite3.h>
#include <stdbool.h>



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



/*******************************************************************************
 * Public method
 ******************************************************************************/
/**
 * This function starts transaction with indicated SQLite3 database as argument.<br>
 *
 * @param[in] database	SQLite3 database manager object
 * @return				true: success, false: failure
 */
bool M2MSQLRunner_beginTransaction (sqlite3 *database);


/**
 * Commit the transaction of the specified SQLite3 database object.<br>
 *
 * @param[in] database	SQLite3 database manager object
 * @return				true: success, false: failure
 */
bool M2MSQLRunner_commitTransaction (sqlite3 *database);


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
bool M2MSQLRunner_executeUpdate (sqlite3 *database, const M2MString *sql);




#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2M_DB_M2MSQLRUNNER_H_ */
