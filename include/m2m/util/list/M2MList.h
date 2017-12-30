/*******************************************************************************
 * M2MList.h : Imitation of STL List Container
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
 *******************************************************************************/

#pragma once

#ifndef M2MLIST_H_
#define M2MLIST_H_


#include "m2m/io/M2MHeap.h"
#include <stdio.h>
#include <stdbool.h>



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



/*******************************************************************************
 * Definition
 ******************************************************************************/
/**
 * A list structure object (dynamic array)<br>
 *
 * @param previous		A list structure object located before (indicating its own pointer in the case of the head)
 * @param next			A list structure object located behind (indicating NULL in the case of the end)
 * @param value			Pointer to "value" stored in list structure object
 * @param valueLength	Integer indicating the size of "value"[Byte]
 */
#ifndef M2MList
typedef struct M2MList
	{
	struct M2MList *previous;
	struct M2MList *next;
	void *value;
	size_t valueLength;
	} M2MList;
#endif /* M2MList */



/*******************************************************************************
 * Public function
 ******************************************************************************/
/**
 * Add a new node with a value to the link of the list structure object.<br>
 *
 * @param[in,out] self		List structure object
 * @param[in] value			リスト構造体オブジェクトに格納させる値
 * @param[in] valueLength	値の大きさ[バイト]
 * @return					新規追加されたリスト構造体オブジェクト or NULL（エラーの場合）
 */
M2MList *M2MList_add (M2MList *self, const void *value, const size_t valueLength);


/**
 * 引数で指定されたリスト構造体オブジェクトのリンクの1つ先に位置するノードを取得する。<br>
 *
 * @param[in] self	List structure object
 * @return			リスト構造体オブジェクトのリンクの先頭ノード or NULL（エラーの場合）
 */
M2MList *M2MList_begin (M2MList *self);


/**
 * 引数で指定されたリスト構造体オブジェクトのリンクの全ノードのメモリ領域を解放する。<br>
 *
 * @param[in,out] self	List structure object
 */
void M2MList_delete (M2MList *self);


/**
 * 引数で指定されたリスト構造体オブジェクトのリンク配列において、引数のインデックス<br>
 * に該当するリスト構造体オブジェクトを返す。<br>
 *
 * @param[in] self		List structure object
 * @param[in] index		インデックスを示す整数（0 〜 n-1）
 * @return 				リスト構造体オブジェクト or NULL（エラーの場合）
 */
M2MList *M2MList_detect (M2MList *self, const unsigned int index);


/**
 * 引数で指定されたリスト構造体オブジェクトのリンクの末端に位置するノードを取得する。<br>
 *
 * @param[in,out] self	List structure object
 * @return				リスト構造体オブジェクトのリンクの末端ノード or NULL（エラーの場合）
 */
M2MList *M2MList_end (M2MList *self);


/**
 * 引数で指定されたインデックス位置に存在するリスト構造体オブジェクトの値を返す。<br>
 *
 * @param[in] self	List structure object
 * @return			リスト構造体オブジェクトに格納されている値 or NULL（エラーの場合）
 */
void *M2MList_getValue (M2MList *self);


/**
 * 引数で指定されたインデックス位置に存在するリスト構造体オブジェクトの値の大きさ<br>
 * を示す整数を返す。<br>
 *
 * @param[in] self	List structure object
 * @return			値のサイズ[バイト] or 0（エラーの場合）
 */
size_t M2MList_getValueLength (M2MList *self);


/**
 * 引数で指定されたリスト構造体オブジェクトが1つ以上の値を持っているかどうかを判定<br>
 * する。<br>
 *
 * @param[in] self	List structure object
 * @return			true : 値が1つも存在しない、false : 値を1つ以上保有している
 */
bool M2MList_isEmpty (M2MList *self);


/**
 * リスト構造体オブジェクトのノード数を示す整数を返す。<br>
 *
 * @param[in] self	List structure object
 * @return			リンクされているリスト構造体オブジェクトのノード数を示す整数
 */
unsigned int M2MList_length (M2MList *self);


/**
 * 引数で指定されたリスト構造体オブジェクトの1つ後ろに位置するリスト構造体<br>
 * オブジェクトを返す。<br>
 *
 * @param[in] self	List structure object
 * @return next 	1つ後ろに位置するリスト構造体オブジェクト（引数ノードが末端の場合はNULLを返す）
 */
M2MList *M2MList_next (const M2MList *self);


/**
 * ヒープ領域のメモリを獲得し、M2MList構造体オブジェクトを新規作成する。<br>
 *
 * @return	新規作成したM2MList構造体オブジェクト
 */
M2MList *M2MList_new ();


/**
 * 引数で指定されたリスト構造体オブジェクトの1つ手前に存在するリスト構造体<br>
 * オブジェクトを取得して返す。<br>
 * 引数で指定されたリスト構造体オブジェクトが先頭ノードの場合、同じポインタを示す。<br>
 *
 * @param[in] self	List structure object
 * @return			1つ手前に存在するリスト構造体オブジェクト or NULL（エラーの場合）
 */
M2MList *M2MList_previous (const M2MList *self);


/**
 * 引数で指定されたインデックスの位置に存在するリスト構造体オブジェクトを削除する。<br>
 *
 * @param[in,out] self	削除対象のリスト構造体オブジェクト
 */
M2MList *M2MList_remove (M2MList *self);


/**
 * 引数で指定されたインデックス位置のリスト構造体オブジェクトの値を変更する。<br>
 *
 * @param[in,out] self		List structure object
 * @param[in] value			変更する値
 * @param[in] valueLength	値の大きさを示す整数[バイト]
 * @return					値を変更したリスト構造体オブジェクト or NULL（エラーの場合）
 */
M2MList *M2MList_set (M2MList *self, const void *value, const size_t valueLength);



#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2MLIST_H_ */
