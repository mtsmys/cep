/*******************************************************************************
 * M2MSystem.h
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

#ifndef M2M_M2MSYSTEM_H_
#define M2M_M2MSYSTEM_H_


#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



/*******************************************************************************
 * Definition
 ******************************************************************************/
/**
 * This is a character encoding capable of encoding all possible characters in Unicode.
 */
#ifndef M2MSystem_UTF8
#define M2MSystem_UTF8 (unsigned char *)"UTF-8"
#endif /* M2MSystem_UTF8 */



#ifndef M2MSystem_DIRECTORY
#define M2MSystem_DIRECTORY (unsigned char *)".m2m"
#endif /* M2MSystem_DIRECTORY */




/*******************************************************************************
 * Public function
 ******************************************************************************/
/**
 *
 * @return
 */
int32_t M2MSystem_getThreadID ();


/**
 *
 * @param[in,out] buffer
 * @param[in] bufferLength
 * @return
 */
unsigned char *M2MSystem_getThreadIDString (unsigned char *buffer, const size_t bufferLength);


/**
 * Print out a line to the "standard" output stream.<br>
 *
 * @param[in] data	Output data
 */
void M2MSystem_println (const unsigned char *data);




#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2M_M2MSYSTEM_H_ */
