/*******************************************************************************
 * M2MDirctory.c
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

#include "m2m/lib/io/M2MDirectory.h"


/*******************************************************************************
 * Private function
 ******************************************************************************/
/**
 * This method deletes indicated directory.<br>
 *
 * @param[in] directoryPath	directory pathname string
 * @return					true : deleted the directory, false : failed to delete
 */
static bool this_deleteDirectory (const M2MString *directoryPath)
	{
	//===== Check argument =====
	if (directoryPath!=NULL)
		{
		//===== Delete directory(this operation is valid when directory is "clean") =====
		if (M2MDirectory_isDirectory(directoryPath)==true
				&& rmdir(directoryPath)==0)
			{
			return true;
			}
		//===== Error handling =====
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
 * This method removes a file whose path is indicated by argument.<br>
 *
 * @param[in] filePath	A file pathname string
 * @return				true : succeeded to remove a file, false : failed to remove a file
 */
static bool this_deleteFile (const M2MString *filePath)
	{
	//===== Check argument =====
	if (filePath!=NULL)
		{
		//===== Delete file =====
		if (M2MDirectory_isDirectory(filePath)==false
				&& unlink(filePath)==0)
			{
			return true;
			}
		//===== Error handling =====
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
 * Return the value indicating the owner's access permission from the argument.<br>
 *
 * @param number	An integer indicating permission (= 1 to 7)
 * @return			Number of user permission
 */
static mode_t this_getUserPermission (const unsigned int number)
	{
	switch (number)
		{
		case 1:
			return S_IXUSR;
			break;
		case 2:
			return S_IWUSR;
			break;
		case 3:
			return S_IWUSR | S_IXUSR;
			break;
		case 4:
			return S_IRUSR;
			break;
		case 5:
			return S_IRUSR | S_IXUSR;
			break;
		case 6:
			return S_IRUSR | S_IWUSR;
			break;
		case 7:
			return S_IRUSR | S_IWUSR | S_IXUSR;
			break;
		}
	return 0;
	}


/**
 * Return the value indicating the group's access permission from the argument.<br>
 *
 * @param number	An integer indicating permission (= 1 to 7)
 * @return			Number of group permission
 */
static mode_t this_getGroupPermission (const unsigned int number)
	{
	switch (number)
		{
		case 1:
			return S_IXGRP;
			break;
		case 2:
			return S_IWGRP;
			break;
		case 3:
			return S_IWGRP | S_IXGRP;
			break;
		case 4:
			return S_IRGRP;
			break;
		case 5:
			return S_IRGRP | S_IXGRP;
			break;
		case 6:
			return S_IRGRP | S_IWGRP;
			break;
		case 7:
			return S_IRGRP | S_IWGRP | S_IXGRP;
			break;
		}
	return 0;
	}


/**
 * Return the value indicating the other user's access permission from the argument.<br>
 *
 * @param number	An integer indicating permission (= 1 to 7)
 * @return			Number of other user permission
 */
static mode_t this_getOthersPermission (const unsigned int number)
	{
	switch (number)
		{
		case 1:
			return S_IXOTH;
			break;
		case 2:
			return S_IWOTH;
			break;
		case 3:
			return S_IWOTH | S_IXOTH;
			break;
		case 4:
			return S_IROTH;
			break;
		case 5:
			return S_IROTH | S_IXOTH;
			break;
		case 6:
			return S_IROTH | S_IWOTH;
			break;
		case 7:
			return S_IROTH | S_IWOTH | S_IXOTH;
			break;
		}
	return 0;
	}


/**
 * @param[in] mode
 * @return
 */
static mode_t this_getPermission (const M2MString *mode)
	{
	//========== Variable ==========
	M2MString number[2];
	size_t modeLength = 0;
	unsigned int userPermission = 0;
	unsigned int groupPermission = 0;
	unsigned int othersPermission = 0;

	//===== Check argument =====
	if (mode!=NULL && ((modeLength=M2MString_length(mode))==3 || modeLength==4))
		{
		//=====  =====
		if (modeLength==4)
			{
			memset(number, 0, sizeof(number));
			memcpy(number, &(mode[1]), 1);
			userPermission = atoi(number);
			memset(number, 0, sizeof(number));
			memcpy(number, &(mode[2]), 1);
			groupPermission = atoi(number);
			memset(number, 0, sizeof(number));
			memcpy(number, &(mode[3]), 1);
			othersPermission = atoi(number);
			}
		//=====  =====
		else
			{
			memset(number, 0, sizeof(number));
			memcpy(number, &(mode[0]), 1);
			userPermission = atoi(number);
			memset(number, 0, sizeof(number));
			memcpy(number, &(mode[1]), 1);
			groupPermission = atoi(number);
			memset(number, 0, sizeof(number));
			memcpy(number, &(mode[2]), 1);
			othersPermission = atoi(number);
			}
		return this_getUserPermission(userPermission) | this_getGroupPermission(groupPermission) | this_getOthersPermission(othersPermission);
		}
	//===== Argument error =====
	else
		{
		return S_IRUSR | S_IWUSR | S_IXUSR;
		}
	}


/**
 * @param[in] filePath	A file pathname string
 * @return				true : pathname indicates file, false : pathname doesn't indicate file
 */
static bool this_isFile (const M2MString *filePath)
	{
	//========== Variable ==========
	struct stat fileStatus;

	//===== Check argument =====
	if (filePath!=NULL)
		{
		//===== Check the status of file descriptor =====
		if (stat(filePath, &fileStatus)==0 && S_ISREG(fileStatus.st_mode))
			{
			return true;
			}
		//===== Error handling =====
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



/*******************************************************************************
 * Public function
 ******************************************************************************/
/**
 * This method closes directory.<br>
 *
 * @param[in,out] self
 */
void M2MDirectory_close (DIR *self)
	{
	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Close directory =====
		if (closedir(self)==0)
			{
			}
		//===== Error handling =====
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
 * Deletes indicated directory.<br>
 *
 * @param[in] directoryPath	A pathname string of deleting target directory
 */
void M2MDirectory_delete (const M2MString *directoryPath)
	{
	//========== Variable ==========
	DIR *this = NULL;
	struct dirent *directoryEntry = NULL;
	M2MString *entityName = NULL;
	M2MString PATH[4096];

	//===== Check argument =====
	if (directoryPath!=NULL)
		{
		//===== Open directory with inode =====
		if ((this=M2MDirectory_open(directoryPath))!=NULL)
			{
			//===== Loop while existing entities =====
			while((directoryEntry=readdir(this))!=NULL)
				{
				//===== In the case of system information(="." or "..") =====
				if ((M2MString_compareTo((entityName=directoryEntry->d_name), M2MDirectory_CURRENT)==0)
						|| (M2MString_compareTo(entityName, M2MDirectory_PARENT)==0))
					{
					continue;
					}
				//===== In the case of existing directory or file =====
				else
					{
					//===== Set new path string =====
					memset(PATH, 0, sizeof(PATH));
					//=====  =====
					if (M2MString_format(PATH, sizeof(PATH)-1, "%s%s%s", directoryPath, M2MDirectory_SEPARATOR, entityName)>0)
						{
						//===== In the case of entity is directory =====
						if (M2MDirectory_isDirectory(PATH)==true)
							{
							//===== Delete child directory(not own) =====
							M2MDirectory_delete(PATH);
							}
						//===== In the case of entity is file =====
						else
							{
							//===== Delete file =====
							this_deleteFile(PATH);
							}
						//===== Delete directory own =====
						if (this_deleteDirectory(PATH)==true)
							{
							}
						//===== Error handling =====
						else
							{
							}
						}
					//===== Error handling =====
					else
						{
						}
					}
				}
			//===== Close directory =====
			M2MDirectory_close(this);
			//===== Delete indicated directory =====
			if (this_deleteDirectory(directoryPath)==true)
				{
				return;
				}
			//===== Error handling =====
			else
				{
				return;
				}
			}
		//===== Argument error =====
		else
			{
			return;
			}
		}
	//===== Argument error =====
	else
		{
		return;
		}
	}


/**
 * Tests whether the directory denoted by this abstract pathname exists.<br>
 *
 * @param[in] directoryPath	Directory pathname string
 * @return					true: if the directory exists, false: otherwise
 */
bool M2MDirectory_exists (const M2MString *directoryPath)
	{
	//========== Variable ==========
	struct stat fileStatus;

	//===== Check argument =====
	if (directoryPath!=NULL)
		{
		//===== Check the existence of directory =====
		if (stat(directoryPath, &fileStatus)==0 && S_ISDIR(fileStatus.st_mode))
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
 * @param directoryPath		A pathname string
 * @param canonicalPath		Buffer for copying canonical pathname string
 * @return					Canonical pathname string or NULL(means error)
 */
M2MString *M2MDirectory_getCanonicalPath (const M2MString *directoryPath, M2MString **canonicalPath)
	{
	//========== Variable ==========
	size_t directoryPathLength = 0;

	//===== Check argument =====
	if (directoryPath!=NULL && (directoryPathLength=M2MString_length(directoryPath))>0 && canonicalPath!=NULL)
		{
		//===== Get canonical pathname string =====
		if (((*canonicalPath)=realpath(directoryPath, NULL))!=NULL)
			{
			return (*canonicalPath);
			}
		//===== Error handling =====
		else
			{
			//===== In the case of existing canonical pathname string =====
			if ((*canonicalPath)!=NULL)
				{
				M2MHeap_free((*canonicalPath));
				}
			//=====  =====
			else
				{
				}
			return NULL;
			}
		}
	//===== Argument error =====
	else if (directoryPath==NULL || directoryPathLength<=0)
		{
		return NULL;
		}
	else
		{
		return NULL;
		}
	}


/**
 * This method allocates new memory and set current working directory
 *
 * @param[out] directoryPath	buffer for copying directory pathname string
 */
M2MString *M2MDirectory_getCurrentWorkingDirectoryPath (M2MString **directoryPath)
	{
	//========== Variable ==========
	const size_t DIRECTORY_PATH_LENGTH = 4096;

	//===== Check argument =====
	if (directoryPath!=NULL)
		{
		//===== In the case of buffer isn't NULL =====
		if ((*directoryPath)!=NULL)
			{
			//===== Initialize buffer =====
			M2MHeap_free((*directoryPath));
			}
		else
			{
			}
		//===== Allocate new memory for copying current working directory path =====
		if (((*directoryPath)=M2MHeap_malloc(DIRECTORY_PATH_LENGTH))!=NULL)
			{
			//===== Copy current working directory path string =====
			if (getcwd((*directoryPath), DIRECTORY_PATH_LENGTH)!=NULL)
				{
				return (*directoryPath);
				}
			//===== Error handling =====
			else
				{
				//===== Initialize buffer =====
				M2MHeap_free((*directoryPath));
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
 * Get pathname string indicating the home directory set in environment.<br>
 *
 * @return 	Pathname string indicating home directory or NULL (in case of error)
 */
M2MString *M2MDirectory_getHomeDirectoryPath ()
	{
	//========== Variable ==========
	M2MString *homeDirectoryPath = NULL;

	//===== Get pathname string indicating home directory =====
	if ((homeDirectoryPath=(M2MString *)getenv(M2MDirectory_HOME))!=NULL)
		{
		return homeDirectoryPath;
		}
	//===== Error handling =====
	else
		{
		return NULL;
		}
	}


/**
 * @param directoryPath	A pathname string
 * @return				The name of the directory denoted by this abstract pathname or NULL (in case of error)
 */
M2MString *M2MDirectory_getName (const M2MString *directoryPath)
	{
	//========== Variable ==========
	DIR *self = NULL;
	struct dirent *directoryEntry = NULL;

	//===== Check argument =====
	if (directoryPath!=NULL)
		{
		//===== Open directory =====
		if ((self=M2MDirectory_open(directoryPath))!=NULL
				&& (directoryEntry=readdir(self))!=NULL)
			{
			//===== Close directory =====
			M2MDirectory_close(self);
			//===== Get directory name =====
			return directoryEntry->d_name;
			}
		//===== Error handling =====
		else if (self==NULL)
			{
			return NULL;
			}
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
 * Tests whether the pathname is directory.<br>
 *
 * @param[in] directoryPath	String indicating the directory pathname
 * @return					true: if the pathname is directory, false: otherwise
 */
bool M2MDirectory_isDirectory (const M2MString *directoryPath)
	{
	//========== Variable ==========
	struct stat STATUS;

	//===== Check the existence of directory =====
	if (directoryPath!=NULL && stat(directoryPath, &STATUS)==0 && S_ISDIR(STATUS.st_mode))
		{
		return true;
		}
	//===== Error handling =====
	else
		{
		return false;
		}
	}


/**
 * @param directoryPath	A pathname string
 * @return
 */
unsigned long M2MDirectory_lastModified (const M2MString *directoryPath)
	{
	//========== Variable ==========
	struct stat FILE_STATUS;

	//===== Check argument =====
	if (directoryPath!=NULL)
		{
		//===== Get directory status =====
		if (stat(directoryPath, &FILE_STATUS)==0)
			{
			return FILE_STATUS.st_ctim.tv_nsec;
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
 * @param directoryPath	A pathname string
 */
M2MList *M2MDirectory_list (const M2MString *directoryPath)
	{
	//========== Variable ==========
	DIR *self = NULL;
	struct dirent *directoryEntry = NULL;
	M2MList *entityList = NULL;
	M2MString *entityName = NULL;
	size_t entityNameLength = 0;

	//===== Check argument =====
	if (directoryPath!=NULL)
		{
		//===== Create new List object for storing directory entities =====
		if ((entityList=M2MList_new())!=NULL)
			{
			//===== Open indicated directory =====
			if ((self=M2MDirectory_open(directoryPath))!=NULL)
				{
				//===== Loop while existing directory entities =====
				while((directoryEntry=readdir(self))!=NULL)
					{
					//===== Get entity name string =====
					if ((entityName=(M2MString *)(directoryEntry->d_name))!=NULL
							&& (entityNameLength=M2MString_length(entityName))>0)
						{
						//===== In the case of system information =====
						if ((M2MString_compareTo(entityName, M2MDirectory_CURRENT)==0)
								|| (M2MString_compareTo(entityName, M2MDirectory_PARENT)==0))
							{
							continue;
							}
						//===== In the case of existing directory or file =====
						else
							{
							//===== Add entity name into List object =====
							if (M2MList_add(entityList, entityName, entityNameLength)!=NULL)
								{
								}
							//===== Error handling =====
							else
								{
								}
							}
						}
					//===== Error handling =====
					else
						{
						}
					}
				//===== Close indicated directory =====
				M2MDirectory_close(self);
				return entityList;
				}
			//===== Error handling =====
			else
				{
				M2MList_delete(entityList);
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
 * This method collects child file names from argument.<br>
 *
 * @param[in] directoryPath	A pathname string
 * @return					Entity name list includes only file
 */
M2MList *M2MDirectory_listFiles (const M2MString *directoryPath)
	{
	//========== Variable ==========
	DIR *self = NULL;
	size_t directoryPathLength = 0;
	struct dirent *directoryEntry = NULL;
	M2MList *fileList = NULL;
	M2MString *entityName = NULL;
	size_t entityNameLength = 0;
	M2MString filePath[4096];

	//===== Check argument =====
	if (directoryPath!=NULL)
		{
		//===== Create new list structure =====
		if ((fileList=M2MList_new())!=NULL)
			{
			//===== Open indicated directory =====
			if ((directoryPathLength=M2MString_length(directoryPath))>0
					&& (self=M2MDirectory_open(directoryPath))!=NULL)
				{
				//===== Loop while existing directory entities =====
				while((directoryEntry=readdir(self))!=NULL)
					{
					//===== Get entity name string =====
					if ((entityName=(M2MString *)(directoryEntry->d_name))!=NULL
							&& (entityNameLength=M2MString_length(entityName))>0)
						{
						//===== In the case of system information =====
						if ((M2MString_compareTo(entityName, M2MDirectory_CURRENT)==0)
								|| (M2MString_compareTo(entityName, M2MDirectory_PARENT)==0))
							{
							continue;
							}
						//===== In the case of existing directory or file =====
						else
							{
							//===== Get file path =====
							memset(filePath, 0, sizeof(filePath));
							if (M2MString_compareTo(&(directoryPath[directoryPathLength-1]), M2MDirectory_SEPARATOR)==0)
								{
								M2MString_format(&filePath[0], sizeof(filePath)-1, (M2MString *)"%s%s", directoryPath, entityName);
								}
							else
								{
								M2MString_format(&filePath[0], sizeof(filePath)-1, (M2MString *)"%s/%s", directoryPath, entityName);
								}
							//===== In the case of indicated path is file =====
							if (this_isFile(filePath)==true)
								{
								//===== Add entity name into List object =====
								if (M2MList_add(fileList, entityName, entityNameLength)!=NULL)
									{
									}
								//===== Error handling =====
								else
									{
									}
								}
							//===== In the case of indicated path isn't file =====
							else
								{
								// do nothing
								}
							}
						}
					//===== Error handling =====
					else
						{
						}
					}
				//===== Close indicated directory =====
				M2MDirectory_close(self);
				return fileList;
				}
			//===== Error handling =====
			else
				{
				M2MList_delete(fileList);
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
 * Create new directory for the pathname.<br>
 * If there is no intermediate directory, an error occurs.<br>
 *
 * @param[in] directoryPath	String indicating the directory pathname
 * @param[in] mode			Three-digit integer string indicating the permission mode (example) "533")
 * @return					true: Success to create new directory, false : Failed to create
 */
bool M2MDirectory_mkdir (const M2MString *directoryPath, const M2MString *mode)
	{
	//===== Check argument =====
	if (directoryPath!=NULL)
		{
		//===== Check the existence of directory =====
		if (M2MDirectory_isDirectory(directoryPath)==true)
			{
			return true;
			}
		//===== If not existing, create new directory =====
		else if (mkdir(directoryPath, this_getPermission(mode))==0)
			{
			return true;
			}
		//===== Error handling =====
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
 * Create all directories including intermediate parent directory.<br>
 *
 * @param[in] directoryPath	String indicating the directory pathname
 * @param[in] mode			Three-digit integer string indicating the permission mode (example) "533")
 * @return					true: Success to create new directory, false : Failed to create
 */
bool M2MDirectory_mkdirs (const M2MString *directoryPath, const M2MString *mode)
	{
	//========== Variable ==========
	const M2MString *CANONICAL_PATH = directoryPath;
	const unsigned int CANONICAL_PATH_LENGTH = M2MString_length(CANONICAL_PATH);
	M2MString path[FILENAME_MAX+1];
	M2MString *pointer = M2MString_indexOf(CANONICAL_PATH, M2MDirectory_SEPARATOR);

	//===== Initialize variable =====
	pointer++;
	memset(path, 0, sizeof(path));
	//===== Loop while not reaching the directory =====
	while ((pointer=M2MString_indexOf(pointer, M2MDirectory_SEPARATOR))!=NULL)
		{
		//===== Copy pathname string of parent directory =====
		memset(path, 0, sizeof(path));
		memcpy(path, CANONICAL_PATH, CANONICAL_PATH_LENGTH - M2MString_length(pointer));
		//===== Create new parent directory =====
		if (M2MDirectory_mkdir(path, mode)==true)
			{
			pointer++;
			continue;
			}
		//===== Error handling =====
		else
			{
			return false;
			}
		}
	//===== Create new directory which is the last of pathname =====
	return M2MDirectory_mkdir(CANONICAL_PATH, mode);
	}


/**
 * @param directoryPath	A pathname string
 * @return				Directory structure object
 */
DIR *M2MDirectory_open (const M2MString *directoryPath)
	{
	//========== Variable ==========
	DIR *self = NULL;

	//===== Check argument =====
	if (directoryPath!=NULL)
		{
		//===== Open directory =====
		if ((self=opendir(directoryPath))!=NULL)
			{
			return self;
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



/* End Of File */
