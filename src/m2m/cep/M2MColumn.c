/*******************************************************************************
 * M2MColumn.c
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

#include "m2m/cep/M2MColumn.h"


/*******************************************************************************
 * 内部関数
 ******************************************************************************/
/**
 * 引数で指定されたカラム構造体オブジェクトがメンバ変数として保持しているカラム名<br>
 * を示す文字列のヒープメモリ領域を解放する。<br>
 *
 * @param[in,out] self	カラム構造体オブジェクト
 */
static void this_deleteName (M2MColumn *self)
	{
	//========== ローカル変数 ==========
	unsigned char *name = NULL;

	//===== 引数の確認 =====
	if (self!=NULL)
		{
		//===== カラム名の存在を確認 =====
		if ((name=M2MColumn_getName(self))!=NULL)
			{
			//===== ヒープメモリ領域を解放 =====
			M2MHeap_free(name);
			}
		//===== エラー処理 =====
		else
			{
			}
		}
	//===== 引数エラー =====
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MColumn.this_deleteName()", __LINE__, (unsigned char *)"引数で指定された\"M2MColumn *\"がNULLです", NULL);
		}
	return;
	}


/*******************************************************************************
 * 公開関数
 ******************************************************************************/
/**
 * デストラクタ。<br>
 * 引数で指定されたカラム情報オブジェクトのヒープメモリ領域を解放し, オブジェクト<br>
 * を削除する。<br>
 *
 * @param[in,out] self	カラム情報オブジェクト
 */
void M2MColumn_delete (M2MColumn **self)
	{
	if (self!=NULL && (*self)!=NULL)
		{
		this_deleteName((*self));
		M2MHeap_free((*self));
		}
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MColumn_delete()", __LINE__, (unsigned char *)"引数で指定された\"M2MColumn **\"がNULLです", NULL);
		}
	return;
	}


/**
 * 引数で指定されたカラム情報オブジェクトがメンバ変数として保持しているカラム名<br>
 * を示す文字列を返す。<br>
 *
 * @param[in] self	カラム情報オブジェクト
 * @return			カラム名を示す文字列 or NULL（エラーの場合）
 */
unsigned char *M2MColumn_getName (const M2MColumn *self)
	{
	//===== 引数の確認 =====
	if (self!=NULL)
		{
		return self->name;
		}
	//===== 引数エラー =====
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MColumn_getName()", __LINE__, (unsigned char *)"引数で指定された\"M2MColumn *\"がNULLです", NULL);
		return NULL;
		}
	}


/**
 * @param[in] self	カラム情報オブジェクト
 */
M2MDataType M2MColumn_getDataType (const M2MColumn *self)
	{
	if (self!=NULL)
		{
		return self->dataType;
		}
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MColumn_getDataType()", __LINE__, (unsigned char *)"引数で指定された\"M2MColumn\"がNULLです", NULL);
		return M2M_DATA_TYPE_ERROR;
		}
	}


/**
 * @param[in] self	カラム情報オブジェクト
 */
bool M2MColumn_getPrimaryKey (const M2MColumn *self)
	{
	if (self!=NULL)
		{
		return self->primaryKey;
		}
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MColumn_getPrimaryKey()", __LINE__, (unsigned char *)"引数で指定された\"M2MColumn\"がNULLです", NULL);
		return false;
		}
	}


/**
 * @param[in] self	カラム情報オブジェクト
 */
bool M2MColumn_getAutoIncrement (const M2MColumn *self)
	{
	if (self!=NULL)
		{
		return self->autoIncrement;
		}
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MColumn_getAutoIncrement()", __LINE__, (unsigned char *)"引数で指定された\"M2MColumn\"がNULLです", NULL);
		return false;
		}
	}


/**
 * @param[in] self	カラム情報オブジェクト
 */
bool M2MColumn_getAllowNULL (const M2MColumn *self)
	{
	if (self!=NULL)
		{
		return self->allowNULL;
		}
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MColumn_getAllowNULL()", __LINE__, (unsigned char *)"引数で指定された\"M2MColumn\"がNULLです", NULL);
		return false;
		}
	}


/**
 * @param[in] self	カラム情報オブジェクト
 */
bool M2MColumn_getUnique (const M2MColumn *self)
	{
	if (self!=NULL)
		{
		return self->unique;
		}
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MColumn_getUnique()", __LINE__, (unsigned char *)"引数で指定された\"M2MColumn\"がNULLです", NULL);
		return false;
		}
	}


/**
 * コンストラクタ。<br>
 * ヒープメモリ領域を獲得し, カラム情報オブジェクトを新規に作成する。<br>
 *
 * @return	カラム情報オブジェクト
 */
M2MColumn *M2MColumn_new ()
	{
	//========== ローカル変数 ==========
	M2MColumn *self = NULL;

	//===== ヒープメモリ領域の獲得 =====
	if ((self=(M2MColumn *)M2MHeap_malloc(sizeof(M2MColumn)))!=NULL)
		{
		M2MColumn_setAllowNULL(self, false);
		M2MColumn_setAutoIncrement(self, false);
		M2MColumn_setDataType(self, M2M_DATA_TYPE_ERROR);
		M2MColumn_setPrimaryKey(self, false);
		M2MColumn_setUnique(self, false);
		return self;
		}
	//===== エラー処理 =====
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MColumn_setName()", __LINE__, (unsigned char *)"カラム情報オブジェクトを新規作成するため, ヒープメモリ領域を獲得するのに失敗しました", NULL);
		return NULL;
		}
	}


/**
 * 引数で指定されたカラム情報オブジェクトにメンバ変数として, 自動インクリメントの<br>
 * 可否を示すフラグをセットする。<br>
 *
 * @param[in,out] self		カラム情報オブジェクト
 * @param[in] autoIncrement	自動インクリメントか否かを示すフラグ
 * @return					自動インクリメント情報をセットしたカラム情報オブジェクト or NULL（エラーの場合）
 */
M2MColumn *M2MColumn_setAutoIncrement (M2MColumn *self, const bool autoIncrement)
	{
	if (self!=NULL)
		{
		self->autoIncrement = autoIncrement;
		return self;
		}
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MColumn_setAutoIncrement()", __LINE__, (unsigned char *)"引数で指定された\"M2MColumn\"がNULLです", NULL);
		return NULL;
		}
	}


/**
 * 引数で指定されたカラム情報オブジェクトのメンバ変数として, レコードにNULLの可否<br>
 * を示すフラグをセットする。<br>
 *
 * @param[in,out] self	カラム情報オブジェクト
 * @param[in] allowNULL	レコードにNULLを許すか否かを示すフラグ
 * @return				NULLの可否を示すフラグをセットしたカラム情報オブジェクト or NULL（エラーの場合）
 */
M2MColumn *M2MColumn_setAllowNULL (M2MColumn *self, const bool allowNULL)
	{
	if (self!=NULL)
		{
		self->allowNULL = allowNULL;
		return self;
		}
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MColumn_setAllowNULL()", __LINE__, (unsigned char *)"引数で指定された\"M2MColumn\"がNULLです", NULL);
		return NULL;
		}
	}


/**
 * 引数で指定されたカラム情報オブジェクトのメンバ変数として, データ型を示す列挙子<br>
 * をセットする。<br>
 *
 * @param[in,out] self	カラム情報オブジェクト
 * @param[in] dataType	データ型を示す列挙子
 * @return				データ型を示す列挙子をセットしたカラム情報オブジェクト or NULL（エラーの場合）
 */
M2MColumn *M2MColumn_setDataType (M2MColumn *self, const M2MDataType dataType)
	{
	if (self!=NULL)
		{
		self->dataType = dataType;
		return self;
		}
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MColumn_setDataType()", __LINE__, (unsigned char *)"引数で指定された\"M2MColumn\"がNULLです", NULL);
		return NULL;
		}
	}


/**
 * 引数で指定されたカラム情報オブジェクトに対し, カラム名を示す文字列をメンバ変数<br>
 * としてセットする。<br>
 *
 * @param[in,out] self	カラム情報オブジェクト
 * @param[in] name		カラム名を示す文字列
 * @return				カラム名をセットされたカラム情報オブジェクト or NULL（エラーの場合）
 */
M2MColumn *M2MColumn_setName (M2MColumn *self, const unsigned char *name)
	{
	size_t nameLength = 0;

	//===== 引数の確認 =====
	if (self!=NULL && name!=NULL && (nameLength=M2MString_length(name))>0)
		{
		//===== 初期化 =====
		this_deleteName(self);
		//===== ヒープメモリ領域の新規獲得 =====
		if ((self->name=(unsigned char *)M2MHeap_malloc(nameLength+1))!=NULL)
			{
			memcpy(self->name, name, nameLength);
			return self;
			}
		//===== エラー処理 =====
		else
			{
			M2MLogger_printErrorMessage((unsigned char *)"M2MColumn_setName()", __LINE__, (unsigned char *)"引数で指定された\"name\"文字列をコピーするため, ヒープメモリ領域を獲得するのに失敗しました", NULL);
			return NULL;
			}
		}
	//===== 引数エラー =====
	else if (self==NULL)
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MColumn_setName()", __LINE__, (unsigned char *)"引数で指定された\"M2MColumn\"がNULLです", NULL);
		return NULL;
		}
	else if (name==NULL)
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MColumn_setName()", __LINE__, (unsigned char *)"引数で指定された\"name\"がNULLです", NULL);
		return NULL;
		}
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MColumn_setName()", __LINE__, (unsigned char *)"引数で指定された\"name\"文字列の長さが0以下です", NULL);
		return NULL;
		}
	}


/**
 * 引数で指定されたカラム情報オブジェクトのメンバ変数として, 主キーか否かを示す<br>
 * フラグをセットする。<br>
 *
 * @param[in,out] self		カラム情報オブジェクト
 * @param[in] primaryKey	主キーか否かを示すフラグ
 * @return					フラグをセットしたカラム情報オブジェクト or NULL（エラーの場合）
 */
M2MColumn *M2MColumn_setPrimaryKey (M2MColumn *self, const bool primaryKey)
	{
	if (self!=NULL)
		{
		self->primaryKey = primaryKey;
		return self;
		}
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MColumn_setPrimaryKey()", __LINE__, (unsigned char *)"引数で指定された\"M2MColumn\"がNULLです", NULL);
		return NULL;
		}
	}


/**
 * 引数で指定されたカラム情報オブジェクトのメンバ変数として, レコードのユニーク性<br>
 * を示すフラグをセットする。<br>
 *
 * @param[in,out] self	カラム情報オブジェクト
 * @param[in] unique	レコードデータのユニーク性の可否を示すフラグ
 * @return				ユニーク性の可否を示すフラグがセットされたカラム情報オブジェクト or NULL（エラーの場合）
 */
M2MColumn *M2MColumn_setUnique (M2MColumn *self, const bool unique)
	{
	if (self!=NULL)
		{
		self->unique = unique;
		return self;
		}
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MColumn_setUnique()", __LINE__, (unsigned char *)"引数で指定された\"M2MColumn\"がNULLです", NULL);
		return NULL;
		}
	}




/* End Of File */
