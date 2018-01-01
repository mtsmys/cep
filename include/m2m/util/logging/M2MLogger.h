/*******************************************************************************
 * M2MLogger.h
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

#ifndef M2M_UTIL_LOGGING_M2MLOGGER_H_
#define M2M_UTIL_LOGGING_M2MLOGGER_H_


#include "m2m/io/M2MFile.h"



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



/*******************************************************************************
 * Definition
 ******************************************************************************/
/**
 * Maximum size of log file(8,388,608[Byte])
 */
#ifndef M2MLogger_MAX_LOG_FILE_LENGTH
#define M2MLogger_MAX_LOG_FILE_LENGTH (unsigned int)8388608
#endif


/*******************************************************************************
 * Public function
 ******************************************************************************/
/**
 * Output debug messages to standard output.<br>
 * Debug message is based on the function name, line number, and message.<br>
 *
 * @param[in] methodName	String indicating function name
 * @param[in] lineNumber	Line number in source file (can be embedded with "__LINE__")
 * @param[in] message		Message string indicating error content
 */
void M2MLogger_printDebugMessage (const M2MString *methodName, const unsigned int lineNumber, const M2MString *message);


/**
 * Output error message to the standard error output.<br>
 * Error message is based on the function name, line number, and message.<br>
 * Also, when a pointer for copying error message is specified as argument, <br>
 * buffering is performed inside this function, and error message is copied.<br>
 *
 * @param[in] methodName					String indicating function name
 * @param[in] lineNumber					Line number in source file (can be embedded with "__LINE__")
 * @param[in] message						Message string indicating error content
 * @param[out] bufferForCopyingErrorMessage	Buffer to copy the error message or NULL (if copying is unnecessary)
 */
void M2MLogger_printErrorMessage (const M2MString *methodName, const unsigned int lineNumber, const M2MString *message, M2MString **bufferForCopyingErrorMessage);



#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2M_UTIL_LOGGING_M2MLOGGER_H_ */
