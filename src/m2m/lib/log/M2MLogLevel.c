/*******************************************************************************
 * M2MLogLevel.c
 *
 * Copyright (c) 2019, Akihisa Yasuda
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

#include "m2m/lib/log/M2MLogLevel.h"


/*******************************************************************************
 * Public method
 ******************************************************************************/
/**
 * @param level
 * @return
 */
M2MString *M2MLogLevel_toString (const M2MLogLevel level)
	{
	if (level==M2MLogLevel_TRACE)
		{
		return (M2MString *)"TRACE";
		}
	else if (level==M2MLogLevel_DEBUG)
		{
		return (M2MString *)"DEBUG";
		}
	else if (level==M2MLogLevel_INFO)
		{
		return (M2MString *)"INFO";
		}
	else if (level==M2MLogLevel_WARN)
		{
		return (M2MString *)"WARN";
		}
	else if (level==M2MLogLevel_INFO)
		{
		return (M2MString *)"INFO";
		}
	else if (level==M2MLogLevel_ERROR)
		{
		return (M2MString *)"ERROR";
		}
	else
		{
		return (M2MString *)"FATAL";
		}
	}


/**
 * @param[in] logLevelString
 * @return
 */
M2MLogLevel M2MLogLevel_translateString (const M2MString *logLevelString)
	{
	//========== Variable ==========
	const size_t UPPER_CASE_VALUE_LENGTH = 16;
	M2MString upperCaseValue[UPPER_CASE_VALUE_LENGTH];
	const M2MLogLevel DEFAULT_LOGGER_LEVEL = M2MLogLevel_INFO;

	//===== Check argument =====
	if (logLevelString!=NULL)
		{
		memset(upperCaseValue, 0, UPPER_CASE_VALUE_LENGTH);
		//===== Change M2MLogLevel into upper case =====
		if (M2MString_toUpperCase(logLevelString, upperCaseValue, UPPER_CASE_VALUE_LENGTH)!=NULL)
			{
			//===== In the case of "TRACE" level =====
			if (M2MString_compareTo((M2MString *)"TRACE", upperCaseValue)==0)
				{
				return M2MLogLevel_TRACE;
				}
			//===== In the case of "DEBUG" level =====
			else if (M2MString_compareTo((M2MString *)"DEBUG", upperCaseValue)==0)
				{
				return M2MLogLevel_DEBUG;
				}
			//===== In the case of "INFO" level =====
			else if (M2MString_compareTo((M2MString *)"INFO", upperCaseValue)==0)
				{
				return M2MLogLevel_INFO;
				}
			//===== In the case of "WARN" level =====
			else if (M2MString_compareTo((M2MString *)"WARN", upperCaseValue)==0)
				{
				return M2MLogLevel_WARN;
				}
			//===== In the case of "ERROR" level =====
			else if (M2MString_compareTo((M2MString *)"ERROR", upperCaseValue)==0)
				{
				return M2MLogLevel_ERROR;
				}
			//===== In the case of "FATAL" level =====
			else if (M2MString_compareTo((M2MString *)"FATAL", upperCaseValue)==0)
				{
				return M2MLogLevel_FATAL;
				}
			//===== Error handling =====
			else
				{
				return DEFAULT_LOGGER_LEVEL;
				}
			}
		//===== Error handling =====
		else
			{
			return DEFAULT_LOGGER_LEVEL;
			}
		}
	//===== Argument error =====
	else
		{
		return DEFAULT_LOGGER_LEVEL;
		}
	}


/* End Of File */
