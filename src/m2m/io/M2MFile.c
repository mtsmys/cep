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
 * 内部関数
 ******************************************************************************/
/**
 *
 * @param filePath	ファイル設置パスを示す文字列
 * @return			オープンしたファイルの入出力オブジェクト or NULL（エラーの場合）
 */
static FILE *this_openAppendFile (const unsigned char *filePath)
	{
	//========== 変数 ==========
	FILE *file = NULL;

	//===== ファイルを追記型で開く（存在しない場合は新規作成) =====
	if ((file=fopen(filePath, "a+b"))!=NULL)
		{
		return file;
		}
	//===== エラー処理 =====
	else
		{
		return NULL;
		}
	}


/*******************************************************************************
 * 公開関数
 ******************************************************************************/
/**
 * 引数で指定されたファイルを閉じる。<br>
 *
 * @param file	ファイルオブジェクト
 */
void M2MFile_close (FILE *file)
	{
	//===== 引数の確認 =====
	if (file!=NULL)
		{
		fclose(file);
		}
	//===== 引数エラー =====
	else
		{
		}
	return;
	}


/**
 * 引数で指定されたパスのファイルが存在するかどうか確認する。<br>
 *
 * @param filePath	存在を確認するファイルの絶対パス
 * @return			true : ファイルが存在, false : ファイルが存在しない
 */
bool M2MFile_exists (const unsigned char *filePath)
	{
	//========== ローカル変数 ==========
	struct stat fileStatus;

	//===== 引数の確認 =====
	if (filePath!=NULL)
		{
		//===== 指定さたファイルの存在を確認 =====
		if (stat(filePath, &fileStatus)==0 && S_ISREG(fileStatus.st_mode))
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
 * 引数で指定されたファイルのファイルディスクリプタ（iノードを示す整数）を取得する。<br>
 *
 * @param[in] file	ファイルオブジェクト
 * @return			引数で指定されたファイルのiノードを示す整数 or -1（エラーの場合）
 */
int M2MFile_getFileDescriptor (const FILE *file)
	{
	//===== 引数の確認 =====
	if (file!=NULL)
		{
		return fileno((FILE *)file);
		}
	//===== 引数エラー =====
	else
		{
		return -1;
		}
	}


/**
 * 引数で指定されたファイルの大きさ[バイト]を返す。<br>
 *
 * @param file	ファイルオブジェクト
 * @return		ファイルの大きさを示す整数[バイト] or 0(エラーの場合)
 */
unsigned long M2MFile_length (const FILE *file)
	{
	//========== ローカル変数 ==========
	struct stat fileStatus;

	//===== 引数の確認 =====
	if (file!=NULL)
		{
		//===== ファイル状態の取得 =====
		if (fstat(M2MFile_getFileDescriptor(file), &fileStatus)==0)
			{
			return fileStatus.st_size;
			}
		//===== エラー処理 =====
		else
			{
			return 0;
			}
		}
	//===== 引数エラー =====
	else
		{
		return 0;
		}
	}


/**
 * 引数で指定されたパスのファイルを開く。<br>
 *
 * @param filePath	ファイルパスを示す文字列
 * @param append	ファイルを開く際の追記の有無を示すフラグ
 * @return			ファイルオブジェクト or NULL（エラーの場合）
 */
FILE *M2MFile_open (const unsigned char *filePath, const bool append)
	{
	//===== 引数の確認 =====
	if (filePath!=NULL)
		{
		//===== 追記有りの場合 =====
		if (append==true)
			{
			//===== ファイルを追記型で開く =====
			return this_openAppendFile(filePath);
			}
		//===== 追記無しの場合 =====
		else
			{
			//===== 既存のファイルの存在有無を確認 =====
			if (M2MFile_exists(filePath)==true)
				{
				//===== ファイルを削除 =====
				M2MFile_remove(filePath);
				}
			//===== 既存ファイルが存在しない場合 =====
			else
				{
				// 何もしない
				}
			//===== ファイルを追記型で開く =====
			return this_openAppendFile(filePath);
			}
		}
	//===== 引数エラー =====
	else
		{
		return NULL;
		}
	}


/**
 * 指定されたファイルからデータを読み取る。<br>
 *
 * @param[in] file		UNIX/Linuxファイル構造体オブジェクト
 * @param[out] data		ファイル入力用バッファ
 * @return				ファイル入力用バッファのポインタ or NULL(エラーの場合)
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
 * 引数で指定されたパスのファイルを削除する。<br>
 *
 * @param filePath	削除対象のファイルパスを示す文字列
 */
void M2MFile_remove (const unsigned char *filePath)
	{
	//===== 引数の確認 =====
	if (filePath!=NULL)
		{
		//===== ファイルが存在する場合 =====
		if (M2MFile_exists(filePath)==true)
			{
			//===== ファイルを削除 =====
			unlink(filePath);
			}
		//===== ファイルが存在しない場合 =====
		else
			{
			// 何もしない
			}
		}
	//===== 引数エラー =====
	else
		{
		}
	return;
	}


/**
 * 指定されたファイルにデータを出力する。<br>
 *
 * @param[in] file			UNIX/Linuxファイル構造体オブジェクト
 * @param[in] data			ファイル出力データ
 * @param[in] dataLength	ファイル出力データサイズ[Byte]
 * @return					true : ファイル出力に成功, false : ファイル出力に失敗
 */
bool M2MFile_write (const FILE *file, const unsigned char *data, const size_t dataLength)
	{
	//========== ローカル変数 ==========
	ssize_t writtenDataLength = 0;
	size_t result = 0;
	unsigned int errorCounter = 0;
	const unsigned int MAX_ERROR = 5;

	//===== 引数の確認 =====
	if (file!=NULL && data!=NULL)
		{
		//===== 引数で指定されたデータ全てをファイル出力するまで繰り返し =====
		for (result=0; result<dataLength; result=result+(size_t)writtenDataLength)
			{
			//===== ファイル出力 =====
			if ((writtenDataLength=write(M2MFile_getFileDescriptor(file), data, dataLength-result))>0)
				{
				}
			//===== ファイル出力に失敗した場合 =====
			else if (writtenDataLength==0)
				{
				//===== エラー回数をカウント =====
				errorCounter++;
				//===== エラー回数が制限以内の場合 =====
				if (errorCounter<MAX_ERROR)
					{
					// 何もしない
					}
				//===== エラー回数が制限を超過した場合 =====
				else
					{
					return false;
					}
				}
			//===== エラー処理 =====
			else
				{
				return false;
				}
			}
		//===== ファイル出力に成功 =====
		return true;
		}
	//===== 引数エラー =====
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
