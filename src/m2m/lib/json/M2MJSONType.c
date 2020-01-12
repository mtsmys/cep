/*******************************************************************************
 * M2MJSONType.c
 *
 * Copyright (c) 2015, Akihisa Yasuda
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

#include "m2m/lib/json/M2MJSONType.h"


/*******************************************************************************
 * Public method
 ******************************************************************************/
/**
 * This method returns definition statement matches with the JSON type.<br>
 *
 * @param self	JSON type
 * @return		JSON type name string
 */
M2MString *M2MJSONType_toString (const M2MJSONType self)
	{
	if (self==M2MJSONType_ARRAY)
		{
		return (M2MString *)"JSON array";
		}
	else if (self==M2MJSONType_BOOLEAN)
		{
		return (M2MString *)"JSON boolean";
		}
	else if (self==M2MJSONType_NULL)
		{
		return (M2MString *)"JSON null";
		}
	else if (self==M2MJSONType_NUMBER)
		{
		return (M2MString *)"JSON number";
		}
	else if (self==M2MJSONType_OBJECT)
		{
		return (M2MString *)"JSON object";
		}
	else if (self==M2MJSONType_STRING)
		{
		return (M2MString *)"JSON string";
		}
	else
		{
		return NULL;
		}
	}



/* End Of File */
