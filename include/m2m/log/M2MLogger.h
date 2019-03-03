/*******************************************************************************
 * M2MLogger.h : Logging library
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

#pragma once

#ifndef M2M_LOG_M2MLogger_H_
#define M2M_LOG_M2MLogger_H_



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



#include "m2m/M2MSystem.h"
#include "m2m/io/M2MHeap.h"
#include "m2m/lang/M2MString.h"
#include "m2m/log/M2MLogLevel.h"
#include "m2m/time/M2MDate.h"
#include <errno.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>



/*******************************************************************************
 * Definition
 ******************************************************************************/
/**
 * String indicating logger name.<br>
 */
#ifndef M2MLogger_DEFAULT_LOGGER_NAME
#define M2MLogger_DEFAULT_LOGGER_NAME (M2MString *)"m2m"
#endif /* M2MLogger_DEFAULT_LOGGER_NAME */


/**
 * Logger structure object.<br>
 *
 * @param loggerName	logger name string
 * @param level			log level
 */
#ifndef M2MLogger
typedef struct
	{
	M2MString *loggerName;
	M2MLogLevel level;
	} M2MLogger;
#endif /* M2MLogger */



/*******************************************************************************
 * public method
 ******************************************************************************/
/**
 * Copy the log message to the argument "buffer" pointer.<br>
 * Buffering of array for copying is executed inside the function.<br>
 * Therefore, it is necessary for caller to call the "M2MHeap_free()" function <br>
 * in order to prevent memory leak after using the variable.<br>
 *
 * @param[in] logLevel			Log level
 * @param[in] functionName		String indicating function name
 * @param[in] lineNumber		Line number in source file (can be embedded with "__LINE__")
 * @param[in] message			Message string
 * @param[out] buffer			Buffer to copy the created log message
 * @return						The pointer of "buffer" copied the created log message string or NULL (in case of error)
 */
M2MString *M2MLogger_createNewLogMessage (const M2MLogLevel logLevel, const M2MString *functionName, const uint32_t lineNumber, const M2MString *message, M2MString **buffer);


/**
 * This method outputs debug level log.<br>
 */
#define M2MLogger_debug( self, functionName, lineNumber, message ) M2MLogger_debugImpl( (void *)(self), (const M2MString *)(functionName), (const unsigned int)(lineNumber), (const M2MString *)(message) )


/**
 *
 * @param[in,out] self
 */
void M2MLogger_delete (M2MLogger **self);


/**
 * This method outputs error level log.<br>
 */
#define M2MLogger_error( self, functionName, lineNumber, message ) M2MLogger_errorImpl( (void *)(self), (const M2MString *)(functionName), (const unsigned int)(lineNumber), (const M2MString *)(message) )


/**
 * This method outputs fatal level log.<br>
 */
#define M2MLogger_fatal( self, functionName, lineNumber, message ) M2MLogger_fatalImpl( (void *)(self), (const M2MString *)(functionName), (const unsigned int)(lineNumber), (const M2MString *)(message) )


/**
 * @param[in] self
 * @return
 */
M2MLogLevel M2MLogger_getLogLevel (const M2MLogger *self);


/**
 * @param[in] self
 * @return
 */
M2MString *M2MLogger_getLoggerName (const M2MLogger *self);


/**
 * This method outputs info level log.<br>
 */
#define M2MLogger_info( self, functionName, lineNumber, message ) M2MLogger_infoImpl( (void *)(self), (const M2MString *)(functionName), (const unsigned int)(lineNumber), (const M2MString *)(message) )


/**
 * Constructor.<br>
 * This method allocates new memory for creating M2MLogger structure object.<br>
 *
 * @return	created new M2MLogger object
 */
M2MLogger *M2MLogger_new ();


/**
 * @param[in,out] self
 * @param[in] level
 * @return
 */
M2MLogger *M2MLogger_setLogLevel (M2MLogger *self, const M2MLogLevel level);


/**
 * @param[in,out] self
 * @param[in] loggerName
 * @return
 */
unsigned char *M2MLogger_setLoggerName (M2MLogger *self, const M2MString *loggerName);


/**
 * This method outputs trace level log.<br>
 */
#define M2MLogger_trace( self, functionName, lineNumber, message ) M2MLogger_traceImpl( (void *)(self), (const M2MString *)(functionName), (const unsigned int)(lineNumber), (const M2MString *)(message) )


/**
 * This method outputs warn level log.<br>
 */
#define M2MLogger_warn( self, functionName, lineNumber, message ) M2MLogger_warnImpl( (void *)(self), (const M2MString *)(functionName), (const unsigned int)(lineNumber), (const M2MString *)(message) )



#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2M_LOG_M2MLOGGER_H_ */
