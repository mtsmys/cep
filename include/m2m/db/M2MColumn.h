/*******************************************************************************
 * M2MColumn.h
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

#ifndef M2MCOLUMN_H_
#define M2MCOLUMN_H_



#include "m2m/lang/M2MString.h"
#include "m2m/db/M2MDataType.h"



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



/*******************************************************************************
 * Definition
 ******************************************************************************/
/**
 * Structure object containing database column information.<br>
 *
 * @param name				String indicating column name
 * @param dataType			カラムのデータ型
 * @param primaryKey		主キー有効化のフラグ
 * @param autoIncrement		自動インクリメント有効化のフラグ
 * @param allowNULL			NULL有効化のフラグ
 * @param unique			ユニーク性有効化のフラグ
 */
#ifndef M2MColumn
typedef struct
	{
	M2MString *name;
	M2MDataType dataType;
	bool primaryKey;
	bool autoIncrement;
	bool allowNULL;
	bool unique;
	} M2MColumn;
#endif /* M2MColumn */


/*******************************************************************************
 * Public function
 ******************************************************************************/
/**
 * デストラクタ。<br>
 * 引数で指定されたカラム情報オブジェクトのヒープメモリ領域を解放し、オブジェクト<br>
 * を削除する。<br>
 *
 * @param[in,out] self	カラム情報オブジェクト
 */
void M2MColumn_delete (M2MColumn **self);


/**
 * 引数で指定されたカラム情報オブジェクトがメンバ変数として保持しているカラム名<br>
 * を示す文字列を返す。<br>
 *
 * @param[in] self	カラム情報オブジェクト
 * @return			カラム名を示す文字列 or NULL（エラーの場合）
 */
unsigned char *M2MColumn_getName (const M2MColumn *self);


/**
 * @param[in] self	カラム情報オブジェクト
 */
M2MDataType M2MColumn_getDataType (const M2MColumn *self);


/**
 * @param[in] self	カラム情報オブジェクト
 */
bool M2MColumn_getPrimaryKey (const M2MColumn *self);


/**
 * @param[in] self	カラム情報オブジェクト
 */
bool M2MColumn_getAutoIncrement (const M2MColumn *self);


/**
 * @param[in] self	カラム情報オブジェクト
 */
bool M2MColumn_getAllowNULL (const M2MColumn *self);


/**
 * @param[in] self	カラム情報オブジェクト
 */
bool M2MColumn_getUnique (const M2MColumn *self);


/**
 * コンストラクタ。<br>
 * ヒープメモリ領域を獲得し、カラム情報オブジェクトを新規に作成する。<br>
 *
 * @return	カラム情報オブジェクト
 */
M2MColumn *M2MColumn_new ();


/**
 * 引数で指定されたカラム情報オブジェクトにメンバ変数として、自動インクリメントの<br>
 * 可否を示すフラグをセットする。<br>
 *
 * @param[in,out] self		カラム情報オブジェクト
 * @param[in] autoIncrement	自動インクリメントか否かを示すフラグ
 * @return					自動インクリメント情報をセットしたカラム情報オブジェクト or NULL（エラーの場合）
 */
M2MColumn *M2MColumn_setAutoIncrement (M2MColumn *self, const bool autoIncrement);


/**
 * 引数で指定されたカラム情報オブジェクトのメンバ変数として、レコードにNULLの可否<br>
 * を示すフラグをセットする。<br>
 *
 * @param[in,out] self	カラム情報オブジェクト
 * @param[in] allowNULL	レコードにNULLを許すか否かを示すフラグ
 * @return				NULLの可否を示すフラグをセットしたカラム情報オブジェクト or NULL（エラーの場合）
 */
M2MColumn *M2MColumn_setAllowNULL (M2MColumn *self, const bool allowNULL);


/**
 * 引数で指定されたカラム情報オブジェクトのメンバ変数として、データ型を示す列挙子<br>
 * をセットする。<br>
 *
 * @param[in,out] self	カラム情報オブジェクト
 * @param[in] dataType	データ型を示す列挙子
 * @return				データ型を示す列挙子をセットしたカラム情報オブジェクト or NULL（エラーの場合）
 */
M2MColumn *M2MColumn_setDataType (M2MColumn *self, const M2MDataType dataType);


/**
 * 引数で指定されたカラム情報オブジェクトに対し、カラム名を示す文字列をメンバ変数<br>
 * としてセットする。<br>
 *
 * @param[in,out] self	カラム情報オブジェクト
 * @param[in] name		カラム名を示す文字列
 * @return				カラム名をセットされたカラム情報オブジェクト or NULL（エラーの場合）
 */
M2MColumn *M2MColumn_setName (M2MColumn *self, const M2MString *name);


/**
 * 引数で指定されたカラム情報オブジェクトのメンバ変数として、主キーか否かを示す<br>
 * フラグをセットする。<br>
 *
 * @param[in,out] self		カラム情報オブジェクト
 * @param[in] primaryKey	主キーか否かを示すフラグ
 * @return					フラグをセットしたカラム情報オブジェクト
 */
M2MColumn *M2MColumn_setPrimaryKey (M2MColumn *self, const bool primaryKey);


/**
 * 引数で指定されたカラム情報オブジェクトのメンバ変数として、レコードのユニーク性<br>
 * を示すフラグをセットする。<br>
 *
 * @param[in,out] self	カラム情報オブジェクト
 * @param[in] unique	レコードデータのユニーク性の可否を示すフラグ
 * @return				ユニーク性の可否を示すフラグがセットされたカラム情報オブジェクト or NULL（エラーの場合）
 */
M2MColumn *M2MColumn_setUnique (M2MColumn *self, const bool unique);



#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2MCOLUMN_H_ */
