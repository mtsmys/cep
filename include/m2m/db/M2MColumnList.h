/*******************************************************************************
 * M2MColumnList.h
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

#ifndef M2M_DB_M2MCOLUMNLIST_H_
#define M2M_DB_M2MCOLUMNLIST_H_


#include "m2m/lang/M2MString.h"
#include "m2m/db/M2MColumn.h"



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



/*******************************************************************************
 * クラス変数
 ******************************************************************************/
/**
 * データベースのテーブルのカラム情報を格納した構造体オブジェクト。<br>
 * リスト構造になっており、複数のカラム情報オブジェクトがリンクで繋がれた構造と<br>
 * なっている。<br>
 *
 * @param previous	A list structure object located before (indicating its own pointer in the case of the head)
 * @param next		A list structure object located behind (indicating NULL in the case of the end)
 * @param column	カラム情報オブジェクト（カラム1つのデータ型を内包する構造体）
 */
#ifndef M2MColumnList
typedef struct M2MColumnList
	{
	struct M2MColumnList *previous;
	struct M2MColumnList *next;
	M2MColumn *column;
	} M2MColumnList;
#endif /* M2MColumnList */


/*******************************************************************************
 * 公開関数
 ******************************************************************************/
/**
 * カラム構造体オブジェクトのリンクに新規ノードを追加する。<br>
 *
 * @param[in,out] self		カラム構造体オブジェクト
 * @param[in] columnName	カラム名
 * @param[in] dataType		カラムのデータ型
 * @param[in] primaryKey	主キー有効化のフラグ
 * @param[in] autoIncrement	自動インクリメント有効化のフラグ
 * @param[in] allowNULL		NULL有効化のフラグ
 * @param[in] unique		ユニーク性有効化のフラグ
 * @return					新規追加されたカラム構造体オブジェクト or NULL（エラーの場合）
 */
M2MColumnList *M2MColumnList_add (M2MColumnList *self, const M2MString *columnName, const M2MDataType dataType, const bool primaryKey, const bool autoIncrement, const bool allowNULL, const bool unique);


/**
 * 引数で指定されたカラム構造体オブジェクトのリンクの1つ先に位置するノードを取得する。<br>
 *
 * @param[in] self	カラム構造体オブジェクト
 * @return			カラム構造体オブジェクトのリンクの先頭ノード or NULL（エラーの場合）
 */
M2MColumnList *M2MColumnList_begin (M2MColumnList *self);


/**
 * 引数で指定されたカラム構造体オブジェクトのリンクの全ノードのメモリ領域を解放する。<br>
 *
 * @param[in,out] self	カラム構造体オブジェクト
 */
void M2MColumnList_delete (M2MColumnList *self);


/**
 * 引数で指定されたカラム構造体オブジェクトのリンクの末端に位置するノードを取得する。<br>
 *
 * @param[in,out] self	カラム構造体オブジェクト
 * @return				カラム構造体オブジェクトのリンクの末端ノード or NULL（エラーの場合）
 */
M2MColumnList *M2MColumnList_end (M2MColumnList *self);


/**
 * 引数で指定されたカラム構造体オブジェクトからカラム情報オブジェクトを取得して<br>
 * 返す。<br>
 *
 * @param[in] self	カラム構造体オブジェクト
 * @return			カラム情報オブジェクト
 */
M2MColumn *M2MColumnList_getColumn (const M2MColumnList *self);


/**
 * 引数で指定されたカラム構造体オブジェクトが1つ以上の値を持っているかどうかを判定<br>
 * する。<br>
 *
 * @param[in] self	カラム構造体オブジェクト
 * @return			true : 値が1つも存在しない、false : 値を1つ以上保有している
 */
bool M2MColumnList_isEmpty (M2MColumnList *self);


/**
 * カラム構造体オブジェクトのノード数を示す整数を返す。<br>
 *
 * @param[in] self	カラム構造体オブジェクト
 * @return			リンクされているカラム構造体オブジェクトのノード数を示す整数
 */
unsigned int M2MColumnList_length (M2MColumnList *self);


/**
 * 引数で指定されたカラム構造体オブジェクトの1つ後ろに位置するカラム構造体<br>
 * オブジェクトを返す。<br>
 *
 * @param[in] self	カラム構造体オブジェクト
 * @return next 	1つ後ろに位置するカラム構造体オブジェクト（引数ノードが末端の場合はNULLを返す）
 */
M2MColumnList *M2MColumnList_next (const M2MColumnList *self);


/**
 * ヒープ領域のメモリを獲得し、M2MColumnList構造体オブジェクトを新規作成する。<br>
 *
 * @return	新規作成したM2MColumnList構造体オブジェクト
 */
M2MColumnList *M2MColumnList_new ();


/**
 * 引数で指定されたカラム構造体オブジェクトの1つ手前に存在するカラム構造体<br>
 * オブジェクトを取得して返す。<br>
 * 引数で指定されたカラム構造体オブジェクトが先頭ノードの場合、同じポインタを示す。<br>
 *
 * @param[in] self	カラム構造体オブジェクト
 * @return			1つ手前に存在するカラム構造体オブジェクト or NULL（エラーの場合）
 */
M2MColumnList *M2MColumnList_previous (const M2MColumnList *self);


/**
 * @param self
 * @param columnName
 * @param columnNameLength
 * @return
 */
M2MColumn *M2MColumnList_search (M2MColumnList *self, const M2MString *columnName, const size_t columnNameLength);



#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2M_DB_M2MCOLUMNLIST_H_ */
