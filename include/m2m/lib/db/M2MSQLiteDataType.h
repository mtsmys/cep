/*******************************************************************************
 * M2MSQLiteDataType.h
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

#ifndef M2M_LIB_DB_M2MSQLITEDATATYPE_H_
#define M2M_LIB_DB_M2MSQLITEDATATYPE_H_



#include "m2m/lib/lang/M2MString.h"



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



/*******************************************************************************
 * Definition
 ******************************************************************************/
/**
 * Enumerated object indicating the data type of the SQLite3 database<br>
 */
#ifndef M2MSQLiteDataType
typedef enum
	{
	M2MSQLiteDataType_BLOB,
	M2MSQLiteDataType_BOOL,
	M2MSQLiteDataType_CHAR,
	M2MSQLiteDataType_DATETIME,
	M2MSQLiteDataType_DOUBLE,
	M2MSQLiteDataType_ERROR,
	M2MSQLiteDataType_FLOAT,
	M2MSQLiteDataType_INTEGER,
	M2MSQLiteDataType_NULL,
	M2MSQLiteDataType_NUMERIC,
	M2MSQLiteDataType_REAL,
	M2MSQLiteDataType_TEXT,
	M2MSQLiteDataType_VARCHAR
	} M2MSQLiteDataType;
#endif /* M2MSQLiteDataType */


/*******************************************************************************
 * Public function
 ******************************************************************************/
/**
 * Convert the argument enumerator to a string.<br>
 *
 * @param[in] self	Enumerator for data type
 * @return			String indicating the name of the enumerator or NULL (In case of error)
 */
M2MString *M2MSQLiteDataType_toString (const M2MSQLiteDataType self);




#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2M_DB_M2MSQLITEDATATYPE_H_ */
