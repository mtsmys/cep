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

#include "m2m/io/M2MDirectory.h"


/*******************************************************************************
 * Private function
 ******************************************************************************/
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



/*******************************************************************************
 * Public function
 ******************************************************************************/
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
 * Get pathname string indicating the home directory set in environment.<br>
 *
 * @return 	Pathname string indicating home directory
 */
M2MString *M2MDirectory_getHomeDirectoryPath ()
	{
	//========== Variable ==========
	M2MString *homeDirectoryPath = NULL;
	const M2MString *HOME = (M2MString *)"HOME";

	//===== Get pathname string indicating home directory =====
	if ((homeDirectoryPath=(M2MString *)getenv(HOME))!=NULL)
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



/* End Of File */
