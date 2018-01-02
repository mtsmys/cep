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
 * Public function
 ******************************************************************************/
/**
 * Add the argument string after the self string.<br>
 *
 * @param[in,out] self	Base string for adding (string after addition: self = self + string)
 * @param[in] string	Additional string
 * @return				Pointer of the buffer to which the string was added or NULL (means error)
 */
M2MString *M2MString_append (M2MString **self, const M2MString *string)
	{
	//========== Variable ==========
	M2MString *tmp = NULL;
	size_t thisLength = 0;
	size_t stringLength = 0;

	//===== Check argument =====
	if (self!=NULL && string!=NULL && (stringLength=M2MString_length(string))>0)
		{
		//===== In the case of concatenation string existing =====
		if ((*self)!=NULL)
			{
			//===== Preparation for temporarily copying the original string =====
			if( (thisLength=M2MString_length((*self)))>0
				&& (tmp=(M2MString *)M2MHeap_malloc(thisLength+1))!=NULL)
				{
				//===== Temporarily copy the source string =====
				memcpy(tmp, (*self), thisLength);
				//===== Release heap memory of consolidation string =====
				M2MHeap_free((*self));
				//===== Acquire heap memory of concatenated string =====
				if (((*self)=(M2MString *)M2MHeap_malloc(thisLength+stringLength+1))!=NULL)
					{
					//===== Concatenate strings =====
					memcpy(&((*self)[0]), tmp, thisLength);
					memcpy(&((*self)[thisLength]), string, stringLength);
					//===== Release temporary heap memory =====
					M2MHeap_free(tmp);
					//===== Return pointer of concatenated string =====
					return (*self);
					}
				//===== Error handling =====
				else
					{
					M2MHeap_free(tmp);
					return NULL;
					}
				}
			//===== Error handling =====
			else
				{
				return NULL;
				}
			}
		//===== In the case of not existing of the concatenation string =====
		else
			{
			//===== Acquire heap memory of string copy =====
			if (((*self)=(M2MString *)M2MHeap_malloc(stringLength+1))!=NULL)
				{
				//===== Copy string =====
				memcpy((*self), string, stringLength);
				//===== Returns a pointer to the string =====
				return (*self);
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
 * Add the string after the "self" string. <br>
 * The length [Byte] of the additional string is specified by argument.<br>
 *
 * @param[in,out] self		The original string to be added to the string (the string after addition is self = self + string)
 * @param[in] string		String to be added
 * @param[in] stringLength	Length of the string to be added[Byte]
 * @return					Pointer of the buffer to which the string was added or NULL (in case of error)
 */
M2MString *M2MString_appendLength (M2MString **self, const M2MString *string, const size_t stringLength)
	{
	//========== Variable ==========
	M2MString *tmp = NULL;
	size_t thisLength = 0;

	//===== Check argument =====
	if (self!=NULL && string!=NULL && 0<stringLength && stringLength<=M2MString_length(string))
		{
		//===== When the string of the concatenation source exists =====
		if ((*self)!=NULL)
			{
			//===== Preparation for temporarily copying the original string =====
			if( (thisLength=M2MString_length((*self)))>0
				&& (tmp=(M2MString *)M2MHeap_malloc(thisLength+1))!=NULL)
				{
				//===== Temporarily copy the source string =====
				memcpy(tmp, (*self), thisLength);
				//===== Rlease heap memory of consolidation source =====
				M2MHeap_free((*self));
				//===== Get heap memory of concatenated string =====
				if (((*self)=(M2MString *)M2MHeap_malloc(thisLength+stringLength+1))!=NULL)
					{
					//===== Concatenate strings =====
					memcpy(&((*self)[0]), tmp, thisLength);
					memcpy(&((*self)[thisLength]), string, stringLength);
					//===== Release temporary buffer heap memory =====
					M2MHeap_free(tmp);
					//===== Return pointer of concatenated string =====
					return (*self);
					}
				//===== Error handling =====
				else
					{
					M2MHeap_free(tmp);
					return NULL;
					}
				}
			//===== Error handling =====
			else
				{
				return NULL;
				}
			}
		//===== When the string of the concatenation source does not exist =====
		else
			{
			//===== Get heap memory for copying string =====
			if (((*self)=(M2MString *)M2MHeap_malloc(stringLength+1))!=NULL)
				{
				//===== Copy string =====
				memcpy((*self), string, stringLength);
				//===== Return pointer of concatenated string =====
				return (*self);
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
 * Compares the two strings specified by the argument and returns the result.<br>
 *
 * @param[in] self		One string to be compared
 * @param[in] string	Another string to be compared
 * @return				0：two are equal, negative：In case of self < string, positive：In case of self > string
 */
signed int M2MString_compareTo (const M2MString *self, const M2MString *string)
	{
	//========== Variable ==========
	size_t thisLength = 0;

	//===== Check argument =====
	if (self!=NULL && (thisLength=M2MString_length(self))>0 && string!=NULL)
		{
		return memcmp(self, string, thisLength);
		}
	//===== Argument error =====
	else
		{
		return -1;
		}
	}


/**
 * Convert double value into a string and copies it to the pointer. <br>
 * Since buffering of arrays is executed inside this function, so call <br>
 * "M2MHeap_free()" function on the caller side in order to prevent memory <br>
 * leak after using the string. <br>
 *
 * @param[in] number	Real number to be converted
 * @param[out] string	Pointer for copying the converted string (buffering is executed inside the function)
 * @return				Copied string or NULL (in case of error)
 */
M2MString *M2MString_convertFromDoubleToString (const double number, M2MString **string)
	{
	//========== Variable ==========
	M2MString tmpBuffer[128];
	size_t stringLength = 0;

	//===== Check argument =====
	if (string!=NULL)
		{
		//===== Initialize array =====
		memset(tmpBuffer, 0, sizeof(tmpBuffer));
		//===== Convert from long to string =====
		if (snprintf(tmpBuffer, sizeof(tmpBuffer), "%f", number)>=0
				&& (stringLength=M2MString_length(tmpBuffer))>0)
			{
			//===== Get heap memory for copying =====
			if (((*string)=(M2MString *)M2MHeap_malloc(stringLength+1))!=NULL)
				{
				memcpy((*string), tmpBuffer, stringLength);
				return (*string);
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
 * 引数で指定された入力データに対し, 文字コードをCRLFに変換して返す．<br>
 *
 * @param[in] self		入力ファイルから取得した文字列
 * @param[out] string	改行コードを補正したCSV形式の文字列を格納するポインタ（バッファリング自体は関数内部で実行)
 * @return				改行コードを補正したCSV形式の文字列ポインタ or NULL（エラーの場合)
 */
M2MString *M2MString_convertFromLFToCRLF (const M2MString *self, M2MString **string)
	{
	//========== Variable ==========
	M2MString *lineStart = NULL;
	M2MString *lineEnd = NULL;
	size_t lineLength = 0;
	const size_t LF_LENGTH = M2MString_length(M2MString_LF);
	const size_t CRLF_LENGTH = M2MString_length(M2MString_CRLF);

	//===== Check argument =====
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
			//===== Error handling =====
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
 * Convert signed integer value into a string and copies it to the pointer. <br>
 * Since buffering of arrays is executed inside this function, so call <br>
 * "M2MHeap_free()" function on the caller side in order to prevent memory <br>
 * leak after using the string. <br>
 *
 * @param[in] number	signed integer number to be converted
 * @param[out] string	Pointer for copying the converted string (buffering is executed inside the function)
 * @return				Copied string or NULL (in case of error)
 */
M2MString *M2MString_convertFromSignedIntegerToString (const signed int number, M2MString **string)
	{
	//========== Variable ==========
	M2MString tmpBuffer[128];
	size_t stringLength = 0;

	//===== Check argument =====
	if (string!=NULL)
		{
		//===== Initialize array =====
		memset(tmpBuffer, 0, sizeof(tmpBuffer));
		//===== Convert from long to string =====
		if (snprintf(tmpBuffer, sizeof(tmpBuffer), "%d", number)>=0
				&& (stringLength=M2MString_length(tmpBuffer))>0)
			{
			//===== Get heap memory for copying =====
			if (((*string)=(M2MString *)M2MHeap_malloc(stringLength+1))!=NULL)
				{
				memcpy((*string), tmpBuffer, stringLength);
				return (*string);
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
 * Convert signed long value into a string and copies it to the pointer. <br>
 * Since buffering of arrays is executed inside this function, so call <br>
 * "M2MHeap_free()" function on the caller side in order to prevent memory <br>
 * leak after using the string. <br>
 *
 * @param[in] number	signed long number to be converted
 * @param[out] string	Pointer for copying the converted string (buffering is executed inside the function)
 * @return				Copied string or NULL (in case of error)
 */
M2MString *M2MString_convertFromSignedLongToString (const signed long number, M2MString **string)
	{
	//========== Variable ==========
	M2MString tmpBuffer[128];
	size_t stringLength = 0;

	//===== Check argument =====
	if (string!=NULL)
		{
		//===== Initialize array =====
		memset(tmpBuffer, 0, sizeof(tmpBuffer));
		//===== Convert from long to string =====
		if (snprintf(tmpBuffer, sizeof(tmpBuffer), "%ld", number)>=0
				&& (stringLength=M2MString_length(tmpBuffer))>0)
			{
			//===== Get heap memory for copying =====
			if (((*string)=(unsigned char *)M2MHeap_malloc(stringLength+1))!=NULL)
				{
				memcpy((*string), tmpBuffer, stringLength);
				return (*string);
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
 * 引数で指定された文字列を実数に変換する。<br>
 *
 * @param string		実数を示す文字列
 * @param stringLength	実数を示す文字列のサイズ[Byte]
 * @return				文字列から変換した実数
 */
double M2MString_convertFromStringToDouble (const M2MString *string, const size_t stringLength)
	{
	//========== Variable ==========
	unsigned char STRING_ARRAY[stringLength+1];

	//===== Check argument =====
	if (stringLength<=M2MString_length(string))
		{
		//===== 変換バッファに文字列をコピー =====
		memset(STRING_ARRAY, 0, sizeof(STRING_ARRAY));
		memcpy(STRING_ARRAY, string, stringLength);
		//===== 文字列を実数に変換 =====
		return strtod(STRING_ARRAY, NULL);
		}
	//===== Argument error =====
	else
		{
		return (double)-1;
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
	//========== Variable ==========
	M2MString STRING_ARRAY[stringLength+1];

	//===== Check argument =====
	if (stringLength<=M2MString_length(string))
		{
		//===== 変換バッファに文字列をコピー =====
		memset(STRING_ARRAY, 0, sizeof(STRING_ARRAY));
		memcpy(STRING_ARRAY, string, stringLength);
		//===== 文字列を整数に変換 =====
		return atoi(STRING_ARRAY);
		}
	//===== Argument error =====
	else
		{
		return (int)-1;
		}
	}


/**
 * Convert the argument string to integer number.<br>
 *
 * @param[in] string		String indicating signed integer
 * @param[in] stringLength	Size of string[Byte]
 * @return					Integer converted from string
 */
signed int M2MString_convertFromStringToSignedInteger (const M2MString *string, const size_t stringLength)
	{
	//========== Variable ==========
	M2MString STRING_ARRAY[stringLength+1];

	//===== Check argument =====
	if (stringLength<=M2MString_length(string))
		{
		//===== 変換バッファに文字列をコピー =====
		memset(STRING_ARRAY, 0, sizeof(STRING_ARRAY));
		memcpy(STRING_ARRAY, string, stringLength);
		//===== 文字列を整数に変換 =====
		return atoi(STRING_ARRAY);
		}
	//===== Argument error =====
	else
		{
		return (int)-1;
		}
	}


/**
 * 2つの文字列を比較し, 同一であるかどうかを確認する。<br>
 *
 * @param[in] one		比較対象の文字列
 * @param[in] another	比較対象のもう一つの文字列
 * @param[in] length	比較対象の文字列の長さ[バイト]
 * @return				true : 同一の場合, false : 異なる場合
 */
bool M2MString_equals (const M2MString *one, const M2MString *another, const size_t length)
	{
	//===== Check argument =====
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
	//===== Argument error =====
	else
		{
		return false;
		}
	}


/**
 * 引数で指定された”buffer”配列にローカルカレンダーの時間を示す文字列をコピー<br>
 * する。<br>
 * なお, この関数内部でバッファリングは行わないため, 呼び出し側でメモリ領域を<br>
 * 確保する事。<br>
 *
 * @param[out] buffer		ローカルカレンダーの時間を示す文字列をコピーするためのバッファ
 * @param[in] bufferLength	引数で指定されたバッファの大きさを示す整数[バイト]
 * @return					バッファにコピーした, ローカル時間を示す文字列の大きさを示す整数[バイト]
 */
unsigned int M2MString_getLocalTime (M2MString *buffer, const unsigned int bufferLength)
	{
	//========== Variable ==========
	struct timeval currentTime;
	struct tm *localCalendar = NULL;
	M2MString *miliSecond = NULL;
	unsigned int miliSecondLength = 0;

	//===== Check argument =====
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
			if (M2MString_convertFromSignedLongToString((currentTime.tv_usec/1000), &miliSecond)!=NULL)
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
				//===== Error handling =====
				else
					{
					//===== ヒープメモリ領域の解放 =====
					M2MHeap_free(miliSecond);
					return 0;
					}
				}
			//===== Error handling =====
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
	//===== Argument error =====
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
 * 引数で指定された文字列の中からキーワードを検索し, 発見した場合は当該キーワード<br>
 * の開始位置を示すポインタを返す。<br>
 * 検索に失敗した場合は NULL を返す。<br>
 *
 * @param[in] string	検索対象の文字列
 * @param[in] keyword	キーワード文字列
 * @return				検索対象の文字列のうち, キーワード開始位置を示すポインタ or NULL（エラーの場合）
 */
M2MString *M2MString_indexOf (const M2MString *string, const M2MString *keyword)
	{
	//===== Check argument =====
	if (string!=NULL && keyword!=NULL)
		{
		//===== キーワード検索結果を返す =====
		return (M2MString *)strstr(string, keyword);
		}
	//===== Argument error =====
	else
		{
		return NULL;
		}
	}


/**
 * 引数で指定された比較対象の文字列の中に, 検出用の文字列が最後に現れる場所を<br>
 * 返す。<br>
 * 検出用文字列が見つからなかった場合は NULL を返す。<br>
 *
 * @param string	比較対象の文字列
 * @param fromIndex	検出用文字列
 * @return			検出された位置のポインタ or NULL（検出出来なかった場合）
 */
M2MString *M2MString_lastIndexOf (const M2MString *string, const M2MString *fromIndex)
	{
	//========== Variable ==========
	M2MString *lastIndex = NULL;
	M2MString *index = (M2MString *)string;

	//===== Check argument =====
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
	//===== Argument error =====
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
	//===== Check argument =====
	if (self!=NULL)
		{
		return strlen(self);
		}
	//===== Argument error =====
	else
		{
		return 0;
		}
	}


/* End Of File */
