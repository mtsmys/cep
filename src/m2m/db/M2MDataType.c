/*******************************************************************************
 * M2MDataType.c
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

#include "m2m/db/M2MDataType.h"


/*******************************************************************************
 * Public function
 ******************************************************************************/
/**
 * Convert the argument enumerator to a string.<br>
 *
 * @param[in] self	Enumerator for data type
 * @return			String indicating the name of the enumerator or NULL (In case of error)
 */
M2MString *M2MDataType_toString (const M2MDataType self)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MDataType_toString()";

	if (self==M2M_DATA_TYPE_BLOB)
		{
		return (M2MString *)"BLOB";
		}
	else if (self==M2M_DATA_TYPE_BOOL)
		{
		return (M2MString *)"BOOL";
		}
	else if (self==M2M_DATA_TYPE_CHAR)
		{
		return (M2MString *)"CHAR";
		}
	else if (self==M2M_DATA_TYPE_DATETIME)
		{
		return (M2MString *)"DATETIME";
		}
	else if (self==M2M_DATA_TYPE_DOUBLE)
		{
		return (M2MString *)"DOUBLE";
		}
	else if (self==M2M_DATA_TYPE_ERROR)
		{
		return NULL;
		}
	else if (self==M2M_DATA_TYPE_FLOAT)
		{
		return (M2MString *)"FLOAT";
		}
	else if (self==M2M_DATA_TYPE_INTEGER)
		{
		return (M2MString *)"INTEGER";
		}
	else if (self==M2M_DATA_TYPE_NUMERIC)
		{
		return (M2MString *)"NUMERIC";
		}
	else if (self==M2M_DATA_TYPE_REAL)
		{
		return (M2MString *)"REAL";
		}
	else if (self==M2M_DATA_TYPE_TEXT)
		{
		return (M2MString *)"TEXT";
		}
	else if (self==M2M_DATA_TYPE_VARCHAR)
		{
		return (M2MString *)"VARCHAR";
		}
	//===== Error handling =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MDataType\" value isn't defined", NULL);
		return NULL;
		}
	}




/* End Of File */
