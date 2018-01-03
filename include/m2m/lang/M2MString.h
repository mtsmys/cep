/*******************************************************************************
 * M2MString.h
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

#ifndef M2M_LANG_M2MSTRING_H_
#define M2M_LANG_M2MSTRING_H_


#include "m2m/io/M2MHeap.h"
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <stdbool.h>



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



/*******************************************************************************
 * Class variable
 ******************************************************************************/
/**
 * String type（＝unsigned char）
 */
#ifndef M2MString
typedef unsigned char M2MString;
#endif /* M2MString */


/**
 * COMMA（＝","）
 */
#ifndef M2MString_COMMA
#define M2MString_COMMA (unsigned char *)","
#endif /* M2MString_COMMA */


/**
 * Line feed and carriage return（＝"\r\n"）
 */
#ifndef M2MString_CRLF
#define M2MString_CRLF (unsigned char *)"\r\n"
#endif /* M2MString_CRLF */


/**
 * Double quotation（＝"\"")
 */
#ifndef M2MString_DOUBLE_QUOTATION
#define M2MString_DOUBLE_QUOTATION (unsigned char *)"\""
#endif /* M2MString_DOUBLE_QUOTATION */


/**
 * Line feed（＝"\n"）
 */
#ifndef M2MString_LF
#define M2MString_LF (unsigned char *)"\n"
#endif /* M2MString_LF */


/**
 * Question mark（＝"?")
 */
#ifndef M2MString_QUESTION_MARK
#define M2MString_QUESTION_MARK (unsigned char *)"?"
#endif /* M2MString_QUESTION_MARK */


/**
 * Quotation（＝"'")
 */
#ifndef M2MString_QUOTATION
#define M2MString_QUOTATION (unsigned char *)"'"
#endif /* M2MString_QUOTATION */


/**
 * White space（＝" "）
 */
#ifndef M2MString_SPACE
#define M2MString_SPACE (unsigned char *)" "
#endif /* M2MString_SPACE */



/*******************************************************************************
 * Public method
 ******************************************************************************/
/**
 * This function adds "string" into after the "self" string.<br>
 * Memory allocation is executed in this function, so caller must release<br>
 * the memory of "self" string.<br>
 *
 * @param[in,out] self	The original string or NULL（"self" = "self + string")
 * @param[in] string	additional string
 * @return				Pointer of connected string or NULL (in case of error)
 */
M2MString *M2MString_append (M2MString **self, const M2MString *string);


/**
 * Add the string after the "self" string. <br>
 * The length [Byte] of the additional string is specified by argument.<br>
 *
 * @param[in,out] self		The original string to be added to the string (the string after addition is self = self + string)
 * @param[in] string		String to be added
 * @param[in] stringLength	Length of the string to be added[Byte]
 * @return					Pointer of the buffer to which the string was added or NULL (in case of error)
 */
M2MString *M2MString_appendLength (M2MString **self, const M2MString *string, const size_t stringLength);


/**
 * Compares the two strings specified by the argument and returns the result.<br>
 *
 * @param[in] self		The original string to be compared
 * @param[in] string	Another string to be compared
 * @return				0：two are equal, negative：In case of self < string, positive：In case of self > string
 */
signed int M2MString_compareTo (const M2MString *self, const M2MString *string);


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
M2MString *M2MString_convertFromDoubleToString (const double number, M2MString **string);


/**
 * 引数で指定された文字列に対し，改行コードをLFからCRLFに変換して返す．<br>
 *
 * @param[in] self		The original string to convert line feed code
 * @param[out] string	改行コードを補正したCSV形式の文字列を格納するポインタ（バッファリング自体は関数内部で実行)
 * @return				改行コードを補正したCSV形式の文字列ポインタ or NULL（エラーの場合)
 */
M2MString *M2MString_convertFromLFToCRLF (const M2MString *self, M2MString **string);


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
M2MString *M2MString_convertFromSignedIntegerToString (const signed int number, M2MString **string);


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
M2MString *M2MString_convertFromSignedLongToString (const signed long number, M2MString **string);


/**
 * 引数で指定された文字列を実数に変換する。<br>
 *
 * @param string		実数を示す文字列
 * @param stringLength	実数を示す文字列のサイズ[Byte]
 * @return				文字列から変換した実数
 */
double M2MString_convertFromStringToDouble (const M2MString *string, const size_t stringLength);


/**
 * 引数で指定された文字列をロング整数に変換する。<br>
 *
 * @param string		ロング整数を示す文字列
 * @param stringLength	ロング整数を示す文字列のサイズ[Byte]
 * @return				文字列から変換したロング整数
 */
long M2MString_convertFromStringToLong (const M2MString *string, const size_t stringLength);


/**
 * Convert the argument string to integer number.<br>
 *
 * @param[in] string		String indicating signed integer
 * @param[in] stringLength	Size of string[Byte]
 * @return					Integer converted from string
 */
signed int M2MString_convertFromStringToSignedInteger (const M2MString *string, const size_t stringLength);


/**
 * 2つの文字列を比較し, 同一であるかどうかを確認する。<br>
 *
 * @param[in] one		比較対象の文字列
 * @param[in] another	比較対象のもう一つの文字列
 * @param[in] length	比較対象の文字列の長さ[バイト]
 * @return				true : 同一の場合, false : 異なる場合
 */
bool M2MString_equals (const M2MString *one, const M2MString *another, const size_t length);


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
unsigned int M2MString_getLocalTime (M2MString *buffer, const unsigned int bufferLength);


/**
 * 引数で指定された文字列の中からキーワードを検索し, 発見した場合は当該キーワード<br>
 * の開始位置を示すポインタを返す。<br>
 * 検索に失敗した場合は NULL を返す。<br>
 *
 * @param[in] string	検索対象の文字列
 * @param[in] keyword	キーワード文字列
 * @return				検索対象の文字列のうち, キーワード開始位置を示すポインタ or NULL（エラーの場合）
 */
M2MString *M2MString_indexOf (const M2MString *string, const M2MString *keyword);


/**
 * 引数で指定された比較対象の文字列の中に, 検出用の文字列が最後に現れる場所を<br>
 * 返す。<br>
 * 検出用文字列が見つからなかった場合は NULL を返す。<br>
 *
 * @param string	比較対象の文字列
 * @param fromIndex	検出用文字列
 * @return			検出された位置のポインタ or NULL（検出出来なかった場合）
 */
M2MString *M2MString_lastIndexOf (const M2MString *string, const M2MString *fromIndex);


/**
 * 引数で指定された文字列の長さを取得して返す。<br>
 *
 * @param[in] string	文字列（必ずヌル終端とする事）
 * @return				文字列の長さ or 0（エラーの場合）
 */
size_t M2MString_length (const M2MString *string);


/**
 * 引数で指定されたソース文字列（＝"string"）に対し, 分割用文字列（＝"delimiter"）<br>
 * で分割した結果を返す。<br>
 * 連続して取得する場合, 初回のみソース文字列を指定し, 次回以降はソース文字列に<br>
 * NULLを指定する。<br>
 * なお, 当該処理を実行すると, ソース文字列自体が書き換えられるため, 壊されてもよい<br>
 * 文字列をソース文字列に指定する事。<br>
 *
 * @param[in] string		分割処理対象のソース文字列
 * @param[in] delimiter		分割文字列
 * @param[in] savePointer	分割処理が施されたソース文字列のコピー用バッファ（毎回同じ変数を渡す事）
 * @return					分割されたソース文字列（の断片に該当する文字列）
 *
M2MString *M2MString_split (M2MString *string, const M2MString *delimiter, M2MString **savePoint);
*/


#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2M_LANG_M2MSTRING_H_ */
