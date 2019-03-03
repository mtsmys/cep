/*******************************************************************************
 * M2MDate.h
 *
 * Copyright (c) 2018, Akihisa Yasuda
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

#ifndef M2M_TIME_M2MDATE_H_
#define M2M_TIME_M2MDATE_H_



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



#include "m2m/io/M2MHeap.h"
#include <inttypes.h>
#include <sys/time.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <time.h>



/*******************************************************************************
 * Public function
 ******************************************************************************/
/**
 * This method returns milliseconds number of current time.<br>
 *
 * @return	Milliseconds number of current time or 0 (in case of error)
 */
uint32_t M2MDate_getCurrentTimeMillis ();


/**
 * This method copies local time string into indicated "buffer" memory.<br>
 * Output string format is "yyyy/MM/dd HH:mm:ss.SSS";
 * This method doesn't allocation, so caller needs to prepare memory<br>
 * before call this method.<br>
 *
 * @param[out] buffer		memory buffer for copying local time string
 * @param[in] bufferLength	memory buffer length(max size)
 * @return					length of local time string or 0 (in case of error)
 */
size_t M2MDate_getLocalTimeString (unsigned char *buffer, const size_t bufferLength);



#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2M_TIME_M2MDATE_H_ */
