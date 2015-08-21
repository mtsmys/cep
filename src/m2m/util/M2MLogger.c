/*******************************************************************************
 * M2MLogger.c : Logging library
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

#include "m2m/util/M2MLogger.h"


/*******************************************************************************
 * 関数定義
 ******************************************************************************/
/**
 * errnoを初期化（＝0)する.<br>
 */
static void this_initErrorNumber ();


/**
 * 引数で指定されたファイルのサイズが既定値を超過しているかどうか確認する．<br>
 *
 * @param[in] file	サイズ確認を行う対象のファイルオブジェクト
 * @return			true : 既定値[Byte]を超過，false : 既定値[Byte]以内
 */
static bool this_overMaxLogFileLength (const FILE *file);


/**
 * エラーメッセージをファイルか標準エラー出力に出力する。<br>
 *
 * @param[in] methodName		関数名を示す文字列
 * @param[in] lineNumber		ソースファイル中の行番号（"__LINE__"で埋め込み可能）
 * @param[in] message			エラー内容を示すメッセージ文字列
 */
static void this_printErrorMessage (const M2MString *methodName, const unsigned int lineNumber, const M2MString *message);



/*******************************************************************************
 * 内部関数
 ******************************************************************************/
/**
 * 引数で指定されたポインタにデバッグメッセージをコピーする。<br>
 * コピー用配列のバッファリングは関数内部で実行する。そのため、呼び出し側では当該<br>
 * 変数使用後、メモリーリークを防止するため必ず"M2MHeap_free()"関数を呼ぶ必要が<br>
 * ある。
 *
 * @param[in] methodName		関数名を示す文字列
 * @param[in] lineNumber		ソースファイル中の行番号（"__LINE__"で埋め込み可能）
 * @param[in] message			デバッグ内容を示すメッセージ文字列
 * @param[out] debugMessage		デバッグメッセージをコピーするためのポインタ（バッファリングは関数内部で実行する）
 * @return						引数のポインタにコピーしたエラーメッセージ or NULL（エラーの場合）
 */
#ifdef DEBUG
static M2MString *this_createNewDebugMessage (const M2MString *methodName, const unsigned int lineNumber, const M2MString *message, M2MString **debugMessage)
	{
	//========== ローカル変数 ==========
	M2MString time[128];
	M2MString lineNumberString[16];
	size_t errorMessageLength = 0;
	size_t timeLength = 0;
	size_t methodNameLength = 0;
	size_t lineNumberLength = 0;
	size_t messageLength = 0;

	//===== 引数の確認 =====
	if (methodName!=NULL && (methodNameLength=M2MString_length(methodName))>0
			&& (messageLength=M2MString_length(message))>0
			&& debugMessage!=NULL)
		{
		//===== 配列の初期化 =====
		memset(time, 0, sizeof(time));
		memset(lineNumberString, 0, sizeof(lineNumberString));
		//===== 行番号文字列数の取得 =====
		snprintf(lineNumberString, sizeof(lineNumberString)-1, (M2MString *)"%d", lineNumber);
		lineNumberLength = M2MString_length(lineNumberString);
		//===== ローカルカレンダから現在時刻文字列の取得 ======
		if (M2MString_getLocalTime(time, sizeof(time))>0
				&& (timeLength=M2MString_length(time))>0)
			{
			//===== ヒープメモリ領域の獲得 ======
			if ((errorMessageLength=M2MString_length((M2MString *)"[DEBUG]%s %s:%d[l]: %s\n")+timeLength+methodNameLength+lineNumberLength+messageLength)>0
					&& ((*debugMessage)=(M2MString *)M2MHeap_malloc(errorMessageLength+1))!=NULL)
				{
				snprintf((*debugMessage), errorMessageLength, (M2MString *)"[DEBUG]%s %s:%d[l]: %s\n", time, methodName, lineNumber, message);
				return (*debugMessage);
				}
			//===== エラー処理 ======
			else
				{
				M2MLogger_printErrorMessage((M2MString *)"M2MLogger.this_createNewDebugMessage()", __LINE__, (M2MString *)"デバッグメッセージを示す文字列をコピーするためのヒープメモリ領域の獲得に失敗しました", NULL);
				return NULL;
				}
			}
		//===== 現在時刻文字列の取得に失敗した場合 ======
		else
			{
			//===== ヒープメモリ領域の獲得 ======
			if ((errorMessageLength=M2MString_length((M2MString *)"[DEBUG]%s:%d[l]: %s\n")+methodNameLength+lineNumberLength+messageLength)>0
					&& ((*debugMessage)=(M2MString *)M2MHeap_malloc(errorMessageLength+1))!=NULL)
				{
				snprintf((*debugMessage), errorMessageLength, (M2MString *)"[DEBUG]%s:%d[l]: %s\n", methodName, lineNumber, message);
				return (*debugMessage);
				}
			//===== エラー処理 ======
			else
				{
				M2MLogger_printErrorMessage((M2MString *)"M2MLogger.this_createNewDebugMessage()", __LINE__, (M2MString *)"デバッグメッセージを示す文字列をコピーするためのヒープメモリ領域の獲得に失敗しました", NULL);
				return NULL;
				}
			}
		}
	//===== 引数エラー =====
	else if (methodName==NULL || methodNameLength<=0)
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MLogger.this_createNewDebugMessage()", __LINE__, (M2MString *)"引数で指定された\"methodName\"がNULLです", NULL);
		return NULL;
		}
	else
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MLogger.this_createNewDebugMessage()", __LINE__, (M2MString *)"引数で指定された\"message\"がNULLです", NULL);
		return NULL;
		}
	}
#endif /* DEBUG */


/**
 * 引数で指定されたポインタにエラーメッセージをコピーする。<br>
 * コピー用配列のバッファリングは関数内部で実行する。そのため、呼び出し側では当該<br>
 * 変数使用後、メモリーリークを防止するため必ず"M2MHeap_free()"関数を呼ぶ必要が<br>
 * ある。
 *
 * @param[in] methodName		関数名を示す文字列
 * @param[in] lineNumber		ソースファイル中の行番号（"__LINE__"で埋め込み可能）
 * @param[in] message			エラー内容を示すメッセージ文字列
 * @param[out] errorMessage		エラーメッセージをコピーするためのポインタ（バッファリングは関数内部で実行する）
 * @return						引数のポインタにコピーしたエラーメッセージ or NULL（エラーの場合）
 */
static M2MString *this_createNewErrorMessage (const M2MString *methodName, const unsigned int lineNumber, const M2MString *message, M2MString **errorMessage)
	{
	//========== ローカル変数 ==========
	M2MString time[128];
	M2MString lineNumberString[16];
	unsigned char buffer[256];
	size_t errorMessageLength = 0;
	size_t timeLength = 0;
	size_t methodNameLength = 0;
	size_t lineNumberLength = 0;
	size_t messageLength = 0;
	size_t bufferLength = 0;

	//===== 引数の確認 =====
	if (methodName!=NULL && (methodNameLength=M2MString_length(methodName))>0
			&& (messageLength=M2MString_length(message))>0
			&& errorMessage!=NULL)
		{
		//===== 配列の初期化 =====
		memset(time, 0, sizeof(time));
		memset(lineNumberString, 0, sizeof(lineNumberString));
		//===== 行番号文字列数の取得 =====
		snprintf(lineNumberString, sizeof(lineNumberString)-1, (M2MString *)"%d", lineNumber);
		lineNumberLength = M2MString_length(lineNumberString);
		//===== エラー番号がある場合 =====
		if (errno!=0
				&& strerror_r(errno, buffer, sizeof(buffer))==0
				&& (bufferLength=M2MString_length(buffer))>0)
			{
			//===== ローカルカレンダから現在時刻文字列の取得 ======
			if (M2MString_getLocalTime(time, sizeof(time))>0
					&& (timeLength=M2MString_length(time))>0)
				{
				//===== ヒープメモリ領域の獲得 ======
				if ((errorMessageLength=M2MString_length((M2MString *)"[ERROR]%s %s:%d[l]: %s: %s\r\n")+timeLength+methodNameLength+lineNumberLength+messageLength+bufferLength)>0
						&& ((*errorMessage)=(M2MString *)M2MHeap_malloc(errorMessageLength+1))!=NULL)
					{
					snprintf((*errorMessage), errorMessageLength, (M2MString *)"[ERROR]%s %s:%d[l]: %s: %s\r\n", time, methodName, lineNumber, message, buffer);
					this_initErrorNumber();
					return (*errorMessage);
					}
				//===== エラー処理 ======
				else
					{
					this_initErrorNumber();
					return NULL;
					}
				}
			//===== 現在時刻文字列の取得に失敗した場合 ======
			else
				{
				//===== ヒープメモリ領域の獲得 ======
				if ((errorMessageLength=M2MString_length((M2MString *)"[ERROR]%s:%d[l]: %s: %s\r\n")+methodNameLength+lineNumberLength+messageLength+bufferLength)>0
						&& ((*errorMessage)=(M2MString *)M2MHeap_malloc(errorMessageLength+1))!=NULL)
					{
					snprintf((*errorMessage), errorMessageLength, (M2MString *)"[ERROR]%s:%d[l]: %s: %s\r\n", methodName, lineNumber, message, buffer);
					this_initErrorNumber();
					return (*errorMessage);
					}
				//===== エラー処理 ======
				else
					{
					this_initErrorNumber();
					return NULL;
					}
				}
			}
		//===== エラー番号が無い場合 =====
		else
			{
			//===== ローカルカレンダから現在時刻文字列の取得 ======
			if (M2MString_getLocalTime(time, sizeof(time))>0
					&& (timeLength=M2MString_length(time))>0)
				{
				//===== ヒープメモリ領域の獲得 ======
				if ((errorMessageLength=M2MString_length((M2MString *)"[ERROR]%s %s:%d[l]: %s\r\n")+timeLength+methodNameLength+lineNumberLength+messageLength)>0
						&& ((*errorMessage)=(M2MString *)M2MHeap_malloc(errorMessageLength+1))!=NULL)
					{
					snprintf((*errorMessage), errorMessageLength, (M2MString *)"[ERROR]%s %s:%d[l]: %s\r\n", time, methodName, lineNumber, message);
					return (*errorMessage);
					}
				//===== エラー処理 ======
				else
					{
					return NULL;
					}
				}
			//===== 現在時刻文字列の取得に失敗した場合 ======
			else
				{
				//===== ヒープメモリ領域の獲得 ======
				if ((errorMessageLength=M2MString_length((M2MString *)"[ERROR]%s:%d[l]: %s\r\n")+methodNameLength+lineNumberLength+messageLength)>0
						&& ((*errorMessage)=(M2MString *)M2MHeap_malloc(errorMessageLength+1))!=NULL)
					{
					snprintf((*errorMessage), errorMessageLength, (M2MString *)"[ERROR]%s:%d[l]: %s\r\n", methodName, lineNumber, message);
					return (*errorMessage);
					}
				//===== エラー処理 ======
				else
					{
					return NULL;
					}
				}
			}
		}
	//===== 引数エラー =====
	else if (methodName==NULL || methodNameLength<=0)
		{
		return NULL;
		}
	else
		{
		return NULL;
		}
	}


/**
 * 引数で指定されたパスに設置されたファイルを開く。<br>
 *
 * @param[in] logFilePath	ログファイルの絶対パス
 * @return					（開いた）ファイルオブジェクト or NULL（エラーの場合）
 */
static FILE *this_getLogFile (const M2MString *logFileName)
	{
	//========== ローカル変数 ==========
	FILE *file = NULL;
	const M2MString *HOME_DIRECTORY = M2MDirectory_getHomeDirectoryPath();
	const size_t HOME_DIRECTORY_LENGTH = M2MString_length(HOME_DIRECTORY);
	const size_t SEPARATOR_LENGTH = M2MString_length((unsigned char *)M2MDirectory_SEPARATOR);
	const size_t DIRECTORY_NAME_LENGTH = M2MString_length(M2MDIRECTORY);
	const size_t LOG_FILE_NAME_LENGTH = M2MString_length(logFileName);
	M2MString LOG_FILE_DIRECTORY[HOME_DIRECTORY_LENGTH+SEPARATOR_LENGTH+DIRECTORY_NAME_LENGTH+2];
	M2MString LOG_FILE_PATH[HOME_DIRECTORY_LENGTH+SEPARATOR_LENGTH+DIRECTORY_NAME_LENGTH+SEPARATOR_LENGTH+LOG_FILE_NAME_LENGTH+2];
	const M2MString *PERMISSION = (M2MString *)"0755";

	//===== ログファイル設置ディレクトリパスの作成 =====
	memset(LOG_FILE_DIRECTORY, 0, sizeof(LOG_FILE_DIRECTORY));
	snprintf(LOG_FILE_DIRECTORY, sizeof(LOG_FILE_DIRECTORY), (M2MString *)"%s/%s", HOME_DIRECTORY, M2MDIRECTORY);
	//===== ディレクトリの所在を確認 =====
	if (M2MDirectory_exists(LOG_FILE_DIRECTORY)==true)
		{
		memset(LOG_FILE_PATH, 0, sizeof(LOG_FILE_PATH));
		snprintf(LOG_FILE_PATH, sizeof(LOG_FILE_PATH), (M2MString *)"%s/%s/%s", HOME_DIRECTORY, M2MDIRECTORY, logFileName);
		//===== ログファイルを開く =====
		if ((file=M2MFile_open(LOG_FILE_PATH, true))!=NULL)
			{
			//===== ログファイルのサイズが規程値を超過している場合 =====
			if (this_overMaxLogFileLength(file)==true)
				{
				M2MFile_remove(LOG_FILE_PATH);
				}
			//===== ログファイルのサイズが規程値以内の場合 =====
			else
				{
				// 何もしない
				}
			return file;
			}
		//===== エラー処理 =====
		else
			{
			return NULL;
			}
		}
	//===== ログファイルが存在しない場合 =====
	else
		{
		//===== ログファイルを新規作成 =====
		if (M2MDirectory_mkdirs(LOG_FILE_DIRECTORY, PERMISSION)==true)
			{
			memset(LOG_FILE_PATH, 0, sizeof(LOG_FILE_PATH));
			snprintf(LOG_FILE_PATH, sizeof(LOG_FILE_PATH), (M2MString *)"%s/%s/%s", HOME_DIRECTORY, M2MDIRECTORY, logFileName);
			//===== ログファイルを開く =====
			if ((file=M2MFile_open(LOG_FILE_PATH, true))!=NULL)
				{
				//===== ログファイルのサイズが規程値を超過している場合 =====
				if (this_overMaxLogFileLength(file)==true)
					{
					M2MFile_remove(LOG_FILE_PATH);
					}
				//===== ログファイルのサイズが規程値以内の場合 =====
				else
					{
					// 何もしない
					}
				return file;
				}
			//===== エラー処理 =====
			else
				{
				return NULL;
				}
			}
		//===== エラー処理 =====
		else
			{
			return NULL;
			}
		}
	}


/**
 * errnoを初期化する.<br>
 */
static void this_initErrorNumber ()
	{
	errno = 0;
	return;
	}


/**
 * 引数で指定されたファイルのサイズが既定値を超過しているかどうか確認する．<br>
 *
 * @param[in] file	サイズ確認を行う対象のファイルオブジェクト
 * @return			true : 既定値[Byte]を超過，false : 既定値[Byte]以内
 */
static bool this_overMaxLogFileLength (const FILE *file)
	{
	//===== 現在のファイルサイズが規程値を超過している場合 =====
	if ((unsigned long)M2MLogger_MAX_LOG_FILE_LENGTH<M2MFile_length(file))
		{
		return true;
		}
	//===== 現在のファイルサイズが規程値以内の場合 =====
	else
		{
		return false;
		}
	}


/**
 * エラーメッセージをファイルか標準エラー出力に出力する。<br>
 *
 * @param[in] methodName		関数名を示す文字列
 * @param[in] lineNumber		ソースファイル中の行番号（"__LINE__"で埋め込み可能）
 * @param[in] message			エラー内容を示すメッセージ文字列
 */
#ifdef DEBUG
static void this_printDebugMessage (const M2MString *methodName, const unsigned int lineNumber, const M2MString *message)
	{
	//========== ローカル変数 ==========
	M2MString *debugMessage = NULL;
	FILE *file = NULL;
	const M2MString *LOG_FILE_NAME = (M2MString *)"m2m.log";

	//===== エラーメッセージの取得 =====
	if (this_createNewDebugMessage(methodName, lineNumber, message, &debugMessage)!=NULL)
		{
		//===== ログファイルを取得 =====
		if ((file=this_getLogFile(LOG_FILE_NAME))!=NULL)
			{
			//===== エラーメッセージを規程ディレクトリのログファイルに出力 =====
			M2MFile_write(file, debugMessage, M2MString_length(debugMessage));
			//===== ログファイルを閉じる =====
			M2MFile_close(file);
			}
		//===== ログファイルが見つからなかった場合 =====
		else
			{
			//===== エラーメッセージを標準エラー出力に出力 =====
			fprintf(stdout, (M2MString *)"%s", debugMessage);
			}
		//===== ヒープメモリ領域の解放 =====
		M2MHeap_free(debugMessage);
		}
	//===== エラー処理 =====
	else
		{
		}
	return;
	}
#endif /* DEBUG */



/**
 * エラーメッセージをファイルか標準エラー出力に出力する。<br>
 *
 * @param[in] methodName		関数名を示す文字列
 * @param[in] lineNumber		ソースファイル中の行番号（"__LINE__"で埋め込み可能）
 * @param[in] message			エラー内容を示すメッセージ文字列
 */
static void this_printErrorMessage (const M2MString *methodName, const unsigned int lineNumber, const M2MString *message)
	{
	//========== ローカル変数 ==========
	M2MString *errorMessage = NULL;
	FILE *file = NULL;
	const M2MString *LOG_FILE_NAME = (M2MString *)"m2m.log";

	//===== エラーメッセージの取得 =====
	if (this_createNewErrorMessage(methodName, lineNumber, message, &errorMessage)!=NULL)
		{
		//===== ログファイルを取得 =====
		if ((file=this_getLogFile(LOG_FILE_NAME))!=NULL)
			{
			//===== エラーメッセージを規程ディレクトリのログファイルに出力 =====
			M2MFile_write(file, errorMessage, M2MString_length(errorMessage));
			//===== ログファイルを閉じる =====
			M2MFile_close(file);
			}
		//===== ログファイルが見つからなかった場合 =====
		else
			{
			//===== エラーメッセージを標準エラー出力に出力 =====
			fprintf(stderr, (M2MString *)"%s", errorMessage);
			}
		//===== ヒープメモリ領域の解放 =====
		M2MHeap_free(errorMessage);
		}
	//===== エラー処理 =====
	else
		{
		}
	return;
	}


/*******************************************************************************
 * 公開関数
 ******************************************************************************/
/**
 * 引数で指定された関数名、行番号、メッセージを基に、デバッグメッセージを標準出力<br>
 * に出力（表示）する。<br>
 *
 * @param[in] methodName					関数名を示す文字列
 * @param[in] lineNumber					ソースファイル中の行番号（"__LINE__"で埋め込み可能）
 * @param[in] message						エラー内容を示すメッセージ文字列
 */
void M2MLogger_printDebugMessage (const M2MString *methodName, const unsigned int lineNumber, const M2MString *message)
	{
#ifdef DEBUG
	//===== デバッグメッセージを出力 =====
	this_printDebugMessage(methodName, lineNumber, message);
#endif /* DEBUG */
	return;
	}


/**
 * 引数で指定された関数名、行番号、メッセージを基に、エラーメッセージを標準エラー<br>
 * 出力に出力（表示）する。<br>
 * また、エラーメッセージコピー用のポインタが指定された場合、標準エラー出力に出力<br>
 * せずに、この関数内部でバッファリングを行い、エラーメッセージをコピーする。<br>
 *
 * @param[in] methodName					関数名を示す文字列
 * @param[in] lineNumber					ソースファイル中の行番号（"__LINE__"で埋め込み可能）
 * @param[in] message						エラー内容を示すメッセージ文字列
 * @param[out] bufferForCopyingErrorMessage	エラーメッセージ全体をコピーするためのバッファ or NULL（コピー不要の場合）
 */
void M2MLogger_printErrorMessage (const M2MString *methodName, const unsigned int lineNumber, const M2MString *message, M2MString **bufferForCopyingErrorMessage)
	{
	//===== エラーメッセージを引数にコピーする場合 =====
	if (bufferForCopyingErrorMessage!=NULL)
		{
		this_createNewErrorMessage(methodName, lineNumber, message, bufferForCopyingErrorMessage);
		}
	//===== エラーメッセージを引数にコピーしない場合 =====
	else
		{
		this_printErrorMessage(methodName, lineNumber, message);
		}
	return;
	}



/* End Of File */
