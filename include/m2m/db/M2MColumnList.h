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


#include "m2m/db/M2MColumn.h"
#include "m2m/io/M2MHeap.h"
#include "m2m/lang/M2MString.h"
#include "m2m/log/M2MLogger.h"



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



/*******************************************************************************
 * Definition
 ******************************************************************************/
/**
 * データベースのテーブルのカラム情報を格納した構造体オブジェクト。<br>
 * リスト構造になっており、複数のカラム情報オブジェクトがリンクで繋がれた構造と<br>
 * なっている。<br>
 *
 * @param previous	A list structure object located before (indicating its own pointer in the case of the head)
 * @param next		A list structure object located behind (indicating NULL in the case of the end)
 * @param column	Column information object (containing one data type of a column)
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
 * Public function
 ******************************************************************************/
/**
 * Add a new node to the link of the column information list object.<br>
 *
 * @param[in,out] self		Column information list object
 * @param[in] columnName	String indicating column name
 * @param[in] dataType		Data type of the column
 * @param[in] primaryKey	Flag for primary key activation
 * @param[in] autoIncrement	Flag for auto increment enable
 * @param[in] allowNULL		Flag for NULL activation
 * @param[in] unique		Flag of uniqueness validation
 * @return					Newly added information list object or NULL (in case of error)
 */
M2MColumnList *M2MColumnList_add (M2MColumnList *self, const M2MString *columnName, const M2MDataType dataType, const bool primaryKey, const bool autoIncrement, const bool allowNULL, const bool unique);


/**
 * Get the node located one column ahead of the link of the column list object.<br>
 *
 * @param[in] self	Column information list object
 * @return			Column information list object which is first node of link or NULL (in case of error)
 */
M2MColumnList *M2MColumnList_begin (M2MColumnList *self);


/**
 * Release the memory of all nodes of the link of the column list object.<br>
 *
 * @param[in,out] self	Column information list object
 */
void M2MColumnList_delete (M2MColumnList *self);


/**
 * 引数で指定されたカラム構造体オブジェクトのリンクの末端に位置するノードを取得する。<br>
 *
 * @param[in,out] self	Column information list object
 * @return				カラム構造体オブジェクトのリンクの末端ノード or NULL（エラーの場合）
 */
M2MColumnList *M2MColumnList_end (M2MColumnList *self);


/**
 * 引数で指定されたカラム構造体オブジェクトからカラム情報オブジェクトを取得して<br>
 * 返す。<br>
 *
 * @param[in] self	Column information list object
 * @return			カラム情報オブジェクト
 */
M2MColumn *M2MColumnList_getColumn (const M2MColumnList *self);


/**
 * 引数で指定されたカラム構造体オブジェクトが1つ以上の値を持っているかどうかを判定<br>
 * する。<br>
 *
 * @param[in] self	Column information list object
 * @return			true : 値が1つも存在しない、false : 値を1つ以上保有している
 */
bool M2MColumnList_isEmpty (M2MColumnList *self);


/**
 * カラム構造体オブジェクトのノード数を示す整数を返す。<br>
 *
 * @param[in] self	Column information list object
 * @return			リンクされているカラム構造体オブジェクトのノード数を示す整数
 */
unsigned int M2MColumnList_length (M2MColumnList *self);


/**
 * 引数で指定されたカラム構造体オブジェクトの1つ後ろに位置するカラム構造体<br>
 * オブジェクトを返す。<br>
 *
 * @param[in] self	Column information list object
 * @return next 	1つ後ろに位置するカラム構造体オブジェクト（引数ノードが末端の場合はNULLを返す）
 */
M2MColumnList *M2MColumnList_next (const M2MColumnList *self);


/**
 * Get heap memory and create a new M2MColumnList structure object.<br>
 *
 * @return	Created new column information list object
 */
M2MColumnList *M2MColumnList_new ();


/**
 * Get and return the column object existing one before the argument. <br>
 * If the column structure object is the first node, it indicates the <br>
 * same pointer.<br>
 *
 * @param[in] self	Column information list object
 * @return			Column structure object which exists one before or NULL (in case of error)
 */
M2MColumnList *M2MColumnList_previous (const M2MColumnList *self);


/**
 * Search and detect an object matching column name from the column list.<br>
 *
 * @param[in] self				Column information list object
 * @param[in] columnName		String indicating column name
 * @param[in] columnNameLength	Length of column name string[Byte]
 * @return						Column information object matching the column name
 */
M2MColumn *M2MColumnList_search (M2MColumnList *self, const M2MString *columnName, const size_t columnNameLength);



#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2M_DB_M2MCOLUMNLIST_H_ */
