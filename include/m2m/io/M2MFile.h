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

#ifndef M2M_IO_M2MFILE_H_
#define M2M_IO_M2MFILE_H_


#include "m2m/io/M2MDirectory.h"



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



/*******************************************************************************
 * Public function
 ******************************************************************************/
/**
 * Close the file specified by the argument.<br>
 *
 * @param[in,out] file	FILE structure object
 */
void M2MFile_close (FILE *file);


/**
 * Tests whether the file denoted by argument pathname exists.<br>
 *
 * @param[in] filePath	Absolute pathname of the file to check for existence
 * @return				true: if the file denoted by pathname, false: otherwise
 */
bool M2MFile_exists (const unsigned char *filePath);


/**
 * Get file descriptor (indicating inode) of the argument file.<br>
 *
 * @param[in] file	FILE structure object
 * @return			Integer indicating the inode of the file or -1 (in case of error)
 */
int M2MFile_getFileDescriptor (const FILE *file);


/**
 * Return the size of the file[bytes].<br>
 *
 * @param[in] file	FILE structure object
 * @return			Integer indicating file size[byte] or 0 (in case of error)
 */
unsigned long M2MFile_length (const FILE *file);


/**
 * Open the file with the argument pathname in readable & writable type.<br>
 *
 * @param[in] filePath	Absolute pathname of the file to open
 * @param[in] append	Flag indicating presence or absence of additional mode
 * @return				FILE structure object or NULL (in case of error)
 */
FILE *M2MFile_open (const unsigned char *filePath, const bool append);


/**
 * Read data from the file and copy to the buffer.<br>
 *
 * @param[in] file		FILE structure object
 * @param[out] data		Buffer for copying the file data
 * @return				Buffer pointer or NULL (in case of error)
 */
unsigned char *M2MFile_read (const FILE *file, unsigned char **data);


/**
 * Deletes a file of the argument pathname.<br>
 *
 * @param[in] filePath	String indicating the file pathname to be deleted
 */
void M2MFile_remove (const unsigned char *filePath);


/**
 * Output data to the specified file as argument.<br>
 *
 * @param[in] file			FILE structure object
 * @param[in] data			Output data
 * @param[in] dataLength	Size of output data[Byte]
 * @return					true: Success to file output, false: Failed to file output
 */
bool M2MFile_write (const FILE *file, const unsigned char *data, const size_t dataLength);



#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2M_IO_M2MFILE_H_ */
