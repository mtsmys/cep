/*******************************************************************************
 * M2MString.c
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

#include "m2m/lang/M2MString.h"


/*******************************************************************************
 * 公開関数
 ******************************************************************************/
/**
 * 引数で指定されたself文字列の後ろに引数stringで指定された文字列を追加する．<br>
 *
 * @param[in,out] self	文字列を追加する、元の文字列（追加後の文字列は self = "self + string" となる）
 * @param[in] string	追加する文字列
 * @return				文字列が追加されたバッファのポインタ or NULL（エラーの場合）
 */
M2MString *M2MString_append (M2MString **self, const M2MString *string)
	{
	//========== ローカル変数 ==========
	M2MString *tmp = NULL;
	size_t thisLength = 0;
	size_t stringLength = 0;

	//===== 引数の確認 =====
	if (self!=NULL && string!=NULL && (stringLength=M2MString_length(string))>0)
		{
		//===== 連結元の文字列が存在する場合 =====
		if ((*self)!=NULL)
			{
			//===== 連結元の文字列を一時的にコピーするための準備 =====
			if( (thisLength=M2MString_length((*self)))>0
				&& (tmp=(M2MString *)M2MHeap_malloc(thisLength+1))!=NULL)
				{
				//===== 連結元の文字列を一時的にコピー =====
				memcpy(tmp, (*self), thisLength);
				//===== 連結元の文字列のヒープメモリ領域を解放 =====
				M2MHeap_free((*self));
				//===== 連結文字列のヒープメモリ領域を獲得 =====
				if (((*self)=(M2MString *)M2MHeap_malloc(thisLength+stringLength+1))!=NULL)
					{
					//===== 文字列を連結 =====
					memcpy(&((*self)[0]), tmp, thisLength);
					memcpy(&((*self)[thisLength]), string, stringLength);
					//===== 一時的なバッファのヒープメモリ領域を解放 =====
					M2MHeap_free(tmp);
					//===== 連結した文字列のポインタを返す =====
					return (*self);
					}
				//===== エラー処理 =====
				else
					{
					M2MHeap_free(tmp);
					return NULL;
					}
				}
			//===== エラー処理 =====
			else
				{
				return NULL;
				}
			}
		//===== 連結元の文字列が存在しない場合 =====
		else
			{
			//===== 文字列コピーのヒープメモリ領域を獲得 =====
			if (((*self)=(M2MString *)M2MHeap_malloc(stringLength+1))!=NULL)
				{
				//===== 文字列をコピー =====
				memcpy((*self), string, stringLength);
				//===== 文字列のポインタを返す =====
				return (*self);
				}
			//===== エラー処理 =====
			else
				{
				return NULL;
				}
			}
		}
	//===== 引数エラー =====
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
 * 引数で指定されたself文字列の後ろに、引数stringで指定された文字列を追加する．<br>
 * 追加文字列の長さ[Byte]は引数のstringLengthで指定する．<br>
 *
 * @param[in,out] self		文字列を追加する、元の文字列（追加後の文字列は self = "self + string" となる）
 * @param[in] string		追加する文字列
 * @param[in] stringLength	追加する文字列の長さを示す整数[Byte]（文字列の一部のみを追加する場合に有効)
 * @return					文字列が追加されたバッファのポインタ or NULL（エラーの場合）
 */
M2MString *M2MString_appendLength (M2MString **self, const M2MString *string, const size_t stringLength)
	{
	//========== ローカル変数 ==========
	M2MString *tmp = NULL;
	size_t thisLength = 0;

	//===== 引数の確認 =====
	if (self!=NULL && string!=NULL && 0<stringLength && stringLength<=M2MString_length(string))
		{
		//===== 連結元の文字列が存在する場合 =====
		if ((*self)!=NULL)
			{
			//===== 連結元の文字列を一時的にコピーするための準備 =====
			if( (thisLength=M2MString_length((*self)))>0
				&& (tmp=(M2MString *)M2MHeap_malloc(thisLength+1))!=NULL)
				{
				//===== 連結元の文字列を一時的にコピー =====
				memcpy(tmp, (*self), thisLength);
				//===== 連結元の文字列のヒープメモリ領域を解放 =====
				M2MHeap_free((*self));
				//===== 連結文字列のヒープメモリ領域を獲得 =====
				if (((*self)=(M2MString *)M2MHeap_malloc(thisLength+stringLength+1))!=NULL)
					{
					//===== 文字列を連結 =====
					memcpy(&((*self)[0]), tmp, thisLength);
					memcpy(&((*self)[thisLength]), string, stringLength);
					//===== 一時的なバッファのヒープメモリ領域を解放 =====
					M2MHeap_free(tmp);
					//===== 連結した文字列のポインタを返す =====
					return (*self);
					}
				//===== エラー処理 =====
				else
					{
					M2MHeap_free(tmp);
					return NULL;
					}
				}
			//===== エラー処理 =====
			else
				{
				return NULL;
				}
			}
		//===== 連結元の文字列が存在しない場合 =====
		else
			{
			//===== 文字列コピーのヒープメモリ領域を獲得 =====
			if (((*self)=(M2MString *)M2MHeap_malloc(stringLength+1))!=NULL)
				{
				//===== 文字列をコピー =====
				memcpy((*self), string, stringLength);
				//===== 文字列のポインタを返す =====
				return (*self);
				}
			//===== エラー処理 =====
			else
				{
				return NULL;
				}
			}
		}
	//===== 引数エラー =====
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
 * 引数で指定された2つの文字列を比較し、その結果を返す。<br>
 *
 * @param[in] self		比較対象の片方の文字列
 * @param[in] string	比較対象のもう一方の文字列
 * @return				0：2つが等しい場合、負：辞書的にthis<stringの場合、正：辞書的にthis>stringの場合
 */
signed int M2MString_compareTo (const M2MString *self, const M2MString *string)
	{
	//========== ローカル変数 ==========
	size_t thisLength = 0;

	//===== 引数の確認 =====
	if (self!=NULL && (thisLength=M2MString_length(self))>0 && string!=NULL)
		{
		return memcmp(self, string, thisLength);
		}
	//===== 引数エラー =====
	else
		{
		return -1;
		}
	}


/**
 * 引数で指定された入力データに対し，文字コードをCRLFに変換して返す．<br>
 *
 * @param[in] self		入力ファイルから取得した文字列
 * @param[out] string	改行コードを補正したCSV形式の文字列を格納するポインタ（バッファリング自体は関数内部で実行)
 * @return				改行コードを補正したCSV形式の文字列ポインタ or NULL（エラーの場合)
 */
M2MString *M2MString_convertFromLFToCRLF (const M2MString *self, M2MString **string)
	{
	//========== ローカル変数 ==========
	M2MString *lineStart = NULL;
	M2MString *lineEnd = NULL;
	size_t lineLength = 0;
	const size_t LF_LENGTH = M2MString_length(M2MString_LF);
	const size_t CRLF_LENGTH = M2MString_length(M2MString_CRLF);

	//===== 引数の確認 =====
	if (self!=NULL && string!=NULL)
		{
		//===== 文字列中にCRLFが1つでも存在する（補正は実行しない)場合 =====
		if ((lineEnd=M2MString_indexOf(self, M2MString_CRLF))!=NULL)
			{
			//===== 出力データのヒープメモリ領域を獲得 =====
			if (((*string)=(M2MString *)M2MHeap_malloc(M2MString_length(self)))!=NULL)
				{
				//===== 入力データをそのままコピー =====
				memcpy((*string), self, M2MString_length(self));
				//===== 出力データを返す =====
				return (*string);
				}
			//===== エラー処理 =====
			else
				{
				return NULL;
				}
			}
		//===== 文字列中にCRLFが1つも存在しない（補正を実行する)場合 =====
		else
			{
			//===== 入力データの先頭位置を取得 =====
			lineStart = (M2MString *)self;
			//===== LFが無くなるまで探索を繰り返し =====
			while ((lineEnd=M2MString_indexOf(lineStart, M2MString_LF))!=NULL)
				{
				//===== 文字列データが存在する場合 =====
				if ((lineLength=M2MString_length(lineStart)-M2MString_length(lineEnd))>0)
					{
					//===== 文字列データをコピー =====
					M2MString_appendLength(string, lineStart, lineLength);
					//===== CRLFを追加 =====
					M2MString_appendLength(string, M2MString_CRLF, CRLF_LENGTH);
					}
				//===== 文字列データが存在しない場合 =====
				else
					{
					//===== CRLFを追加 =====
					M2MString_append(string, M2MString_CRLF);
					}
				//===== ポインタを次の行の先頭へ移動 =====
				lineEnd += LF_LENGTH;
				lineStart = lineEnd;
				}
			//===== 最後の行の文字列データが存在する場合 =====
			if ((lineLength=M2MString_length(lineStart))>0)
				{
				//===== 文字列データをコピー =====
				M2MString_appendLength(string, lineStart, lineLength);
				}
			//===== 最後の行の文字列データが存在しない場合 =====
			else
				{
				// 何もしない
				}
			//===== 出力データを返す =====
			return (*string);
			}
		}
	//===== 引数エラー =====
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
 * 引数で指定された“double”値を文字列に変換し、引数で指定されたポインタにコピー<br>
 * する。<br>
 * 配列のバッファリングはこの関数内部で実行するため、呼び出し側では文字列利用後、<br>
 * メモリーリークを防止するため、必ず“M2MHeap_free()”関数を呼ぶ事。<br>
 *
 * @param[in] number	文字列変換対象の実数
 * @param[out] string	変換した文字列をコピーするためのポインタ（バッファリングは関数内部で実行する）
 * @return				コピーした文字列 or NULL（エラーの場合）
 */
M2MString *M2MString_convertFromDoubleToString (const double number, M2MString **string)
	{
	//========== ローカル変数 ==========
	M2MString tmpBuffer[128];
	size_t stringLength = 0;

	//===== 引数の確認 =====
	if (string!=NULL)
		{
		//===== 配列の初期化 =====
		memset(tmpBuffer, 0, sizeof(tmpBuffer));
		//===== Convert from long to string =====
		if (snprintf(tmpBuffer, sizeof(tmpBuffer), "%f", number)>=0
				&& (stringLength=M2MString_length(tmpBuffer))>0)
			{
			//===== ヒープメモリ領域の獲得 =====
			if (((*string)=(M2MString *)M2MHeap_malloc(stringLength+1))!=NULL)
				{
				memcpy((*string), tmpBuffer, stringLength);
				return (*string);
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
	//===== 引数エラー =====
	else
		{
		return NULL;
		}
	}


/**
 * 引数で指定された“int”値を文字列に変換し、引数で指定されたポインタにコピーする。<br>
 * 配列のバッファリングはこの関数内部で実行するため、呼び出し側では文字列利用後、<br>
 * メモリーリークを防止するため、必ず“M2MHeap_free()”関数を呼ぶ事。<br>
 *
 * @param[in] number	文字列変換対象の整数
 * @param[out] string	変換した文字列をコピーするためのポインタ（バッファリングは関数内部で実行する）
 * @return				コピーした文字列 or NULL（エラーの場合）
 */
M2MString *M2MString_convertFromIntegerToString (const signed int number, M2MString **string)
	{
	//========== ローカル変数 ==========
	M2MString tmpBuffer[128];
	size_t stringLength = 0;

	//===== 引数の確認 =====
	if (string!=NULL)
		{
		//===== 配列の初期化 =====
		memset(tmpBuffer, 0, sizeof(tmpBuffer));
		//===== Convert from long to string =====
		if (snprintf(tmpBuffer, sizeof(tmpBuffer), "%d", number)>=0
				&& (stringLength=M2MString_length(tmpBuffer))>0)
			{
			//===== ヒープメモリ領域の獲得 =====
			if (((*string)=(M2MString *)M2MHeap_malloc(stringLength+1))!=NULL)
				{
				memcpy((*string), tmpBuffer, stringLength);
				return (*string);
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
	//===== 引数エラー =====
	else
		{
		return NULL;
		}
	}


/**
 * 引数で指定された“long int”値を文字列に変換し、引数で指定されたポインタにコピー<br>
 * する。<br>
 * 配列のバッファリングはこの関数内部で実行するため、呼び出し側では文字列利用後、<br>
 * メモリーリークを防止するため、必ず“M2MHeap_free()”関数を呼ぶ事。<br>
 *
 * @param[in]	文字列変換対象の整数
 * @param[out]	変換した文字列をコピーするためのポインタ（バッファリングは関数内部で実行する）
 * @return		コピーした文字列 or NULL（エラーの場合）
 */
M2MString *M2MString_convertFromLongToString (const signed long number, M2MString **string)
	{
	//========== ローカル変数 ==========
	M2MString tmpBuffer[128];
	size_t stringLength = 0;

	//===== 引数の確認 =====
	if (string!=NULL)
		{
		//===== 配列の初期化 =====
		memset(tmpBuffer, 0, sizeof(tmpBuffer));
		//===== Convert from long to string =====
		if (snprintf(tmpBuffer, sizeof(tmpBuffer), "%ld", number)>=0
				&& (stringLength=M2MString_length(tmpBuffer))>0)
			{
			//===== ヒープメモリ領域の獲得 =====
			if (((*string)=(unsigned char *)M2MHeap_malloc(stringLength+1))!=NULL)
				{
				memcpy((*string), tmpBuffer, stringLength);
				return (*string);
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
	//===== 引数エラー =====
	else
		{
		return NULL;
		}
	}


/**
 * 引数で指定された文字列を実数に変換する。<br>
 *
 * @param string		実数を示す文字列
 * @param stringLength	実数を示す文字列のサイズ[Byte]
 * @return				文字列から変換した実数
 */
double M2MString_convertFromStringToDouble (const M2MString *string, const size_t stringLength)
	{
	//========== ローカル変数 ==========
	unsigned char STRING_ARRAY[stringLength+1];

	//===== 引数の確認 =====
	if (stringLength<=M2MString_length(string))
		{
		//===== 変換バッファに文字列をコピー =====
		memset(STRING_ARRAY, 0, sizeof(STRING_ARRAY));
		memcpy(STRING_ARRAY, string, stringLength);
		//===== 文字列を実数に変換 =====
		return strtod(STRING_ARRAY, NULL);
		}
	//===== 引数エラー =====
	else
		{
		return (double)-1;
		}
	}


/**
 * 引数で指定された文字列を整数に変換する。<br>
 *
 * @param string	整数を示す文字列
 * @return			文字列から変換した整数
 */
int M2MString_convertFromStringToInteger (const M2MString *string, const size_t stringLength)
	{
	//========== ローカル変数 ==========
	M2MString STRING_ARRAY[stringLength+1];

	//===== 引数の確認 =====
	if (stringLength<=M2MString_length(string))
		{
		//===== 変換バッファに文字列をコピー =====
		memset(STRING_ARRAY, 0, sizeof(STRING_ARRAY));
		memcpy(STRING_ARRAY, string, stringLength);
		//===== 文字列を整数に変換 =====
		return atoi(STRING_ARRAY);
		}
	//===== 引数エラー =====
	else
		{
		return (int)-1;
		}
	}


/**
 * 引数で指定された文字列をロング整数に変換する。<br>
 *
 * @param string		ロング整数を示す文字列
 * @param stringLength	ロング整数を示す文字列のサイズ[Byte]
 * @return				文字列から変換したロング整数
 */
long M2MString_convertFromStringToLong (const M2MString *string, const size_t stringLength)
	{
	//========== ローカル変数 ==========
	M2MString STRING_ARRAY[stringLength+1];

	//===== 引数の確認 =====
	if (stringLength<=M2MString_length(string))
		{
		//===== 変換バッファに文字列をコピー =====
		memset(STRING_ARRAY, 0, sizeof(STRING_ARRAY));
		memcpy(STRING_ARRAY, string, stringLength);
		//===== 文字列を整数に変換 =====
		return atoi(STRING_ARRAY);
		}
	//===== 引数エラー =====
	else
		{
		return (int)-1;
		}
	}


/**
 * 2つの文字列を比較し、同一であるかどうかを確認する。<br>
 *
 * @param[in] one		比較対象の文字列
 * @param[in] another	比較対象のもう一つの文字列
 * @param[in] length	比較対象の文字列の長さ[バイト]
 * @return				true : 同一の場合、false : 異なる場合
 */
bool M2MString_equals (const M2MString *one, const M2MString *another, const size_t length)
	{
	//===== 引数の確認 =====
	if (one!=NULL && another!=NULL && length>0)
		{
		//===== 一致するかどうか検証 =====
		if (memcmp(one, another, length)==0)
			{
			return true;
			}
		//===== 一致しなかった場合 =====
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
 * 引数で指定された”buffer”配列にローカルカレンダーの時間を示す文字列をコピー<br>
 * する。<br>
 * なお、この関数内部でバッファリングは行わないため、呼び出し側でメモリ領域を<br>
 * 確保する事。<br>
 *
 * @param[out] buffer		ローカルカレンダーの時間を示す文字列をコピーするためのバッファ
 * @param[in] bufferLength	引数で指定されたバッファの大きさを示す整数[バイト]
 * @return					バッファにコピーした、ローカル時間を示す文字列の大きさを示す整数[バイト]
 */
unsigned int M2MString_getLocalTime (M2MString *buffer, const unsigned int bufferLength)
	{
	//========== ローカル変数 ==========
	struct timeval currentTime;
	struct tm *localCalendar = NULL;
	M2MString *miliSecond = NULL;
	unsigned int miliSecondLength = 0;

	//===== 引数の確認 =====
	if (buffer!=NULL && bufferLength>0)
		{
		//===== 配列の初期化 =====
		memset(buffer, 0, bufferLength);
		//===== ローカルカレンダの時刻を取得 =====
		if (gettimeofday(&currentTime, NULL)==0
				&& (localCalendar=localtime(&(currentTime.tv_sec)))!=NULL)
			{
			//===== 日時を文字列へ変換 =====
			strftime(buffer, bufferLength-1, "%Y/%m/%d %H:%M:%S:", localCalendar);
			//===== マイクロ秒構造体を文字列へ変換 =====
			if (M2MString_convertFromLongToString((currentTime.tv_usec/1000), &miliSecond)!=NULL)
				{
				//===== 変換した文字列の長さの確認 =====
				if ((miliSecondLength=M2MString_length(miliSecond))>0
						&& miliSecondLength<(bufferLength-M2MString_length(buffer)-1))
					{
					//===== 変換した文字列をコピー =====
					memcpy(&(buffer[M2MString_length(buffer)]), miliSecond, miliSecondLength);
					//===== ヒープメモリ領域の解放 =====
					M2MHeap_free(miliSecond);
					return M2MString_length(buffer);
					}
				//===== エラー処理 =====
				else
					{
					//===== ヒープメモリ領域の解放 =====
					M2MHeap_free(miliSecond);
					return 0;
					}
				}
			//===== エラー処理 =====
			else
				{
				return 0;
				}
			}
		//===== エラー処理 =====
		else
			{
			return 0;
			}
		}
	//===== 引数エラー =====
	else if (buffer==NULL)
		{
		return 0;
		}
	else
		{
		return 0;
		}
	}


/**
 * 引数で指定された文字列の中からキーワードを検索し、発見した場合は当該キーワード<br>
 * の開始位置を示すポインタを返す。<br>
 * 検索に失敗した場合は NULL を返す。<br>
 *
 * @param[in] string	検索対象の文字列
 * @param[in] keyword	キーワード文字列
 * @return				検索対象の文字列のうち、キーワード開始位置を示すポインタ or NULL（エラーの場合）
 */
M2MString *M2MString_indexOf (const M2MString *string, const M2MString *keyword)
	{
	//===== 引数の確認 =====
	if (string!=NULL && keyword!=NULL)
		{
		//===== キーワード検索結果を返す =====
		return (M2MString *)strstr(string, keyword);
		}
	//===== 引数エラー =====
	else
		{
		return NULL;
		}
	}


/**
 * 引数で指定された比較対象の文字列の中に、検出用の文字列が最後に現れる場所を<br>
 * 返す。<br>
 * 検出用文字列が見つからなかった場合は NULL を返す。<br>
 *
 * @param string	比較対象の文字列
 * @param fromIndex	検出用文字列
 * @return			検出された位置のポインタ or NULL（検出出来なかった場合）
 */
M2MString *M2MString_lastIndexOf (const M2MString *string, const M2MString *fromIndex)
	{
	//========== ローカル変数 ==========
	M2MString *lastIndex = NULL;
	M2MString *index = (M2MString *)string;

	//===== 引数の確認 =====
	if (string!=NULL && fromIndex!=NULL)
		{
		//=====  =====
		while ((index=strstr(index, fromIndex))!=NULL)
			{
			lastIndex = index;
			index++;
			}
		//=====  =====
		return lastIndex;
		}
	//===== 引数エラー =====
	else
		{
		return NULL;
		}
	}


/**
 * 引数で指定された文字列の長さを取得して返す。<br>
 *
 * @param[in] self		文字列（必ずヌル終端とする事）
 * @return				文字列の長さを示す整数 or 0（エラーの場合）
 */
size_t M2MString_length (const M2MString *self)
	{
	//===== 引数の確認 =====
	if (self!=NULL)
		{
		return strlen(self);
		}
	//===== 引数エラー =====
	else
		{
		return 0;
		}
	}


/**
 * 引数で指定されたソース文字列（＝"string"）に対し、分割用文字列（＝"delimiter"）<br>
 * で分割した結果を返す。<br>
 * 連続して取得する場合、初回のみソース文字列を指定し、次回以降はソース文字列に<br>
 * NULLを指定する。<br>
 * なお、当該処理を実行すると、ソース文字列自体が書き換えられるため、壊されてもよい<br>
 * 文字列をソース文字列に指定する事。<br>
 *
 * @param[in] self			分割処理対象のソース文字列
 * @param[in] delimiter		分割文字列
 * @param[in] savePointer	分割処理が施されたソース文字列のコピー用バッファ（毎回同じ変数を渡す事）
 * @return					分割されたソース文字列（の断片に該当する文字列）
 *
M2MString *M2MString_split (M2MString *self, const M2MString *delimiter, M2MString **savePoint)
	{
	return strtok_r(self, delimiter, savePoint);
	}
*/


/* End Of File */
