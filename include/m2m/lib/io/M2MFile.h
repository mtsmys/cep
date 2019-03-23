/*******************************************************************************
 * M2MFile.h
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

#ifndef M2M_LIB_IO_M2MFILE_H_
#define M2M_LIB_IO_M2MFILE_H_


#include "m2m/lib/io/M2MDirectory.h"
#include "m2m/lib/io/M2MHeap.h"
#include "m2m/lib/lang/M2MString.h"
#include "m2m/lib/security/acl/M2MACL.h"
#include <fcntl.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
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
 * File structure object
 *
 * @param acl			permission handler
 * @param canonicalPath	canonical path string
 * @param file			Linux/UNIX system file object
 * @param filePath		file path string
 * @param name			simple file name string
 */
#ifndef M2MFile
typedef struct
	{
	M2MACL *acl;
	M2MString *canonicalPath;
	FILE *file;
	M2MString *filePath;
	M2MString *name;
	} M2MFile;
#endif /* M2MFile */



/*******************************************************************************
 * Public function
 ******************************************************************************/
/**
 * Close the file specified by the argument.<br>
 *
 * @param[in,out] self	FILE structure object
 */
void M2MFile_close (M2MFile *self);


/**
 * @param[in] self			File structure object
 * @param[in] permission
 * @return
 */
M2MFile *M2MFile_createNewFile (M2MFile *self, const M2MString *permission);


/**
 * Destructor.
 *
 * @param[in,out] self	File structure object
 */
void M2MFile_delete (M2MFile **self);


/**
 * Tests whether the file denoted by argument pathname exists.<br>
 *
 * @param[in] self		File structure object
 * @return				true: if the file denoted by pathname, false: otherwise
 */
bool M2MFile_exists (const M2MFile *self);


/**
 * This method returns ACL object owned by argument.<br>
 *
 * @param[in] self	File structure object
 * @return			ACL object which handles file permission or NULL(means error)
 */
M2MACL *M2MFile_getACL (const M2MFile *self);


/**
 * @param[in] self	File structure object
 * @return			canonical pathname string
 */
M2MString *M2MFile_getCanonicalPath (const M2MFile *self);


/**
 * Get file descriptor (indicating inode) of the argument file.<br>
 *
 * @param[in] self	FILE structure object
 * @return			Integer indicating the inode of the file or -1 (in case of error)
 */
int M2MFile_getFileDescriptor (const M2MFile *self);


/**
 * @param[in] self	File structure object
 * @return			pathname string
 */
M2MString *M2MFile_getFilePath (const M2MFile *self);


/**
 * This method returns file group ID which is used in Linux/UNIX system.<br>
 *
 * @param[in] self	File structure object
 * @return			group ID(defined unsigned short in "sys/types.h") or 0(means error)
 */
gid_t M2MFile_getGroupID (const M2MFile *self);


/**
 * @param[in] self	File structure object
 * @return			simple file name string
 */
M2MString *M2MFile_getName (const M2MFile *self);


/**
 * This method copies parent directory pathname string into pointer.<br>
 * Buffering is existed in this method, so caller not need to allocation<br>
 * and must call M2MHeap_free() for releasing allocated memory.<br>
 *
 * @param[in] self					File structure object
 * @param[out] parentDirectoryPath	pointer for copying parent directory pathname string
 * @return							parent directory pathname string or NULL (in case of error)
 */
M2MString *M2MFile_getParentDirectoryPath (M2MFile *self, M2MString **parentDirectoryPath);


/**
 * This method returns file owner ID which is used in Linux/UNIX system.<br>
 *
 * @param[in] self	File structure object
 * @return			user ID(defined unsigned short in "sys/types.h") or 0(means error)
 */
uid_t M2MFile_getUserID (const M2MFile *self);


/**
 * @param[in] self	File structure object
 * @return			owner name string
 */
M2MString *M2MFile_getUserName (const M2MFile *self);


/**
 * @param[in] self	File structure object
 * @return			true: the file was closed, false: the file is opening
 */
bool M2MFile_isClosed (const M2MFile *self);


/**
 * @param[in] self	File structure object
 * @return			true : pathname indicates file, false : pathname doesn't indicate file
 */
bool M2MFile_isFile (const M2MFile *self);


/**
 * @param[in] self	File structure object
 * @return
 */
unsigned long M2MFile_lastModified (const M2MFile *self);


/**
 * Return the size of the file[bytes].<br>
 *
 * @param[in] file	FILE structure object
 * @return			Integer indicating file size[byte] or 0 (in case of error)
 */
uint32_t M2MFile_length (const M2MFile *self);


/**
 * Constructor.<br>
 *
 * @param[in] filePath	A pathname string of file
 * @return				created new File structure object
 */
M2MFile *M2MFile_new (const M2MString *filePath);


/**
 * This method opens file which is indicated path string as argument.<br>
 *
 * @param[in] self		File structure object
 * @return				File structure object which handles opened file or NULL(means error)
 */
M2MFile *M2MFile_open (M2MFile *self);


/**
 * This method reads data from indicated file as argument.<br>
 *
 * @param[in] self		File structure object
 * @param[out] data		Buffer for storing read data (allocation process is executed in this function)
 * @return				Length of read file[Byte]
 */
unsigned long M2MFile_read (const M2MFile *self, unsigned char **data);


/**
 * Deletes a file of the argument pathname.<br>
 *
 * @param[in] self	File structure object
 */
void M2MFile_remove (const M2MFile *self);


/**
 * This method renames existing file.<br>
 *
 * @param[in,out] self		File structure object
 * @param[in] newFileName	new file name(which isn't pathname string)
 * @return					renamed new File structure object or old File structure object(means error)
 */
M2MFile *M2MFile_rename (M2MFile **self, const M2MString *newFileName);


/**
 * Output data to the specified file as argument.<br>
 *
 * @param[in] self			File structure object
 * @param[in] data			Data for writing into file
 * @param[in] dataLength	Length of write data[Byte]
 * @return					Written data length[Byte]
 */
size_t M2MFile_write (const M2MFile *self, const unsigned char *data, const size_t dataLength);



#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2M_IO_M2MFILE_H_ */
