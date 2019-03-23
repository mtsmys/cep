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

#include "m2m/lib/io/M2MFile.h"


/*******************************************************************************
 * Declaration of private function
 ******************************************************************************/
/**
 * @param[in,out] self
 * @return
 */
static M2MFile *this_setName (M2MFile *self);


/*******************************************************************************
 * Private function
 ******************************************************************************/
/**
 * This method allocates new memory for creating new File object.<br>
 *
 * @return	created new File structure object
 */
static M2MFile *this_createNewFile ()
	{
	return (M2MFile *)M2MHeap_malloc(sizeof(M2MFile));
	}


/**
 * @param self
 */
static void this_deleteACL (M2MFile *self)
	{
	//========== Variable ==========
	M2MACL *acl = NULL;

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Check existence of canonical path string =====
		if ((acl=M2MFile_getACL(self))!=NULL)
			{
			//===== Release allocated memory =====
			M2MACL_delete(&acl);
			}
		//=====  =====
		else
			{
			}
		}
	//===== Argument error =====
	else
		{
		}
	return;
	}


/**
 * @param self
 */
static void this_deleteCanonicalPath (M2MFile *self)
	{
	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Check existence of canonical path string =====
		if (self->canonicalPath!=NULL)
			{
			//===== Release allocated memory =====
			M2MHeap_free(self->canonicalPath);
			}
		//=====  =====
		else
			{
			}
		}
	//===== Argument error =====
	else
		{
		}
	return;
	}


/**
 * @param[in,out] self	File structure object
 */
static void this_deleteFileName (M2MFile *self)
	{
	if (self!=NULL)
		{
		if (M2MFile_getName(self)!=NULL)
			{
			M2MHeap_free(self->name);
			}
		else
			{
			}
		}
	else
		{
		}
	return;
	}


/**
 * @param[in,out] self	File structure object
 */
static void this_deleteFilePath (M2MFile *self)
	{
	if (self!=NULL)
		{
		if (self->filePath!=NULL)
			{
			M2MHeap_free(self->filePath);
			}
		else
			{
			}
		}
	else
		{
		}
	return;
	}


/**
 * @param[in] self	File structure object
 * @return
 */
static FILE *this_getFile (const M2MFile *self)
	{
	//===== Check argument =====
	if (self!=NULL)
		{
		return self->file;
		}
	//===== Argument error =====
	else
		{
		return NULL;
		}
	}


/**
 * Open the file in writable type (="a+b").<br>
 *
 * @param filePath	String indicating file pathname to open
 * @return			Opened file object or NULL (in case of error)
 */
static FILE *this_openFileInAppendType (const M2MString *filePath)
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


/**
 * @param filePath				file pathname string
 * @param directoryPath			buffer for copying directory pathname string
 * @param directoryPathLength	length of buffer of argument "directoryPath"[Byte]
 * @return
 */
static M2MString *this_pullDirectoryPath (const M2MString *filePath, M2MString *directoryPath, const unsigned int directoryPathLength)
	{
	//========== Variable ==========
	M2MString *fileNameStartPoint = NULL;
	unsigned int fileNameLength = 0;
	size_t filePathLength = 0;

	//===== Check argument =====
	if (filePath!=NULL
			&& (filePathLength=M2MString_length(filePath))>0
			&& directoryPath!=NULL
			&& directoryPathLength>0)
		{
		//===== Get file name length and check buffer size =====
		if ((fileNameStartPoint=M2MString_lastIndexOf(filePath, M2MDirectory_SEPARATOR))!=NULL
				&& (fileNameLength=M2MString_length(fileNameStartPoint))>0
				&& (filePathLength-fileNameLength)<directoryPathLength)
			{
			//===== Copy directory path =====
			memset(directoryPath, 0, directoryPathLength);
			memcpy(directoryPath, filePath, (filePathLength-fileNameLength));
			return directoryPath;
			}
		//===== Error handling =====
		else
			{
			return NULL;
			}
		}
	//===== Argument error =====
	else if (filePath==NULL || filePathLength<=0)
		{
		return NULL;
		}
	else if (directoryPath==NULL)
		{
		return NULL;
		}
	else
		{
		return NULL;
		}
	}


/**
 * @param self
 * @param file
 * @return
 */
static M2MFile *this_setFile (M2MFile *self, FILE *file)
	{
	//===== Check argument =====
	if (self!=NULL && file!=NULL)
		{
		//===== Close opened file =====
		M2MFile_close(self);
		//=====  =====
		self->file = file;
		//=====  =====
		this_setName(self);
		return self;
		}
	//===== Argument error =====
	else
		{
		return NULL;
		}
	}


/**
 * @param[in,out] self
 * @return
 */
static M2MFile *this_setName (M2MFile *self)
	{
	//========== Variable ==========
	M2MString *canonicalPath = NULL;
	M2MString *index = NULL;
	size_t nameLength = 0;

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Detect file name =====
		if ((canonicalPath=M2MFile_getCanonicalPath(self))!=NULL
				&& (index=M2MString_lastIndexOf(canonicalPath, M2MDirectory_SEPARATOR))!=NULL
				&& (index += M2MString_length(M2MDirectory_SEPARATOR))!=NULL
				&& (nameLength=M2MString_length(index))>0)
			{
			//===== Initialize buffer =====
			this_deleteFileName(self);
			//===== Allocate new memory =====
			if ((self->name=(M2MString *)M2MHeap_malloc(nameLength+1))!=NULL)
				{
				memcpy(self->name, index, nameLength);
				return self;
				}
			//===== Error handling =====
			else
				{
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			return NULL;
			}
		}
	//===== Argument error =====
	else
		{
		return NULL;
		}
	}


/**
 * @param[in,out] self
 * @param[in] filePath
 * @return
 */
static M2MFile *this_setFilePath (M2MFile *self, const M2MString *filePath)
	{
	//========== Variable ==========
	size_t filePathLength = 0;

	//===== Check argument =====
	if (self!=NULL
			&& filePath!=NULL
			&& (filePathLength=M2MString_length(filePath))>0)
		{
		//===== Initialization =====
		this_deleteFilePath(self);
		//===== Allocate memory for copying file path string =====
		if ((self->filePath=(M2MString *)M2MHeap_malloc(filePathLength+1))!=NULL)
			{
			//===== Copy file path =====
			memcpy(self->filePath, filePath, filePathLength);
			return self;
			}
		//===== Error handling =====
		else
			{
			return NULL;
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		return NULL;
		}
	else
		{
		return NULL;
		}
	}


/**
 * This method sets a canonical pathname string into File object<br>
 * (In setting, heap memory is allocated for copying).
 *
 * @param[in,out] this
 * @param[in] filePath
 * @return
 */
static M2MFile *this_setCanonicalPath (M2MFile *self, const M2MString *filePath)
	{
	//===== Check argument =====
	if (self!=NULL
			&& filePath!=NULL && M2MString_length(filePath)>0)
		{
		//===== Initialize canonical path =====
		this_deleteCanonicalPath(self);
		//===== Allocate heap memory for copying canonical path string =====
		if ((self->canonicalPath=(M2MString *)realpath(filePath, NULL))!=NULL)
			{
			return self;
			}
		//===== Error handling =====
		else
			{
			if (M2MFile_exists(self)==true)
				{
				return NULL;
				}
			else
				{
				return NULL;
				}
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		return NULL;
		}
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
 * @param[in,out] self	FILE structure object
 */
void M2MFile_close (M2MFile *self)
	{
	//===== Check argument =====
	if (self!=NULL)
		{
		//===== In case of existing FILE pointer =====
		if (self->file!=NULL)
			{
			fclose(self->file);
			self->file = NULL;
			}
		//===== In case of no existing FILE pointer =====
		else
			{
			}
		}
	//===== Argument error =====
	else
		{
		}
	return;
	}


/**
 * @param self			File structure object
 * @param permission	Permission string of created directories and files  ex.) "0755"
 * @return
 */
M2MFile *M2MFile_createNewFile (M2MFile *self, const M2MString *permission)
	{
	//========== Variable ==========
	M2MString directoryPath[4096];

	//===== Check argument =====
	if (self!=NULL && permission!=NULL)
		{
		//===== Check existence of file =====
		if (M2MFile_open(self)!=NULL)
			{
			return self;
			}
		//===== Create new file =====
		else
			{
			//===== Initialization =====
			memset(directoryPath, 0, sizeof(directoryPath));
			//===== Get parent directory pathname string =====
			if (this_pullDirectoryPath(M2MFile_getFilePath(self), directoryPath, sizeof(directoryPath))!=NULL)
				{
				//===== Create parent directory =====
				if (M2MDirectory_mkdirs(directoryPath, permission)==true)
					{
					//===== Create new file =====
					return M2MFile_open(self);
					}
				//===== Error handling =====
				else
					{
					return NULL;
					}
				}
			//===== Error handling =====
			else
				{
				return NULL;
				}
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		return NULL;
		}
	else
		{
		return NULL;
		}
	}


/**
 * Destructor.
 *
 * @param[in,out] self	File structure object
 */
void M2MFile_delete (M2MFile **self)
	{
	//===== Check argument =====
	if (self!=NULL && (*self)!=NULL)
		{
		//===== Delete file permission handler =====
		this_deleteACL((*self));
		//===== Delete file path string =====
		this_deleteFilePath((*self));
		//===== Delete file name string =====
		this_deleteFileName((*self));
		//===== Delete canonical file path string =====
		this_deleteCanonicalPath((*self));
		//===== In the case of opening file =====
		if (M2MFile_isClosed((*self))==false)
			{
			//===== Close file =====
			M2MFile_close((*self));
			}
		else
			{
			}
		//===== Delete File object =====
		M2MHeap_free((*self));
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
 * @param[in] self		File structure object
 * @return				true: if the file denoted by pathname, false: otherwise
 */
bool M2MFile_exists (const M2MFile *self)
	{
	return M2MFile_isFile(self);
	}


/**
 * This method returns ACL object owned by argument.<br>
 *
 * @param[in] self	File structure object
 * @return			ACL object which handles file permission or NULL(means error)
 */
M2MACL *M2MFile_getACL (const M2MFile *self)
	{
	if (self!=NULL)
		{
		return self->acl;
		}
	else
		{
		return NULL;
		}
	}


/**
 * @param[in] self	File structure object
 * @return
 */
M2MString *M2MFile_getCanonicalPath (const M2MFile *self)
	{
	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Check existence of canonical path =====
		if (self->canonicalPath!=NULL)
			{
			return self->canonicalPath;
			}
		//===== Set canonical path into File object =====
		else if (this_setCanonicalPath((M2MFile *)self, M2MFile_getFilePath(self))!=NULL)
			{
			return self->canonicalPath;
			}
		//===== Error handling =====
		else
			{
			return M2MFile_getFilePath(self);
			}
		}
	//===== Argument error =====
	else
		{
		return NULL;
		}
	}


/**
 * Get file descriptor (indicating inode) of the argument file.<br>
 *
 * @param[in] file	FILE structure object
 * @return			Integer indicating the inode of the file or -1 (in case of error)
 */
int M2MFile_getFileDescriptor (const M2MFile *self)
	{
	//========== Variable ==========
	FILE *file = NULL;

	//===== Check argument =====
	if (self!=NULL && (file=this_getFile(self))!=NULL)
		{
		return fileno(file);
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		return -1;
		}
	else
		{
		return -1;
		}
	}


/**
 * @param[in] self	File structure object
 * @return
 */
M2MString *M2MFile_getFilePath (const M2MFile *self)
	{
	//===== Argument error =====
	if (self!=NULL)
		{
		return self->filePath;
		}
	//===== Argument error =====
	else
		{
		return NULL;
		}
	}


/**
 * This method returns file group ID which is used in Linux/UNIX system.<br>
 *
 * @param[in] self	File structure object
 * @return			group ID(defined unsigned short in "sys/types.h") or 0(means error)
 */
gid_t M2MFile_getGroupID (const M2MFile *self)
	{
	//========== Variable ==========
	struct stat fileStatus;

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Get file status =====
		if (M2MFile_exists(self)==true
				&& stat(M2MFile_getCanonicalPath(self), &fileStatus)==0
				&& M2MACL_setGroupID(M2MFile_getACL(self), fileStatus.st_gid)!=NULL)
			{
			return fileStatus.st_gid;
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
 * @param[in] self	File structure object
 * @return			simple file name string
 */
M2MString *M2MFile_getName (const M2MFile *self)
	{
	//===== Check argument =====
	if (self!=NULL)
		{
		return self->name;
		}
	//===== Argument error =====
	else
		{
		return NULL;
		}
	}


/**
 * This method copies parent directory pathname string into pointer.<br>
 * Buffering is existed in this method, so caller not need to allocation<br>
 * and must call M2MHeap_free() for releasing allocated memory.<br>
 *
 * @param[in] self					File structure object
 * @param[out] parentDirectoryPath	pointer for copying parent directory pathname string
 * @return							parent directory pathname string or NULL(means error)
 */
M2MString *M2MFile_getParentDirectoryPath (M2MFile *self, M2MString **parentDirectoryPath)
	{
	//========== Variable ==========
	M2MString *canonicalPath = NULL;
	size_t canonicalPathLength = 0;
	size_t parentDirectoryPathLength = 0;
	M2MString *index = NULL;

	//===== Check argument =====
	if (self!=NULL && parentDirectoryPath!=NULL)
		{
		//===== Detect position of parent directory path =====
		if ((canonicalPath=M2MFile_getCanonicalPath(self))!=NULL
				&& (canonicalPathLength=M2MString_length(canonicalPath))>0
				&& (index=M2MString_lastIndexOf(canonicalPath, M2MDirectory_SEPARATOR))!=NULL
				&& (parentDirectoryPathLength=canonicalPathLength-M2MString_length(index))>0)
			{
			//===== Allocate new memory =====
			if (((*parentDirectoryPath)=(M2MString *)M2MHeap_malloc(parentDirectoryPathLength+1))!=NULL)
				{
				//===== Copy parent directory pathname string =====
				memset((*parentDirectoryPath), 0, parentDirectoryPathLength+1);
				memcpy((*parentDirectoryPath), canonicalPath, parentDirectoryPathLength);
				return (*parentDirectoryPath);
				}
			//===== Error handling =====
			else
				{
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			return NULL;
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		return NULL;
		}
	else
		{
		return NULL;
		}
	}


/**
 * This method returns file owner ID which is used in Linux/UNIX system.<br>
 *
 * @param[in] self	File structure object
 * @return			user ID(defined unsigned short in "sys/types.h") or 0(means error)
 */
uid_t M2MFile_getUserID (const M2MFile *self)
	{
	//========== Variable ==========
	struct stat fileStatus;

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Get file status =====
		if (M2MFile_exists(self)==true
				&& stat(M2MFile_getCanonicalPath(self), &fileStatus)==0
//				&& M2MACL_setOwnerID(M2MFile_getACL(self), fileStatus.st_uid)!=NULL
				)
			{
			return fileStatus.st_uid;
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
 * @param[in] self	Check target file structure object
 * @return			true: the file was closed, false: the file is opening
 */
bool M2MFile_isClosed (const M2MFile *self)
	{
	//===== Check existence of file descriptor =====
	if (self!=NULL && this_getFile(self)!=NULL)
		{
		return false;
		}
	else
		{
		return true;
		}
	}


/**
 * @param[in] self	File structure object
 * @return			true : pathname indicates file, false : pathname doesn't indicate file
 */
bool M2MFile_isFile (const M2MFile *self)
	{
	//========== Variable ==========
	struct stat fileStatus;
	M2MString *filePath = NULL;

	//===== Check argument =====
	if (self!=NULL)
		{
		if ((filePath=M2MFile_getFilePath(self))!=NULL)
			{
			//===== Check the status of file descriptor =====
			if (stat(filePath, &fileStatus)==0 && S_ISREG(fileStatus.st_mode))
				{
				return true;
				}
			else
				{
				return false;
				}
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
 * @param filePath	A pathname string
 */
unsigned long M2MFile_lastModified (const M2MFile *self)
	{
	return M2MDirectory_lastModified(M2MFile_getFilePath(self));
	}


/**
 * Return the size of the file[bytes].<br>
 *
 * @param[in] file	FILE structure object
 * @return			Unsigned integer indicating file size[byte] or 0 (in case of error)
 */
uint32_t M2MFile_length (const M2MFile *self)
	{
	//========== Variable ==========
	int fileDescriptor = 0;
	struct stat fileStatus;

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Get file status =====
		if ((fileDescriptor=M2MFile_getFileDescriptor(self))>0
				&& fstat(fileDescriptor, &fileStatus)==0)
			{
			return fileStatus.st_size;
			}
		//===== Error handling =====
		else if (fileDescriptor<=0)
			{
			return 0;
			}
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
 * Constructor.<br>
 *
 * @param[in] filePath	A pathname string of file
 * @return				created new File structure object
 */
M2MFile *M2MFile_new (const M2MString *filePath)
	{
	//========== Variable ==========
	M2MFile *self = NULL;

	//===== Check argument =====
	if (filePath!=NULL && M2MString_length(filePath)>0)
		{
		//===== Allocate new memory =====
		if ((self=this_createNewFile())!=NULL)
			{
			//===== Set file permission handler =====
			if ((self->acl=M2MACL_new())!=NULL)
				{
				//===== Set file pathname string =====
				if (this_setFilePath(self, filePath)!=NULL)
					{
					return self;
					}
				//===== Error handling =====
				else
					{
					M2MFile_delete(&self);
					return NULL;
					}
				}
			//===== Error handling =====
			else
				{
				M2MFile_delete(&self);
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			return NULL;
			}
		}
	//===== Argument error =====
	else
		{
		return NULL;
		}
	}


/**
 * This method opens file which is indicated path string as argument.<br>
 *
 * @param[in] self	File structure object
 * @return			File structure object which handles opened file or NULL(means error)
 */
M2MFile *M2MFile_open (M2MFile *self)
	{
	//========== Variable ==========
	FILE *file = NULL;
	M2MString *filePath = NULL;

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== In the case of opening the file =====
		if (M2MFile_isClosed(self)==false)
			{
			return self;
			}
		//===== In the case of closing the file =====
		else
			{
			//===== Open file =====
			if ((filePath=M2MFile_getFilePath(self))!=NULL
					&& (file=this_openFileInAppendType(filePath))!=NULL)
				{
				//===== Set opened file into File object =====
				return this_setFile(self, file);
				}
			//===== Error handling =====
			else if (filePath==NULL)
				{
				return NULL;
				}
			else
				{
				return NULL;
				}
			}
		}
	//===== Argument error =====
	else
		{
		return NULL;
		}
	}


/**
 * Read data from the file object and copy to the buffer.<br>
 *
 * @param[in] self		File structure object
 * @param[out] data		Buffer for storing read data
 * @return				Length of red file[Byte]
 */
unsigned long M2MFile_read (const M2MFile *self, unsigned char **data)
	{
	//========== Variable ==========
	int fileDescriptor = -1;
	unsigned long fileSize = 0;
	ssize_t readLength = 0;
	unsigned long fileLength = 0;
	unsigned int errorCounter = 0;
	unsigned char READ_BUFFER[16384];
	const unsigned int MAX_ERROR = 5;

	//===== Check argument =====
	if (self!=NULL && data!=NULL)
		{
		//===== Allocate new memory for copying read data =====
		if ((fileDescriptor=M2MFile_getFileDescriptor(self))>0
				&& (fileSize=M2MFile_length(self))>0
				&& ((*data)=(unsigned char *)M2MHeap_malloc(fileSize+1))!=NULL)
			{
			//===== Initialize buffer =====
			memset(READ_BUFFER, 0, sizeof(READ_BUFFER));
			//===== Loop while arrived at end of file =====
			while (fileLength<fileSize)
				{
				//===== Read file =====
				if ((readLength=read(fileDescriptor, READ_BUFFER, sizeof(READ_BUFFER)))>0)
					{
					//===== Check read error =====
					if (ferror(this_getFile(self))==0)
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
						clearerr(this_getFile(self));
						errorCounter++;
						}
					}
				//===== Error handling =====
				else
					{
					errorCounter++;
					}
				//===== In the case of arrived at EOF =====
				if (feof(this_getFile(self))!=0)
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
					return 0;
					}
				}
			//===== Return read data length =====
			return fileLength;
			}
		//===== Error handling =====
		else if (fileDescriptor<=0)
			{
			return 0;
			}
		else if (fileSize<=0)
			{
			return 0;
			}
		else
			{
			return 0;
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		return 0;
		}
	else
		{
		return 0;
		}
	}


/**
 * Deletes a file of the argument pathname.<br>
 *
 * @param[in] self	File structure object
 */
void M2MFile_remove (const M2MFile *self)
	{
	//========== Variable ==========
	M2MString *canonicalPath = NULL;

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Check existence of file =====
		if (M2MFile_exists(self)==true
				&& (canonicalPath=M2MFile_getCanonicalPath(self))!=NULL)
			{
			//===== Delete file =====
			if (unlink(canonicalPath)==0)
				{
				}
			//===== Error handling =====
			else
				{
				}
			}
		//===== In the case of indicated path isn't file =====
		else if (M2MFile_isFile(self)==false)
			{
			}
		else
			{
			}
		}
	//===== Argument error =====
	else
		{
		}
	return;
	}


/**
 * This method renames existing file.<br>
 *
 * @param[in,out] self		File structure object
 * @param[in] newFileName	new file name(which isn't pathname string)
 * @return					renamed new File structure object or old File structure object(means error)
 */
M2MFile *M2MFile_rename (M2MFile **self, const M2MString *newFileName)
	{
	//========== Variable ==========
	M2MFile *newFile = NULL;
	size_t newNameLength = 0;
	M2MString *parendDirectoryPath = NULL;
	M2MString *newFileNamePath = NULL;

	//===== Check argument =====
	if (self!=NULL && (*self)!=NULL
			&& newFileName!=NULL && (newNameLength=M2MString_length(newFileName))>0)
		{
		//===== Get parent directory pathname string =====
		if (M2MFile_getParentDirectoryPath((*self), &parendDirectoryPath)!=NULL)
			{
			//===== Construct new file pathname string =====
			if (M2MString_append(&newFileNamePath, parendDirectoryPath)!=NULL
					&& M2MString_append(&newFileNamePath, M2MDirectory_SEPARATOR)!=NULL
					&& M2MString_append(&newFileNamePath, newFileName)!=NULL)
				{
				//===== Close file for renaming =====
				M2MFile_close((*self));
				//===== Rename file =====
				if (rename(M2MFile_getCanonicalPath((*self)), newFileNamePath)==0)
					{
					//===== Create new File object =====
					if ((newFile=M2MFile_new(newFileNamePath))!=NULL)
						{
						//===== Delete old File object =====
						M2MFile_delete(self);
						//===== Release allocated memory =====
						M2MHeap_free(parendDirectoryPath);
						M2MHeap_free(newFileNamePath);
						return newFile;
						}
					//===== Error handling =====
					else
						{
						//===== Release allocated memory =====
						M2MHeap_free(parendDirectoryPath);
						M2MHeap_free(newFileNamePath);
						return (*self);
						}
					}
				//===== Error handling =====
				else
					{
					M2MHeap_free(parendDirectoryPath);
					M2MHeap_free(newFileNamePath);
					return (*self);
					}
				}
			//===== Error handling =====
			else
				{
				M2MHeap_free(parendDirectoryPath);
				//=====  =====
				if (newFileNamePath!=NULL)
					{
					M2MHeap_free(newFileNamePath);
					}
				//=====  =====
				else
					{
					}
				return (*self);
				}
			}
		//===== Error handling =====
		else
			{
			return (*self);
			}
		}
	//===== Argument error =====
	else if (self==NULL || (*self)==NULL)
		{
		return NULL;
		}
	else
		{
		return (*self);
		}
	}


/**
 * Output data to the specified file as argument.<br>
 *
 * @param[in] self			FILE structure object
 * @param[in] data			Output data
 * @param[in] dataLength	Size of output data[Byte]
 * @return					true: Success to file output, false: Failed to file output
 */
size_t M2MFile_write (const M2MFile *self, const unsigned char *data, const size_t dataLength)
	{
	//========== Variable ==========
	ssize_t writtenDataLength = 0;
	size_t result = 0;
	unsigned int errorCounter = 0;
	const unsigned int MAX_ERROR = 5;

	//===== Check argument =====
	if (self!=NULL && data!=NULL)
		{
		//===== Check opened file descriptor =====
		if (M2MFile_isClosed(self)==false)
			{
			// do nothing
			}
		//===== In the case of closing file descriptor =====
		else
			{
			//===== Open file descriptor =====
			if (M2MFile_open((M2MFile *)self)!=NULL)
				{
				}
			//===== Error handling =====
			else
				{
				return 0;
				}
			}
		//=====  =====
		for (result=0; result<dataLength; result=result+(size_t)writtenDataLength)
			{
			//===== Write data into file =====
			if ((writtenDataLength=write(M2MFile_getFileDescriptor(self), data, dataLength-result))>0)
				{
				}
			//===== In the case of not written data =====
			else if (writtenDataLength==0)
				{
				//===== Count number of error =====
				errorCounter++;
				//===== Check number of error overing limit =====
				if (errorCounter<MAX_ERROR)
					{
					}
				//===== In the case of number of error over limit =====
				else
					{
					return 0;
					}
				}
			//===== Error handling =====
			else
				{
				return 0;
				}
			}
		//===== Succeed to write =====
		return result;
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		return 0;
		}
	else
		{
		return 0;
		}
	}



/* End Of File */
