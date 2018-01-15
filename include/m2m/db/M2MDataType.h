/*******************************************************************************
 * M2MDataType.h
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

#ifndef M2M_DB_M2MDATATYPE_H_
#define M2M_DB_M2MDATATYPE_H_



#include "m2m/lang/M2MString.h"
#include "m2m/log/M2MLogger.h"



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
#ifndef M2MDataType
typedef enum
	{
	M2MDataType_BLOB,
	M2MDataType_BOOL,
	M2MDataType_CHAR,
	M2MDataType_DATETIME,
	M2MDataType_DOUBLE,
	M2MDataType_ERROR,
	M2MDataType_FLOAT,
	M2MDataType_INTEGER,
	M2MDataType_NULL,
	M2MDataType_NUMERIC,
	M2MDataType_REAL,
	M2MDataType_TEXT,
	M2MDataType_VARCHAR
	} M2MDataType;
#endif /* M2MDataType */


/*******************************************************************************
 * Public function
 ******************************************************************************/
/**
 * Convert the argument enumerator to a string.<br>
 *
 * @param[in] self	Enumerator for data type
 * @return			String indicating the name of the enumerator or NULL (In case of error)
 */
M2MString *M2MDataType_toString (const M2MDataType self);




#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2M_DB_M2MDATATYPE_H_ */
