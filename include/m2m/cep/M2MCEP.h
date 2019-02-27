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
#include "m2m/io/M2MDirectory.h"
#include "m2m/lang/M2MString.h"
#include "m2m/log/M2MFileAppender.h"
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
#define M2MCEP_VERSION (M2MString *)"0.5.0"
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
 * @param logger			File logging object
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
	M2MFileAppender *logger;
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
 * Return the logging object owned by the argument CEP object.<br>
 *
 * @param[in] self	CEP structure object
 * @return			Logging object
 */
M2MFileAppender *M2MCEP_getLogger (const M2MCEP *self);


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
 * Constructor.<br>
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
 * Executes the SQL statement specified by the argument on the SQLite 3 database <br>
 * in memory and returns the result as a CSV format string.<br>
 * For the character string of the CSV format as the execution result, the <br>
 * column name character string is the first line, and the data after the second <br>
 * line is the data.<br>
 * <br>
 * [Attention!]<br>
 * In order to execute this function, "SQLITE_ENABLE_COLUMN_METADATA" must be <br>
 * enabled as option setting at compile time of SQLite 3.<br>
 * This is because this function can not create a character string of CSV format <br>
 * to be output as a result unless table information is acquired from the SELECT <br>
 * result.<br>
 * If "SQLITE_ENABLE_COLUMN_METADATA" is not valid, keep in mind that this <br>
 * function will cause a compile error (because undefined functions are called).<br>
 *
 * @param[in] self		CEP structure object (used for database management)
 * @param[in] sql		String indicating SQL statement
 * @param[out] result	CSV character string of execution result or NULL (in case of error)
 * @return
 */
unsigned char *M2MCEP_select (M2MCEP *self, const M2MString *sql, M2MString **result);


/**
 * Set logging structure object in column information object.<br>
 *
 * @param[in,out] self	Column information object
 * @param[in] logger	Logging structure object
 * @return				Column information object with logger set or NULL (in case of error)
 */
M2MCEP *M2MCEP_setLogger (M2MCEP *self, const M2MFileAppender *logger);


/**
 * Change the maximum number of records that can be stored in each table in the <br>
 * SQLite 3 database existing in memory to the integer value specified by the <br>
 * argument.<br>
 * Note that it is not possible to set an integer number of 500 or more to the <br>
 * record number upper limit value.<br>
 *
 * @param[in,out] self		Maximum number of records CEP execution object to be changed
 * @param[in] maxRecord		Integer indicating the maximum number of records in one table or 0 (in case of error)
 * @return					CEP execution object or 0 (maximum number of records set)
 */
M2MCEP *M2MCEP_setMaxRecord (M2MCEP *self, const unsigned int maxRecord);


/**
 * Set a flag indicating permanence (recording in the SQLite 3 database on the <br>
 * file) to the CEP structure object specified by the argument.<br>
 * When set to true, all records inserted by M2MCEP_insertCSV () are recorded in <br>
 * the SQLite 3 database on the file.<br>
 * If set to "false", do not record to the SQLite 3 database on the file.<br>
 *
 * @param[in,out] self		CEP structure object
 * @param[in] persistence	Flag indicating permanence (record in SQLite 3 database on file)
 * @return					CEP execution object with a flag indicating permanence availability or NULL (in case of error)
 */
M2MCEP *M2MCEP_setPersistence (M2MCEP *self, const bool persistence);


/**
 * When forcibly terminating when using the shared library (libcep.so), set the <br>
 * event handler to acquire the signal and perform the termination processing.<br>
 *
 * @param[in] self	CEP structure object
 */
void M2MCEP_setSignalHandler (const M2MCEP *self);


/**
 * SQLite 3 Sets the number of records for vacuuming the database.<br>
 * For this vacuum processing setting, it is a setting of sharing SQLite 3 <br>
 * database on memory and SQLite 3 database on file.<br>
 *
 * @param self			Number of records for vacuum processing CEP execution object to be set
 * @param vacuumRecord	An integer value indicating the number of records of vacuum processing (automatic vacuum setting when 0 is specified)
 * @return				CEP execution object or number of records to be vacuum processed or NULL (in case of error)
 */
M2MCEP *M2MCEP_setVacuumRecord (M2MCEP *self, const unsigned int vacuumRecord);



#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2M_CEP_M2MCEP_H_ */
