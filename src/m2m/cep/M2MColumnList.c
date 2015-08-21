/*******************************************************************************
 * M2MColumnList.c
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

#include "m2m/cep/M2MColumnList.h"


/*******************************************************************************
 * 内部関数
 ******************************************************************************/
/**
 *
 * @param self
 */
static void this_deleteColumn (M2MColumnList *self)
	{
	M2MColumn *column = NULL;

	if (self!=NULL)
		{
		if ((column=M2MColumnList_getColumn(self))!=NULL)
			{
			M2MColumn_delete(&column);
			}
		else
			{
			}
		}
	else
		{
		}
	return;
	}


/**
 * @param[in] self
 * @param[in] columnName
 * @return
 */
static M2MColumnList *this_detect (M2MColumnList *self, const unsigned char *columnName)
	{
	size_t columnNameLength = 0;
	M2MColumn *column = NULL;

	//===== 引数の確認 =====
	if (self!=NULL && columnName!=NULL && (columnNameLength=M2MString_length(columnName))>0)
		{
		//===== 先頭ノードの取得 =====
		if ((self=M2MColumnList_begin(self))!=NULL)
			{
			//===== 末端ノードに辿り着くまで繰り返し =====
			while (M2MColumnList_next(self)!=NULL)
				{
				//=====  =====
				if ((column=M2MColumnList_getColumn(self))!=NULL
						&& M2MString_equals(M2MColumn_getName(column), columnName, columnNameLength)==true)
					{
					return self;
					}
				//=====  =====
				else
					{
					self = M2MColumnList_next(self);
					}
				}
			//=====  =====
			if ((column=M2MColumnList_getColumn(self))!=NULL
					&& M2MString_equals(M2MColumn_getName(column), columnName, columnNameLength)==true)
				{
				return self;
				}
			//=====  =====
			else
				{
				return NULL;
				}
			}
		//=====  =====
		else
			{
			return NULL;
			}
		}
	//=====  =====
	else
		{
		return NULL;
		}
	}


static M2MColumnList *this_setColumn (M2MColumnList *self, M2MColumn *column)
	{
	//===== 引数の確認 =====
	if (self!=NULL && column!=NULL)
		{
		this_deleteColumn(self);
		self->column = column;
		return self;
		}
	else
		{
		return NULL;
		}
	}


/**
 * @param[in,out] self
 * @param[in] next
 * @return
 */
static M2MColumnList *this_setNext (M2MColumnList *self, M2MColumnList *next)
	{
	if (self!=NULL)
		{
		self->next = next;
		return self;
		}
	else
		{
		return NULL;
		}
	}


/**
 * @param[in,out] self
 * @param[in] previous
 * @return
 */
static M2MColumnList *this_setPrevious (M2MColumnList *self, M2MColumnList *previous)
	{
	if (self!=NULL)
		{
		self->previous = previous;
		return self;
		}
	else
		{
		return NULL;
		}
	}


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
M2MColumnList *M2MColumnList_add (M2MColumnList *self, const unsigned char *columnName, const M2MDataType dataType, const bool primaryKey, const bool autoIncrement, const bool allowNULL, const bool unique)
	{
	//========== ローカル変数 ==========
	M2MColumnList *detectedNode = NULL;
	M2MColumnList *newNode = NULL;
	M2MColumn *column = NULL;

	//===== 引数の確認 =====
	if (self!=NULL && columnName!=NULL)
		{
		//=====  =====
		if ((detectedNode=this_detect(self, columnName))!=NULL)
			{
			return detectedNode;
			}
		//=====  =====
		else
			{
			//=====  =====
			if (M2MColumnList_isEmpty(self)==true)
				{
				//=====  =====
				if ((column=M2MColumn_new())!=NULL)
					{
					//=====  =====
					if (M2MColumn_setName(column, columnName)!=NULL
							&& M2MColumn_setDataType(column, dataType)!=NULL
							&& M2MColumn_setPrimaryKey(column, primaryKey)!=NULL
							&& M2MColumn_setAutoIncrement(column, autoIncrement)!=NULL
							&& M2MColumn_setAllowNULL(column, allowNULL)!=NULL
							&& M2MColumn_setUnique(column, unique)!=NULL)
						{
						this_setColumn(self, column);
						return self;
						}
					//===== エラー処理 =====
					else
						{
						M2MLogger_printErrorMessage((unsigned char *)"M2MColumnList_add()", __LINE__, (unsigned char *)"引数で指定されたカラム情報をカラム情報オブジェクトにセットする処理でエラーが発生しました", NULL);
						M2MColumn_delete(&column);
						return NULL;
						}
					}
				//===== エラー処理 =====
				else
					{
					M2MLogger_printErrorMessage((unsigned char *)"M2MColumnList_add()", __LINE__, (unsigned char *)"カラム情報オブジェクトを新規作成するため、ヒープメモリ領域を獲得するのに失敗しました", NULL);
					return NULL;
					}

				}
			//=====  =====
			else
				{
				//=====  =====
				if ((self=M2MColumnList_end(self))!=NULL
						&& (newNode=M2MColumnList_new())!=NULL
						&& (column=M2MColumn_new())!=NULL)
					{
					//=====  =====
					if (M2MColumn_setName(column, columnName)!=NULL
							&& M2MColumn_setDataType(column, dataType)!=NULL
							&& M2MColumn_setPrimaryKey(column, primaryKey)!=NULL
							&& M2MColumn_setAutoIncrement(column, autoIncrement)!=NULL
							&& M2MColumn_setAllowNULL(column, allowNULL)!=NULL
							&& M2MColumn_setUnique(column, unique)!=NULL)
						{
						this_setColumn(newNode, column);
						this_setNext(self, newNode);
						this_setPrevious(newNode, self);
						return newNode;
						}
					//===== エラー処理 =====
					else
						{
						M2MLogger_printErrorMessage((unsigned char *)"M2MColumnList_add()", __LINE__, (unsigned char *)"引数で指定されたカラム情報をカラム情報オブジェクトにセットする処理でエラーが発生しました", NULL);
						M2MColumn_delete(&column);
						M2MColumnList_delete(newNode);
						return NULL;
						}
					}
				//===== エラー処理 =====
				else if (self==NULL)
					{
					M2MLogger_printErrorMessage((unsigned char *)"M2MColumnList_add()", __LINE__, (unsigned char *)"引数で指定された\"M2MColumnList *\"オブジェクトから末端ノードを取得するのに失敗しました", NULL);
					return NULL;
					}
				else if (newNode==NULL)
					{
					M2MLogger_printErrorMessage((unsigned char *)"M2MColumnList_add()", __LINE__, (unsigned char *)"カラム構造体オブジェクトを新規作成するため、ヒープメモリ領域を獲得するのに失敗しました", NULL);
					return NULL;
					}
				else
					{
					M2MLogger_printErrorMessage((unsigned char *)"M2MColumnList_add()", __LINE__, (unsigned char *)"カラム情報オブジェクトを新規作成するため、ヒープメモリ領域を獲得するのに失敗しました", NULL);
					M2MColumnList_delete(newNode);
					return NULL;
					}
				}
			}
		}
	//===== 引数エラー =====
	else if (self==NULL)
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MColumnList_add()", __LINE__, (unsigned char *)"引数で指定された\"M2MColumnList *\"がNULLです", NULL);
		return NULL;
		}
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MColumnList_add()", __LINE__, (unsigned char *)"引数で指定された\"columnName\"がNULLです", NULL);
		return NULL;
		}
	}


/**
 * 引数で指定されたカラム構造体オブジェクトのリンクの1つ先に位置するノードを取得する。<br>
 *
 * @param[in] self	カラム構造体オブジェクト
 * @return			カラム構造体オブジェクトのリンクの先頭ノード or NULL（エラーの場合）
 */
M2MColumnList *M2MColumnList_begin (M2MColumnList *self)
	{
	if (self!=NULL)
		{
		while (M2MColumnList_previous(self)!=self)
			{
			self = M2MColumnList_previous(self);
			}
		return self;
		}
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MColumnList_length()", __LINE__, (unsigned char *)"引数で指定された\"M2MColumnList *\"がNULLです", NULL);
		return NULL;
		}
	}


/**
 * 引数で指定されたカラム構造体オブジェクトのリンクの全ノードのメモリ領域を解放する。<br>
 *
 * @param[in,out] self	カラム構造体オブジェクト
 */
void M2MColumnList_delete (M2MColumnList *self)
	{
	//========== ローカル変数 ==========
	M2MColumnList *next = NULL;

	//===== 引数の確認 =====
	if (self!=NULL)
		{
		//===== 先頭ノードの取得 =====
		if ((self=M2MColumnList_begin(self))!=NULL)
			{
			//===== 末端のノードに辿り着くまで繰り返し =====
			while (M2MColumnList_next(self)!=NULL)
				{
				//===== 1つ後ろのノードのポインタを一時的に取得 =====
				next = M2MColumnList_next(self);
				//===== 削除対象のノードをリンクから切り離し =====
				this_setNext(self, NULL);
				this_setPrevious(next, next);
				//===== カラム構造体オブジェクトのヒープメモリ領域を解放 =====
				this_deleteColumn(self);
				M2MHeap_free(self);
				//===== 次のノードに移動 =====
				self = next;
				}
			//===== （末端の）カラム構造体オブジェクトの有無を確認 =====
			if (self!=NULL)
				{
				//===== （末端の）カラム構造体オブジェクトのヒープメモリ領域を解放 =====
				this_deleteColumn(self);
				M2MHeap_free(self);
				}
			//===== エラー処理 =====
			else
				{
				M2MLogger_printErrorMessage((unsigned char *)"M2MColumnList_delete()", __LINE__, (unsigned char *)"カラム構造体オブジェクトのリンクの末端ノードがNULLです", NULL);
				}
			}
		//===== エラー処理 =====
		else
			{
			M2MLogger_printErrorMessage((unsigned char *)"M2MColumnList_delete()", __LINE__, (unsigned char *)"カラム構造体オブジェクトの先頭ノードの取得に失敗しました", NULL);
			}
		}
	//===== 引数エラー =====
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MColumnList_delete()", __LINE__, (unsigned char *)"引数で指定された\"M2MColumnList *\"がNULLです", NULL);
		}
	return;
	}


/**
 * 引数で指定されたカラム構造体オブジェクトのリンクの末端に位置するノードを取得する。<br>
 *
 * @param[in,out] self	カラム構造体オブジェクト
 * @return				カラム構造体オブジェクトのリンクの末端ノード or NULL（エラーの場合）
 */
M2MColumnList *M2MColumnList_end (M2MColumnList *self)
	{
	//===== 引数の確認 =====
	if (self!=NULL)
		{
		while (M2MColumnList_next(self)!=NULL)
			{
			self = M2MColumnList_next(self);
			}
		return self;
		}
	//===== 引数エラー =====
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MColumnList_end()", __LINE__, (unsigned char *)"引数で指定された\"M2MColumnList *\"がNULLです", NULL);
		return NULL;
		}
	}


/**
 * 引数で指定されたカラム構造体オブジェクトからカラム情報オブジェクトを取得して<br>
 * 返す。<br>
 *
 * @param[in] self	カラム構造体オブジェクト
 * @return			カラム情報オブジェクト
 */
M2MColumn *M2MColumnList_getColumn (const M2MColumnList *self)
	{
	//===== 引数の確認 =====
	if (self!=NULL)
		{
		return self->column;
		}
	//===== 引数エラー =====
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MColumnList_getColumn()", __LINE__, (unsigned char *)"引数で指定された\"M2MColumnList *\"がNULLです", NULL);
		return NULL;
		}
	}


/**
 * 引数で指定されたカラム構造体オブジェクトが1つ以上の値を持っているかどうかを判定<br>
 * する。<br>
 *
 * @param[in] self	カラム構造体オブジェクト
 * @return			true : 値が1つも存在しない、false : 値を1つ以上保有している
 */
bool M2MColumnList_isEmpty (M2MColumnList *self)
	{
	//===== カラム名を設定しているノードが1つでも存在する場合 =====
	if (M2MColumnList_length(self)>0)
		{
		return false;
		}
	//===== カラム名を設定しているノードが1つも存在しない場合 =====
	else
		{
		return true;
		}
	}


/**
 * カラム構造体オブジェクトのノード数を示す整数を返す。<br>
 *
 * @param[in] self	カラム構造体オブジェクト
 * @return			リンクされているカラム構造体オブジェクトのノード数を示す整数
 */
unsigned int M2MColumnList_length (M2MColumnList *self)
	{
	//========== ローカル変数 ==========
	unsigned int length = 0;

	//===== 引数の確認 =====
	if (self!=NULL)
		{
		//===== 先頭ノードの取得 =====
		if ((self=M2MColumnList_begin(self))!=NULL
				&& M2MColumnList_getColumn(self)!=NULL)
			{
			length++;
			//===== 末端ノードに辿り着くまで繰り返し =====
			while (M2MColumnList_next(self)!=NULL)
				{
				//===== カラム名の確認 =====
				if (M2MColumnList_getColumn(self)!=NULL)
					{
					length++;
					}
				//===== カラム名が未設定の場合 =====
				else
					{
					// 何しない
					}
				self = M2MColumnList_next(self);
				}
			return length;
			}
		//===== エラー処理 =====
		else
			{
			return 0;
			}
		}
	//===== 引数エラー =====
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MColumnList_length()", __LINE__, (unsigned char *)"引数で指定された\"M2MColumnList *\"がNULLです", NULL);
		return 0;
		}
	}


/**
 * 引数で指定されたカラム構造体オブジェクトの1つ後ろに位置するカラム構造体<br>
 * オブジェクトを返す。<br>
 *
 * @param[in] self	カラム構造体オブジェクト
 * @return next 	1つ後ろに位置するカラム構造体オブジェクト（引数ノードが末端の場合はNULLを返す）
 */
M2MColumnList *M2MColumnList_next (const M2MColumnList *self)
	{
	//===== 引数の確認 =====
	if (self!=NULL)
		{
		return self->next;
		}
	//===== 引数エラー =====
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MColumnList_next()", __LINE__, (unsigned char *)"引数で指定された\"M2MColumnList *\"がNULLです", NULL);
		return NULL;
		}
	}


/**
 * ヒープ領域のメモリを獲得し、M2MColumnList構造体オブジェクトを新規作成する。<br>
 *
 * @return	新規作成したM2MColumnList構造体オブジェクト
 */
M2MColumnList *M2MColumnList_new ()
	{
	//========== ローカル変数 ==========
	M2MColumnList *self = NULL;

	//===== ヒープメモリ領域の獲得 =====
	if ((self=(M2MColumnList *)M2MHeap_malloc(sizeof(M2MColumnList)))!=NULL)
		{
		this_setPrevious(self, self);
		this_setNext(self, NULL);
		return self;
		}
	//===== エラー処理 =====
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MColumnList_new()", __LINE__, (unsigned char *)"\"M2MColumnList\"オブジェクトを生成すためのヒープメモリ領域獲得に失敗しました", NULL);
		return NULL;
		}
	}


/**
 * 引数で指定されたカラム構造体オブジェクトの1つ手前に存在するカラム構造体<br>
 * オブジェクトを取得して返す。<br>
 * 引数で指定されたカラム構造体オブジェクトが先頭ノードの場合、同じポインタを示す。<br>
 *
 * @param[in] self	カラム構造体オブジェクト
 * @return			1つ手前に存在するカラム構造体オブジェクト or NULL（エラーの場合）
 */
M2MColumnList *M2MColumnList_previous (const M2MColumnList *self)
	{
	//===== 引数の確認 =====
	if (self!=NULL)
		{
		return self->previous;
		}
	//===== 引数エラー =====
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MColumnList_previous()", __LINE__, (unsigned char *)"引数で指定された\"M2MColumnList *\"がNULLです", NULL);
		return NULL;
		}
	}


/**
 * @param self
 * @param columnName
 * @param columnNameLength
 * @return
 */
M2MColumn *M2MColumnList_search (M2MColumnList *self, const unsigned char *columnName, const size_t columnNameLength)
	{
	//========== ローカル変数 ==========
	M2MColumn *column = NULL;

	//===== 引数の確認 =====
	if (self!=NULL && columnName!=NULL && columnNameLength<=M2MString_length(columnName))
		{
		//=====  =====
		if ((self=(M2MColumnList_begin(self)))!=NULL)
			{
			//=====  =====
			while (M2MColumnList_next(self)!=NULL)
				{
				//=====  =====
				if ((column=M2MColumnList_getColumn(self))!=NULL
						&& M2MString_equals(M2MColumn_getName(column), columnName, columnNameLength)==true)
					{
					return column;
					}
				//=====  =====
				else if (column==NULL)
					{
					return NULL;
					}
				//=====  =====
				else
					{
					self = M2MColumnList_next(self);
					}
				}
			//=====  =====
			if ((column=M2MColumnList_getColumn(self))!=NULL
					&& M2MString_equals(M2MColumn_getName(column), columnName, columnNameLength)==true)
				{
				return column;
				}
			//=====  =====
			else if (column==NULL)
				{
				return NULL;
				}
			//=====  =====
			else
				{
				return NULL;
				}
			}
		//=====  =====
		else
			{
			return NULL;
			}
		}
	//===== 引数エラー =====
	else if (self==NULL)
		{
		return NULL;
		}
	else if (columnName==NULL)
		{
		return NULL;
		}
	else
		{
		return NULL;
		}
	}



/* End Of File */
