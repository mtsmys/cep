/*******************************************************************************
 * M2MLogger.h
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

#ifndef M2MLOGGER_H_
#define M2MLOGGER_H_


#include "m2m/io/M2MFile.h"



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



/*******************************************************************************
 * クラス変数
 ******************************************************************************/
/**
 * ログファイルの規程最大サイズ（8,388,608[Byte]）
 */
#ifndef M2MLogger_MAX_LOG_FILE_LENGTH
#define M2MLogger_MAX_LOG_FILE_LENGTH (unsigned int)8388608
#endif


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
void M2MLogger_printDebugMessage (const M2MString *methodName, const unsigned int lineNumber, const M2MString *message);


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
void M2MLogger_printErrorMessage (const M2MString *methodName, const unsigned int lineNumber, const M2MString *message, M2MString **bufferForCopyingErrorMessage);



#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2MLOGGER_H_ */
