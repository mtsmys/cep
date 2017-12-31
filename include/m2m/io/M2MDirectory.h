/*******************************************************************************
 * M2MDirectory.h
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

#ifndef M2MDIRECTORY_H_
#define M2MDIRECTORY_H_


#include "m2m/lang/M2MString.h"
#include <dirent.h>
#include <fts.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



/*******************************************************************************
 * Definition
 ******************************************************************************/
/**
 * Directory name related to M2M processing(=".m2m")
 */
#ifndef M2MDIRECTORY
#define M2MDIRECTORY (unsigned char *)".m2m"
#endif /* M2MDIRECTORY */


/**
 * Pathname division character(＝"/")<br>
 */
#ifndef M2MDirectory_SEPARATOR
#define M2MDirectory_SEPARATOR (unsigned char *)"/"
#endif /* M2MDirectory_SEPARATOR */



/*******************************************************************************
 * Public function
 ******************************************************************************/
/**
 * Tests whether the directory denoted by this abstract pathname exists.<br>
 *
 * @param directoryPath	Directory pathname string
 * @return				true: if the directory exists, false: otherwise
 */
bool M2MDirectory_exists (const M2MString *directoryPath);


/**
 * Get pathname string indicating the home directory set in environment.<br>
 *
 * @return 	Pathname string indicating home directory
 */
M2MString *M2MDirectory_getHomeDirectoryPath ();


/**
 * Tests whether the pathname is directory.<br>
 *
 * @param directoryPath	String indicating the directory pathname
 * @return				true: if the pathname is directory, false: otherwise
 */
bool M2MDirectory_isDirectory (const M2MString *directoryPath);


/**
 * Create new directory for the pathname.<br>
 * If there is no intermediate directory, an error occurs.<br>
 *
 * @param[in] directoryPath	String indicating the directory pathname
 * @param[in] mode			Three-digit integer string indicating the permission mode (example) "533")
 * @return					true: Success to create new directory, false : Failed to create
 */
bool M2MDirectory_mkdir (const M2MString *directoryPath, const M2MString *mode);


/**
 * Create all directories including intermediate parent directory.<br>
 *
 * @param[in] directoryPath	String indicating the directory pathname
 * @param[in] mode			Three-digit integer string indicating the permission mode (example) "533")
 * @return					true: Success to create new directory, false : Failed to create
 */
bool M2MDirectory_mkdirs (const M2MString *directoryPath, const M2MString *mode);



#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2MDIRECTORY_H_ */
