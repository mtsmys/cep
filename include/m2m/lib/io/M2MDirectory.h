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

#ifndef M2M_LIB_IO_M2MDIRECTORY_H_
#define M2M_LIB_IO_M2MDIRECTORY_H_



#include "m2m/lib/io/M2MHeap.h"
#include "m2m/lib/lang/M2MString.h"
#include "m2m/lib/security/acl/M2MACLPermission.h"
#include "m2m/lib/util/list/M2MList.h"
#include <dirent.h>
#include <fcntl.h>
#include <fts.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



/*******************************************************************************
 * Definition
 ******************************************************************************/
/**
 * UNIX/Linux system-dependent path-separator character(="/").<br>
 */
#ifndef M2MDirectory_SEPARATOR
#ifdef WIN32 /* for Windows */
#define M2MDirectory_SEPARATOR (M2MString *)"\\"
#else /* for UNIX/Linux */
#define M2MDirectory_SEPARATOR M2MString_SLASH
#endif
#endif /* M2MDirectory_SEPARATOR */


/**
 * Current directory character(=".").<br>
 */
#ifndef M2MDirectory_CURRENT
#define M2MDirectory_CURRENT M2MString_DOT
#endif /* M2MDirectory_CURRENT */


/**
 * Environment variable indicated user home directory
 */
#ifndef M2MDirectory_HOME
#define M2MDirectory_HOME (M2MString *)"HOME"
#endif /* M2MDirectory_HOME */


/**
 * Parent directory character(="..").<br>
 */
#ifndef M2MDirectory_PARENT
#define M2MDirectory_PARENT M2MString_TWO_DOT_LEADER
#endif /* M2MDirectory_PARENT */



/*******************************************************************************
 * Public function
 ******************************************************************************/
/**
 * This method closes directory.<br>
 *
 * @param[in,out] self
 */
void M2MDirectory_close (DIR *self);


/**
 * Deletes indicated directory.<br>
 *
 * @param[in] directoryPath	A pathname string of deleting target directory
 */
void M2MDirectory_delete (const M2MString *directoryPath);


/**
 * Tests whether the directory denoted by this abstract pathname exists.<br>
 *
 * @param[in] directoryPath	Directory pathname string
 * @return					true: if the directory exists, false: otherwise
 */
bool M2MDirectory_exists (const M2MString *directoryPath);


/**
 * @param directoryPath		A pathname string
 * @param canonicalPath		Buffer for copying canonical pathname string
 * @return					Canonical pathname string or NULL(means error)
 */
M2MString *M2MDirectory_getCanonicalPath (const M2MString *directoryPath, M2MString **canonicalPath);


/**
 * This method allocates new memory and set current working directory
 *
 * @param[out] directoryPath	buffer for copying directory pathname string
 */
M2MString *M2MDirectory_getCurrentWorkingDirectoryPath (M2MString **directoryPath);


/**
 * Get pathname string indicating the home directory set in environment.<br>
 *
 * @return 	Pathname string indicating home directory
 */
M2MString *M2MDirectory_getHomeDirectoryPath ();


/**
 * @param directoryPath	A pathname string
 * @return				The name of the directory denoted by this abstract pathname or NULL (in case of error)
 */
M2MString *M2MDirectory_getName (const M2MString *directoryPath);


/**
 * Tests whether the pathname is directory.<br>
 *
 * @param[in] directoryPath	String indicating the directory pathname
 * @return					true: if the pathname is directory, false: otherwise
 */
bool M2MDirectory_isDirectory (const M2MString *directoryPath);


/**
 * @param directoryPath	A pathname string
 * @return
 */
unsigned long M2MDirectory_lastModified (const M2MString *directoryPath);


/**
 * @param directoryPath	A pathname string
 */
M2MList *M2MDirectory_list (const M2MString *directoryPath);


/**
 * This method collects child file names from argument.<br>
 *
 * @param[in] directoryPath	A pathname string
 * @return					Entity name list includes only file
 */
M2MList *M2MDirectory_listFiles (const M2MString *directoryPath);


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


/**
 * @param directoryPath	A pathname string
 * @return				Directory structure object
 */
DIR *M2MDirectory_open (const M2MString *directoryPath);



#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2M_IO_M2MDIRECTORY_H_ */
