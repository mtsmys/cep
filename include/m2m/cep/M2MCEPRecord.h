/*******************************************************************************
 * M2MCEPRecord.h
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

#ifndef M2M_CEP_M2MCEPRECORD_H_
#define M2M_CEP_M2MCEPRECORD_H_


#include "m2m/util/logging/M2MLogger.h"
#include "m2m/util/list/M2MList.h"



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



/*******************************************************************************
 * Definition
 ******************************************************************************/
/**
 * CEP用レコード管理オブジェクト。<br>
 * （レコード挿入対象である）テーブル名毎にノードを分割し、リスト構造として連結<br>
 * する。<br>
 * なお、同一テーブル名のレコード情報を示す構造体オブジェクトは、M2MCEP構造体の<br>
 * メンバ変数によって、その保持可能な最大数が規定される。<br>
 *
 * @param previousRecord	1つ前のレコード管理オブジェクトのポインタ（先頭の場合は自分のポインタを示す）
 * @param nextRecord		1つ後ろのレコード管理オブジェクトのポインタ（末端の場合はNULLを示す）
 * @param tableName			テーブル名を示す文字列
 * @param columnName		テーブルのフィールド名を示すCSV形式の文字列（＝[フィールド名,フィールド名,フィールド名・・・]）
 * @param newRecordList 	新規に挿入したレコード情報（文字列）を示す構造体オブジェクト（＝[CSV1行 -> CSV1行 -> CSV1行・・・]）
 * @param newRecordList 	過去に挿入されたレコード情報（文字列）を示す構造体オブジェクト（＝[CSV1行 -> CSV1行 -> CSV1行・・・]）．過去データより昇順
 */
#ifndef M2MCEPRecord
typedef struct M2MCEPRecord
	{
	struct M2MCEPRecord *previous;
	struct M2MCEPRecord *next;
	M2MString *tableName;
	M2MString *columnName;
	M2MList *newRecordList;
	M2MList *oldRecordList;
	} M2MCEPRecord;
#endif /* M2MCEPRecord */



/*******************************************************************************
 * Public function
 ******************************************************************************/
/**
 * 引数で指定されたレコード管理オブジェクトから先頭のレコード管理オブジェクト<br>
 * を取得して返す．<br>
 * 【注意】<br>
 * 先頭のレコード管理オブジェクトは1つ前のポインタが自分と同じアドレスを示す．<br>
 *
 * @param[in] self	レコード管理オブジェクト
 * @return			先頭のレコード管理オブジェクト
 */
M2MCEPRecord *M2MCEPRecord_begin (M2MCEPRecord *self);


/**
 * 引数で指定されたレコード管理オブジェクトのメモリ領域を全て解放する．<br>
 *
 * @param[in,out] self	メモリ領域解放対象のレコード管理オブジェクト
 */
void M2MCEPRecord_delete (M2MCEPRecord **self);


/**
 * 引数で指定されたレコード管理オブジェクトがメンバ変数として保持するカラム名<br>
 * を示すCSV形式の文字列を返す．<br>
 *
 * @param[in] self		レコード管理オブジェクト
 * @return				引数で指定されたレコード管理オブジェクトが保持するカラム名を示すCSV形式の文字列 or NULL（エラーの場合）
 */
M2MString *M2MCEPRecord_getColumnName (const M2MCEPRecord *self);


/**
 * 引数で指定されたレコード管理オブジェクトがメンバ変数として保持する、新規に<br>
 * 挿入されたレコードリストを返す．<br>
 *
 * @param[in] self		レコード管理オブジェクト
 * @return				引数で指定されたレコード管理オブジェクトが保持する、新規に挿入された（未だメモリーDB未挿入の）レコードリスト
 */
M2MList *M2MCEPRecord_getNewRecordList (const M2MCEPRecord *self);


/**
 * 引数で指定されたレコード管理オブジェクトがメンバ変数として保持する、過去に<br>
 * 挿入されたレコードリストを返す．<br>
 *
 * @param[in] self		レコード管理オブジェクト
 * @return				引数で指定されたレコード管理オブジェクトが保持する、過去に挿入された（メモリーDB挿入済みの）レコードリスト
 */
M2MList *M2MCEPRecord_getOldRecordList (const M2MCEPRecord *self);


/**
 * 引数で指定されたレコード管理オブジェクトがメンバ変数として保持するテーブル名<br>
 * 文字列を返す．<br>
 *
 * @param[in] self		レコード管理オブジェクト
 * @return				引数で指定されたレコード管理オブジェクトが保持するテーブル名
 */
M2MString *M2MCEPRecord_getTableName (const M2MCEPRecord *self);


/**
 * 引数で指定されたレコード管理オブジェクトの“新規に挿入したレコード情報”<br>
 * 構造体オブジェクトのレコード情報を、“過去に挿入されたレコード情報”構造体<br>
 * オブジェクトへコピーする。<br>
 * この操作は、メモリ上のSQLite3データベースへレコードを挿入した後、ファイル上の<br>
 * SQLite3データベースへ永続化のためデータを整理するために実行する。<br>
 *
 * @param[in,out] self
 */
void M2MCEPRecord_moveFromNewRecordListToOldRecordList (M2MCEPRecord *self);


/**
 * （ヒープ）メモリ領域を新規に獲得し，レコード管理オブジェクトを生成する．<br>
 *
 * @return	新規に生成したレコード管理オブジェクト
 */
M2MCEPRecord *M2MCEPRecord_new ();


/**
 * 引数で指定されたレコード管理オブジェクトがメンバ変数として保持する，1つ後ろの<br>
 * レコード管理オブジェクトを返す．<br>
 *
 * @param[in] self		レコード管理オブジェクト
 * @return				引数で指定されたレコード管理オブジェクトが保持する，1つ後ろのレコード管理オブジェクト
 */
M2MCEPRecord *M2MCEPRecord_next (const M2MCEPRecord *self);


/**
 * 引数で指定されたレコード管理オブジェクトがメンバ変数として保持する，1つ手前の<br>
 * レコード管理オブジェクトを返す．<br>
 *
 * @param[in] self		レコード管理オブジェクト
 * @return				引数で指定されたレコード管理オブジェクトが保持する，1つ手前のレコード管理オブジェクト
 */
M2MCEPRecord *M2MCEPRecord_previous (const M2MCEPRecord *self);


/**
 * 引数で指定されたテーブル名に該当するレコード管理オブジェクトだけ，メモリ領域<br>
 * を解放する（ポインタで接続された他のレコード管理オブジェクトは存続する）．<br>
 *
 * @param[in,out] self		レコード管理オブジェクト
 * @param[in] tableName		削除対象のレコード管理オブジェクトが保有するテーブル名を示す文字列
 */
void M2MCEPRecord_remove (M2MCEPRecord *self, const M2MString *tableName);


/**
 * 引数で指定されたテーブル名をメンバ変数に持つレコード管理オブジェクトに対し，<br>
 * CSV形式の文字列データをリスト構造体オブジェクトに格納する．<br>
 *
 * @param[in,out] self		レコード管理オブジェクト
 * @param[in] tableName		テーブル名を示す文字列
 * @param[in] csv			CSV形式の文字列データ（1行目にカラム名を示すヘッダ，2行目以降がデータとなる）
 * @return					オブジェクトに格納したレコード数[個] or -1（エラーの場合）
 */
int M2MCEPRecord_setCSV (M2MCEPRecord *self, const M2MString *tableName, const M2MString *csv);


/**
 * 引数で指定されたレコード管理オブジェクトに対し，1つ後ろのレコード管理<br>
 * オブジェクトをセットする．<br>
 *
 * @param[in] self			レコード管理オブジェクト
 * @param[in] nextRecord	1つ後ろのレコード管理オブジェクト
 */
void M2MCEPRecord_setNextRecord (M2MCEPRecord *self, M2MCEPRecord *nextRecord);


/**
 * 引数で指定されたレコード管理オブジェクトに対し，1つ手前のレコード管理<br>
 * オブジェクトをセットする．<br>
 *
 * @param[in,out] self			レコード管理オブジェクト
 * @param[in] previousRedord	1つ手前のレコード管理オブジェクト
 */
void M2MCEPRecord_setPreviousRecord (M2MCEPRecord *self, M2MCEPRecord *previousRecord);


/**
 * 引数で指定されたレコード管理オブジェクトの要素数を返す。<br>
 *
 * @param[in] self	レコード管理オブジェクト
 * @return			要素数
 */
unsigned int M2MCEPRecord_size (M2MCEPRecord *self);



#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2M_CEP_M2MCEPRECORD_H_ */
