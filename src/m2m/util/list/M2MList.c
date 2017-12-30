/*******************************************************************************
 * M2MList.c : Imitation of STL M2MList Container
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

#include "m2m/util/list/M2MList.h"


/*******************************************************************************
 * Declaration of function
 ******************************************************************************/
/**
 * Set the size of the value held in the list structure.<br>
 *
 * @param[in,out] self	List structure object
 * @param[in] length	Size of value held by the list structure object[byte]
 * @return				List structure object or NULL (In case of error)
 */
static M2MList *this_setValueLength (M2MList *self, const size_t valueLength);


/*******************************************************************************
 * Private function
 ******************************************************************************/
/**
 * 引数で指定されたリスト構造体オブジェクトが保持する値のヒープメモリ領域を解放する。<br>
 *
 * @param[in,out] self	List structure object
 */
static void this_deleteValue (M2MList *self)
	{
	//===== 値の存在を確認 =====
	if (M2MList_getValue(self)!=NULL)
		{
		M2MHeap_free(self->value);
		this_setValueLength(self, 0);
		}
	//===== Error handling =====
	else
		{
		}
	return;
	}


/**
 * Set the size of the value held in the list structure.<br>
 *
 * @param[in,out] self	List structure object
 * @param[in] length	Size of value held by the list structure object[byte]
 * @return				List structure object or NULL (In case of error)
 */
static M2MList *this_setValueLength (M2MList *self, const size_t valueLength)
	{
	//===== Check argument =====
	if (self!=NULL)
		{
		self->valueLength = valueLength;
		return self;
		}
	//===== Argument error =====
	else
		{
		return NULL;
		}
	}


/**
 * 引数で指定されたリスト構造体オブジェクトの1つ後ろに位置するリスト構造体オブジェクト<br>
 * をセットする。<br>
 *
 * @param[in,out] self	List structure object
 * @param[in] next		リスト構造体オブジェクト
 * @return				リスト構造体オブジェクト or NULL（エラーの場合）
 */
static M2MList *this_setNext (M2MList *self, M2MList *next)
	{
	//===== Check argument =====
	if (self!=NULL)
		{
		self->next = next;
		return self;
		}
	//===== Argument error =====
	else
		{
		return NULL;
		}
	}


/**
 * 引数で指定されたリスト構造体オブジェクトの1つ前に位置するリスト構造体オブジェクト<br>
 * をセットする。<br>
 *
 * @param[in,out] self	List structure object
 * @param[in] previous	リスト構造体オブジェクト
 * @return				リスト構造体オブジェクト or NULL（エラーの場合）
 */
static M2MList *this_setPrevious (M2MList *self, M2MList *previous)
	{
	//===== Check argument =====
	if (self!=NULL && previous!=NULL)
		{
		self->previous = previous;
		return NULL;
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		return NULL;
		}
	else
		{
		return NULL;
		}
	}



/*******************************************************************************
 * Public function
 ******************************************************************************/
/**
 * リスト構造体オブジェクトのリンクに新規ノードを追加する。<br>
 * そして、当該新規ノードのメンバ変数として、同じく引数で指定された値をコピー<br>
 * （ディープコピー）する。<br>
 * そのため、当該関数呼び出し後に "value" の値のヒープメモリ領域を解放したと<br>
 * しても、リスト構造体オブジェクトのノードのメンバ変数には全く影響を及ぼさない。<br>
 *
 * @param[in,out] self		List structure object
 * @param[in] value			リスト構造体オブジェクトに格納させる値
 * @param[in] valueLength	値の大きさ[バイト]
 * @return					新規追加されたリスト構造体オブジェクト or NULL（エラーの場合）
 */
M2MList *M2MList_add (M2MList *self, const void *value, const size_t valueLength)
	{
	//========== ローカル変数 ==========
	M2MList *node = NULL;

	//===== Check argument =====
	if (self!=NULL && value!=NULL && 0<valueLength)
		{
		//===== 先頭ノードの取得 =====
		if ((self=M2MList_begin(self))!=NULL)
			{
			//===== 先頭ノードに値が格納されている場合 =====
			if (M2MList_getValue(self)!=NULL)
				{
				//===== 末端のノードまで移動 =====
				self = M2MList_end(self);
				//===== 新規ノードの作成 =====
				node = M2MList_new();
				this_setPrevious(node, self);
				this_setNext(node, NULL);
				M2MList_set(node, value, valueLength);
				//===== 新規ノードをリスト構造の末端に追加 =====
				this_setNext(self, node);
				return node;
				}
			//===== 先頭ノードに値が格納されていない場合 =====
			else
				{
				return M2MList_set(self, value, valueLength);
				}
			}
		//===== Error handling =====
		else
			{
			return NULL;
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		return NULL;
		}
	else if (value==NULL)
		{
		return NULL;
		}
	else
		{
		return NULL;
		}
	}


/**
 * 引数で指定されたリスト構造体オブジェクトのリンクの1つ先に位置するノードを取得<br>
 * する。<br>
 *
 * @param[in] self	List structure object
 * @return			リスト構造体オブジェクトのリンクの先頭ノード or NULL（エラーの場合）
 */
M2MList *M2MList_begin (M2MList *self)
	{
	//===== Check argument =====
	if (self!=NULL)
		{
		//===== 先頭ノードでない場合 =====
		while (M2MList_previous(self)!=self)
			{
			//===== 1つ前に位置するノードへ移動 =====
			self = M2MList_previous(self);
			}
		return self;
		}
	//===== Argument error =====
	else
		{
		return NULL;
		}
	}


/**
 * 引数で指定されたリスト構造体オブジェクトのリンクの全ノードのメモリ領域を解放<br>
 * する。<br>
 *
 * @param[in,out] self	List structure object
 */
void M2MList_delete (M2MList *self)
	{
	//========== ローカル変数 ==========
	M2MList *next = NULL;

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== 先頭ノードの取得 =====
		if ((self=M2MList_begin(self))!=NULL)
			{
			//===== 末端ノードに辿り着くまで繰り返し =====
			while ((next=M2MList_next(self))!=NULL)
				{
				//===== リスト構造体オブジェクトのノードの削除 =====
				this_deleteValue(self);
				M2MHeap_free(self);
				//===== 1つ後ろのノードへ移動e =====
				self = next;
				}
			//===== 末端ノードの削除 =====
			this_deleteValue(self);
			M2MHeap_free(self);
			}
		//===== Error handling =====
		else
			{
			}
		}
	//===== Argument error =====
	else
		{
		}
	return;
	}


/**
 * 引数で指定されたリスト構造体オブジェクトのリンク配列において、引数のインデックス<br>
 * に該当するリスト構造体オブジェクトを返す。<br>
 *
 * @param[in] self		List structure object
 * @param[in] index		インデックスを示す整数（0 ? n-1）
 * @return 				リスト構造体オブジェクト or NULL（エラーの場合）
 */
M2MList *M2MList_detect (M2MList *self, const unsigned int index)
	{
	//========== ローカル変数 ==========
	unsigned int position = 0;

	//===== Check argument =====
	if (self!=NULL && index<M2MList_length(self))
		{
		//===== リスト構造体オブジェクトの先頭ノードを取得 =====
		if ((self=M2MList_begin(self))!=NULL)
			{
			//===== 末端ノードへ辿り着くまで繰り返し =====
			while (self!=NULL)
				{
				//===== インデックスが一致するかどうか確認 =====
				if (index==position)
					{
					return self;
					}
				//===== インデックスが不一致の場合 =====
				else
					{
					//===== リスト構造体の次のノードへ移動 =====
					self = M2MList_next(self);
					//===== インデックス番号をインクリメント =====
					position++;
					}
				}
			return self;
			}
		//===== Error handling =====
		else
			{
			return NULL;
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		return NULL;
		}
	else
		{
		return NULL;
		}
	}


/**
 * 引数で指定されたリスト構造体オブジェクトのリンクの末端に位置するノードを取得<br>
 * する。<br>
 *
 * @param[in,out] self	List structure object
 * @return				リスト構造体オブジェクトのリンクの末端ノード or NULL（エラーの場合）
 */
M2MList *M2MList_end (M2MList *self)
	{
	//===== Check argument =====
	if (self!=NULL)
		{
		//===== 末端ノードに辿り着くまで繰り返し =====
		while (M2MList_next(self)!=NULL)
			{
			self = M2MList_next(self);
			}
		//===== 末端ノードを返す =====
		return self;
		}
	//===== Argument error =====
	else
		{
		return NULL;
		}
	}


/**
 * 引数で指定されたインデックス位置に存在するリスト構造体オブジェクトの値を返す。<br>
 *
 * @param[in] self	List structure object
 * @return			リスト構造体オブジェクトに格納されている値 or NULL（エラーの場合）
 */
void *M2MList_getValue (M2MList *self)
	{
	//===== Check argument =====
	if (self!=NULL)
		{
		return self->value;
		}
	//===== Argument error =====
	else
		{
		return NULL;
		}
	}


/**
 * 引数で指定されたインデックス位置に存在するリスト構造体オブジェクトの値の大きさ<br>
 * を示す整数を返す。<br>
 *
 * @param[in] self	List structure object
 * @return			値のサイズ[バイト] or 0（エラーの場合）
 */
size_t M2MList_getValueLength (M2MList *self)
	{
	//===== Check argument =====
	if (self!=NULL)
		{
		return self->valueLength;
		}
	//===== Argument error =====
	else
		{
		return 0;
		}
	}


/**
 * 引数で指定されたリスト構造体オブジェクトのリンク内の全てのノードに値が格納され<br>
 * ていないかどうか確認する。<br>
 * もし値が格納されたノードが一つも存在しない場合は true を返す。<br>
 *
 * @param[in] self	List structure object
 * @return			true : 全てのノードに値が格納されていない場合、false : 値が格納されたノードが一つでも存在する場合
 */
bool M2MList_isEmpty (M2MList *self)
	{
	//===== Check argument =====
	if (self!=NULL)
		{
		//===== 先頭ノードの取得 =====
		if ((self=M2MList_begin(self))!=NULL)
			{
			//===== 末端ノードに移動するまで繰り返し =====
			while (M2MList_next(self)!=NULL)
				{
				//===== 値の有無を確認 =====
				if (M2MList_getValue(self)!=NULL)
					{
					return false;
					}
				//===== 値が格納されていない場合 =====
				else
					{
					//===== 次のノードへ移動 =====
					self = M2MList_next(self);
					}
				}
			//===== 末端ノードの値の有無を確認 =====
			if (M2MList_getValue(self)!=NULL)
				{
				return false;
				}
			//===== 末端ノードに値が格納されていない場合 =====
			else
				{
				return true;
				}
			}
		//===== Error handling =====
		else
			{
			return true;
			}
		}
	//===== Argument error =====
	else
		{
		return true;
		}
	}


/**
 * リスト構造体オブジェクトのノード数を示す整数を返す。<br>
 *
 * @param[in] self	List structure object
 * @return			リンクされているリスト構造体オブジェクトのノード数を示す整数
 */
unsigned int M2MList_length (M2MList *self)
	{
	//========== ローカル変数 ==========
	unsigned int length = 0;

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== 先頭ノードの取得 =====
		if ((self=M2MList_begin(self))!=NULL)
			{
			//===== 値の有無を確認 =====
			if (M2MList_getValue(self)!=NULL)
				{
				//===== ノード数をカウント =====
				length++;
				}
			//===== 値を保持していない場合 =====
			else
				{
				// 何もしない
				}
			//===== 値の有無を確認 =====
			while (M2MList_next(self)!=NULL)
				{
				//===== 値の有無を確認 =====
				if (M2MList_getValue(self)!=NULL)
					{
					//===== ノード数をカウント =====
					length++;
					}
				//===== 値を保持していない場合 =====
				else
					{
					// 何もしない
					}
				//===== 次のノードへ移動 =====
				self = M2MList_next(self);
				}
			//===== リスト構造体オブジェクトの数を返す =====
			return length;
			}
		//===== Argument error =====
		else
			{
			return 0;
			}
		}
	//===== Argument error =====
	else
		{
		return 0;
		}
	}


/**
 * 引数で指定されたリスト構造体オブジェクトの1つ後ろに位置するリスト構造体<br>
 * オブジェクトを返す。<br>
 *
 * @param[in] self	List structure object
 * @return next 	1つ後ろに位置するリスト構造体オブジェクト（末端の場合はNULLを返す）
 */
M2MList *M2MList_next (const M2MList *self)
	{
	//===== Check argument =====
	if (self!=NULL)
		{
		return self->next;
		}
	//===== Argument error =====
	else
		{
		return NULL;
		}
	}


/**
 * ヒープ領域のメモリを獲得し、M2MList構造体オブジェクトを新規作成する。<br>
 *
 * @return	新規作成したM2MList構造体オブジェクト
 */
M2MList *M2MList_new ()
	{
	//========== ローカル変数 ==========
	M2MList *self = NULL;

	//===== ヒープメモリの獲得 =====
	if ((self=(M2MList *)M2MHeap_malloc(sizeof(M2MList)))!=NULL)
		{
		//===== 初期化 =====
		this_setPrevious(self, self);
		this_setNext(self, NULL);
		return self;
		}
	//===== Error handling =====
	else
		{
		return NULL;
		}
	}


/**
 * 引数で指定されたリスト構造体オブジェクトの1つ手前に存在するリスト構造体<br>
 * オブジェクトを取得して返す。<br>
 * 引数で指定されたリスト構造体オブジェクトが先頭ノードの場合、同じポインタを示す。<br>
 *
 * @param[in] self	List structure object
 * @return			1つ手前に存在するリスト構造体オブジェクト or NULL（エラーの場合）
 */
M2MList *M2MList_previous (const M2MList *self)
	{
	//===== Check argument =====
	if (self!=NULL)
		{
		return self->previous;
		}
	//===== Argument error =====
	else
		{
		return NULL;
		}
	}


/**
 * 引数で指定されたインデックスの位置に存在するリスト構造体オブジェクトを削除する。<br>
 * もしリスト構造体オブジェクトのノードが1つであった場合、オブジェクト自体は削除<br>
 * せず、値のみ削除する。<br>
 * <br>
 * 【注意】<br>
 * このメソッドを呼び出した場合、呼び出し側では必ず戻り値を受け取らなければならない。<br>
 * 何故なら、引数で与えたリスト構造体オブジェクトのノード自体のメモリ領域を解放<br>
 * した場合、残りのノードのリンクを受け取る手段が無くなり、エラーとなるからである。<br>
 *
 * @param[in,out] self	List structure object
 * @return				リスト構造体オブジェクト
 */
M2MList *M2MList_remove (M2MList *self)
	{
	//========== ローカル変数 ==========
	unsigned int length = 0;
	M2MList *previous = NULL;
	M2MList *next = NULL;

	//===== 指定されたインデックスのリスト構造体オブジェクトの取得 =====
	if (self!=NULL)
		{
		//===== 値を持ったノードが1つも無い場合 =====
		if ((length=M2MList_length(self))==0)
			{
			//===== 前後のリンク情報の初期化 =====
			this_setPrevious(self, self);
			this_setNext(self, NULL);
			return self;
			}
		//===== 値を持ったノードが1つの場合 =====
		else if (length==1)
			{
			//===== 値の削除 =====
			this_deleteValue(self);
			//===== 前後のリンク情報の初期化 =====
			this_setPrevious(self, self);
			this_setNext(self, NULL);
			return self;
			}
		//===== 値を持ったノードが2つ以上の場合 =====
		else
			{
			//===== 先頭ノードであった場合 =====
			if (M2MList_previous(self)==self)
				{
				//===== 次のノードを取得 =====
				if ((next=M2MList_next(self))!=NULL)
					{
					//===== 先頭ノードをリンクから外す =====
					this_setPrevious(next, next);
					//===== ノードの削除 =====
					this_deleteValue(self);
					M2MHeap_free(self);
					return next;
					}
				//===== Error handling =====
				else
					{
					return self;
					}
				}
			//===== 末端ノードであった場合 =====
			else if (M2MList_next(self)==NULL)
				{
				//===== 前のノードを取得 =====
				if ((previous=M2MList_previous(self))!=NULL)
					{
					//===== 末端ノードの変更 =====
					this_setNext(previous, NULL);
					//===== ノードの削除 =====
					this_deleteValue(self);
					M2MHeap_free(self);
					return previous;
					}
				//===== Error handling =====
				else
					{
					return self;
					}
				}
			//===== 中間ノードであった場合 =====
			else
				{
				//===== 1つ前後のノードを取得 =====
				if ((previous = M2MList_previous(self))!=NULL
						&& (next = M2MList_next(self))!=NULL)
					{
					//===== 前後のノードを相互に連結 =====
					this_setNext(previous, next);
					this_setPrevious(next, previous);
					//===== ノードの削除 =====
					this_deleteValue(self);
					M2MHeap_free(self);
					return previous;
					}
				//===== Error handling =====
				else
					{
					return self;
					}
				}
			}
		}
	//===== Error handling =====
	else
		{
		return self;
		}
	}


/**
 * 引数で指定されたリスト構造体オブジェクトのメンバ変数として、同じく引数で指定<br>
 * された値をコピー（ディープコピー）する。<br>
 * そのため、当該関数呼び出し後に "value" の値のヒープメモリ領域を解放したと<br>
 * しても、リスト構造体オブジェクトのメンバ変数には全く影響を及ぼさない。<br>
 *
 * @param[in,out] self		List structure object
 * @param[in] value			値（を示すオブジェクト）
 * @param[in] valueLength	値の大きさを示す整数[バイト]
 * @return					値を変更したリスト構造体オブジェクト or NULL（エラーの場合）
 */
M2MList *M2MList_set (M2MList *self, const void *value, const size_t valueLength)
	{
	//===== Check argument =====
	if (self!=NULL && value!=NULL && 0<valueLength)
		{
		//===== ノードの初期化 =====
		this_deleteValue(self);
		//===== ヒープメモリの獲得 =====
		if ((self->value=M2MHeap_malloc(valueLength+1))!=NULL)
			{
			//===== 値のセット =====
			memcpy(self->value, value, valueLength);
			this_setValueLength(self, valueLength);
			return self;
			}
		//===== Error handling =====
		else
			{
			return NULL;
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		return NULL;
		}
	else if (value==NULL)
		{
		return NULL;
		}
	else
		{
		return NULL;
		}
	}



/* End of File */
