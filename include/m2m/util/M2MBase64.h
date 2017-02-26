/*******************************************************************************
 * M2MBase64.h
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

#ifndef M2MBASE64_H_
#define M2MBASE64_H_


#include "m2m/lang/M2MString.h"



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



/*******************************************************************************
 * Class variable
 ******************************************************************************/
/**
 * 4[バイト]の文字列
 */
#ifndef M2MBase64_FOUR_WORD
#define M2MBase64_FOUR_WORD (unsigned int)4
#endif /* M2MBase64_FOUR_WORD */


/**
 * 3[バイト]のデータサイズ
 */
#ifndef M2MBase64_THREE_OCTET
#define M2MBase64_THREE_OCTET (unsigned int)3
#endif /* M2MBase64_THREE_OCTET */


/**
 * （改行を毎回添付する際の)1行の最大の文字列数
 */
#ifndef M2MBase64_CHUNK_LENGTH
#define M2MBase64_CHUNK_LENGTH (unsigned int)76
#endif /* M2MBase64_CHUNK_LENGTH */


/*******************************************************************************
 * Public method
 ******************************************************************************/
/**
 * 引数で指定された文字列をBase64逆変換でバイナリデータに変換する。<br>
 * 逆変換したバイナリデータのバッファリングはこの関数内部で実行するため，<br>
 * 呼び出し側では当該バイナリデータ使用後、メモリーリークを防止するため、必ず<br>
 * "M2MHeap_free()"関数を呼び出す事。<br>
 *
 * @param string		Target string for Base64 decoding
 * @param stringLength	Length of string[Byte]
 * @param buffer		Buffer for copying decoded data Base64(buffering is executed in this function)
 * @return				Length of decoded data[Byte]
 */
size_t M2MBase64_decode (const M2MString *string, const unsigned long stringLength, unsigned char **buffer);


/**
 * 引数で指定されたバイナリデータをBase64変換で文字列に変換する。<br>
 * 変換した文字列のバッファリングはこの関数内部で実行するため，呼び出し側では<br>
 * 当該文字列使用後、メモリーリークを防止するため必ず"M2MHeap_free()"関数を<br>
 * 呼び出す事。<br>
 * また，変換した文字列を76文字毎に改行コード（"\r\m")で区切りたい場合、引数の<br>
 * "chunk"に true をセットする。<br>
 * 変換した文字列全てを1行に収めたい場合は "chunk" に false をセットする。<br>
 *
 * @param src		Base64変換対象のバイナリーデータ
 * @param srcLength	Base64変換対象のバイナリーデータサイズ[Byte]
 * @param string	Base64で変換された文字列の格納ポインタ（バッファリング自体は関数内で実行する）
 * @param chunk		true : 76文字毎に改行コード（\r\n)を挿入する，false : 改行コードを挿入しない
 * @return			Base64で変換された文字列のポインタ or NULL（エラーの場合)
 */
M2MString *M2MBase64_encode (const unsigned char *src, const size_t srcLength, M2MString **string, const bool chunk);



#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2MBASE64_H_ */
