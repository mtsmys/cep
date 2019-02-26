/*******************************************************************************
 * M2MFileAppender.h
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

#ifndef M2M_LOG_M2MFILEAPPENDER_H_
#define M2M_LOG_M2MFILEAPPENDER_H_



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



#include "m2m/M2MSystem.h"
#include "m2m/io/M2MDirectory.h"
#include "m2m/io/M2MFile.h"
#include "m2m/io/M2MHeap.h"
#include "m2m/lang/M2MString.h"
#include "m2m/log/M2MLogger.h"
#include "m2m/log/M2MLogLevel.h"
#include "m2m/util/list/M2MList.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*******************************************************************************
 * Definition
 ******************************************************************************/
/**
 * FileAppender object
 *
 * @param super
 * @param append
 * @param encoding
 * @param logFile
 * @param logFilePath
 * @param maxBackupIndex
 * @param maxFileSize
 */
#ifndef M2MFileAppender
typedef struct
	{
	M2MLogger *super;
	bool append;
	M2MString *encoding;
	M2MFile *logFile;
	M2MString *logFilePath;
	uint32_t maxBackupIndex;
	uint32_t maxFileSize;
	} M2MFileAppender;
#endif /* M2MFileAppender */



/*******************************************************************************
 * Public method
 ******************************************************************************/
/**
 * This method implements debug level logging procedure.<br>
 *
 * @param self FileAppender object
 */
void M2MLogger_debugImpl (M2MFileAppender *self, const M2MString *methodName, const unsigned int lineNumber, const M2MString *message);


/**
 * This method implements error level logging procedure.<br>
 *
 * @param self FileAppender object
 */
void M2MLogger_errorImpl (M2MFileAppender *self, const M2MString *methodName, const unsigned int lineNumber, const M2MString *message);


/**
 * This method implements fatal level logging procedure.<br>
 *
 * @param self FileAppender object
 */
void M2MLogger_fatalImpl (M2MFileAppender *self, const M2MString *methodName, const unsigned int lineNumber, const M2MString *message);


/**
 * This method implements info level logging procedure.<br>
 *
 * @param self FileAppender object
 */
void M2MLogger_infoImpl (M2MFileAppender *self, const M2MString *methodName, const unsigned int lineNumber, const M2MString *message);


/**
 * This method implements trace level logging procedure.<br>
 *
 * @param self FileAppender object
 */
void M2MLogger_traceImpl (M2MFileAppender *self, const M2MString *methodName, const unsigned int lineNumber, const M2MString *message);


/**
 * This method implements warn level logging procedure.<br>
 *
 * @param self FileAppender object
 */
void M2MLogger_warnImpl (M2MFileAppender *self, const M2MString *methodName, const unsigned int lineNumber, const M2MString *message);


/**
 * This method releases allocated memory for FileAppender structure object.<br>
 *
 * @param[in,out] self	logger object
 */
void M2MFileAppender_delete (M2MFileAppender **self);


/**
 * Create new default log file pathname string and copy it into the argument buffer.<br>
 *
 * @param[out] buffer		Buffer for copying default log file pathname string
 * @param[in] bufferLength	Length of prepared buffer
 * @return					The pointer of buffer copied log file pathname string or NULL (in case of error)
 */
M2MString *M2MFileAppender_getDefaultLogFilePath (M2MString *buffer, const size_t bufferLength);


/**
 * This method copies log file pathname string into indicated buffer.<br>
 * The pathname independent with backbeat library, so if caller won't<br>
 * use the pathname, it isn't suitable for calling this method.<br>
 * The pathname is follow.<br>
 * <br>
 * ~/.backbeat/log/(loggerName).log<br>
 *
 * @param[in] loggerName	logger name string
 * @param[out] buffer		buffer for copying log file pathname string
 * @param[in] bufferLength	lengh of buffer[Byte]
 * @return					log file pathname string or NULL(means error)
 */
M2MString *M2MFileAppender_getLogFilePath (const M2MString *loggerName, M2MString *buffer, const size_t bufferLength);


/**
 * @param[in] self
 * @return
 */
M2MLogLevel M2MFileAppender_getLogLevel (const M2MFileAppender *self);


/**
 * @param[in] self
 * @return
 */
M2MString *M2MFileAppender_getLoggerName (const M2MFileAppender *self);


/**
 * This method creates new FileAppender object which must set-up configurations.<br>
 *
 * @return 	new logger object(caller must release this memory with "Heap_free()") or NULL(means error)
 */
M2MFileAppender *M2MFileAppender_new ();


/**
 * This method parses configuration file and creates new FileAppender object.<br>
 * The configuration file must be written in JSON format and caller must<br>
 * release the memory of created FileAppender object with "FileAppender_delete()".<br>
 *
 * @param[in] jsonFilePath	configuration file path
 * @return					logger object or NULL(error happened)
 *
M2MFileAppender *M2MFileAppender_parseJSONFile (const M2MString *jsonFilePath);
*/

/**
 * This method parses JSON format string for constructing FileAppendar object.<br>
 *
 * @param[in] jsonString	JSON format configuration string
 * @return					new created logging object or NULL(means error)
 *
M2MFileAppender *M2MFileAppender_parseJSONString (const M2MString *jsonString);
*/

/**
 * @param[in] self
 * @param[in] flag
 */
M2MFileAppender *M2MFileAppender_setAppend (M2MFileAppender *self, const bool flag);


/**
 * @param[in] self
 * @param[in] encoding
 */
unsigned char *M2MFileAppender_setEncoding (M2MFileAppender *self, const M2MString *encoding);


/**
 * @param[in] self
 * @param[in] level
 */
M2MFileAppender *M2MFileAppender_setLogLevel (M2MFileAppender *self, const M2MLogLevel level);


/**
 * @param[in] self
 * @param[in] filePath
 */
unsigned char *M2MFileAppender_setLogFilePath (M2MFileAppender *self, const M2MString *filePath);


/**
 * @param[in] self
 * @param[in] loggerName
 */
unsigned char *M2MFileAppender_setLoggerName (const M2MFileAppender *self, const M2MString *loggerName);


/**
 * @param[in] self
 * @param[in] maxBackupIndex
 */
M2MFileAppender *M2MFileAppender_setMaxBackupIndex (M2MFileAppender *self, const unsigned int maxBackupIndex);


/**
 * @param[in] self
 * @param[in] maxFileSize
 */
M2MFileAppender *M2MFileAppender_setMaxFileSize (M2MFileAppender *self, unsigned int maxFileSize);



#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2M_LOG_M2MFILEAPPENDER_H_ */
