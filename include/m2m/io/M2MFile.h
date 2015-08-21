/*******************************************************************************
 * M2MFile.h
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

#ifndef M2MFILE_H_
#define M2MFILE_H_


#include "m2m/io/M2MDirectory.h"



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



/*******************************************************************************
 * 公開関数
 ******************************************************************************/
/**
 * 引数で指定されたファイルを閉じる。<br>
 *
 * @param file	ファイルオブジェクト
 */
void M2MFile_close (FILE *file);


/**
 * 引数で指定されたパスのファイルが存在するかどうか確認する。<br>
 *
 * @param filePath	存在を確認するファイルの絶対パス
 * @return			true : ファイルが存在, false : ファイルが存在しない
 */
bool M2MFile_exists (const unsigned char *filePath);


/**
 * 引数で指定されたファイルのファイルディスクリプタ（iノードを示す整数）を取得する。<br>
 *
 * @param[in] file	ファイルオブジェクト
 * @return			引数で指定されたファイルのiノードを示す整数
 */
int M2MFile_getFileDescriptor (const FILE *file);


/**
 * 引数で指定されたファイルの大きさ[バイト]を返す。<br>
 *
 * @param file	ファイルオブジェクト
 * @return		ファイルの大きさを示す整数[バイト] or 0(エラーの場合)
 */
unsigned long M2MFile_length (const FILE *file);


/**
 * 引数で指定されたパスのファイルを開く。<br>
 *
 * @param filePath	ファイルパスを示す文字列
 * @param append	ファイルを開く際の追記の有無を示すフラグ
 * @return			ファイルオブジェクト or NULL（エラーの場合）
 */
FILE *M2MFile_open (const unsigned char *filePath, const bool append);


/**
 * 指定されたファイルからデータを読み取る。<br>
 *
 * @param[in] file		UNIX/Linuxファイル構造体オブジェクト
 * @param[out] data		ファイル入力用バッファ
 * @return				ファイル入力用バッファのポインタ or NULL(エラーの場合)
 */
unsigned char *M2MFile_read (const FILE *file, unsigned char **data);


/**
 * 引数で指定されたパスのファイルを削除する。<br>
 *
 * @param filePath	削除対象のファイルパスを示す文字列
 */
void M2MFile_remove (const unsigned char *filePath);


/**
 * 指定されたファイルにデータを出力する。<br>
 *
 * @param[in] file			UNIX/Linuxファイル構造体オブジェクト
 * @param[in] data			ファイル出力データ
 * @param[in] dataLength	ファイル出力データサイズ[Byte]
 * @return					true : ファイル出力に成功, false : ファイル出力に失敗
 */
bool M2MFile_write (const FILE *file, const unsigned char *data, const size_t dataLength);



#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2MFILE_H_ */
