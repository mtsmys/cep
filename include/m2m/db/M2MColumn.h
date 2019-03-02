/*******************************************************************************
 * M2MColumn.h
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

#ifndef M2M_DB_M2MCOLUMN_H_
#define M2M_DB_M2MCOLUMN_H_



#include "m2m/db/M2MSQLiteDataType.h"
#include "m2m/io/M2MHeap.h"
#include "m2m/lang/M2MString.h"
#include "m2m/log/M2MFileAppender.h"
#include <stdbool.h>



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



/*******************************************************************************
 * Definition
 ******************************************************************************/
/**
 * Structure object containing database column information.<br>
 *
 * @param name				String indicating column name
 * @param dataType			Data type of the column
 * @param primaryKey		Flag for primary key activation
 * @param autoIncrement		Flag for auto increment enable
 * @param allowNULL			Flag for NULL activation
 * @param unique			Flag of uniqueness validation
 * @param logger			Logger object
 */
#ifndef M2MColumn
typedef struct
	{
	M2MString *name;
	M2MSQLiteDataType dataType;
	bool primaryKey;
	bool autoIncrement;
	bool allowNULL;
	bool unique;
	M2MFileAppender *logger;
	} M2MColumn;
#endif /* M2MColumn */



/*******************************************************************************
 * Public function
 ******************************************************************************/
/**
 * Destructor.<br>
 * Release the heap memory of column information object.<br>
 *
 * @param[in,out] self	Column information object
 */
void M2MColumn_delete (M2MColumn **self);


/**
 * Return string indicating the column name held by column information object.<br>
 *
 * @param[in] self	Column information object
 * @return			String indicating the column name or NULL (in case of error)
 */
unsigned char *M2MColumn_getName (const M2MColumn *self);


/**
 * Return the data type held by column information object.<br>
 *
 * @param[in] self	Column information object
 */
M2MSQLiteDataType M2MColumn_getDataType (const M2MColumn *self);


/**
 * Return flag for primary key activation held by column information object.<br>
 *
 * @param[in] self	Column information object
 */
bool M2MColumn_getPrimaryKey (const M2MColumn *self);


/**
 * Return flag for auto increment enable held by column information object.<br>
 *
 * @param[in] self	Column information object
 */
bool M2MColumn_getAutoIncrement (const M2MColumn *self);


/**
 * Return flag for NULL activation held by column information object.<br>
 *
 * @param[in] self	Column information object
 */
bool M2MColumn_getAllowNULL (const M2MColumn *self);


/**
 * Return Logger object held by column information object.<br>
 *
 * @param[in] self	Column information object
 * @return			Logger object owned by argument or NULL (in case of error)
 */
M2MFileAppender *M2MColumn_getLogger (const M2MColumn *self);


/**
 * Return flag of uniqueness validation held by column information object.<br>
 *
 * @param[in] self	Column information object
 */
bool M2MColumn_getUnique (const M2MColumn *self);


/**
 * Constructor.<br>
 * Get the heap memory and create a new column information object.
 *
 * @return	Created new column information object or NULL (in case of error)
 */
M2MColumn *M2MColumn_new ();


/**
 * Set flag whether auto increment is enabled or not in column information object.<br>
 *
 * @param[in,out] self		Column information object
 * @param[in] autoIncrement	Flag indicating whether auto increment
 * @return					Column information object with automatic increment information set or NULL (in case of error)
 */
M2MColumn *M2MColumn_setAutoIncrement (M2MColumn *self, const bool autoIncrement);


/**
 * Set a flag indicating whether NULL is allowed or not in the record.<br>
 *
 * @param[in,out] self	Column information object
 * @param[in] allowNULL	Flag indicating whether NULL is allowed for record
 * @return				Column information object set with flag indicating NULL availability or NULL (in case of error)
 */
M2MColumn *M2MColumn_setAllowNULL (M2MColumn *self, const bool allowNULL);


/**
 * Set an enumerator indicating the data type of the column information object.<br>
 *
 * @param[in,out] self	Column information object
 * @param[in] dataType	Enumerator for data type
 * @return				Column information object with enumerator indicating data type or NULL (in case of error)
 */
M2MColumn *M2MColumn_setDataType (M2MColumn *self, const M2MSQLiteDataType dataType);


/**
 * Set logging structure object in column information object.<br>
 *
 * @param[in,out] self	Column information object
 * @param[in] logger	Logging structure object
 * @return				Column information object with logger set or NULL (in case of error)
 */
M2MColumn *M2MColumn_setLogger (M2MColumn *self, const M2MFileAppender *logger);


/**
 * Set string indicating a column name in column information object.<br>
 *
 * @param[in,out] self	Column information object
 * @param[in] name		String indicating the column name
 * @return				Column information object with column name set or NULL (in case of error)
 */
M2MColumn *M2MColumn_setName (M2MColumn *self, const M2MString *name);


/**
 * Set a flag indicating whether or not it is a primary key.<br>
 *
 * @param[in,out] self		Column information object
 * @param[in] primaryKey	Flag indicating whether or not it is a primary key
 * @return					Column information object with flag set or NULL (in case of error)
 */
M2MColumn *M2MColumn_setPrimaryKey (M2MColumn *self, const bool primaryKey);


/**
 * Set a flag indicating the uniqueness of the record in column information object.<br>
 *
 * @param[in,out] self	Column information object
 * @param[in] unique	Flag indicating whether or not the record data has uniqueness
 * @return				Column information object or flag with flag indicating whether uniqueness is set or NULL (in case of error)
 */
M2MColumn *M2MColumn_setUnique (M2MColumn *self, const bool unique);



#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2M_DB_M2MCOLUMN_H_ */
