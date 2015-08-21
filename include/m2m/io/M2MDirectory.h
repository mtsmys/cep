/*******************************************************************************
 * M2MDirectory.h
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

#ifndef M2MDIRECTORY_H_
#define M2MDIRECTORY_H_


#include "m2m/lang/M2MString.h"
#include <dirent.h>
#include <fts.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



/*******************************************************************************
 * クラス変数
 ******************************************************************************/
/**
 * M2M処理に関わるディレクトリ
 */
#ifndef M2MDIRECTORY
#define M2MDIRECTORY (unsigned char *)".m2m"
#endif /* M2MDIRECTORY */


/**
 * パス分割文字(＝"/")<br>
 */
#ifndef M2MDirectory_SEPARATOR
#define M2MDirectory_SEPARATOR (unsigned char *)"/"
#endif /* M2MDirectory_SEPARATOR */



/*******************************************************************************
 * 公開関数
 ******************************************************************************/
/**
 * 引数で指定されたパスのディレクトリが存在するかどうか確認する。<br>
 *
 * @param directoryPath	存在を確認するディレクトリの絶対パス
 * @return				true : ディレクトリが存在, false : ディレクトリが存在しない
 */
bool M2MDirectory_exists (const M2MString *directoryPath);


/**
 * 環境変数で設定されているホームディレクトリを示すパス文字列を返す。<br>
 *
 * @return 			ホームディレクトリを示すパス文字列
 */
M2MString *M2MDirectory_getHomeDirectoryPath ();


/**
 * 引数で指定されたパスがディレクトリであるかどうかを確認する。<br>
 *
 * @param directoryPath	ディレクトリパスを示す文字列
 * @return				true : ディレクトリの場合, false : ディレクトリでない場合
 */
bool M2MDirectory_isDirectory (const M2MString *directoryPath);


/**
 * 引数で指定されたパスに対し、ディレクトリを作成する。<br>
 * もし、指定パスに対し、途中のディレクトリが存在しない場合はエラーとなる。<br>
 *
 * @param[in] directoryPath	ディレクトリパスを示す文字列
 * @param[in] mode			パーミッションモードを示す3桁の整数文字列（例）"533"）
 * @return					true : ディレクトリの作成に成功, false : ディレクトリの作成に失敗
 */
bool M2MDirectory_mkdir (const M2MString *directoryPath, const M2MString *mode);


/**
 * 引数で指定されたパスに対し、途中の親ディレクトリを含め、全てのディレクトリを作成<br>
 * する。<br>
 *
 * @param[in] directoryPath	ディレクトリパスを示す文字列
 * @param[in] mode			パーミッションモードを示す3桁の整数文字列（例）"533"）
 * @return					true : ディレクトリの作成に成功, false : ディレクトリの作成に失敗
 */
bool M2MDirectory_mkdirs (const M2MString *directoryPath, const M2MString *mode);



#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2MDIRECTORY_H_ */
