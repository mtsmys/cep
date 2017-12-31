/*******************************************************************************
 * M2MFile.c
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

#include "m2m/io/M2MFile.h"


/*******************************************************************************
 * Private function
 ******************************************************************************/
/**
 * Open the file in writable type (="a+b").<br>
 *
 * @param filePath	String indicating file pathname to open
 * @return			Opened file object or NULL (in case of error)
 */
static FILE *this_openFileInAppendType (const unsigned char *filePath)
	{
	//========== Variable ==========
	FILE *file = NULL;

	//===== Open the file as writable type (create if it doesn't exist) =====
	if ((file=fopen(filePath, "a+b"))!=NULL)
		{
		return file;
		}
	//===== Error handling =====
	else
		{
		return NULL;
		}
	}


/*******************************************************************************
 * Public function
 ******************************************************************************/
/**
 * Close the file specified by the argument.<br>
 *
 * @param[in,out] file	FILE structure object
 */
void M2MFile_close (FILE *file)
	{
	//===== Check argument =====
	if (file!=NULL)
		{
		fclose(file);
		}
	//===== Argument error =====
	else
		{
		}
	return;
	}


/**
 * Tests whether the file denoted by argument pathname exists.<br>
 *
 * @param[in] filePath	Absolute pathname of the file to check for existence
 * @return				true: if the file denoted by pathname, false: otherwise
 */
bool M2MFile_exists (const unsigned char *filePath)
	{
	//========== Variable ==========
	struct stat fileStatus;

	//===== Check argument =====
	if (filePath!=NULL)
		{
		//===== Confirm existence of specified file =====
		if (stat(filePath, &fileStatus)==0 && S_ISREG(fileStatus.st_mode))
			{
			return true;
			}
		else
			{
			return false;
			}
		}
	//===== Argument error =====
	else
		{
		return false;
		}
	}


/**
 * Get file descriptor (indicating inode) of the argument file.<br>
 *
 * @param[in] file	FILE structure object
 * @return			Integer indicating the inode of the file or -1 (in case of error)
 */
int M2MFile_getFileDescriptor (const FILE *file)
	{
	//===== Check argument =====
	if (file!=NULL)
		{
		return fileno((FILE *)file);
		}
	//===== Argument error =====
	else
		{
		return -1;
		}
	}


/**
 * Return the size of the file[bytes].<br>
 *
 * @param[in] file	FILE structure object
 * @return			Integer indicating file size[byte] or 0 (in case of error)
 */
unsigned long M2MFile_length (const FILE *file)
	{
	//========== Variable ==========
	struct stat fileStatus;

	//===== Check argument =====
	if (file!=NULL)
		{
		//===== Get file status object =====
		if (fstat(M2MFile_getFileDescriptor(file), &fileStatus)==0)
			{
			return fileStatus.st_size;
			}
		//===== Error handling =====
		else
			{
			return 0;
			}
		}
	//===== Argument error =====
	else
		{
		return 0;
		}
	}


/**
 * Open the file with the argument pathname in readable & writable type.<br>
 *
 * @param[in] filePath	Absolute pathname of the file to open
 * @param[in] append	Flag indicating presence or absence of additional mode
 * @return				FILE structure object or NULL (in case of error)
 */
FILE *M2MFile_open (const unsigned char *filePath, const bool append)
	{
	//===== Check argument =====
	if (filePath!=NULL)
		{
		//===== In case of additional writable permission =====
		if (append==true)
			{
			//===== Open the file in additional writing mode =====
			return this_openFileInAppendType(filePath);
			}
		//===== In the case of no additional mode =====
		else
			{
			//===== Confirm existence of existing file =====
			if (M2MFile_exists(filePath)==true)
				{
				//===== Delete the file because prohibited additional writing =====
				M2MFile_remove(filePath);
				}
			//===== When there is no existing file =====
			else
				{
				// do nothing
				}
			//===== Open the file in additional writing mode =====
			return this_openFileInAppendType(filePath);
			}
		}
	//===== Argument error =====
	else
		{
		return NULL;
		}
	}


/**
 * Read data from the file and copy to the buffer.<br>
 *
 * @param[in] file		FILE structure object
 * @param[out] data		Buffer for copying the file data
 * @return				Buffer pointer or NULL (in case of error)
 */
unsigned char *M2MFile_read (const FILE *file, unsigned char **data)
	{
	//========== Variable ==========
	int fileDescriptor = -1;
	ssize_t readLength = 0;
	unsigned long fileLength = 0;
	unsigned int errorCounter = 0;
	unsigned char READ_BUFFER[16384];
	const unsigned long FILE_LENGTH = M2MFile_length(file);
	const unsigned int MAX_ERROR = 5;

	//===== Check argument =====
	if (file!=NULL && FILE_LENGTH>0 && (fileDescriptor=M2MFile_getFileDescriptor(file))>0
			&& data!=NULL)
		{
		//===== Allocate new memory for copying read data =====
		if (((*data)=(unsigned char *)M2MHeap_malloc(FILE_LENGTH+1))!=NULL)
			{
			//===== Initialize buffer =====
			memset(READ_BUFFER, 0, sizeof(READ_BUFFER));
			//===== Loop while arrived at end of file =====
			while (fileLength<FILE_LENGTH)
				{
				//===== Read file =====
				if ((readLength=read(fileDescriptor, READ_BUFFER, sizeof(READ_BUFFER)))>0)
					{
					//===== Check read error =====
					if (ferror((FILE *)file)==0)
						{
						//===== Copy read data into buffer =====
						memcpy(&((*data)[fileLength]), READ_BUFFER, readLength);
						//===== Update length of read data =====
						fileLength += readLength;
						//===== Initialize buffer =====
						memset(READ_BUFFER, 0, sizeof(READ_BUFFER));
						}
					//===== Error handling =====
					else
						{
						clearerr((FILE *)file);
						errorCounter++;
						}
					}
				//===== Error handling =====
				else
					{
					errorCounter++;
					}
				//===== In the case of arrived at EOF =====
				if (feof((FILE *)file)!=0)
					{
					break;
					}
				//===== In the case of continue reading =====
				else if (errorCounter<MAX_ERROR)
					{
					continue;
					}
				//===== In the case of number of error overs limit =====
				else
					{
					//===== Release allocated memory =====
					M2MHeap_free((*data));
					return NULL;
					}
				}
			//===== Return read data =====
			return (*data);
			}
		//===== Error handling =====
		else
			{
			return NULL;
			}
		}
	//===== Argument error =====
	else if (file==NULL)
		{
		return NULL;
		}
	else if (FILE_LENGTH<=0)
		{
		return NULL;
		}
	else if (fileDescriptor<=0)
		{
		return NULL;
		}
	else
		{
		return NULL;
		}

	}


/**
 * Deletes a file of the argument pathname.<br>
 *
 * @param[in] filePath	String indicating the file pathname to be deleted
 */
void M2MFile_remove (const unsigned char *filePath)
	{
	//===== Check argument =====
	if (filePath!=NULL)
		{
		//===== When the file exists =====
		if (M2MFile_exists(filePath)==true)
			{
			//===== Delete the file =====
			unlink(filePath);
			}
		//===== If the file does not exist =====
		else
			{
			// do nothing
			}
		}
	//===== Argument error =====
	else
		{
		}
	return;
	}


/**
 * Output data to the specified file as argument.<br>
 *
 * @param[in] file			FILE structure object
 * @param[in] data			Output data
 * @param[in] dataLength	Size of output data[Byte]
 * @return					true: Success to file output, false: Failed to file output
 */
bool M2MFile_write (const FILE *file, const unsigned char *data, const size_t dataLength)
	{
	//========== Variable ==========
	ssize_t writtenDataLength = 0;
	size_t result = 0;
	unsigned int errorCounter = 0;
	const unsigned int MAX_ERROR = 5;

	//===== Check argument =====
	if (file!=NULL && data!=NULL)
		{
		//===== Repeat until all data is output to file =====
		for (result=0; result<dataLength; result=result+(size_t)writtenDataLength)
			{
			//===== Output to file =====
			if ((writtenDataLength=write(M2MFile_getFileDescriptor(file), data, dataLength-result))>0)
				{
				}
			//===== In the case of failed to file output =====
			else if (writtenDataLength==0)
				{
				//===== Count error number =====
				errorCounter++;
				//===== When the number of errors is within the limit =====
				if (errorCounter<MAX_ERROR)
					{
					// do nothing
					}
				//===== When the number of errors exceeds the limit =====
				else
					{
					return false;
					}
				}
			//===== Error handling =====
			else
				{
				return false;
				}
			}
		//===== Success to file output =====
		return true;
		}
	//===== Argument error =====
	else if (file==NULL)
		{
		return false;
		}
	else
		{
		return false;
		}
	}



/* End Of File */
