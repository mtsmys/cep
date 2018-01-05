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

#include "m2m/db/M2MColumn.h"


/*******************************************************************************
 * Private function
 ******************************************************************************/
/**
 * 引数で指定されたカラム構造体オブジェクトがメンバ変数として保持しているカラム名<br>
 * を示す文字列のヒープメモリ領域を解放する。<br>
 *
 * @param[in,out] self	カラム構造体オブジェクト
 */
static void this_deleteName (M2MColumn *self)
	{
	//========== Variable ==========
	M2MString *name = NULL;
	const M2MString *METHOD_NAME = (M2MString *)"M2MColumn.this_deleteName()";

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== カラム名の存在を確認 =====
		if ((name=M2MColumn_getName(self))!=NULL)
			{
			//===== ヒープメモリ領域を解放 =====
			M2MHeap_free(name);
			}
		//===== Error handling =====
		else
			{
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"引数で指定された\"M2MColumn *\"がNULLです", NULL);
		}
	return;
	}


/*******************************************************************************
 * Public function
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
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MColumn_delete()";

	//===== Check argument =====
	if (self!=NULL && (*self)!=NULL)
		{
		//=====  =====
		this_deleteName((*self));
		M2MHeap_free((*self));
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"引数で指定された\"M2MColumn **\"がNULLです", NULL);
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
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MColumn_getName()";

	//===== Check argument =====
	if (self!=NULL)
		{
		return self->name;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"引数で指定された\"M2MColumn *\"がNULLです", NULL);
		return NULL;
		}
	}


/**
 * @param[in] self	カラム情報オブジェクト
 */
M2MDataType M2MColumn_getDataType (const M2MColumn *self)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MColumn_getDataType()";

	//===== Check argument =====
	if (self!=NULL)
		{
		return self->dataType;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"引数で指定された\"M2MColumn\"がNULLです", NULL);
		return M2M_DATA_TYPE_ERROR;
		}
	}


/**
 * @param[in] self	カラム情報オブジェクト
 */
bool M2MColumn_getPrimaryKey (const M2MColumn *self)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MColumn_getPrimaryKey()";

	//===== Check argument =====
	if (self!=NULL)
		{
		return self->primaryKey;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"引数で指定された\"M2MColumn\"がNULLです", NULL);
		return false;
		}
	}


/**
 * @param[in] self	カラム情報オブジェクト
 */
bool M2MColumn_getAutoIncrement (const M2MColumn *self)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MColumn_getAutoIncrement()";

	//===== Check argument =====
	if (self!=NULL)
		{
		return self->autoIncrement;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"引数で指定された\"M2MColumn\"がNULLです", NULL);
		return false;
		}
	}


/**
 * @param[in] self	カラム情報オブジェクト
 */
bool M2MColumn_getAllowNULL (const M2MColumn *self)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MColumn_getAllowNULL()";

	//===== Check argument =====
	if (self!=NULL)
		{
		return self->allowNULL;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"引数で指定された\"M2MColumn\"がNULLです", NULL);
		return false;
		}
	}


/**
 * @param[in] self	カラム情報オブジェクト
 */
bool M2MColumn_getUnique (const M2MColumn *self)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MColumn_getUnique()";

	//===== Check argument =====
	if (self!=NULL)
		{
		return self->unique;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"引数で指定された\"M2MColumn\"がNULLです", NULL);
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
	//========== Variable ==========
	M2MColumn *self = NULL;
	const M2MString *METHOD_NAME = (M2MString *)"M2MColumn_new()";

	//===== Get heap memory =====
	if ((self=(M2MColumn *)M2MHeap_malloc(sizeof(M2MColumn)))!=NULL)
		{
		//===== Initialize column information object =====
		if (M2MColumn_setAllowNULL(self, false)!=NULL
				&& M2MColumn_setAutoIncrement(self, false)!=NULL
				&& M2MColumn_setDataType(self, M2M_DATA_TYPE_ERROR)!=NULL
				&& M2MColumn_setPrimaryKey(self, false)!=NULL
				&& M2MColumn_setUnique(self, false)!=NULL)
			{
			return self;
			}
		//===== Error handling =====
		else
			{
			M2MColumn_delete(&self);
			M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to initialize a new column information object", NULL);
			return NULL;
			}
		}
	//===== Error handling =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to get heap memory to create a new column information object", NULL);
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
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MColumn_setAutoIncrement()";

	//===== Check argument =====
	if (self!=NULL)
		{
		self->autoIncrement = autoIncrement;
		return self;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"引数で指定された\"M2MColumn\"がNULLです", NULL);
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
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MColumn_setAllowNULL()";

	//===== Check argument =====
	if (self!=NULL)
		{
		self->allowNULL = allowNULL;
		return self;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"引数で指定された\"M2MColumn\"がNULLです", NULL);
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
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MColumn_setDataType()";

	//===== Check argument =====
	if (self!=NULL)
		{
		self->dataType = dataType;
		return self;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"引数で指定された\"M2MColumn\"がNULLです", NULL);
		return NULL;
		}
	}


/**
 * 引数で指定されたカラム情報オブジェクトに対し, カラム名を示す文字列をメンバ変数<br>
 * としてセットする。<br>
 *
 * @param[in,out] self	カラム情報オブジェクト
 * @param[in] name		String indicating the column name
 * @return				カラム名をセットされたカラム情報オブジェクト or NULL（エラーの場合）
 */
M2MColumn *M2MColumn_setName (M2MColumn *self, const M2MString *name)
	{
	//========== Variable ==========
	size_t nameLength = 0;
	const M2MString *METHOD_NAME = (M2MString *)"M2MColumn_setName()";

	//===== Check argument =====
	if (self!=NULL && name!=NULL && (nameLength=M2MString_length(name))>0)
		{
		//===== Initialize variable =====
		this_deleteName(self);
		//===== Copy the column name into member variable =====
		if (M2MString_appendLength(&(self->name), name, nameLength)!=NULL)
			{
			return self;
			}
		//===== Error handling =====
		else
			{
			M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to get heap memory for copying the column name string", NULL);
			return NULL;
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"self\" object is NULL", NULL);
		return NULL;
		}
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"name\" string is NULL or vacant", NULL);
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
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MColumn_setPrimaryKey()";

	//===== Check argument =====
	if (self!=NULL)
		{
		self->primaryKey = primaryKey;
		return self;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"引数で指定された\"M2MColumn\"がNULLです", NULL);
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
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MColumn_setUnique()";

	//===== Check argument =====
	if (self!=NULL)
		{
		self->unique = unique;
		return self;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"引数で指定された\"M2MColumn\"がNULLです", NULL);
		return NULL;
		}
	}



/* End Of File */
