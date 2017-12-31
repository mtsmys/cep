/*******************************************************************************
 * M2MCEP.h
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

#ifndef M2MCEP_H_
#define M2MCEP_H_


#include "m2m/cep/M2MCEPRecord.h"
#include "m2m/cep/M2MTableBuilder.h"
#include "m2m/util/M2MBase64.h"
#include <signal.h>



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



/*******************************************************************************
 * Definition
 ******************************************************************************/
/**
 * CEP処理に関わるディレクトリ
 */
#ifndef M2MCEP_DIRECTORY
#define M2MCEP_DIRECTORY (unsigned char *)".m2m/cep"
#endif /* M2MCEP_DIRECTORY */


/**
 * CEP(Complex Event Processing)実行オブジェクト<br>
 *
 * @param databaseName		データベース名を示す文字列
 * @param tableBuilder		テーブル構築オブジェクト
 * @param memoryDatabase	メモリ上のSQLite3データベース
 * @param fileDatabase		ファイル上のSQLite3データベース
 * @param record			レコード情報オブジェクト
 * @param maxRecord			メモリ上のテーブルの最大レコード数[行]
 * @param vacuumRecord		SQLite3データベースに対してバキューム処理を実行するレコード数（0の場合は自動バキューム設定)[行]
 * @param recordCounter		SQLite3データベースに対してバキューム処理を実行するためのレコード数カウンタ[行]
 * @param persistence		ファイル上のSQLite3データベースへの永続性可否を示すフラグ
 */
#ifndef M2MCEP
typedef struct
	{
	M2MString *databaseName;
	M2MTableBuilder *tableBuilder;
	sqlite3 *memoryDatabase;
	sqlite3 *fileDatabase;
	M2MCEPRecord *record;
	unsigned int maxRecord;
	unsigned int vacuumRecord;
	unsigned int recordCounter;
	bool persistence;
	} M2MCEP;
#endif /* M2MCEP */



/*******************************************************************************
 * Public function
 ******************************************************************************/
/**
 * 引数で指定されたCEP構造体オブジェクトのメモリ領域を解放する．<br>
 * この関数呼び出し後は引数で師弟されたCEP構造体オブジェクトにアクセス出来なく<br>
 * なる事に注意する．<br>
 *
 * @param[in,out] self	メモリ領域解放対象のCEP構造体オブジェクト
 */
void M2MCEP_delete (M2MCEP **self);


/**
 * 引数で指定されたCSV形式の文字列データをCEP用SQLite3データベースのテーブルに<br>
 * 挿入する．<br>
 * 内部の処理手順は下記の通りである．<br>
 * <br>
 * 1) CSV形式の文字列をパースし，CEPRecord構造体オブジェクトに取り込む．<br>
 * 2) 引数で指定されたテーブルにデータを挿入する．
 * 3) テーブルの現在のレコード数を確認し，規定値を超過した場合は古いレコードから<br>
 *    削除する．<br>
 * 4) 削除したレコードと同じデータをCEPRecord構造体オブジェクトから取得し，<br>
 *    永続化のためにファイルに記録しているSQLite3データベースに挿入する．<br>
 * 5) CEPRecord構造体オブジェクトのレコードから超過分を削除する．<br>
 *
 * @param[in,out] self	CEP構造体オブジェクト
 * @param[in] tableName	データ挿入対象のテーブル名
 * @param[in] csv		挿入データであるCSV形式の文字列（1行目はヘッダとしてカラム名を指定する事）
 * @return				CEP用SQLite3データベースに挿入したレコードの行数[行] or 0（エラーの場合）
 */
int M2MCEP_insertCSV (M2MCEP *self, const M2MString *tableName, const M2MString *csv);


/**
 * CEP構造体オブジェクトを新規作成し、SQLite3データベース処理のための準備を行う．<br>
 * テーブルについては、メモリ上のSQLite3データベースは"M2MCEP"オブジェクトが作成<br>
 * される度に毎回構築する必要があるため、必ず指定する事。<br>
 *
 * @param[in] databaseName	SQLite3データベース名
 * @param[in] tableBuilder	SQLite3データベースのテーブルを構築するためのオブジェクト
 * @return					CEP構造体オブジェクト or NULL（エラーの場合）
 */
M2MCEP *M2MCEP_new (const M2MString *databaseName, const M2MTableBuilder *tableBuilder);


/**
 * メモリ上のSQLite3データベースに対し，引数で指定されたSQL文を実行し，その結果を<br>
 * CSV形式の文字列で返す．<br>
 * 実行結果であるCSV形式の文字列は，1行めにカラム名文字列，2行目以降がデータとなる．<br>
 *
 * @param[in] self		CEP構造体オブジェクト（データベース管理に使用する）
 * @param[in] sql		SQL文を示す文字列
 * @param[out] result	実行結果のCSV文字列 or NULL（エラーの場合）
 * @return
 */
unsigned char *M2MCEP_select (M2MCEP *self, const M2MString *sql, M2MString **result);


/**
 * メモリに存在するSQLite3データベース内の各テーブルに格納可能な最大レコード数を<br>
 * 引数で指定された整数値に変更する．<br>
 *
 * @param[in,out] self		最大レコード数変更対象のCEP実行オブジェクト
 * @param[in] maxRecord		1つのテーブルにおける最大レコード数を示す整数 or 0（エラーの場合）
 * @return					最大レコード数を設定したCEP実行オブジェクト or 0（エラーの場合）
 */
M2MCEP *M2MCEP_setMaxRecord (M2MCEP *self, const unsigned int maxRecord);


/**
 * 引数で指定されたCEP構造体オブジェクトに対し、永続性（ファイル上のSQLite3<br>
 * データベースへの記録)の可否を示すフラグをセットする．<br>
 * true を設定した場合、M2MCEP_insertCSV() で挿入されたレコードは全て、<br>
 * ファイル上のSQLite3データベースへ記録する．<br>
 * false を設定した場合、ファイル上のSQLite3データベースへの記録は実行しない．<br>
 *
 * @param[in,out] self		CEP構造体オブジェクト
 * @param[in] persistence	永続性（ファイル上のSQLite3データベースへの記録)の可否を示すフラグ
 * @return					永続性の可否を示すフラグを設定したCEP実行オブジェクト or NULL（エラーの場合）
 */
M2MCEP *M2MCEP_setPersistence (M2MCEP *self, const bool persistence);


/**
 * SQLite3データベースのバキューム処理を行うレコード数を設定する．<br>
 * このバキューム処理設定については，メモリ上のSQLite3データベースとファイル上の
 * SQLite3データベース共用の設定となる．<br>
 *
 * @param self			バキューム処理のレコード数設定対象のCEP実行オブジェクト
 * @param vacuumRecord	バキューム処理のレコード数を示す整数値（0を指定した場合は自動バキューム設定)
 * @return				バキューム処理を行うレコード数を設定したCEP実行オブジェクト or NULL（エラーの場合）
 */
M2MCEP *M2MCEP_setVacuumRecord (M2MCEP *self, const unsigned int vacuumRecord);



#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2MCEP_H_ */
