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

#ifndef M2MSTRING_H_
#define M2MSTRING_H_


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
 * @param[in,out] self	original string or NULL（"self" = "self + string")
 * @param[in] string	additional string
 * @return				pointer of connected string or NULL(means error)
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
 * 引数で指定された2つの文字列を比較し、その結果を返す。<br>
 *
 * @param[in] self		比較対象の片方の文字列
 * @param[in] string	比較対象のもう一方の文字列
 * @return				0：2つが等しい場合、負：辞書的にself<stringの場合、正：辞書的にself>stringの場合
 */
signed int M2MString_compareTo (const M2MString *self, const M2MString *string);


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
M2MString *M2MString_convertFromDoubleToString (const double number, M2MString **string);


/**
 * 引数で指定された“int int”値を文字列に変換し、引数で指定されたポインタにコピー<br>
 * する。<br>
 * 配列のバッファリングはこの関数内部で実行するため、呼び出し側では文字列利用後、<br>
 * メモリーリークを防止するため、必ず“M2MHeap_free()”関数を呼ぶ事。<br>
 *
 * @param[in] number	文字列変換対象の整数
 * @param[out] string	変換した文字列をコピーするためのポインタ（バッファリングは関数内部で実行する）
 * @return				コピーした文字列 or NULL（エラーの場合）
 */
M2MString *M2MString_convertFromIntegerToString (const signed int number, M2MString **string);


/**
 * 引数で指定された文字列に対し，改行コードをLFからCRLFに変換して返す．<br>
 *
 * @param[in] self		改行コード変換対象の文字列
 * @param[out] string	改行コードを補正したCSV形式の文字列を格納するポインタ（バッファリング自体は関数内部で実行)
 * @return				改行コードを補正したCSV形式の文字列ポインタ or NULL（エラーの場合)
 */
M2MString *M2MString_convertFromLFToCRLF (const M2MString *self, M2MString **string);


/**
 * 引数で指定された“long int”値を文字列に変換し、引数で指定されたポインタにコピー<br>
 * する。<br>
 * 配列のバッファリングはこの関数内部で実行するため、呼び出し側では文字列利用後、<br>
 * メモリーリークを防止するため、必ず“M2MHeap_free()”関数を呼ぶ事。<br>
 *
 * @param[in] number	文字列変換対象の整数
 * @param[out] string	変換した文字列をコピーするためのポインタ（バッファリングは関数内部で実行する）
 * @return				コピーした文字列 or NULL（エラーの場合）
 */
M2MString *M2MString_convertFromLongToString (const signed long number, M2MString **string);


/**
 * 引数で指定された文字列を実数に変換する。<br>
 *
 * @param string		実数を示す文字列
 * @param stringLength	実数を示す文字列のサイズ[Byte]
 * @return				文字列から変換した実数
 */
double M2MString_convertFromStringToDouble (const M2MString *string, const size_t stringLength);


/**
 * 引数で指定された文字列を整数に変換する。<br>
 *
 * @param string		整数を示す文字列
 * @param stringLength	整数を示す文字列のサイズ[Byte]
 * @return				文字列から変換した整数
 */
int M2MString_convertFromStringToInteger (const M2MString *string, const size_t stringLength);


/**
 * 引数で指定された文字列をロング整数に変換する。<br>
 *
 * @param string		ロング整数を示す文字列
 * @param stringLength	ロング整数を示す文字列のサイズ[Byte]
 * @return				文字列から変換したロング整数
 */
long M2MString_convertFromStringToLong (const M2MString *string, const size_t stringLength);


/**
 * 2つの文字列を比較し、同一であるかどうかを確認する。<br>
 *
 * @param[in] one		比較対象の文字列
 * @param[in] another	比較対象のもう一つの文字列
 * @param[in] length	比較対象の文字列の長さ[バイト]
 * @return				true : 同一の場合、false : 異なる場合
 */
bool M2MString_equals (const M2MString *one, const M2MString *another, const size_t length);


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
unsigned int M2MString_getLocalTime (M2MString *buffer, const unsigned int bufferLength);


/**
 * 引数で指定された文字列の中からキーワードを検索し、発見した場合は当該キーワード<br>
 * の開始位置を示すポインタを返す。<br>
 * 検索に失敗した場合は NULL を返す。<br>
 *
 * @param[in] string	検索対象の文字列
 * @param[in] keyword	キーワード文字列
 * @return				検索対象の文字列のうち、キーワード開始位置を示すポインタ or NULL（エラーの場合）
 */
M2MString *M2MString_indexOf (const M2MString *string, const M2MString *keyword);


/**
 * 引数で指定された比較対象の文字列の中に、検出用の文字列が最後に現れる場所を<br>
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
 * 引数で指定されたソース文字列（＝"string"）に対し、分割用文字列（＝"delimiter"）<br>
 * で分割した結果を返す。<br>
 * 連続して取得する場合、初回のみソース文字列を指定し、次回以降はソース文字列に<br>
 * NULLを指定する。<br>
 * なお、当該処理を実行すると、ソース文字列自体が書き換えられるため、壊されてもよい<br>
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



#endif /* M2MSTRING_H_ */
