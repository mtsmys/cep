/*******************************************************************************
 * M2MCEP.h
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

#ifndef M2M_CEP_M2MCEP_H_
#define M2M_CEP_M2MCEP_H_


#include "m2m/cep/M2MDataFrame.h"
#include "m2m/db/M2MTableManager.h"
#include "m2m/db/M2MSQLiteConfig.h"
#include "m2m/db/M2MSQLRunner.h"
#include "m2m/lang/M2MString.h"
#include "m2m/log/M2MLogger.h"
#include "m2m/util/M2MBase64.h"
#include <signal.h>
#include <sqlite3.h>



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



/*******************************************************************************
 * Definition
 ******************************************************************************/
/**
 */
#ifndef M2MCEP_VERSION
#define M2MCEP_VERSION (M2MString *)"0.4.1"
#endif /* M2MCEP_VERSION */


/**
 * CEP working directory
 */
#ifndef M2MCEP_DIRECTORY
#define M2MCEP_DIRECTORY (M2MString *)".m2m/cep"
#endif /* M2MCEP_DIRECTORY */


/**
 * CEP (Complex Event Processing) execution structure object.<br>
 *
 * @param databaseName		String indicating database name
 * @param tableManager		Database table manager structure object
 * @param memoryDatabase	SQLite3 database on memory
 * @param fileDatabase		SQLite3 database on file
 * @param dataFrame			DataFrame which is the core data for CEP
 * @param maxRecord			Maximum record number of table in memory
 * @param vacuumRecord		Number of records to execute database vacuum processing (auto vacuum setting in case of 0)
 * @param recordCounter		Record number counter for executing vacuum process of database
 * @param persistence		Flag indicating permanence of SQLite3 database on file
 */
#ifndef M2MCEP
typedef struct
	{
	M2MString *databaseName;
	M2MTableManager *tableManager;
	sqlite3 *memoryDatabase;
	sqlite3 *fileDatabase;
	M2MDataFrame *dataFrame;
	unsigned int maxRecord;
	unsigned int vacuumRecord;
	unsigned int recordCounter;
	bool persistence;
	} M2MCEP;
#endif /* M2MCEP */



/*******************************************************************************
 * Public function
 ******************************************************************************/
/**
 * Release the heap memory of CEP structure object. <br>
 * Note that after calling this function, caller will not be able to access <br>
 * the CEP structure object appointed by argument.<br>
 *
 * @param[in,out] self	CEP structure object to be freed of memory area
 */
void M2MCEP_delete (M2MCEP **self);


/**
 * Return string indicating the database name possessed by the CEP object.<br>
 *
 * @param[in] self	CEP structure object
 * @return			String indicating database file name or NULL (in case of error)
 */
M2MString *M2MCEP_getDatabaseName (const M2MCEP *self);


/**
 * Get the SQLite3 database object on the file managed by the CEP object. <br>
 * If the CEP object does not hold the SQLite 3 database object on the file <br>
 * and the record persistence flag is set up, it will acquire and return <br>
 * the SQLite 3 database object newly created.<br>
 *
 * @param[in] self	CEP structure object
 * @return			SQLite3 database object on file or NULL (in case of error)
 */
sqlite3 *M2MCEP_getFileDatabase (M2MCEP *self);


/**
 * Return the SQLite 3 database object on the memory managed by the CEP object.<br>
 *
 * @param[in] self	CEP structure object
 * @return			SQLite3 database object on memory or NULL (in case of error)
 */
sqlite3 *M2MCEP_getMemoryDatabase (M2MCEP *self);


/**
 * Return CEP library version number string defined in "M2MCEP.h" file.<br>
 *
 * @return	CEP library version number string
 */
M2MString *M2MCEP_getVersion ();


/**
 * Insert string in CSV format into the table of SQLite 3 database for CEP. <br>
 * The internal processing procedure is as follows.<br>
 * <br>
 * 1) Parse CSV string and import into a CEPRecord structure object. <br>
 * 2) Insert record data into the table of SQLite3 memory database. <br>
 * 3) Check the current record number of the table of SQLite3 memory database, <br>
 *    and delete it in the oldest order if it exceeds the specified <br>
 *    maximum value. <br>
 * 4) Fetch the same data as the deleted record from the CEPRecord <br>
 *    structure object and insert it into the SQLite3 database file for <br>
 *    persistence. <br>
 * 5) Delete excess from record data of CEPRecord structure object. <br>
 *
 * @param[in,out] self	CEP structure object
 * @param[in] tableName	String indicating the table name to be inserted record
 * @param[in] csv		String in CSV format as insert data (the first line specifies the column name as a header)
 * @return				Number of inserted records[row] or -1 (in case of error)
 */
int M2MCEP_insertCSV (M2MCEP *self, const M2MString *tableName, const M2MString *csv);


/**
 * Create new CEP structure object and prepare for SQLite3 database processing. <br>
 * For the table, be sure to specify the SQLite3 database on memory as it <br>
 * needs to be built every time "M2MCEP" object is created. <br>
 *
 * @param[in] databaseName	String indicating SQLite3 database name
 * @param[in] tableBuilder	Structure object for building SQLite3 database table
 * @return					Created CEP structure object or NULL (in case of error)
 */
M2MCEP *M2MCEP_new (const M2MString *databaseName, const M2MTableManager *tableManager);


/**
 * メモリ上のSQLite3データベースに対し，引数で指定されたSQL文を実行し，その結果を<br>
 * CSV形式の文字列で返す．<br>
 * 実行結果であるCSV形式の文字列は，1行めにカラム名文字列，2行目以降がデータとなる．<br>
 *
 * @param[in] self		CEP構造体オブジェクト（データベース管理に使用する）
 * @param[in] sql		SQL文を示す文字列
 * @param[out] result	実行結果のCSV文字列 or NULL（エラーの場合）
 * @return
 */
unsigned char *M2MCEP_select (M2MCEP *self, const M2MString *sql, M2MString **result);


/**
 * メモリに存在するSQLite3データベース内の各テーブルに格納可能な最大レコード数を<br>
 * 引数で指定された整数値に変更する．<br>
 *
 * @param[in,out] self		最大レコード数変更対象のCEP実行オブジェクト
 * @param[in] maxRecord		1つのテーブルにおける最大レコード数を示す整数 or 0（エラーの場合）
 * @return					最大レコード数を設定したCEP実行オブジェクト or 0（エラーの場合）
 */
M2MCEP *M2MCEP_setMaxRecord (M2MCEP *self, const unsigned int maxRecord);


/**
 * 引数で指定されたCEP構造体オブジェクトに対し、永続性（ファイル上のSQLite3<br>
 * データベースへの記録)の可否を示すフラグをセットする．<br>
 * true を設定した場合、M2MCEP_insertCSV() で挿入されたレコードは全て、<br>
 * ファイル上のSQLite3データベースへ記録する．<br>
 * false を設定した場合、ファイル上のSQLite3データベースへの記録は実行しない．<br>
 *
 * @param[in,out] self		CEP構造体オブジェクト
 * @param[in] persistence	永続性（ファイル上のSQLite3データベースへの記録)の可否を示すフラグ
 * @return					永続性の可否を示すフラグを設定したCEP実行オブジェクト or NULL（エラーの場合）
 */
M2MCEP *M2MCEP_setPersistence (M2MCEP *self, const bool persistence);


/**
 * SQLite3データベースのバキューム処理を行うレコード数を設定する．<br>
 * このバキューム処理設定については，メモリ上のSQLite3データベースとファイル上の
 * SQLite3データベース共用の設定となる．<br>
 *
 * @param self			バキューム処理のレコード数設定対象のCEP実行オブジェクト
 * @param vacuumRecord	バキューム処理のレコード数を示す整数値（0を指定した場合は自動バキューム設定)
 * @return				バキューム処理を行うレコード数を設定したCEP実行オブジェクト or NULL（エラーの場合）
 */
M2MCEP *M2MCEP_setVacuumRecord (M2MCEP *self, const unsigned int vacuumRecord);



#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2M_CEP_M2MCEP_H_ */
