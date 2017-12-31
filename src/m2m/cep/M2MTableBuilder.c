/*******************************************************************************
 * M2MTableBuilder.c
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

#include "m2m/cep/M2MTableBuilder.h"


/*******************************************************************************
 * 関数定義
 ******************************************************************************/
/**
 * 引数で指定されたテーブル構築オブジェクトの先頭ノードを取得する。<br>
 *
 * @param[in] self	テーブル構築オブジェクト
 * @return			先頭に位置するテーブル構築オブジェクト
 */
static M2MTableBuilder *this_begin (M2MTableBuilder *self);


/**
 * 引数で指定されたテーブル構築オブジェクトがメンバ変数として保持するカラム構造体<br>
 * オブジェクトを返す。<br>
 *
 * @param[in] self	テーブル構築オブジェクト
 * @return			カラム構造体オブジェクト or NULL（エラーの場合）
 */
static M2MColumnList *this_getColumnList (const M2MTableBuilder *self);


/**
 * 引数で指定されたテーブル構築オブジェクトが保有しているテーブル名文字列を取得する。<br>
 *
 * @param[in] self	テーブル構築オブジェクト
 * @return			テーブル名を示す文字列
 */
static unsigned char *this_getTableName (const M2MTableBuilder *self);


/**
 * 引数で指定されたテーブル構築オブジェクトの1つ次のノードを返す．<br>
 *
 * @param[in] tableBuilder	テーブル構築オブジェクト
 * @return					引数で指定されたテーブル構築オブジェクトの1つ次のノード or NULL（エラーの場合）
 */
static M2MTableBuilder *this_next (const M2MTableBuilder *self);


/**
 * 引数で指定されたテーブル構築オブジェクトの前のオブジェクトを返す．<br>
 *
 * @param[in] self		テーブル構築オブジェクト
 * @return				引数で指定されたテーブル構築オブジェクトの1つ前のノード or NULL（エラーの場合）
 */
static M2MTableBuilder *this_previous (const M2MTableBuilder *self);



/*******************************************************************************
 * 内部関数
 ******************************************************************************/
/**
 * 引数で指定されたテーブル構築オブジェクトの先頭ノードを取得する。<br>
 *
 * @param[in] self	テーブル構築オブジェクト
 * @return			先頭に位置するテーブル構築オブジェクト
 */
static M2MTableBuilder *this_begin (M2MTableBuilder *self)
	{
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
		M2MLogger_printErrorMessage((unsigned char *)"M2MTableBuilder.this_begin()", __LINE__, (unsigned char *)"引数で指定された\"M2MTableBuilder *\"がNULLです", NULL);
		return NULL;
		}
	}


/**
 * 引数で指定されたテーブル構築オブジェクトがメンバ変数として保持するカラム構造体<br>
 * オブジェクトのヒープメモリ領域を解放する。<br>
 *
 * @param[in,out] self	テーブル構築オブジェクト
 */
static void this_deleteColumnList (M2MTableBuilder *self)
	{
	//========== Variable ==========
	M2MColumnList *columnList = NULL;

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
		M2MLogger_printErrorMessage((unsigned char *)"M2MTableBuilder.this_deleteColumnList()", __LINE__, (unsigned char *)"引数で指定された\"M2MTableBuilder *\"がNULLです", NULL);
		}
	return;
	}


/**
 * 引数で指定されたテーブル構築オブジェクトがメンバ変数として保持するテーブル名を<br>
 * 示す文字列のヒープメモリ領域を解放する。<br>
 *
 * @param[in] self	テーブル構築オブジェクト
 */
static void this_deleteTableName (M2MTableBuilder *self)
	{
	//========== Variable ==========
	unsigned char *tableName = NULL;

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
		M2MLogger_printErrorMessage((unsigned char *)"M2MTableBuilder.this_deleteTableName()", __LINE__, (unsigned char *)"引数で指定された\"M2MTableBuilder *\"がNULLです", NULL);
		}
	return;
	}


/**
 * @param[in] columnList
 * @param[out] columnDefinition
 * @return
 */
static unsigned char *this_getColumnDefinition (const M2MColumnList *columnList, unsigned char **columnDefinition)
	{
	//========== Variable ==========
	M2MColumn *column = NULL;
	unsigned char *columnName = NULL;
	M2MDataType dataType = 0;
	unsigned char *dataTypeString = NULL;

	//===== Check argument =====
	if (columnList!=NULL && columnDefinition!=NULL)
		{
		//===== カラム情報オブジェクトの取得 =====
		if ((column=M2MColumnList_getColumn(columnList))!=NULL
				&& (columnName=M2MColumn_getName(column))!=NULL
				&& (dataTypeString=M2MDataType_toString(dataType=M2MColumn_getDataType(column)))!=NULL)
			{
			//===== カラム名をセット =====
			M2MString_append(columnDefinition, (unsigned char *)"'");
			M2MString_append(columnDefinition, columnName);
			M2MString_append(columnDefinition, (unsigned char *)"' ");
			//===== データ型をセット =====
			M2MString_append(columnDefinition, dataTypeString);
			M2MString_append(columnDefinition, (unsigned char *)M2MString_SPACE);
			//===== NULLを許す場合 =====
			if (M2MColumn_getAllowNULL(column)==true)
				{
				}
			//===== NULLを許さない場合 =====
			else
				{
				M2MString_append(columnDefinition, (unsigned char *)"NOT NULL ");
				}
			//===== 主キーの場合 =====
			if (M2MColumn_getPrimaryKey(column)==true)
				{
				//===== 主キーをセット =====
				M2MString_append(columnDefinition, (unsigned char *)"PRIMARY KEY ");
				//===== 更にデータ型が整数で自動インクリメント設定の場合 =====
				if (dataType==M2M_DATA_TYPE_INTEGER
						&& M2MColumn_getAutoIncrement(column)==true)
					{
					//===== 自動インクリメントをセット =====
					M2MString_append(columnDefinition, (unsigned char *)"AUTOINCREMENT ");
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
				M2MString_append(columnDefinition, (unsigned char *)"UNIQUE ");
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
			M2MLogger_printErrorMessage((unsigned char *)"M2MTableBuilder.this_getColumnDefinition()", __LINE__, (unsigned char *)"引数で指定された\"M2MColumnList *\"から取得した\"M2MColumn *\"がNULLです", NULL);
			return NULL;
			}
		else if (columnName==NULL)
			{
			M2MLogger_printErrorMessage((unsigned char *)"M2MTableBuilder.this_getColumnDefinition()", __LINE__, (unsigned char *)"引数で指定された\"M2MColumnList *\"から取得した\"M2MColumn *\"のメンバ変数であるカラム名がNULLです", NULL);
			return NULL;
			}
		else
			{
			M2MLogger_printErrorMessage((unsigned char *)"M2MTableBuilder.this_getColumnDefinition()", __LINE__, (unsigned char *)"引数で指定された\"M2MColumnList *\"から取得した\"M2MColumn *\"のデータ型を示す文字列がNULLです", NULL);
			return NULL;
			}
		}
	//===== Argument error =====
	else if (columnList==NULL)
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MTableBuilder.this_getColumnDefinition()", __LINE__, (unsigned char *)"引数で指定された\"M2MColumnList *\"がNULLです", NULL);
		return NULL;
		}
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MTableBuilder.this_getColumnDefinition()", __LINE__, (unsigned char *)"引数で指定された\"columnDefinition\"がNULLです", NULL);
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
static M2MColumnList *this_getColumnList (const M2MTableBuilder *self)
	{
	//===== Check argument =====
	if (self!=NULL)
		{
		return self->columnList;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MTableBuilder.this_getColumnList()", __LINE__, (unsigned char *)"引数で指定された\"M2MTableBuilder *\"がNULLです", NULL);
		return NULL;
		}
	}


/**
 * @param[in] self	テーブル構築オブジェクト
 * @param[out] sql	テーブル構築のためのSQL文を示す文字列をコピーするためのバッファ（バッファリング自体は関数内で実施するため呼び出し側はポインタ指定のみ）
 * @return			引数に与えた"sql"ポインタにコピーされた文字列の先頭ポインタ or NULL（エラーの場合）
 */
static unsigned char *this_getTableCreateSQL (const M2MTableBuilder *self, unsigned char **sql)
	{
	//========== Variable ==========
	unsigned char *tableName = NULL;
	M2MColumnList *columnList = NULL;
	unsigned int columnListLength = 0;
	unsigned int i = 0;
	unsigned char *columnDefinition = NULL;

	//===== Check argument =====
	if (self!=NULL && sql!=NULL)
		{
		//===== テーブル情報の取得 =====
		if ((tableName=this_getTableName(self))!=NULL
				&& (columnList=M2MColumnList_begin(this_getColumnList(self)))!=NULL
				&& (columnListLength=M2MColumnList_length(columnList))>0)
			{
			//===== SQLコマンドとテーブル名をセット =====
			M2MString_append(sql, (unsigned char *)"CREATE TABLE '");
			M2MString_append(sql, tableName);
			//===== カラム定義の準備 =====
			M2MString_append(sql, (unsigned char *)"' (");
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
						M2MString_append(sql, (unsigned char *)", ");
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
					M2MLogger_printErrorMessage((unsigned char *)"M2MTableBuilder.this_getTableCreateSQL()", __LINE__, (unsigned char *)"カラム定義文字列の作成に失敗しました", NULL);
					//===== ヒープメモリ領域の解放 =====
					M2MHeap_free(columnDefinition);
					M2MHeap_free((*sql));
					return NULL;
					}
				//===== 次のカラムに移動 =====
				columnList = M2MColumnList_next(columnList);
				}
			//===== SQL文の作成を終了 =====
			M2MString_append(sql, (unsigned char *)") ");
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
		M2MLogger_printErrorMessage((unsigned char *)"M2MTableBuilder.this_getTableCreateSQL()", __LINE__, (unsigned char *)"引数で指定された\"M2MTableBuilder *\"がNULLです", NULL);
		return NULL;
		}
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MTableBuilder.this_getTableCreateSQL()", __LINE__, (unsigned char *)"引数で指定された\"sql\"がNULLです", NULL);
		return NULL;
		}
	}


/**
 * 引数で指定されたテーブル構築オブジェクトが保有しているテーブル名文字列を取得する。<br>
 *
 * @param[in] self	テーブル構築オブジェクト
 * @return			テーブル名を示す文字列
 */
static unsigned char *this_getTableName (const M2MTableBuilder *self)
	{
	//===== Check argument =====
	if (self!=NULL)
		{
		return self->tableName;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MTableBuilder.this_getTableName()", __LINE__, (unsigned char *)"引数で指定された\"M2MTableBuilder *\"がNULLです", NULL);
		return NULL;
		}
	}


/**
 * 引数で指定されたテーブル構築オブジェクトがメンバ変数として保持するテーブル名を<br>
 * 示す文字列に対し, 同一名のテーブルがデータベースに存在するかどうかを確認する。<br>
 *
 * @param[in] self		テーブル構築オブジェクト
 * @param[in] database	テーブル名確認対象のSQLite3データベース
 * @return				true : 同一テーブル名のテーブルが存在, false : 同一テーブル名のテーブルは存在しない
 */
static bool this_isExistingTable (const M2MTableBuilder *self, sqlite3 *database)
	{
	//========== Variable ==========
	sqlite3_stmt *statement = NULL;
	unsigned char sql[256];
	unsigned char *tableName = NULL;
#ifdef DEBUG
	unsigned char MESSAGE[256];
#endif /* DEBUG */

	//===== Check argument =====
	if (self!=NULL && database!=NULL)
		{
		//===== 配列の初期化 =====
		memset(sql, 0, sizeof(sql));
		//===== SQL文を配列へコピー =====
		snprintf(sql, sizeof(sql)-1, (unsigned char *)"SELECT COUNT(DISTINCT name) AS result FROM sqlite_master WHERE type='table' AND name='%s'", tableName=this_getTableName(self));
		//===== SQLをセット =====
		if (sqlite3_prepare(database, sql, -1, &statement, NULL)==SQLITE_OK)
			{
			//===== SQL実行 =====
			while (sqlite3_step(statement)==SQLITE_ROW)
				{
				//===== 同一テーブル名が見つかった場合 =====
				if (sqlite3_column_int(statement, 0)==1)
					{
					sqlite3_finalize(statement);
#ifdef DEBUG
					memset(MESSAGE, 0, sizeof(MESSAGE));
					snprintf(MESSAGE, sizeof(MESSAGE)-1, (unsigned char *)"SQLiteデータベース上に\"%s\"テーブルは既に存在しています", tableName);
					M2MLogger_printDebugMessage((unsigned char *)"M2MTableBuilder.this_isExistingTable()", __LINE__, MESSAGE);
#endif /* DEBUG */
					return true;
					}
				//===== 同一テーブル名が見つからなかった場合 =====
				else
					{
					sqlite3_finalize(statement);
#ifdef DEBUG
					memset(MESSAGE, 0, sizeof(MESSAGE));
					snprintf(MESSAGE, sizeof(MESSAGE)-1, (unsigned char *)"SQLiteデータベース上に\"%s\"テーブルは未だ存在していません", tableName);
					M2MLogger_printDebugMessage((unsigned char *)"M2MTableBuilder.this_isExistingTable()", __LINE__, MESSAGE);
#endif /* DEBUG */
					return false;
					}
				}
			//===== Error handling =====
			M2MLogger_printErrorMessage((unsigned char *)"M2MTableBuilder.this_isExistingTable()", __LINE__, sqlite3_errmsg(database), NULL);
			return false;
			}
		//===== Error handling =====
		else
			{
			M2MLogger_printErrorMessage((unsigned char *)"M2MTableBuilder.this_isExistingTable()", __LINE__, sqlite3_errmsg(database), NULL);
			sqlite3_finalize(statement);
			return false;
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MTableBuilder.this_isExistingTable()", __LINE__, (unsigned char *)"引数で指定された\"M2MTableBuilder *\"がNULLです", NULL);
		return false;
		}
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MTableBuilder.this_isExistingTable()", __LINE__, (unsigned char *)"引数で指定された\"sqlite3 *\"がNULLです", NULL);
		return false;
		}
	}


/**
 * 引数で指定されたテーブル構築オブジェクトの1つ次のノードを返す．<br>
 *
 * @param[in] tableBuilder	テーブル構築オブジェクト
 * @return					引数で指定されたテーブル構築オブジェクトの1つ次のノード or NULL（エラーの場合）
 */
static M2MTableBuilder *this_next (const M2MTableBuilder *self)
	{
	//===== Check argument =====
	if (self!=NULL)
		{
		return self->next;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MTableBuilder.this_next()", __LINE__, (unsigned char *)"引数で指定された\"M2MTableBuilder *\"がNULLです", NULL);
		return NULL;
		}
	}


/**
 * 引数で指定されたテーブル構築オブジェクトの1つ前のノードを返す．<br>
 *
 * @param[in] tableBuilder	テーブル構築オブジェクト
 * @return					引数で指定されたテーブル構築オブジェクトの1つ前のノード or NULL（エラーの場合）
 */
static M2MTableBuilder *this_previous (const M2MTableBuilder *self)
	{
	//===== Check argument =====
	if (self!=NULL)
		{
		return self->previous;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MTableBuilder.this_previous()", __LINE__, (unsigned char *)"引数で指定された\"M2MTableBuilder *\"がNULLです", NULL);
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
static M2MTableBuilder *this_setColumnList (M2MTableBuilder *self, M2MColumnList *columnList)
	{
	//===== Check argument =====
	if (self!=NULL && columnList!=NULL)
		{
		self->columnList = columnList;
		return self;
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MTableBuilder.this_begin()", __LINE__, (unsigned char *)"引数で指定された\"M2MTableBuilder *\"がNULLです", NULL);
		return NULL;
		}
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MTableBuilder.this_begin()", __LINE__, (unsigned char *)"引数で指定された\"M2MColumnList *\"がNULLです", NULL);
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
static M2MTableBuilder *this_setNext (M2MTableBuilder *self, M2MTableBuilder *next)
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
		M2MLogger_printErrorMessage((unsigned char *)"M2MTableBuilder.this_setNext()", __LINE__, (unsigned char *)"引数で指定された\"M2MTableBuilder *\"がNULLです", NULL);
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
static M2MTableBuilder *this_setPrevious (M2MTableBuilder *self, M2MTableBuilder *previous)
	{
	//===== Check argument =====
	if (self!=NULL && previous!=NULL)
		{
		self->previous = previous;
		return self;
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MTableBuilder.this_setPrevious()", __LINE__, (unsigned char *)"引数で指定された\"M2MTableBuilder *\"がNULLです", NULL);
		return NULL;
		}
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MTableBuilder.this_setPrevious()", __LINE__, (unsigned char *)"引数で指定された\"previous\"がNULLです", NULL);
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
static M2MTableBuilder *this_setTableName (M2MTableBuilder *self, const unsigned char *tableName)
	{
	//========== Variable ==========
	size_t tableNameLength = 0;

	//===== Check argument =====
	if (self!=NULL && tableName!=NULL)
		{
		//===== テーブル名を示す文字列の長さを取得 =====
		if ((tableNameLength=M2MString_length(tableName))>0)
			{
			//===== テーブル名を示すメンバ変数の初期化 =====
			this_deleteTableName(self);
			//===== ヒープメモリ領域の獲得 =====
			if ((self->tableName=(unsigned char *)M2MHeap_malloc(tableNameLength+1))!=NULL)
				{
				//===== テーブル名を示す文字列のコピー =====
				memcpy(self->tableName, tableName, tableNameLength);
				return self;
				}
			//===== Error handling =====
			else
				{
				M2MLogger_printErrorMessage((unsigned char *)"M2MTableBuilder.this_setTableName()", __LINE__, (unsigned char *)"テーブル名を示す文字列をコピーするためのヒープメモリ領域の獲得に失敗しました", NULL);
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			M2MLogger_printErrorMessage((unsigned char *)"M2MTableBuilder.this_setTableName()", __LINE__, (unsigned char *)"引数で指定された\"tableName\"の文字列の大きさが0以下です", NULL);
			return NULL;
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MTableBuilder.this_setTableName()", __LINE__, (unsigned char *)"引数で指定された\"M2MTableBuilder *\"がNULLです", NULL);
		return NULL;
		}
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MTableBuilder.this_setTableName()", __LINE__, (unsigned char *)"引数で指定された\"tableName\"がNULLです", NULL);
		return NULL;
		}
	}



/*******************************************************************************
 * 公開メソッド
 ******************************************************************************/
/**
 * 引数で指定されたSQLite3データベースに対し, テーブルを作成する。<br>
 *
 * @param[in] self		（テーブル情報挿入済みの）テーブル構築オブジェクト
 * @param[in] database	テーブル作成対象のSQLite3データベース
 */
void M2MTableBuilder_createTable (M2MTableBuilder *self, sqlite3 *database)
	{
	//========== Variable ==========
	M2MTableBuilder *next = NULL;
	bool executeUpdate = false;
	unsigned char *sql = NULL;
	unsigned char MESSAGE[256];

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
				if (this_isExistingTable(self, database)==true)
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
						M2MLogger_printDebugMessage((unsigned char *)"M2MTableBuilder_createTable()", __LINE__, (unsigned char *)"CEPテーブルの構築処理を開始します");
#endif // DEBUG
						//===== トランザクション開始 =====
						SQLRunner_beginTransaction(database);
						//===== フラグを立てる =====
						executeUpdate = true;
						}
					//===== テーブル構築用のSQL文の文字列を取得 =====
					if (this_getTableCreateSQL(self, &sql)!=NULL)
						{
						//===== SQL実行 =====
						if (SQLRunner_executeUpdate(database, sql)==true)
							{
#ifdef DEBUG
							M2MLogger_printDebugMessage((unsigned char *)"M2MTableBuilder_createTable()", __LINE__, (unsigned char *)"CEPテーブル構築のためのCREATE文を実行しました");
#endif // DEBUG
							}
						//===== Error handling =====
						else
							{
							M2MLogger_printErrorMessage((unsigned char *)"M2MTableBuilder_createTable()", __LINE__, (unsigned char *)sqlite3_errmsg(database), NULL);
							}
						//===== SQL文のヒープメモリ領域を解放 =====
						M2MHeap_free(sql);
						}
					//===== Error handling =====
					else
						{
						memset(MESSAGE, 0, sizeof(MESSAGE));
						snprintf(MESSAGE, sizeof(MESSAGE)-1, (unsigned char *)"テーブル（＝\"%s\"）を構築するためのSQL文を取得するのに失敗しました", this_getTableName(self));
						M2MLogger_printErrorMessage((unsigned char *)"M2MTableBuilder_createTable()", __LINE__, MESSAGE, NULL);
						}
					}
				//===== 次のノードへ進む =====
				self = next;
				}
			//===== 同一名のテーブルが存在する場合 =====
			if (this_isExistingTable(self, database)==true)
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
					M2MLogger_printDebugMessage((unsigned char *)"M2MTableBuilder_createTable()", __LINE__, (unsigned char *)"CEPテーブルの構築処理を開始します");
#endif // DEBUG
					//===== トランザクション開始 =====
					SQLRunner_beginTransaction(database);
					//===== フラグを立てる =====
					executeUpdate = true;
					}
				//===== 末端ノードのテーブル構築用のSQL文の文字列を取得 =====
				if (this_getTableCreateSQL(self, &sql)!=NULL)
					{
					//===== SQL実行 =====
					if (SQLRunner_executeUpdate(database, sql)==true)
						{
#ifdef DEBUG
						M2MLogger_printDebugMessage((unsigned char *)"M2MTableBuilder_createTable()", __LINE__, (unsigned char *)"CEPテーブル構築のためのCREATE文を実行しました");
#endif // DEBUG
						}
					//===== Error handling =====
					else
						{
						M2MLogger_printErrorMessage((unsigned char *)"M2MTableBuilder_createTable()", __LINE__, sqlite3_errmsg(database), NULL);
						}
					//===== SQL文のヒープメモリ領域を解放 =====
					M2MHeap_free(sql);
					}
				//===== Error handling =====
				else
					{
					memset(MESSAGE, 0, sizeof(MESSAGE));
					snprintf(MESSAGE, sizeof(MESSAGE)-1, (unsigned char *)"テーブル（＝\"%s\"）を構築するためのSQL文を取得するのに失敗しました", this_getTableName(self));
					M2MLogger_printErrorMessage((unsigned char *)"M2MTableBuilder_createTable()", __LINE__, MESSAGE, NULL);
					}
				}
			//===== テーブル構築用のSQL文を実行済みの場合 =====
			if (executeUpdate==true)
				{
				//===== トランザクション終了 =====
				SQLRunner_commitTransaction(database);
#ifdef DEBUG
				M2MLogger_printDebugMessage((unsigned char *)"M2MTableBuilder_createTable()", __LINE__, (unsigned char *)"CEPテーブル構築処理を終了します");
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
			M2MLogger_printErrorMessage((unsigned char *)"M2MTableBuilder_createTable()", __LINE__, (unsigned char *)"引数で指定された\"M2MTableBuilder *\"から先頭ノードを取得するのに失敗しました", NULL);
			return;
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MTableBuilder_createTable()", __LINE__, (unsigned char *)"引数で指定された\"M2MTableBuilder *\"がNULLです", NULL);
		return;
		}
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MTableBuilder_createTable()", __LINE__, (unsigned char *)"引数で指定された\"sqlite3 *\"がNULLです", NULL);
		return;
		}
	}


/**
 * 引数で指定されたテーブル構築オブジェクトのメモリ領域を解放する．<br>
 * この関数を呼び出した後は当該変数にアクセス出来なくなる事に留意する．<br>
 *
 * @param[in,out] self	（ヒープ）メモリ領域解放対象のテーブル構築オブジェクト
 */
void M2MTableBuilder_delete (M2MTableBuilder **self)
	{
	//========== Variable ==========
	M2MTableBuilder *next = NULL;

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
				M2MLogger_printDebugMessage((unsigned char *)"M2MTableBuilder_delete()", __LINE__, (unsigned char *)"CEPテーブル構築オブジェクトのメモリ領域を解放しました");
#endif // DEBUG
				}
			//===== Error handling =====
			else
				{
				M2MLogger_printErrorMessage((unsigned char *)"M2MTableBuilder_delete()", __LINE__, (unsigned char *)"CEPテーブル構築オブジェクトの末端ノードがNULLのため, メモリ領域を解放出来ません", NULL);
				}
			}
		//===== Error handling =====
		else
			{
			M2MLogger_printErrorMessage((unsigned char *)"M2MTableBuilder_delete()", __LINE__, (unsigned char *)"CEPテーブル構築オブジェクトのメモリ領域を解放する処理において先頭ノードの取得に失敗しました", NULL);
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MTableBuilder_delete()", __LINE__, (unsigned char *)"引数で指定されたCEPテーブル構築オブジェクトがNULLです", NULL);
		}
	return;
	}


/**
 * 引数で指定されたテーブル構築オブジェクトからテーブル名に一致するカラム構造体オブジェクトを取得する．
 *
 * @param self		テーブル構築オブジェクト
 * @param tableName	テーブル名を示す文字列
 * @return			テーブル名が一致したカラム構造体オブジェクト or NULL（エラーの場合)
 */
M2MColumnList *M2MTableBuilder_getColumnList (M2MTableBuilder *self, const unsigned char *tableName)
	{
	//===== Check argument =====
	if (self!=NULL && tableName!=NULL)
		{
		//===== 先頭ノードの取得 =====
		if ((self=this_begin(self))!=NULL)
			{
			//===== 末端に辿り着くまで繰り返し =====
			while (this_next(self)!=NULL)
				{
				//===== テーブル名が一致した場合 =====
				if (M2MString_compareTo(this_getTableName(self), tableName)==0)
					{
					//===== カラム構造体オブジェクトを返す =====
					return this_getColumnList(self);
					}
				//===== テーブル名が一致しない場合 =====
				else
					{
					// そのまま繰り返し
					}
				//===== 次のノードへ移動 =====
				self = (M2MTableBuilder *)this_next(self);
				}
			//===== 末端ノードのテーブル名が一致した場合 =====
			if (M2MString_compareTo(this_getTableName(self), tableName)==0)
				{
				//===== カラム構造体オブジェクトを返す =====
				return this_getColumnList(self);
				}
			//===== 末端ノードのテーブル名が一致しない場合 =====
			else
				{
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			return NULL;
			}
		}
	//===== Argument error =====
	else
		{
		return NULL;
		}
	}


/**
 * 引数で指定されたテーブル名に対し, テーブル情報（フィールド名）を表示するための<br>
 * SQL文を構築し, 引数で指定されたポインタにコピーする。<br>
 *
 * @param[in] tableName	テーブル名を示す文字列
 * @param[out] sql		SQL文をコピーするためのポインタ（バッファリングは関数内部で実行する）
 * @return				引数のポインタにコピーした文字列 or NULL（エラーの場合）
 */
unsigned char *M2MTableBuilder_getTableInfoSQL (const unsigned char *tableName, unsigned char **sql)
	{
	//========== Variable ==========
	size_t tableNameLength = 0;
	const unsigned char *FORMAT = (unsigned char *)"PRAGMA table_info(%s)";
	const size_t FORMAT_LENGTH = M2MString_length(FORMAT);

	//===== Check argument =====
	if (tableName!=NULL && sql!=NULL)
		{
		//===== テーブル名の取得 =====
		if ((tableNameLength=M2MString_length(tableName))>0)
			{
			//===== SQL文のためのヒープメモリ領域の獲得 =====
			if (((*sql)=(unsigned char *)M2MHeap_malloc(FORMAT_LENGTH+tableNameLength+1))!=NULL)
				{
				//===== SQL文のコピー =====
				snprintf((*sql), FORMAT_LENGTH+tableNameLength, FORMAT, tableName);
				return (*sql);
				}
			//===== Error handling =====
			else
				{
				M2MLogger_printErrorMessage((unsigned char *)"M2MTableBuilder_getTableInfoSQL()", __LINE__, (unsigned char *)"SQL文字列をコピーするためのヒープメモリ領域の獲得に失敗しました", NULL);
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			M2MLogger_printErrorMessage((unsigned char *)"M2MTableBuilder_getTableInfoSQL()", __LINE__, (unsigned char *)"引数で指定された\"tableName\"が示す文字列の長さが0以下です", NULL);
			return NULL;
			}
		}
	//===== Argument error =====
	else if (tableName==NULL)
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MTableBuilder_getTableInfoSQL()", __LINE__, (unsigned char *)"引数で指定された\"tableName\"がNULLです", NULL);
		return NULL;
		}
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MTableBuilder_getTableInfoSQL()", __LINE__, (unsigned char *)"引数で指定された\"sql\"がNULLです", NULL);
		return NULL;
		}
	}


/**
 * ヒープメモリ領域を獲得し, 新規にテーブル構築オブジェクトを作成する．<br>
 *
 * @return		新規作成したテーブル構築オブジェクト
 */
M2MTableBuilder *M2MTableBuilder_new ()
	{
	//========== Variable ==========
	M2MTableBuilder *self = NULL;

	//===== ヒープメモリ領域の獲得 =====
	if ((self=(M2MTableBuilder *)M2MHeap_malloc(sizeof(M2MTableBuilder)))!=NULL)
		{
		//===== 初期化 =====
		this_setPrevious(self, self);
		this_setNext(self, NULL);
#ifdef DEBUG
		M2MLogger_printDebugMessage((unsigned char *)"M2MTableBuilder_new()", __LINE__, (unsigned char *)"CEPテーブル構築オブジェクトを新規作成しました");
#endif // DEBUG
		return self;
		}
	//===== Error handling =====
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MTableBuilder_new()", __LINE__, (unsigned char *)"ヒープメモリ領域の獲得に失敗しました", NULL);
		return NULL;
		}
	}


/**
 * 引数で指定されたカラム情報を, 同じく引数で指定されたテーブル構築オブジェクトに<br>
 * 格納する。<br>
 *
 * @param[in,out] self		テーブル作成のためのカラム設定情報を格納したテーブル構築オブジェクト
 * @param[in] tableName		テーブル名を示す文字列
 * @param[in] columnList	カラム設定情報を格納したカラム構造体オブジェクト
 * @return					カラム設定情報を含むテーブル構築オブジェクト or NULL（エラーの場合)
 */
M2MTableBuilder *M2MTableBuilder_setConfig (M2MTableBuilder *self, const unsigned char *tableName, M2MColumnList *columnList)
	{
	//===== Check argument =====
	if (self!=NULL && tableName!=NULL && columnList!=NULL)
		{
		//===== メンバ変数へセット =====
		if (this_setTableName(self, tableName)!=NULL
				&& this_setColumnList(self, columnList)!=NULL)
			{
			return self;
			}
		//===== Error handling =====
		else
			{
			M2MLogger_printErrorMessage((unsigned char *)"M2MTableBuilder_setConfig()", __LINE__, (unsigned char *)"引数で指定された\"M2MTableBuilder *\"にテーブル名, もしくはカラム構造体オブジェクトをセットするのに失敗しました", NULL);
			return NULL;
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MTableBuilder_setConfig()", __LINE__, (unsigned char *)"引数で指定された\"M2MTableBuilder *\"がNULLです", NULL);
		return NULL;
		}
	else if (tableName==NULL)
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MTableBuilder_setConfig()", __LINE__, (unsigned char *)"引数で指定された\"tableName\"がNULLです", NULL);
		return NULL;
		}
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MTableBuilder_setConfig()", __LINE__, (unsigned char *)"引数で指定された\"M2MColumnList *\"がNULLです", NULL);
		return NULL;
		}
	}



/* End Of File */
