/*******************************************************************************
 * M2MLogLevel.h
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

#ifndef M2M_LIB_LOG_M2MLOGLEVEL_H_
#define M2M_LIB_LOG_M2MLOGLEVEL_H_



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



#include "m2m/lib/lang/M2MString.h"
#include <string.h>



/*******************************************************************************
 * Definition
 ******************************************************************************/
/**
 * Log level denifition
 */
#ifndef M2MLogLevel
typedef enum
	{
	M2MLogLevel_TRACE,
	M2MLogLevel_DEBUG,
	M2MLogLevel_INFO,
	M2MLogLevel_WARN,
	M2MLogLevel_ERROR,
	M2MLogLevel_FATAL,
	} M2MLogLevel;
#endif /* M2MLogLevel */



/*******************************************************************************
 * Public method
 ******************************************************************************/
/**
 * @param logLevel
 * @return
 */
M2MString *M2MLogLevel_toString (const M2MLogLevel logLevel);


/**
 * @param[in] logLevelString
 * @return
 */
M2MLogLevel M2MLogLevel_translateString (const M2MString *logLevelString);



#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2M_LOG_M2MLOGLEVEL_H_ */
