/*******************************************************************************
 * M2MTableManager.h
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

#ifndef M2M_LIB_DB_M2MTABLEMANAGER_H_
#define M2M_LIB_DB_M2MTABLEMANAGER_H_


#include "m2m/lib/db/M2MSQLite.h"
#include "m2m/lib/db/M2MColumnList.h"
#include "m2m/lib/lang/M2MString.h"
#include "m2m/lib/log/M2MFileAppender.h"
#include <sqlite3.h>
#include <stdio.h>
#include <string.h>



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



/*******************************************************************************
 * Definition
 ******************************************************************************/
/**
 * Structure object holding table column information.<br>
 * This structure can store many table informations, and each table information <br>
 * is held in the form of a list structure.<br>
 *
 * @param previous		A list structure object located before (indicating its own pointer in the case of the head)
 * @param next			A list structure object located behind (indicating NULL in the case of the end)
 * @param tableName		String indicating table name
 * @param columnList	List structure object containing table column informations
 */
#ifndef M2MTableManager
typedef struct M2MTableManager
	{
	struct M2MTableManager *previous;
	struct M2MTableManager *next;
	M2MString *tableName;
	M2MColumnList *columnList;
	} M2MTableManager;
#endif /* M2MTableManager */


/*******************************************************************************
 * Public function
 ******************************************************************************/
/**
 * Create a table for the SQLite3 database.<br>
 *
 * @param[in] self		Table construction object (column information inserted)
 * @param[in] database	SQLite3 database to create table
 */
void M2MTableManager_createTable (M2MTableManager *self, sqlite3 *database);


/**
 * Releases heap memory of the table construction object. <br>
 * Be aware that after calling this function you can no longer access <br>
 * that variable.<br>
 *
 * @param[in,out] self	Table construction object to be freed heap memory
 */
void M2MTableManager_delete (M2MTableManager **self);


/**
 * Get column list object matching the table name from table construction object.<br>
 *
 * @param[in] self		Table construction object
 * @param[in] tableName	String indicating table name
 * @return				Column structure object with matching table name or NULL (in case of error)
 */
M2MColumnList *M2MTableManager_getColumnList (M2MTableManager *self, const M2MString *tableName);


/**
 * Get heap memory and create a new table construction object.<br>
 *
 * @return	Created new table construction object
 */
M2MTableManager *M2MTableManager_new ();


/**
 * Store the column information in the table construction object.<br>
 *
 * @param[in,out] self		Table construction object
 * @param[in] tableName		String indicating table name
 * @param[in] columnList	Column structure object storing column setting information
 * @return					Table construction object containing column setting information or NULL (in case of error)
 */
M2MTableManager *M2MTableManager_setConfig (M2MTableManager *self, const M2MString *tableName, M2MColumnList *columnList);



#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2M_DB_M2MTABLEMANAGER_H_ */
