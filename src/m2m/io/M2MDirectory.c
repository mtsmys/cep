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
 * 内部関数
 ******************************************************************************/
/**
 * 引数から所有者のアクセス権を示す値を算出して返す．<br>
 *
 * @param number	パーミッションを示す整数(=1〜7)
 * @return			規程の当該ユーザーパーミッション数
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
 * 引数からグループのアクセス権を示す値を算出して返す．<br>
 *
 * @param number	パーミッションを示す整数(=1〜7)
 * @return			規程の当該グループパーミッション数
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
 * 引数から所有者以外のユーザーのアクセス権を示す値を算出して返す．<br>
 *
 * @param number	パーミッションを示す整数(=1〜7)
 * @return			規程の当該他ユーザーパーミッション数
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
	//========== ローカル変数 ==========
	M2MString number[2];
	size_t modeLength = 0;
	unsigned int userPermission = 0;
	unsigned int groupPermission = 0;
	unsigned int othersPermission = 0;

	//===== 引数の確認 =====
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
	//===== 引数エラー =====
	else
		{
		return S_IRUSR | S_IWUSR | S_IXUSR;
		}
	}



/*******************************************************************************
 * 公開関数
 ******************************************************************************/
/**
 * 引数で指定されたパスのディレクトリが存在するかどうか確認する。<br>
 *
 * @param directoryPath	存在を確認するディレクトリの絶対パス
 * @return				true : ディレクトリが存在, false : ディレクトリが存在しない
 */
bool M2MDirectory_exists (const M2MString *directoryPath)
	{
	//========== ローカル変数 ==========
	struct stat fileStatus;

	//===== 引数の確認 =====
	if (directoryPath!=NULL)
		{
		//===== ディレクトリの所在を確認 =====
		if (stat(directoryPath, &fileStatus)==0 && S_ISDIR(fileStatus.st_mode))
			{
			return true;
			}
		else
			{
			return false;
			}
		}
	//===== 引数エラー =====
	else
		{
		return false;
		}
	}


/**
 * 環境変数で設定されているホームディレクトリを示すパス文字列を返す。<br>
 *
 * @return ホームディレクトリを示すパス文字列
 */
M2MString *M2MDirectory_getHomeDirectoryPath ()
	{
	//========== ローカル変数 ==========
	M2MString *homeDirectoryPath = NULL;
	const M2MString *HOME = (M2MString *)"HOME";

	//===== ホームディレクトリを示すパス文字列の取得 =====
	if ((homeDirectoryPath=(M2MString *)getenv(HOME))!=NULL)
		{
		return homeDirectoryPath;
		}
	//===== エラー処理 =====
	else
		{
		return NULL;
		}
	}


/**
 * 引数で指定されたパスがディレクトリであるかどうかを確認する。<br>
 *
 * @param directoryPath	ディレクトリパスを示す文字列
 * @return				true : ディレクトリの場合, false : ディレクトリでない場合
 */
bool M2MDirectory_isDirectory (const M2MString *directoryPath)
	{
	//========== ローカル変数 ==========
	struct stat STATUS;

	//===== ディレクトリの存在を確認 =====
	if (directoryPath!=NULL && stat(directoryPath, &STATUS)==0 && S_ISDIR(STATUS.st_mode))
		{
		return true;
		}
	//===== ディレクトリが存在しない場合 =====
	else
		{
		return false;
		}
	}


/**
 * 引数で指定されたパスに対し、ディレクトリを作成する。<br>
 * もし、指定パスに対し、途中のディレクトリが存在しない場合はエラーとなる。<br>
 *
 * @param[in] directoryPath	ディレクトリパスを示す文字列
 * @param[in] mode			パーミッションモードを示す3桁の整数文字列（例）"533"）
 * @return					true : ディレクトリの作成に成功, false : ディレクトリの作成に失敗
 */
bool M2MDirectory_mkdir (const M2MString *directoryPath, const M2MString *mode)
	{
	//===== 引数の確認 =====
	if (directoryPath!=NULL)
		{
		//===== ディレクトリの存在を確認 =====
		if (M2MDirectory_isDirectory(directoryPath)==true)
			{
			return true;
			}
		//===== ディレクトリが存在しない場合は新規作成 =====
		else if (mkdir(directoryPath, this_getPermission(mode))==0)
			{
			return true;
			}
		//===== エラー処理 =====
		else
			{
			return false;
			}
		}
	//===== 引数エラー =====
	else
		{
		return false;
		}
	}


/**
 * 引数で指定されたパスに対し、途中の親ディレクトリを含め、全てのディレクトリを作成<br>
 * する。<br>
 *
 * @param[in] directoryPath	ディレクトリパスを示す文字列
 * @param[in] mode			パーミッションモードを示す3桁の整数文字列（例）"533"）
 * @return					true : ディレクトリの作成に成功, false : ディレクトリの作成に失敗
 */
bool M2MDirectory_mkdirs (const M2MString *directoryPath, const M2MString *mode)
	{
	//========== ローカル変数 ==========
	const M2MString *CANONICAL_PATH = directoryPath;
	const unsigned int CANONICAL_PATH_LENGTH = M2MString_length(CANONICAL_PATH);
	M2MString path[FILENAME_MAX+1];
	M2MString *pointer = M2MString_indexOf(CANONICAL_PATH, M2MDirectory_SEPARATOR);

	//===== 初期化 =====
	pointer++;
	memset(path, 0, sizeof(path));
	//===== 指定ディレクトリに辿り着くまで繰り返し =====
	while ((pointer=M2MString_indexOf(pointer, M2MDirectory_SEPARATOR))!=NULL)
		{
		//===== 親ディレクトリを示す文字列をコピー =====
		memset(path, 0, sizeof(path));
		memcpy(path, CANONICAL_PATH, CANONICAL_PATH_LENGTH - M2MString_length(pointer));
		//===== 親ディレクトリの作成 =====
		if (M2MDirectory_mkdir(path, mode)==true)
			{
			pointer++;
			continue;
			}
		//===== エラー処理 =====
		else
			{
			return false;
			}
		}
	//===== 指定ディレクトリの作成 =====
	return M2MDirectory_mkdir(CANONICAL_PATH, mode);
	}



/* End Of File */
