/*******************************************************************************
 * M2MTableManager.c
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

#include "m2m/db/M2MTableManager.h"


/*******************************************************************************
 * Definition of private function
 ******************************************************************************/
/**
 * 引数で指定されたテーブル構築オブジェクトの先頭ノードを取得する。<br>
 *
 * @param[in] self	テーブル構築オブジェクト
 * @return			先頭に位置するテーブル構築オブジェクト
 */
static M2MTableManager *this_begin (M2MTableManager *self);


/**
 * 引数で指定されたテーブル構築オブジェクトがメンバ変数として保持するカラム構造体<br>
 * オブジェクトを返す。<br>
 *
 * @param[in] self	テーブル構築オブジェクト
 * @return			カラム構造体オブジェクト or NULL（エラーの場合）
 */
static M2MColumnList *this_getColumnList (const M2MTableManager *self);


/**
 * 引数で指定されたテーブル構築オブジェクトが保有しているテーブル名文字列を取得する。<br>
 *
 * @param[in] self	テーブル構築オブジェクト
 * @return			テーブル名を示す文字列
 */
static unsigned char *this_getTableName (const M2MTableManager *self);


/**
 * 引数で指定されたテーブル構築オブジェクトの1つ次のノードを返す．<br>
 *
 * @param[in] tableBuilder	テーブル構築オブジェクト
 * @return					引数で指定されたテーブル構築オブジェクトの1つ次のノード or NULL（エラーの場合）
 */
static M2MTableManager *this_next (const M2MTableManager *self);


/**
 * 引数で指定されたテーブル構築オブジェクトの前のオブジェクトを返す．<br>
 *
 * @param[in] self		テーブル構築オブジェクト
 * @return				引数で指定されたテーブル構築オブジェクトの1つ前のノード or NULL（エラーの場合）
 */
static M2MTableManager *this_previous (const M2MTableManager *self);



/*******************************************************************************
 * Private function
 ******************************************************************************/
/**
 * 引数で指定されたテーブル構築オブジェクトの先頭ノードを取得する。<br>
 *
 * @param[in] self	テーブル構築オブジェクト
 * @return			先頭に位置するテーブル構築オブジェクト
 */
static M2MTableManager *this_begin (M2MTableManager *self)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MTableManager.this_begin()";

	//===== Check argument =====
	if (self!=NULL)
		{
		while (this_previous(self)!=self)
			{
			self = this_previous(self);
			}
		return self;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MTableManager\" object is NULL", NULL);
		return NULL;
		}
	}


/**
 * 引数で指定されたテーブル構築オブジェクトがメンバ変数として保持するカラム構造体<br>
 * オブジェクトのヒープメモリ領域を解放する。<br>
 *
 * @param[in,out] self	テーブル構築オブジェクト
 */
static void this_deleteColumnList (M2MTableManager *self)
	{
	//========== Variable ==========
	M2MColumnList *columnList = NULL;
	const M2MString *METHOD_NAME = (M2MString *)"M2MTableManager.this_deleteColumnList()";

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== カラム構造体オブジェクトの存在を確認 =====
		if ((columnList=this_getColumnList(self))!=NULL)
			{
			M2MColumnList_delete(columnList);
			}
		//===== Error handling =====
		else
			{
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MTableManager\" object is NULL", NULL);
		}
	return;
	}


/**
 * 引数で指定されたテーブル構築オブジェクトがメンバ変数として保持するテーブル名を<br>
 * 示す文字列のヒープメモリ領域を解放する。<br>
 *
 * @param[in] self	テーブル構築オブジェクト
 */
static void this_deleteTableName (M2MTableManager *self)
	{
	//========== Variable ==========
	M2MString *tableName = NULL;
	const M2MString *METHOD_NAME = (M2MString *)"M2MTableManager.this_deleteTableName()";

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== テーブル名を示す文字列の存在を確認 =====
		if ((tableName=this_getTableName(self))!=NULL)
			{
			M2MHeap_free(tableName);
			}
		//===== Error handling =====
		else
			{
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MTableManager\" object is NULL", NULL);
		}
	return;
	}


/**
 * @param[in] columnList
 * @param[out] columnDefinition
 * @return
 */
static M2MString *this_getColumnDefinition (const M2MColumnList *columnList, M2MString **columnDefinition)
	{
	//========== Variable ==========
	M2MColumn *column = NULL;
	M2MString *columnName = NULL;
	M2MDataType dataType = 0;
	M2MString *dataTypeString = NULL;
	const M2MString *METHOD_NAME = (M2MString *)"M2MTableManager.this_getColumnDefinition()";

	//===== Check argument =====
	if (columnList!=NULL && columnDefinition!=NULL)
		{
		//===== カラム情報オブジェクトの取得 =====
		if ((column=M2MColumnList_getColumn(columnList))!=NULL
				&& (columnName=M2MColumn_getName(column))!=NULL
				&& (dataTypeString=M2MDataType_toString(dataType=M2MColumn_getDataType(column)))!=NULL)
			{
			//===== カラム名をセット =====
			M2MString_append(columnDefinition, M2MString_QUOTATION);
			M2MString_append(columnDefinition, columnName);
			M2MString_append(columnDefinition, (M2MString *)"' ");
			//===== データ型をセット =====
			M2MString_append(columnDefinition, dataTypeString);
			M2MString_append(columnDefinition, (M2MString *)M2MString_SPACE);
			//===== NULLを許す場合 =====
			if (M2MColumn_getAllowNULL(column)==true)
				{
				}
			//===== NULLを許さない場合 =====
			else
				{
				M2MString_append(columnDefinition, (M2MString *)"NOT NULL ");
				}
			//===== 主キーの場合 =====
			if (M2MColumn_getPrimaryKey(column)==true)
				{
				//===== 主キーをセット =====
				M2MString_append(columnDefinition, (M2MString *)"PRIMARY KEY ");
				//===== 更にデータ型が整数で自動インクリメント設定の場合 =====
				if (dataType==M2M_DATA_TYPE_INTEGER
						&& M2MColumn_getAutoIncrement(column)==true)
					{
					//===== 自動インクリメントをセット =====
					M2MString_append(columnDefinition, (M2MString *)"AUTOINCREMENT ");
					}
				//===== 自動インクリメント設定でない場合 =====
				else
					{
					// 何もしない
					}
				}
			//===== 主キーでない場合 =====
			else
				{
				// 何もしない
				}
			//===== 値がユニークな場合 =====
			if (M2MColumn_getUnique(column)==true)
				{
				//===== ユニーク設定をセット =====
				M2MString_append(columnDefinition, (M2MString *)"UNIQUE ");
				}
			//===== 値がユニークでない場合 =====
			else
				{
				// 何もしない
				}
			//===== カラム定義を示す文字列を返す =====
			return (*columnDefinition);
			}
		//===== Error handling =====
		else if (column==NULL)
			{
			M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"引数で指定された\"M2MColumnList *\"から取得した\"M2MColumn *\"がNULLです", NULL);
			return NULL;
			}
		else if (columnName==NULL)
			{
			M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"引数で指定された\"M2MColumnList *\"から取得した\"M2MColumn *\"のメンバ変数であるカラム名がNULLです", NULL);
			return NULL;
			}
		else
			{
			M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"引数で指定された\"M2MColumnList *\"から取得した\"M2MColumn *\"のデータ型を示す文字列がNULLです", NULL);
			return NULL;
			}
		}
	//===== Argument error =====
	else if (columnList==NULL)
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"引数で指定された\"M2MColumnList *\"がNULLです", NULL);
		return NULL;
		}
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"引数で指定された\"columnDefinition\"がNULLです", NULL);
		return NULL;
		}
	}


/**
 * 引数で指定されたテーブル構築オブジェクトがメンバ変数として保持するカラム構造体<br>
 * オブジェクトを返す。<br>
 *
 * @param[in] self	テーブル構築オブジェクト
 * @return			カラム構造体オブジェクト or NULL（エラーの場合）
 */
static M2MColumnList *this_getColumnList (const M2MTableManager *self)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MTableManager.this_getColumnList()";

	//===== Check argument =====
	if (self!=NULL)
		{
		return self->columnList;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MTableManager\" object is NULL", NULL);
		return NULL;
		}
	}


/**
 * @param[in] self	テーブル構築オブジェクト
 * @param[out] sql	テーブル構築のためのSQL文を示す文字列をコピーするためのバッファ（バッファリング自体は関数内で実施するため呼び出し側はポインタ指定のみ）
 * @return			引数に与えた"sql"ポインタにコピーされた文字列の先頭ポインタ or NULL（エラーの場合）
 */
static M2MString *this_getTableCreateSQL (const M2MTableManager *self, M2MString **sql)
	{
	//========== Variable ==========
	M2MString *tableName = NULL;
	M2MColumnList *columnList = NULL;
	unsigned int columnListLength = 0;
	unsigned int i = 0;
	M2MString *columnDefinition = NULL;
	const M2MString *METHOD_NAME = (M2MString *)"M2MTableManager.this_getTableCreateSQL()";

	//===== Check argument =====
	if (self!=NULL && sql!=NULL)
		{
		//===== テーブル情報の取得 =====
		if ((tableName=this_getTableName(self))!=NULL
				&& (columnList=M2MColumnList_begin(this_getColumnList(self)))!=NULL
				&& (columnListLength=M2MColumnList_length(columnList))>0)
			{
			//===== SQLコマンドとテーブル名をセット =====
			M2MString_append(sql, (M2MString *)"CREATE TABLE '");
			M2MString_append(sql, tableName);
			//===== カラム定義の準備 =====
			M2MString_append(sql, (M2MString *)"' (");
			//===== カラム数だけ繰り返し =====
			for (i=0; i<columnListLength; i++)
				{
				//===== カラム定義文字列を取得 =====
				if (this_getColumnDefinition(columnList, &columnDefinition)!=NULL)
					{
					//===== カラム定義文字列をセット =====
					M2MString_append(sql, columnDefinition);
					//===== 最後のカラム定義でない場合 =====
					if (i!=columnListLength-1)
						{
						//===== カンマ文字を追加 =====
						M2MString_append(sql, (M2MString *)", ");
						}
					//===== 最後のカラム定義の場合 =====
					else
						{
						// 何もしない
						}
					//===== カラム定義文字列のヒープメモリ領域を解放 =====
					M2MHeap_free(columnDefinition);
					}
				//===== カラム定義文字列の取得に失敗した場合 =====
				else
					{
					M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"カラム定義文字列の作成に失敗しました", NULL);
					//===== ヒープメモリ領域の解放 =====
					M2MHeap_free(columnDefinition);
					M2MHeap_free((*sql));
					return NULL;
					}
				//===== 次のカラムに移動 =====
				columnList = M2MColumnList_next(columnList);
				}
			//===== SQL文の作成を終了 =====
			M2MString_append(sql, (M2MString *)") ");
			//===== SQL文を返す =====
			return (*sql);
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
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MTableManager\" object is NULL", NULL);
		return NULL;
		}
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"sql\" pointer is NULL", NULL);
		return NULL;
		}
	}


/**
 * 引数で指定されたテーブル構築オブジェクトが保有しているテーブル名文字列を取得する。<br>
 *
 * @param[in] self	テーブル構築オブジェクト
 * @return			テーブル名を示す文字列
 */
static M2MString *this_getTableName (const M2MTableManager *self)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MTableManager.this_getTableName()";

	//===== Check argument =====
	if (self!=NULL)
		{
		return self->tableName;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MTableManager\" object is NULL", NULL);
		return NULL;
		}
	}


/**
 * 引数で指定されたテーブル構築オブジェクトの1つ次のノードを返す．<br>
 *
 * @param[in] tableBuilder	テーブル構築オブジェクト
 * @return					引数で指定されたテーブル構築オブジェクトの1つ次のノード or NULL（エラーの場合）
 */
static M2MTableManager *this_next (const M2MTableManager *self)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MTableManager.this_next()";

	//===== Check argument =====
	if (self!=NULL)
		{
		return self->next;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MTableManager\" object is NULL", NULL);
		return NULL;
		}
	}


/**
 * 引数で指定されたテーブル構築オブジェクトの1つ前のノードを返す．<br>
 *
 * @param[in] tableBuilder	テーブル構築オブジェクト
 * @return					引数で指定されたテーブル構築オブジェクトの1つ前のノード or NULL（エラーの場合）
 */
static M2MTableManager *this_previous (const M2MTableManager *self)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MTableManager.this_previous()";

	//===== Check argument =====
	if (self!=NULL)
		{
		return self->previous;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MTableManager\" object is NULL", NULL);
		return NULL;
		}
	}


/**
 * 引数で指定されたテーブル構築オブジェクトのメンバ変数として, カラム構造体<br>
 * オブジェクトをセットする。<br>
 *
 * @param[in] self			テーブル構築オブジェクト
 * @param[in] columnList	カラム構造体オブジェクト
 * @return					テーブル構築オブジェクト or NULL（エラーの場合）
 */
static M2MTableManager *this_setColumnList (M2MTableManager *self, M2MColumnList *columnList)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MTableManager.this_setColumnList()";

	//===== Check argument =====
	if (self!=NULL && columnList!=NULL)
		{
		self->columnList = columnList;
		return self;
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MTableManager\" object is NULL", NULL);
		return NULL;
		}
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MColumnList\" object is NULL", NULL);
		return NULL;
		}
	}


/**
 * 引数で指定されたテーブル構築オブジェクトに対し, 1つ後ろに位置するテーブル構築<br>
 * オブジェクトのノードをセットする。<br>
 *
 * @param[in,out] self	テーブル構築オブジェクト
 * @param[in] next		テーブル構築オブジェクト
 * @return				テーブル構築オブジェクト or NULL（エラーの場合）
 */
static M2MTableManager *this_setNext (M2MTableManager *self, M2MTableManager *next)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MTableManager.this_setNext()";

	//===== Check argument =====
	if (self!=NULL)
		{
		self->next = next;
		return self;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MTableManager\" object is NULL", NULL);
		return NULL;
		}
	}


/**
 * 引数で指定されたテーブル構築オブジェクトに対し, 1つ前に位置するテーブル構築<br>
 * オブジェクトのノードをセットする。<br>
 *
 * @param[in,out] self	テーブル構築オブジェクト
 * @param[in] previous	テーブル構築オブジェクト
 * @return				テーブル構築オブジェクト or NULL（エラーの場合）
 */
static M2MTableManager *this_setPrevious (M2MTableManager *self, M2MTableManager *previous)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MTableManager.this_setPrevious()";

	//===== Check argument =====
	if (self!=NULL && previous!=NULL)
		{
		self->previous = previous;
		return self;
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MTableManager\" object is NULL", NULL);
		return NULL;
		}
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"previous\" object is NULL", NULL);
		return NULL;
		}
	}


/**
 * テーブル構築オブジェクトのメンバ変数としてテーブル名を示す文字列をセットする。<br>
 *
 * @param[in,out] self		テーブル構築オブジェクト
 * @param[in] tableName		テーブル名を示す文字列
 * @return					テーブル構築オブジェクト or NULL（エラーの場合）
 */
static M2MTableManager *this_setTableName (M2MTableManager *self, const M2MString *tableName)
	{
	//========== Variable ==========
	size_t tableNameLength = 0;
	const M2MString *METHOD_NAME = (M2MString *)"M2MTableManager.this_setTableName()";

	//===== Check argument =====
	if (self!=NULL && tableName!=NULL)
		{
		//===== テーブル名を示す文字列の長さを取得 =====
		if ((tableNameLength=M2MString_length(tableName))>0)
			{
			//===== テーブル名を示すメンバ変数の初期化 =====
			this_deleteTableName(self);
			//===== ヒープメモリ領域の獲得 =====
			if ((self->tableName=(M2MString *)M2MHeap_malloc(tableNameLength+1))!=NULL)
				{
				//===== テーブル名を示す文字列のコピー =====
				memcpy(self->tableName, tableName, tableNameLength);
				return self;
				}
			//===== Error handling =====
			else
				{
				M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"テーブル名を示す文字列をコピーするためのヒープメモリ領域の獲得に失敗しました", NULL);
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"引数で指定された\"tableName\"の文字列の大きさが0以下です", NULL);
			return NULL;
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MTableManager\" object is NULL", NULL);
		return NULL;
		}
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"tableName\" string is NULL", NULL);
		return NULL;
		}
	}



/*******************************************************************************
 * Public function
 ******************************************************************************/
/**
 * Create a table for the SQLite3 database.<br>
 *
 * @param[in] self		Table construction object (column information inserted)
 * @param[in] database	SQLite3 database to create table
 */
void M2MTableManager_createTable (M2MTableManager *self, sqlite3 *database)
	{
	//========== Variable ==========
	M2MTableManager *next = NULL;
	bool executeUpdate = false;
	M2MString *sql = NULL;
	M2MString MESSAGE[256];
	const M2MString *METHOD_NAME = (M2MString *)"M2MTableManager_createTable()";

	//===== Check argument =====
	if (self!=NULL && database!=NULL)
		{
		//===== テーブル構築オブジェクトの先頭ノードを取得 =====
		if ((self=this_begin(self))!=NULL)
			{
			//===== 末端ノードに辿り着くまで繰り返し =====
			while ((next=this_next(self))!=NULL)
				{
				//===== 同一名のテーブルが存在する場合 =====
				if (M2MSQLiteConfig_isExistingTable(database, this_getTableName(self))==true)
					{
					// 何もしない
					}
				//===== 同一名のテーブルが存在しない場合 =====
				else
					{
					//===== テーブル構築用のSQL文を実行済みの場合 =====
					if (executeUpdate==true)
						{
						// 何もしない
						}
					//===== テーブル構築用のSQL文が未実行の場合 =====
					else
						{
#ifdef DEBUG
						M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, (M2MString *)"CEPテーブルの構築処理を開始します");
#endif // DEBUG
						//===== トランザクション開始 =====
						M2MSQLRunner_beginTransaction(database);
						//===== フラグを立てる =====
						executeUpdate = true;
						}
					//===== テーブル構築用のSQL文の文字列を取得 =====
					if (this_getTableCreateSQL(self, &sql)!=NULL)
						{
						//===== SQL実行 =====
						if (M2MSQLRunner_executeUpdate(database, sql)==true)
							{
#ifdef DEBUG
							M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, (M2MString *)"CEPテーブル構築のためのCREATE文を実行しました");
#endif // DEBUG
							}
						//===== Error handling =====
						else
							{
							M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)sqlite3_errmsg(database), NULL);
							}
						//===== SQL文のヒープメモリ領域を解放 =====
						M2MHeap_free(sql);
						}
					//===== Error handling =====
					else
						{
						memset(MESSAGE, 0, sizeof(MESSAGE));
						snprintf(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"テーブル（＝\"%s\"）を構築するためのSQL文を取得するのに失敗しました", this_getTableName(self));
						M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, MESSAGE, NULL);
						}
					}
				//===== 次のノードへ進む =====
				self = next;
				}
			//===== 同一名のテーブルが存在する場合 =====
			if (M2MSQLiteConfig_isExistingTable(database, this_getTableName(self))==true)
				{
				// 何もしない
				}
			//===== 同一名のテーブルが存在しない場合 =====
			else
				{
				//===== テーブル構築用のSQL文を実行済みの場合 =====
				if (executeUpdate==true)
					{
					// 何もしない
					}
				//===== テーブル構築用のSQL文が未実行の場合 =====
				else
					{
#ifdef DEBUG
					M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, (M2MString *)"CEPテーブルの構築処理を開始します");
#endif // DEBUG
					//===== トランザクション開始 =====
					M2MSQLRunner_beginTransaction(database);
					//===== フラグを立てる =====
					executeUpdate = true;
					}
				//===== 末端ノードのテーブル構築用のSQL文の文字列を取得 =====
				if (this_getTableCreateSQL(self, &sql)!=NULL)
					{
					//===== SQL実行 =====
					if (M2MSQLRunner_executeUpdate(database, sql)==true)
						{
#ifdef DEBUG
						M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, (M2MString *)"CEPテーブル構築のためのCREATE文を実行しました");
#endif // DEBUG
						}
					//===== Error handling =====
					else
						{
						M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, sqlite3_errmsg(database), NULL);
						}
					//===== SQL文のヒープメモリ領域を解放 =====
					M2MHeap_free(sql);
					}
				//===== Error handling =====
				else
					{
					memset(MESSAGE, 0, sizeof(MESSAGE));
					snprintf(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"テーブル（＝\"%s\"）を構築するためのSQL文を取得するのに失敗しました", this_getTableName(self));
					M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, MESSAGE, NULL);
					}
				}
			//===== テーブル構築用のSQL文を実行済みの場合 =====
			if (executeUpdate==true)
				{
				//===== トランザクション終了 =====
				M2MSQLRunner_commitTransaction(database);
#ifdef DEBUG
				M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, (M2MString *)"CEPテーブル構築処理を終了します");
#endif // DEBUG
				}
			//===== テーブル構築用のSQL文が未実行の場合 =====
			else
				{
				// 何もしない
				}
			//===== 正常終了 =====
			return;
			}
		//===== Error handling =====
		else
			{
			M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"引数で指定された\"M2MTableManager *\"から先頭ノードを取得するのに失敗しました", NULL);
			return;
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MTableManager\" object is NULL", NULL);
		return;
		}
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"database\" object is NULL", NULL);
		return;
		}
	}


/**
 * Releases heap memory of the table construction object. <br>
 * Be aware that after calling this function you can no longer access <br>
 * that variable.<br>
 *
 * @param[in,out] self	Table construction object to be freed heap memory
 */
void M2MTableManager_delete (M2MTableManager **self)
	{
	//========== Variable ==========
	M2MTableManager *next = NULL;
	const M2MString *METHOD_NAME = (M2MString *)"M2MTableManager_delete()";

	//===== Check argument =====
	if (self!=NULL && (*self)!=NULL)
		{
		//===== テーブル構築オブジェクトの先頭ノード取得 =====
		if (((*self)=this_begin((*self)))!=NULL)
			{
			//===== 末端ノードに辿り着くまで繰り返し =====
			while ((*self)!=NULL && (next=this_next((*self)))!=NULL)
				{
				//===== テーブル構築オブジェクトのヒープメモリ領域を解放 =====
				this_deleteTableName((*self));
				this_deleteColumnList((*self));
				M2MHeap_free((*self));
				//===== 次のノードへ移動 =====
				(*self) = next;
				}
			//===== （末端の）テーブル構築オブジェクトの確認 =====
			if ((*self)!=NULL)
				{
				//===== （末端の）テーブル構築オブジェクトのヒープメモリ領域を解放 =====
				this_deleteTableName((*self));
				this_deleteColumnList((*self));
				M2MHeap_free((*self));
#ifdef DEBUG
				M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, (M2MString *)"CEPテーブル構築オブジェクトのメモリ領域を解放しました");
#endif // DEBUG
				}
			//===== Error handling =====
			else
				{
				M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"CEPテーブル構築オブジェクトの末端ノードがNULLのため, メモリ領域を解放出来ません", NULL);
				}
			}
		//===== Error handling =====
		else
			{
			M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"CEPテーブル構築オブジェクトのメモリ領域を解放する処理において先頭ノードの取得に失敗しました", NULL);
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MTableManager\" object is NULL", NULL);
		}
	return;
	}


/**
 * Get column list object matching the table name from table construction object.<br>
 *
 * @param[in] self		Table construction object
 * @param[in] tableName	String indicating table name
 * @return				Column structure object with matching table name or NULL (in case of error)
 */
M2MColumnList *M2MTableManager_getColumnList (M2MTableManager *self, const M2MString *tableName)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MTableManager_getColumnList()";

	//===== Check argument =====
	if (self!=NULL && tableName!=NULL)
		{
		//===== Get the first node =====
		if ((self=this_begin(self))!=NULL)
			{
			//===== Repeat until reaching the end node =====
			while (this_next(self)!=NULL)
				{
				//===== When the table names match =====
				if (M2MString_compareTo(this_getTableName(self), tableName)==0)
					{
					//===== Return column list object =====
					return this_getColumnList(self);
					}
				//===== When table names don't match =====
				else
					{
					// do nothing
					}
				//===== Move to next node =====
				self = (M2MTableManager *)this_next(self);
				}
			//===== When the table name of the end node matches =====
			if (M2MString_compareTo(this_getTableName(self), tableName)==0)
				{
				//===== Return column list object =====
				return this_getColumnList(self);
				}
			//===== When the table name of the end node doesn't match =====
			else
				{
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to get begin node from indicated \"M2MTableManager\" object", NULL);
			return NULL;
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MTableManager\" object is NULL", NULL);
		return NULL;
		}
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"tableName\" string is NULL", NULL);
		return NULL;
		}
	}


/**
 * Get heap memory and create a new table construction object.<br>
 *
 * @return	Created new table construction object
 */
M2MTableManager *M2MTableManager_new ()
	{
	//========== Variable ==========
	M2MTableManager *self = NULL;
	const M2MString *METHOD_NAME = (M2MString *)"M2MTableManager_new()";

	//===== Get heap memory =====
	if ((self=(M2MTableManager *)M2MHeap_malloc(sizeof(M2MTableManager)))!=NULL)
		{
		//===== Set member variables =====
		this_setPrevious(self, self);
		this_setNext(self, NULL);
#ifdef DEBUG
		M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, (M2MString *)"Created new table construction object");
#endif // DEBUG
		return self;
		}
	//===== Error handling =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to get heap memory for creating \"M2MTableManager\" object", NULL);
		return NULL;
		}
	}


/**
 * Store the column information in the table construction object.<br>
 *
 * @param[in,out] self		Table construction object
 * @param[in] tableName		String indicating table name
 * @param[in] columnList	Column structure object storing column setting information
 * @return					Table construction object containing column setting information or NULL (in case of error)
 */
M2MTableManager *M2MTableManager_setConfig (M2MTableManager *self, const M2MString *tableName, M2MColumnList *columnList)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MTableManager_setConfig()";

	//===== Check argument =====
	if (self!=NULL && tableName!=NULL && columnList!=NULL)
		{
		//===== Set member variables =====
		if (this_setTableName(self, tableName)!=NULL
				&& this_setColumnList(self, columnList)!=NULL)
			{
			return self;
			}
		//===== Error handling =====
		else
			{
			M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to set table name or column structure object in \"M2MTableManager\" object", NULL);
			return NULL;
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MTableManager\" object is NULL", NULL);
		return NULL;
		}
	else if (tableName==NULL)
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"tableName\" string is NULL", NULL);
		return NULL;
		}
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MColumnList\" object is NULL", NULL);
		return NULL;
		}
	}



/* End Of File */
