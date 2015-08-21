/*******************************************************************************
 * M2MCEP.c
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

#include "m2m/cep/M2MCEP.h"


/*******************************************************************************
 * 関数定義
 ******************************************************************************/
/**
 * 引数で指定されたSQLite3データベースファイル名を示す文字列から、データベース<br>
 * ファイルパスを示す文字列を作成し、引数で指定されたバッファにコピーする。<br>
 * 処理に失敗した場合は戻り値として NULL を返す。<br>
 *
 * @param[in] databaseName			SQLite3データベースファイル名を示す文字列
 * @param[out] databaseFilePath		SQLite3データベースファイルパスを示す文字列を格納するためのバッファ
 * @return							SQLite3データベースファイルパスを示す文字列のポインタ or NULL（エラーの場合）
 */
static unsigned char *this_getDatabaseFilePath (const unsigned char *databaseName, unsigned char **databaseFilePath);


/**
 * 引数で指定されたCEP実行オブジェクトが保有するデータベース名を示す文字列を返す。<br>
 *
 * @param[in] self	CEP実行オブジェクト
 * @return			データベース名を示す文字列 or NULL（エラーの場合）
 */
static unsigned char *this_getDatabaseName (const M2MCEP *self);


/**
 * 引数で指定されたCEP実行オブジェクトが管理しているSQLite3ファイルデータベース<br>
 * オブジェクトを取得して返す。<br>
 *
 * @param[in] self	CEP実行オブジェクト
 * @return			SQLite3ファイルデータベース or NULL（エラーの場合）
 */
static sqlite3 *this_getFileDatabase (M2MCEP *self);


/**
 * 引数で指定されたCEP実行オブジェクトが管理するCEPレコード情報オブジェクトを取得<br>
 * して返す。<br>
 *
 * @param[in] self	CEP実行オブジェクト
 * @return			CEPレコード情報オブジェクト
 */
static M2MCEPRecord *this_getCEPRecord (const M2MCEP *self);


/**
 * 引数で指定されたCEP実行オブジェクトが保持しているCEPレコード数の上限値を返す。<br>
 *
 * @param[in] self	CEP実行オブジェクト
 * @return			CEPレコード数の上限値
 */
static unsigned int this_getMaxRecord (const M2MCEP *self);


/**
 * 引数で指定されたCEP実行オブジェクトが保持している挿入済みレコード数を返す．<br>
 *
 * @param self	CEP実行オブジェクト
 * @return		挿入済みレコード数
 */
static unsigned int this_getRecordCounter (const M2MCEP *self);


/**
 * SQLite3データベースがバキューム処理を実行するレコード数を返す．<br>
 *
 * @param[in] self	CEP実行オブジェクト
 * @return			SQLite3データベースがバキューム処理を実行するレコード数
 */
static unsigned int this_getVacuumRecord (const M2MCEP *self);


/**
 * 引数で指定されたCEP実行オブジェクトが管理しているSQLite3メモリーデータベース<br>
 * オブジェクトを取得して返す。<br>
 *
 * @param[in] self	CEP実行オブジェクト
 * @return			SQLite3メモリーデータベース or NULL（エラーの場合）
 */
static sqlite3 *this_getMemoryDatabase (M2MCEP *self);


/**
 * 引数で指定されたCEP構造体オブジェクトから、永続性の可否を示すフラグを取得する．<br>
 *
 * @param[in,out] self		CEP構造体オブジェクト
 * @return					永続性の可否を示すフラグ
 */
static bool this_getPersistence (const M2MCEP *self);


/**
 * CEP実行オブジェクトがメンバ変数として保持しているテーブル構築オブジェクトを<br>
 * 返す。<br>
 *
 * @param[in] self	CEP実行オブジェクト
 * @return			テーブル構築オブジェクト or NULL（エラーの場合）
 */
static M2MTableBuilder *this_getTableBuilder (const M2MCEP *self);


/**
 * 引数で指定されたSQLite3データベースにレコードを一括挿入する．<br>
 *
 * @param[in] database		SQLite3データベース管理オブジェクト
 * @param[in] record		レコード管理オブジェクト
 * @param[in] tableBuilder	テーブル構築オブジェクト
 * @param[in] recordList	レコード情報オブジェクト
 * @return					挿入したレコード数 or -1（エラーの場合)
 */
static int this_insertRecordList (sqlite3 *database, const M2MCEPRecord *record, M2MTableBuilder *tableBuilder, M2MList *recordList);


/**
 * 引数で指定されたCEP実行オブジェクトが保持する、CEPレコード管理オブジェクトの<br>
 * “メモリ上のSQLite3データベースに挿入済み”のレコードに対し、規定している最大<br>
 * レコード数を超過している場合、超過分のレコードをファイル上のSQLite3データベース<br>
 * に挿入し、永続化を行う。<br>
 *
 * @param[in,out] self	CEP実行オブジェクト
 * @return				処理に成功したCEP実行オブジェクト or NULL（挿入エラーの場合）
 */
static M2MCEP *this_insertRecordListToFileDatabase (M2MCEP *self);


/**
 * 引数で指定されたCEP実行オブジェクトのメンバ変数が保有する、メモリ上のSQLite3<br>
 * データベースに未挿入のレコード（＝CEPRecord->newRecordList）に対し、メモリ<br>
 * 上のSQLite3データベースへ一括挿入処理を行う。<br>
 *
 * @param[in] self	CEP実行オブジェクト
 * @return			挿入処理を行ったレコード数 or -1（エラーの場合）
 */
static int this_insertRecordListToMemoryDatabase (M2MCEP *self);


/**
 * 引数で指定されたパスに、同じく引数で指定されたパーミッションのディレクトリを<br>
 * 作成する。<br>
 *
 * @param[in] directoryPath		ディレクトリパスを示す文字列
 * @param[in] permission		パーミッションを示す3桁の整数文字列
 * @return						ディレクトリパスを示す文字列 or NULL（エラーの場合）
 */
static unsigned char *this_makeDirectory (const unsigned char *directoryPath, const unsigned char *permission);


/**
 * 引数で指定されたデータベース名のSQLite3データベースファイルを規定ディレクトリ下<br>
 * に作成し、当該データベースオブジェクトを返す。<br>
 * 作成に失敗した場合は NULL を返す。<br>
 *
 * @param[in] databaseName	データベース名を示す文字列
 * @param[in] tableBuilder	テーブル構築用オブジェクト
 * @param[in] vacuumRecord	バキューム処理を実行するレコード数（この値が0の場合は自動バキュームを設定する)
 * @return					SQLite3データベースファイルオブジェクト or NULL（エラーの場合）
 */
static sqlite3 *this_openFileDatabase (const unsigned char *databaseName, const M2MTableBuilder *tableBuilder, const unsigned int vacuumRecord);


/**
 * メモリ上のSQLite3データベースを開く。<br>
 * 当該データベースには引数で指定されたテーブルを構築する。
 *
 * @param[in] tableBuilder	テーブル構築用オブジェクト
 * @param[in] vacuumRecord	バキューム処理を実行するレコード数（この値が0の場合は自動バキュームを設定する)
 * @return					メモリ上のSQLite3データベース or NULL（エラーの場合）
 */
static sqlite3 *this_openMemoryDatabase (const M2MTableBuilder *tableBuilder, const unsigned int vacuumRecord);


/**
 * 引数で指定されたデータベースに対し，自動バキュームを設定する．<br>
 *
 * @param[in] database	自動バキュームを設定するSQLite3データベースオブジェクト
 * @param[in] flag		true : 自動バキュームを有効にする，false : 自動バキュームを無効にする
 */
static void this_setAutoVacuum (sqlite3 *database, const bool flag);


/**
 * 引数で指定されたCEP実行オブジェクトにファイル上のSQLite3データベース名をメンバ<br>
 * 変数に設定する。<br>
 *
 * @param[in,out] self		CEP実行オブジェクト
 * @param[in] databaseName	ファイル上のSQLite3データベース名（ファイル名）
 * @return					メンバ変数が設定されたCEP実行オブジェクト or NULL（エラーの場合）
 */
static M2MCEP *this_setDatabaseName (M2MCEP *self, const unsigned char *databaseName);


/**
 * 引数で指定されたCEP実行オブジェクトの保有するメンバー変数として、SQLite3<br>
 * ファイルデータベース管理オブジェクトをセットする。<br>
 *
 * @param[in,out] self		CEP実行オブジェクト
 * @param[in] fileDatabase	SQLite3ファイルデータベース管理オブジェクト
 * @return					CEP実行オブジェクト or NULL（エラーの場合）
 */
static M2MCEP *this_setFileDatabase (M2MCEP *self, sqlite3 *fileDatabase);


/**
 * 引数で指定されたCEP実行オブジェクトの保有するメンバー変数として、メモリー上の<br>
 * SQLite3データベース管理オブジェクトをセットする。<br>
 *
 * @param[in,out] self		CEP実行オブジェクト
 * @param[in] fileDatabase	メモリー上のSQLite3データベース管理オブジェクト
 * @return					CEP実行オブジェクト or NULL（エラーの場合）
 */
static M2MCEP *this_setMemoryDatabase (M2MCEP *self, sqlite3 *memoryDatabase);


/**
 * 引数で指定されたCEP実行オブジェクトに対し、メンバ変数としてテーブル構築<br>
 * オブジェクトをセットする。<br>
 *
 * @param[in,out] self		CEP実行オブジェクト
 * @param[in] tableBuilder	テーブル構築オブジェクト
 * @return					テーブル構築オブジェクトをセットしたCEP実行オブジェクト
 */
static M2MCEP *this_setTableBuilder (M2MCEP *self, const M2MTableBuilder *tableBuilder);


/**
 * 引数で指定されたデータベースに対し、文字コードをUTF-8に設定する。<br>
 *
 * @param[in] database	文字コードをUTF-8に設定するSQLite3データベースオブジェクト
 */
static void this_setUTF8 (sqlite3 *database);


/**
 * @param[in] dataType		フィールドのデータ型を示す列挙子
 * @param[in] index			フィールドのインデックス（>=1)
 * @param[in] value			フィールドの入力データを示す文字列
 * @param[in] valueLength	フィールドの入力データを示す文字列サイズ[Byte]
 * @param[out] statement	プリペアドステートメントオブジェクト
 */
static void this_setValueIntoPreparedStatement (const M2MDataType dataType, unsigned int index, const unsigned char *value, const size_t valueLength, sqlite3_stmt *statement);


/**
 * SQLite3データベースのジャーナルモードをWAL(Write Ahead Logging)にセットする．<br>
 *
 * @param[in] database		WAL設定対象のSQLite3データベース管理オブジェクト
 * @param[in] synchronous	true : 同期モード，false : 非同期モード
 */
static void this_setWAL (sqlite3 *database, const bool synchronous);


/**
 * SQLite3データベースに対してバキューム処理を実行するためのレコード数を更新する．<br>
 *
 * @param[in] self	CEP実行オブジェクト
 * @param[in] count	カウンターに付加するレコード数（>0) or 0（カウンターを初期化する場合に指定)
 */
static void this_updateRecordCounter (M2MCEP *self, const unsigned int count);


/**
 * SQLite3データベースのバキューム処理を実行する．<br>
 *
 * @param database	バキューム処理対象のSQLite3データベース管理オブジェクト
 */
static void this_vacuum (sqlite3 *database);



/*******************************************************************************
 * 内部関数
 ******************************************************************************/
/**
 * メモリ上のSQLite3データベースに対し、引数で指定されたテーブルのレコード数が<br>
 * 上限値を超えていない事を確認する。<br>
 * もし上限値を超えている場合、挿入されたレコードの先頭順に、レコード数が上限値<br>
 * に一致するまでレコードを削除する。<br>
 *
 * @param[in] self		CEP実行オブジェクト
 * @param[in] tableName	テーブル名を示す文字列
 */
static void this_adjustMemoryDatabaseRecord (M2MCEP *self, const unsigned char *tableName)
	{
	//========== ローカル変数 ==========
	int excess = 0;
	unsigned char *sql = NULL;
	const unsigned int MAX_RECORD = this_getMaxRecord(self);
	const M2MCEPRecord *RECORD = M2MCEPRecord_begin(this_getCEPRecord(self));
	const M2MList *OLD_RECORD_LIST = M2MList_begin(M2MCEPRecord_getOldRecordList(RECORD));
	const unsigned int OLD_RECORD_LIST_LENGTH = M2MList_length((M2MList *)OLD_RECORD_LIST);
	const unsigned char *DELETE_SQL = (unsigned char *)"DELETE FROM %s WHERE rowid IN (SELECT rowid FROM %s ORDER BY rowid LIMIT %u) ";
	const size_t TABLE_NAME_LENGTH = M2MString_length(tableName);
	const size_t DELETE_SQL_LENGTH = M2MString_length(DELETE_SQL) + TABLE_NAME_LENGTH + TABLE_NAME_LENGTH + 64;
#ifdef DEBUG
	unsigned char MESSAGE[256];
#endif // DEBUG

	//===== 引数の確認 =====
	if (self!=NULL && tableName!=NULL && TABLE_NAME_LENGTH>0)
		{
		//===== レコード情報の取得 =====
		if (MAX_RECORD>0 && OLD_RECORD_LIST!=NULL && OLD_RECORD_LIST_LENGTH>0)
			{
			//===== レコード数が上限値を超過している場合 =====
			if ((excess=OLD_RECORD_LIST_LENGTH-MAX_RECORD)>0)
				{
#ifdef DEBUG
				memset(MESSAGE, 0, sizeof(MESSAGE));
				snprintf(MESSAGE, sizeof(MESSAGE)-1, (unsigned char *)"メモリ上のSQLite3データベースの\"%s\"テーブルのレコード数は上限値を\"%d\"個超過しています", tableName, excess);
				M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_adjustMemoryDatabaseRecord()", __LINE__, MESSAGE);
#endif // DEBUG
				//===== メモリ上のSQLite3データベースを取得 =====
				if ((sql=(unsigned char *)M2MHeap_malloc(DELETE_SQL_LENGTH+1))!=NULL)
					{
					//===== DELETE文の作成 =====
					snprintf(sql, DELETE_SQL_LENGTH, DELETE_SQL, tableName, tableName, excess);
					//===== DELETE実行 =====
					if (SQLRunner_executeUpdate(this_getMemoryDatabase(self), sql)==true)
						{
#ifdef DEBUG
						memset(MESSAGE, 0, sizeof(MESSAGE));
						snprintf(MESSAGE, sizeof(MESSAGE)-1, (unsigned char *)"メモリ上のSQLite3データベースの\"%s\"テーブルのレコードを\"%d\"個削除しました", tableName, excess);
						M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_adjustMemoryDatabaseRecord()", __LINE__, MESSAGE);
#endif // DEBUG
						M2MHeap_free(sql);
						return;
						}
					//===== エラー処理 =====
					else
						{
						M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_executeUpdate()", __LINE__, (unsigned char *)"メモリ上のSQLite3データベースの超過レコード削除処理に失敗しました", NULL);
						M2MHeap_free(sql);
						return;
						}
					}
				//===== エラー処理 =====
				else
					{
					M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_adjustMemoryDatabaseRecord()", __LINE__, (unsigned char *)"超過レコード削除用のDELETE文を作成するためのヒープメモリ領域の獲得に失敗しました", NULL);
					return;
					}
				}
			//===== レコード数が上限値以内の場合 =====
			else
				{
#ifdef DEBUG
				memset(MESSAGE, 0, sizeof(MESSAGE));
				snprintf(MESSAGE, sizeof(MESSAGE)-1, (unsigned char *)"メモリ上のSQLite3データベースの\"%s\"テーブルの現在のレコード数は\"%u\"です", tableName, OLD_RECORD_LIST_LENGTH);
				M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_adjustMemoryDatabaseRecord()", __LINE__, MESSAGE);
#endif // DEBUG
				return;
				}
			}
		//===== エラー処理 =====
		else if (MAX_RECORD<=0)
			{
			M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_adjustMemoryDatabaseRecord()", __LINE__, (unsigned char *)"引数で指定されたCEP実行オブジェクトで設定されているメモリ上のSQLite3データベースのレコード数の上限値が0です", NULL);
			return;
			}
		else if (OLD_RECORD_LIST==NULL)
			{
			M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_adjustMemoryDatabaseRecord()", __LINE__, (unsigned char *)"引数で指定されたCEP実行オブジェクトが保有する過去のレコード情報オブジェクトがNULLです", NULL);
			return;
			}
		else
			{
			M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_adjustMemoryDatabaseRecord()", __LINE__, (unsigned char *)"引数で指定されたCEP実行オブジェクトが保有する過去のレコード情報オブジェクト数が0です", NULL);
			return;
			}
		}
	//===== 引数エラー =====
	else if (self==NULL)
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_adjustMemoryDatabaseRecord()", __LINE__, (unsigned char *)"引数で指定されたCEP実行オブジェクトがNULLです", NULL);
		return;
		}
	else if (tableName==NULL)
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_adjustMemoryDatabaseRecord()", __LINE__, (unsigned char *)"引数で指定されたテーブル名を示す文字列がNULLです", NULL);
		return;
		}
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_adjustMemoryDatabaseRecord()", __LINE__, (unsigned char *)"引数で指定されたテーブル名を示す文字列の文字列数が0です", NULL);
		return;
		}
	}


/**
 * 挿入したレコード数を確認し，既定値に達した場合はバキューム処理を実行する．<br>
 * バキューム処理について，メモリ上のSQLite3データベースとファイル上のSQLite3<br>
 * データベース両者に対して実行する．<br>
 *
 * @param self	CEP実行オブジェクト
 */
static void this_checkRecordCounterForVacuum (M2MCEP *self)
	{
	//========== ローカル変数 ==========
	unsigned int vacuumRecord = 0;

	//===== 引数の確認 =====
	if (self!=NULL)
		{
		//===== バキューム処理を実行するレコード数を確認 =====
		if ((vacuumRecord=this_getVacuumRecord(self))>0)
			{
			//===== バキューム処理を行う，規程のレコード数に達した場合 =====
			if (vacuumRecord<this_getRecordCounter(self))
				{
#ifdef DEBUG
				M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_checkRecordCounterForVacuum()", __LINE__, (unsigned char *)"メモリ上のSQLite3データベースに対してバキューム処理を実行します");
#endif // DEBUG
				//===== メモリ上のSQLite3データベースをバキューム =====
				this_vacuum(this_getMemoryDatabase(self));
#ifdef DEBUG
				M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_checkRecordCounterForVacuum()", __LINE__, (unsigned char *)"メモリ上のSQLite3データベースに対してバキューム処理を実行しました");
#endif // DEBUG
				//===== レコード永続化の場合 =====
				if (this_getPersistence(self)==true)
					{
#ifdef DEBUG
					M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_checkRecordCounterForVacuum()", __LINE__, (unsigned char *)"ファイル上のSQLite3データベースに対してバキューム処理を実行します");
#endif // DEBUG
					//===== ファイル上のSQLite3データベースをバキューム =====
					this_vacuum(this_getFileDatabase(self));
#ifdef DEBUG
					M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_checkRecordCounterForVacuum()", __LINE__, (unsigned char *)"ファイル上のSQLite3データベースに対してバキューム処理を実行しました");
#endif // DEBUG
					}
				//===== レコード非永続化の場合 =====
				else
					{
					// 何もしない
					}
				//===== カウンターを初期化 =====
				this_updateRecordCounter(self, 0);
				}
			//===== バキューム処理を行う，規程のレコード数に達していない場合 =====
			else
				{
				// 何もしない
				}
			return;
			}
		//===== バキューム処理を実行しない（自動バキューム)設定の場合 =====
		else
			{
			return;
			}
		}
	//===== 引数エラー =====
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_checkVacuum()", __LINE__, (unsigned char *)"", NULL);
		return;
		}
	}


/**
 * 引数で指定されたCEP実行オブジェクトが管理するSQLite3ファイルデータベースを終了<br>
 * し、ヒープメモリ領域を解放する。<br>
 *
 * @param[in,out] self	CEP実行オブジェクト
 */
static void this_closeFileDatabase (M2MCEP *self)
	{
	//========== ローカル変数 ==========
	sqlite3 *fileDatabase = NULL;
	sqlite3_stmt *statement = NULL;
	sqlite3_stmt *next = NULL;

	//===== 引数の確認 =====
	if (self!=NULL && (fileDatabase=this_getFileDatabase(self))!=NULL)
		{
		//===== SQLite3メモリーデータベースを閉じる =====
		if (sqlite3_close(fileDatabase)==SQLITE_OK)
			{
			return;
			}
		//===== SQLite3メモリーデータベースを閉じるのに失敗した場合 =====
		else
			{
			//===== ステートメントの取得 =====
			statement = sqlite3_next_stmt(fileDatabase, NULL);
			//===== ステートメントが存在している場合 =====
			while (statement!=NULL)
				{
				//===== 次のステートメントを取得 =====
				next = sqlite3_next_stmt(fileDatabase, statement);
				//===== ステートメントのメモリー領域を解放 =====
				sqlite3_finalize(statement);
				//===== 次のステートメントに移る =====
				statement = next;
				}
			//===== （改めて）SQLite3メモリーデータベースを閉じる =====
			sqlite3_close(fileDatabase);
			return;
			}
		}
	//===== 引数エラー =====
	else if (self==NULL)
		{
		return;
		}
	else
		{
		return;
		}
	}


/**
 * 引数で指定されたCEP実行オブジェクトが管理するSQLite3メモリデータベースを終了<br>
 * し、ヒープメモリ領域を解放する。<br>
 *
 * @param[in,out] self	CEP実行オブジェクト
 */
static void this_closeMemoryDatabase (M2MCEP *self)
	{
	//========== ローカル変数 ==========
	sqlite3 *memoryDatabase = NULL;
	sqlite3_stmt *statement = NULL;
	sqlite3_stmt *next = NULL;

	//===== 引数の確認 =====
	if (self!=NULL && (memoryDatabase=this_getMemoryDatabase(self))!=NULL)
		{
		//===== SQLite3メモリーデータベースを閉じる =====
		if (sqlite3_close(memoryDatabase)==SQLITE_OK)
			{
			return;
			}
		//===== SQLite3メモリーデータベースを閉じるのに失敗した場合 =====
		else
			{
			//===== ステートメントの取得 =====
			statement = sqlite3_next_stmt(memoryDatabase, NULL);
			//===== ステートメントが存在している場合 =====
			while (statement!=NULL)
				{
				//===== 次のステートメントを取得 =====
				next = sqlite3_next_stmt(memoryDatabase, statement);
				//===== ステートメントのメモリー領域を解放 =====
				sqlite3_finalize(statement);
				//===== 次のステートメントに移る =====
				statement = next;
				}
			//===== （改めて）SQLite3メモリーデータベースを閉じる =====
			sqlite3_close(memoryDatabase);
			return;
			}
		}
	//===== 引数エラー =====
	else if (self==NULL)
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_closeMemoryDatabase()", __LINE__, (unsigned char *)"引数で指定された\"M2MCEP *\"がNULLです", NULL);
		return;
		}
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_closeMemoryDatabase()", __LINE__, (unsigned char *)"引数で指定された\"M2MCEP *\"から取得した\"fileDatabase\"がNULLです", NULL);
		return;
		}
	}


/**
 * プリペアドステートメント用のINSERT文を新規作成する．<br>
 *
 * @param[in] tableName			テーブル名を示す文字列
 * @param[in] columnNameCSV		カラム名を示すCSV形式の文字列
 * @param[in] columnListLength	カラム数を示す整数
 * @param[out] sql				新規作成したINSERT文をコピーするためのポインタ（バッファリングは関数内部で実行する)
 * @return						新規作成したINSERT文のポインタ or NULL（エラーの場合)
 */
static unsigned char *this_createInsertSQL (const unsigned char *tableName, const unsigned char *columnNameCSV, unsigned int columnListLength, unsigned char **sql)
	{
	//========== ローカル変数 ==========
	unsigned int i = 0;
	unsigned int index = 0;
	size_t sqlLength = 0;
	const unsigned char *INSERT_SQL = (unsigned char *)"INSERT INTO %s (%s) VALUES (%s) ";
	const size_t QUESTION_MARK_LENGTH = M2MString_length((unsigned char *)M2MString_QUESTION_MARK);
	const size_t COMMA_LENGTH = M2MString_length((unsigned char *)M2MString_COMMA);
	const size_t PARAMETER_LENGTH = columnListLength * (QUESTION_MARK_LENGTH + COMMA_LENGTH);
	unsigned char PARAMETER[PARAMETER_LENGTH];
#ifdef DEBUG
	unsigned char MESSAGE[256];
#endif // DEBUG

	//===== 引数の確認 =====
	if (tableName!=NULL && columnNameCSV!=NULL && columnListLength>0 && sql!=NULL)
		{
		//===== 配列の初期化 =====
		memset(PARAMETER, 0, PARAMETER_LENGTH);
		//===== カラム数だけ繰り返し =====
		for (i=0; i<columnListLength; i++)
			{
			//===== 最初の場合 =====
			if (i==0)
				{
				//===== "?"を追加 =====
				memcpy(&(PARAMETER[index]), (unsigned char *)M2MString_QUESTION_MARK, QUESTION_MARK_LENGTH);
				index++;
				}
			//===== 2つ目以降の場合 =====
			else
				{
				//===== ","を追加 =====
				memcpy(&(PARAMETER[index]), (unsigned char *)M2MString_COMMA, COMMA_LENGTH);
				index++;
				//===== "?"を追加 =====
				memcpy(&(PARAMETER[index]), (unsigned char *)M2MString_QUESTION_MARK, QUESTION_MARK_LENGTH);
				index++;
				}
			}
		//===== INSERT文を新規作成するためのヒープメモリ領域の獲得 =====
		if ((sqlLength=M2MString_length(INSERT_SQL)+M2MString_length(tableName)+M2MString_length(columnNameCSV)+M2MString_length(PARAMETER))>0
				&& ((*sql)=(unsigned char *)M2MHeap_malloc(sqlLength+1))!=NULL)
			{
			//===== INSERT文の新規作成 =====
			if (snprintf((*sql), sqlLength, INSERT_SQL, tableName, columnNameCSV, PARAMETER)>0)
				{
#ifdef DEBUG
				memset(MESSAGE, 0, sizeof(MESSAGE));
				snprintf(MESSAGE, sizeof(MESSAGE)-1, (unsigned char *)"INSERT文(=\"%s\")を新規作成しました", (*sql));
				M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_createInsertSQL()", __LINE__, MESSAGE);
#endif // DEBUG
				return (*sql);
				}
			//===== エラー処理 =====
			else
				{
				M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_createInsertSQL()", __LINE__, (unsigned char *)"INSERT文の構築に失敗しました", NULL);
				//===== INSERT文のヒープメモリ領域が残っていた場合 =====
				if ((*sql)!=NULL)
					{
					M2MHeap_free((*sql));
					}
				//===== INSERT文のヒープメモリ領域が存在しない場合 =====
				else
					{
					}
				return NULL;
				}
			}
		//===== エラー処理 =====
		else
			{
			M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_createInsertSQL()", __LINE__, (unsigned char *)"INSERT文を新規作成するためのヒープメモリ領域の獲得に失敗しました", NULL);
			return NULL;
			}
		}
	//===== 引数エラー =====
	else if (tableName==NULL)
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_createInsertSQL()", __LINE__, (unsigned char *)"引数で指定されたテーブル名を示す文字列がNULLです", NULL);
		return NULL;
		}
	else if (columnNameCSV==NULL)
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_createInsertSQL()", __LINE__, (unsigned char *)"引数で指定されたカラム名を示すCSV形式の文字列がNULLです", NULL);
		return NULL;
		}
	else if (columnListLength<=0)
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_createInsertSQL()", __LINE__, (unsigned char *)"引数で指定されたカラム数を示す整数が0以下です", NULL);
		return NULL;
		}
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_createInsertSQL()", __LINE__, (unsigned char *)"引数で指定されたINSERT文をコピーするためのポインタがNULLです", NULL);
		return NULL;
		}
	}


/**
 * 引数で指定されたCEP実行オブジェクトが保有するデータベース名のメモリ領域を解放<br>
 * する。<br>
 *
 * @param[in,out] self	CEP実行オブジェクト
 */
static void this_deleteDatabaseName (M2MCEP *self)
	{
	//===== 引数の確認 =====
	if (self!=NULL && this_getDatabaseName(self)!=NULL)
		{
		M2MHeap_free(self->databaseName);
		}
	//===== 引数エラー =====
	else if (self==NULL)
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_deleteDatabaseName()", __LINE__, (unsigned char *)"引数で指定されたCEP実行オブジェクトがNULLです", NULL);
		}
	else
		{
		}
	return;
	}


/**
 * 引数で指定されたCEP実行オブジェクトがメンバ変数として保持しているテーブル<br>
 * 構築オブジェクトのヒープメモリ領域を解放する。<br>
 * 当該メソッド呼び出し後、テーブル構築オブジェクトにはアクセス出来なくなる事に<br>
 * 留意する事。<br>
 *
 * @param[in,out] self	CEP実行オブジェクト
 */
static void this_deleteTableBuilder (M2MCEP *self)
	{
	//========== ローカル変数 ==========
	M2MTableBuilder *tableBuilder = NULL;

	//===== 引数の確認 =====
	if (self!=NULL && (tableBuilder=this_getTableBuilder(self))!=NULL)
		{
		M2MTableBuilder_delete(&tableBuilder);
		}
	//===== 引数エラー =====
	else if (self==NULL)
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_deleteTableBuilder()", __LINE__, (unsigned char *)"引数で指定されたCEP実行オブジェクトがNULLです", NULL);
		}
	else
		{
		}
	return;
	}


/**
 * 永続化未処理レコードを全てファイル上のSQLite3データベースへ格納する．<br>
 *
 * @param[in] self	CEP実行オブジェクト
 */
static void this_flushCEPRecord (M2MCEP *self)
	{
	//========== ローカル変数 ==========
	sqlite3 *fileDatabase = NULL;
	M2MTableBuilder *tableBuilder = NULL;
	M2MCEPRecord *record = NULL;
	bool persistence = false;

	//===== 引数の確認 =====
	if (self!=NULL)
		{
		//===== レコード管理オブジェクト（の先頭ノード）を取得 =====
		if ((tableBuilder=this_getTableBuilder(self))!=NULL
				&& (record=M2MCEPRecord_begin(this_getCEPRecord(self)))!=NULL)
			{
			//===== レコード永続化の場合 =====
			if ((persistence=this_getPersistence(self))==true
					&& (fileDatabase=this_getFileDatabase(self))!=NULL)
				{
#ifdef DEBUG
				M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_insertRecordListToFileDatabase()", __LINE__, (unsigned char *)"ファイル上のSQLiteデータベースへ未挿入レコード情報を一括挿入するトランザクション処理を開始します");
#endif // DEBUG
				//===== トランザクション開始 =====
				SQLRunner_beginTransaction(fileDatabase);
				//===== レコード管理オブジェクトが末端に辿り着くまで繰り返し =====
				while (M2MCEPRecord_next(record)!=NULL)
					{
					//===== レコード管理オブジェクトの挿入／削除 =====
					this_insertRecordList(fileDatabase, record, tableBuilder, M2MCEPRecord_getOldRecordList(record));
					//===== 次のレコード管理オブジェクトへ移動 =====
					record = M2MCEPRecord_next(record);
					}
				//===== 末端のレコード管理オブジェクトの挿入／削除 =====
				this_insertRecordList(fileDatabase, record, tableBuilder, M2MCEPRecord_getOldRecordList(record));
				//===== トランザクション終了 =====
				SQLRunner_commitTransaction(this_getFileDatabase(self));
#ifdef DEBUG
				M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_insertRecordListToFileDatabase()", __LINE__, (unsigned char *)"ファイル上のSQLiteデータベースへ未挿入レコードを一括挿入するトランザクション処理を終了しました");
#endif // DEBUG
				}
			//===== レコード非永続化の場合 =====
			else
				{
				}
			//===== 正常終了 =====
			return;
			}
		//===== エラー処理 =====
		else if (tableBuilder==NULL)
			{
			M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_insertRecordListToFileDatabase()", __LINE__, (unsigned char *)"引数で指定されたCEP実行オブジェクトから取得したテーブル構築オブジェクトがNULLです", NULL);
			return;
			}
		else
			{
			M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_insertRecordListToFileDatabase()", __LINE__, (unsigned char *)"引数で指定されたCEP実行オブジェクトから取得したレコード管理オブジェクトがNULLです", NULL);
			return;
			}
		}
	//===== 引数エラー =====
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_insertRecordListToFileDatabase()", __LINE__, (unsigned char *)"引数で指定されたCEP実行オブジェクトがNULLです", NULL);
		return;
		}
	}


/**
 * 引数で指定されたCEP実行オブジェクトが管理するCEPレコード情報オブジェクトを取得<br>
 * して返す。<br>
 *
 * @param[in] self	CEP実行オブジェクト
 * @return			CEPレコード情報オブジェクト
 */
static M2MCEPRecord *this_getCEPRecord (const M2MCEP *self)
	{
	if (self!=NULL)
		{
		return self->record;
		}
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_getM2MCEPRecord()", __LINE__, (unsigned char *)"引数で指定された\"M2MCEP *\"がNULLです", NULL);
		return NULL;
		}
	}


/**
 * 引数で指定されたバッファにSQLite3データベースファイルを設置するディレクトリ<br>
 * （＝"/home/(ユーザ名)/.m2m/cep"）を示す文字列をコピーして返す．<br>
 *
 * @param[out] directoryPath	SQLite3データベースファイルを設置するディレクトリを示す文字列を格納するためのバッファ or NULL（エラーの場合）
 * @return						SQLite3データベースファイルを設置するディレクトリを示す文字列（引数の先頭ポインタ） or NULL（エラーの場合）
 */
static unsigned char *this_getDatabaseDirectoryPath (unsigned char **directoryPath)
	{
	//========== ローカル変数 ==========
	unsigned char *home = NULL;
	size_t homeLength = 0;
	const unsigned char *SEPARATOR = (unsigned char *)M2MDirectory_SEPARATOR;
	const size_t CEP_DIRECTORY_LENGTH = M2MString_length((unsigned char *)M2MCEP_DIRECTORY);
	const size_t SEPARATOR_LENGTH = M2MString_length(SEPARATOR);

	//===== 引数の確認 =====
	if (directoryPath!=NULL)
		{
		//===== バッファのヒープメモリを取得 =====
		if ((home=M2MDirectory_getHomeDirectoryPath())!=NULL
				&& (homeLength=M2MString_length(home))>0
				&& ((*directoryPath)=(unsigned char *)M2MHeap_malloc(homeLength+SEPARATOR_LENGTH+CEP_DIRECTORY_LENGTH+1))!=NULL)
			{
			//===== バッファへのパス文字列のコピー =====
			memcpy(&((*directoryPath)[0]), home, homeLength);
			memcpy(&((*directoryPath)[homeLength]), SEPARATOR, SEPARATOR_LENGTH);
			memcpy(&((*directoryPath)[homeLength+SEPARATOR_LENGTH]), (unsigned char *)M2MCEP_DIRECTORY, CEP_DIRECTORY_LENGTH);
			return (*directoryPath);
			}
		//===== エラー処理 =====
		else
			{
			M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_getDatabaseDirectoryPath()", __LINE__, (unsigned char *)"ホームディレクトリパスを示す文字列をコピーするためのヒープメモリ領域の獲得に失敗しました", NULL);
			return NULL;
			}
		}
	//===== 引数エラー =====
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_getDatabaseDirectoryPath()", __LINE__, (unsigned char *)"引数で指定された\"directoryPath\"がNULLです", NULL);
		return NULL;
		}
	}


/**
 * 引数で指定されたSQLite3データベースファイル名を示す文字列から、データベース<br>
 * ファイルパスを示す文字列を作成し、引数で指定されたバッファにコピーする。<br>
 * 処理に失敗した場合は戻り値として NULL を返す。<br>
 *
 * @param[in] databaseName			SQLite3データベースファイル名を示す文字列
 * @param[out] databaseFilePath		SQLite3データベースファイルパスを示す文字列を格納するためのバッファ
 * @return							SQLite3データベースファイルパスを示す文字列のポインタ or NULL（エラーの場合）
 */
static unsigned char *this_getDatabaseFilePath (const unsigned char *databaseName, unsigned char **databaseFilePath)
	{
	//========== ローカル変数 ==========
	unsigned char *directoryPath = NULL;
	size_t directoryPathLength = 0;
	size_t databaseNameLength = 0;
	const size_t SEPARATOR_LENGTH = M2MString_length((unsigned char *)M2MDirectory_SEPARATOR);
	const unsigned char *PERMISSION = (unsigned char *)"0755";

	//===== 引数の確認 =====
	if (databaseName!=NULL && (databaseNameLength=M2MString_length(databaseName))>0
			&& databaseFilePath!=NULL)
		{
		//===== データベースファイル設置ディレクトリパス文字列の取得 =====
		if (this_getDatabaseDirectoryPath(&directoryPath)!=NULL
				&& (directoryPathLength=M2MString_length(directoryPath))>0
				&& this_makeDirectory(directoryPath, PERMISSION)!=NULL)
			{
			//===== ヒープメモリ領域の獲得 =====
			if ((*databaseFilePath=(unsigned char *)M2MHeap_malloc(directoryPathLength+SEPARATOR_LENGTH+databaseNameLength+1))!=NULL)
				{
				//===== データベースファイルパス文字列のコピー =====
				memcpy(&((*databaseFilePath)[0]), directoryPath, directoryPathLength);
				memcpy(&((*databaseFilePath)[directoryPathLength]), (unsigned char *)M2MDirectory_SEPARATOR, SEPARATOR_LENGTH);
				memcpy(&((*databaseFilePath)[directoryPathLength+SEPARATOR_LENGTH]), databaseName, databaseNameLength);
				//===== ディレクトリパス文字列のメモリ領域を解放 =====
				M2MHeap_free(directoryPath);
				//=====  =====
				return (*databaseFilePath);
				}
			//===== エラー処理 =====
			else
				{
				//===== ディレクトリパス文字列が存在する場合 =====
				if (directoryPath!=NULL)
					{
					//===== ディレクトリパス文字列のメモリ領域を解放 =====
					M2MHeap_free(directoryPath);
					}
				else
					{
					}
				return NULL;
				}
			}
		//===== エラー処理 =====
		else
			{
			//===== ディレクトリパス文字列が存在する場合 =====
			if (directoryPath!=NULL)
				{
				//===== ディレクトリパス文字列のメモリ領域を解放 =====
				M2MHeap_free(directoryPath);
				}
			else
				{
				}
			return NULL;
			}
		}
	//===== 引数エラー =====
	else if (databaseName==NULL || databaseNameLength<=0)
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_getDatabaseFilePath()", __LINE__, (unsigned char *)"引数で指定された\"databaseName\"がNULL、または文字列数が0以下です", NULL);
		return NULL;
		}
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_getDatabaseFilePath()", __LINE__, (unsigned char *)"引数で指定された\"databaseFilePath\"がNULLです", NULL);
		return NULL;
		}
	}


/**
 * 引数で指定されたCEP実行オブジェクトが保有するデータベース名を示す文字列を返す。<br>
 *
 * @param[in] self	CEP実行オブジェクト
 * @return			データベース名を示す文字列 or NULL（エラーの場合）
 */
static unsigned char *this_getDatabaseName (const M2MCEP *self)
	{
	//===== 引数の確認 =====
	if (self!=NULL)
		{
		return self->databaseName;
		}
	//===== 引数エラー =====
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_getDatabaseName()", __LINE__, (unsigned char *)"引数で指定された\"M2MCEP *\"がNULLです", NULL);
		return NULL;
		}
	}


/**
 * @param columnList
 * @param columnNameCSV
 * @param dataTypeArray
 * @param dataTypeArrayLength
 * @return
 */
static unsigned int this_getDataTypeArray (M2MColumnList *columnList, const unsigned char *columnNameCSV, M2MDataType dataTypeArray[], const size_t dataTypeArrayLength)
	{
	//========== ローカル変数 ==========
	unsigned char *columnName = NULL;
	unsigned char *index = NULL;
	unsigned int dataTypeIndex = 0;
	M2MColumn *column = NULL;
	M2MDataType dataType;
	const size_t COMMA_LENGTH = M2MString_length((unsigned char *)M2MString_COMMA);

	if (columnList!=NULL && columnNameCSV!=NULL && M2MString_length(columnNameCSV)>0 && dataTypeArray!=NULL && dataTypeArrayLength>0)
		{
		//===== CSV形式のカラム名の先頭ポインタ取得 =====
		columnName = (unsigned char *)columnNameCSV;
		//===== カラムのデータ型格納配列の新規作成 =====
		memset(dataTypeArray, 0, dataTypeArrayLength);
		dataTypeIndex = 0;
		//===== CSV形式のカラム名をカンマで分割 =====
		while (columnName!=NULL
				&& (index=M2MString_indexOf(columnName, (unsigned char *)M2MString_COMMA))!=NULL)
			{
			//===== 同一カラム名のオブジェクトを検索 =====
			if ((column=M2MColumnList_search(columnList, columnName, M2MString_length(columnName) - M2MString_length(index)))!=NULL)
				{
				//===== カラムのデータ型取得 =====
				dataType = M2MColumn_getDataType(column);
				//===== カラムのデータ型を配列へ格納 =====
				dataTypeArray[dataTypeIndex] = dataType;
				}
			//===== エラー処理 =====
			else
				{
				dataTypeArray[dataTypeIndex] = M2M_DATA_TYPE_ERROR;
				}
			dataTypeIndex++;
			//===== カンマ分だけポインタを移動 =====
			index += COMMA_LENGTH;
			//===== 先頭ポインタを次のカラム名へ移動 =====
			columnName = index;
			}
		//===== 同一カラム名のオブジェクトを検索 =====
		if ((column=M2MColumnList_search(columnList, columnName, M2MString_length(columnName)))!=NULL)
			{
			//===== カラムのデータ型取得 =====
			dataType = M2MColumn_getDataType(column);
			//===== カラムのデータ型を配列へ格納 =====
			dataTypeArray[dataTypeIndex] = dataType;
			dataTypeIndex++;
			}
		//===== エラー処理 =====
		else
			{
			dataTypeArray[dataTypeIndex] = M2M_DATA_TYPE_ERROR;
			}
		return dataTypeIndex;
		}
	else
		{
		return 0;
		}
	}


/**
 * 引数で指定されたCEP実行オブジェクトが管理しているファイル上のSQLite3データベース<br>
 * オブジェクトを取得して返す。<br>
 * もしCEP実行オブジェクトがファイル上のSQLite3データベース管理オブジェクトを<br>
 * 保持していなかった場合、且つレコード永続化のフラグが立っている場合、新規に<br>
 * ファイル上のSQLite3データベース管理オブジェクトを取得して返す．<br>
 *
 * @param[in] self	CEP実行オブジェクト
 * @return			ファイル上のSQLite3データベース管理オブジェクト or NULL（エラーの場合）
 */
static sqlite3 *this_getFileDatabase (M2MCEP *self)
	{
	//========== ローカル変数 ==========
	unsigned char *databaseName = NULL;
	M2MTableBuilder *tableBuilder = NULL;
	sqlite3 *fileDatabase = NULL;

	//===== 引数の確認 =====
	if (self!=NULL)
		{
		//===== レコード永続化のフラグが立っている事を確認 =====
		if (this_getPersistence(self)==true)
			{
			//===== ファイル上のSQLite3データベース管理オブジェクトの確認 =====
			if (self->fileDatabase!=NULL)
				{
				return self->fileDatabase;
				}
			//===== ファイル上のSQLite3データベース管理オブジェクトを保持していない場合 =====
			else
				{
				//===== ファイル上のSQLite3データベース管理オブジェクトの取得 =====
				if ((databaseName=this_getDatabaseName(self))!=NULL
						&& (tableBuilder=this_getTableBuilder(self))!=NULL
						&& (fileDatabase=this_openFileDatabase(databaseName, tableBuilder, this_getVacuumRecord(self)))!=NULL)
					{
					//===== ファイル上のSQLite3データベース管理オブジェクトをセット =====
					this_setFileDatabase(self, fileDatabase);
					//===== ファイル上のSQLite3データベース管理オブジェクトを返す =====
					return fileDatabase;
					}
				//===== エラー処理 =====
				else if (databaseName==NULL)
					{
					M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_getFileDatabase()", __LINE__, (unsigned char *)"ファイル上のSQLite3データベース管理オブジェクト作成に必要なデータベースファイル名取得に失敗しました", NULL);
					return NULL;
					}
				//===== エラー処理 =====
				else if (tableBuilder==NULL)
					{
					M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_getFileDatabase()", __LINE__, (unsigned char *)"ファイル上のSQLite3データベース管理オブジェクト作成に必要なテーブル構築オブジェクト取得に失敗しました", NULL);
					return NULL;
					}
				//===== エラー処理 =====
				else
					{
					M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_getFileDatabase()", __LINE__, (unsigned char *)"ファイル上のSQLite3データベース管理オブジェクト作成に失敗しました", NULL);
					return NULL;
					}
				}
			}
		//===== レコード永続化のフラグが立っていない場合 =====
		else
			{
#ifdef DEBUG
			M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_getFileDatabase()", __LINE__, (unsigned char *)"レコード永続化は無効のため，ファイル上のSQLite3データベース管理オブジェクトを取得しません");
#endif // DEBUG
			return NULL;
			}
		}
	//===== 引数のエラー =====
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_getFileDatabase()", __LINE__, (unsigned char *)"引数で指定された\"M2MCEP *\"がNULLです", NULL);
		return NULL;
		}
	}


/**
 * SQLite3規程のテーブルに許される最大カラム数を返す．<br>
 *
 * @param database	SQLite3データベース管理オブジェクト
 * @return			1つのテーブルの最大カラム数[個]
 */
static unsigned int this_getMaxColumnLength (sqlite3 *database)
	{
	//========== ローカル変数 ==========
	const unsigned int DEFAULT_MAX_COLUMN_LENGTH = (unsigned int)2000;

	//===== 引数の確認 =====
	if (database!=NULL)
		{
		return (unsigned int)sqlite3_limit(database, SQLITE_LIMIT_COLUMN, -1);
		}
	//===== 引数エラー =====
	else
		{
		return DEFAULT_MAX_COLUMN_LENGTH;
		}
	}


/**
 * 引数で指定されたCEP実行オブジェクトが保持しているCEPレコード数の上限値を返す。<br>
 *
 * @param[in] self	CEP実行オブジェクト
 * @return			CEPレコード数の上限値
 */
static unsigned int this_getMaxRecord (const M2MCEP *self)
	{
	//========== ローカル変数 ==========
	unsigned int DEFAULT_MAX_RECORD = 50;

	//===== 引数の確認 =====
	if (self!=NULL)
		{
		return self->maxRecord;
		}
	//===== 引数エラー =====
	else
		{
		return DEFAULT_MAX_RECORD;
		}
	}


/**
 * CEP実行オブジェクトが管理しているメモリ上のSQLite3データベース管理オブジェクトを返す．<br>
 *
 * @param[in] self	CEP実行オブジェクト
 * @return			メモリ上のSQLite3データベース管理オブジェクト or NULL（エラーの場合）
 */
static sqlite3 *this_getMemoryDatabase (M2MCEP *self)
	{
	//========== ローカル変数 ==========
	sqlite3 *memoryDatabase = NULL;

	//===== 引数の確認 =====
	if (self!=NULL)
		{
		//===== メモリ上のSQLite3データベース管理オブジェクトの確認 =====
		if (self->memoryDatabase!=NULL)
			{
			return self->memoryDatabase;
			}
		//===== メモリ上のSQLite3データベース管理オブジェクトが存在していない場合 =====
		else
			{
			//===== メモリ上のSQLite3データベース管理オブジェクトの取得 =====
			if ((memoryDatabase=this_openMemoryDatabase(this_getTableBuilder(self), this_getVacuumRecord(self)))!=NULL)
				{
				//===== メモリ上のSQLite3データベース管理オブジェクトをセット =====
				this_setMemoryDatabase(self, memoryDatabase);
				//===== メモリ上のSQLite3データベース管理オブジェクトを返す =====
				return memoryDatabase;
				}
			//===== エラー処理 =====
			else
				{
				M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_getMemoryDatabase()", __LINE__, (unsigned char *)"メモリ上のSQLite3データベース管理オブジェクト作成に失敗しました", NULL);
				return NULL;
				}
			}
		}
	//===== 引数のエラー =====
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_getMemoryDatabase()", __LINE__, (unsigned char *)"引数で指定された\"M2MCEP *\"がNULLです", NULL);
		return NULL;
		}
	}


/**
 * 引数で指定されたCEP構造体オブジェクトから、永続性の可否を示すフラグを取得する．<br>
 *
 * @param[in,out] self		CEP構造体オブジェクト
 * @return					永続性の可否を示すフラグ
 */
static bool this_getPersistence (const M2MCEP *self)
	{
	const bool PERSISTENCE = true;

	if (self!=NULL)
		{
		return self->persistence;
		}
	else
		{
		return PERSISTENCE;
		}
	}


/**
 * 引数で指定されたCEP実行オブジェクトが保持している挿入済みレコード数を返す．<br>
 *
 * @param self	CEP実行オブジェクト
 * @return		挿入済みレコード数
 */
static unsigned int this_getRecordCounter (const M2MCEP *self)
	{
	if (self!=NULL)
		{
		return self->recordCounter;
		}
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_getRecordCounter()", __LINE__, (unsigned char *)"引数で指定されたCEP実行オブジェクトがNULLです", NULL);
		return 0;
		}
	}


/**
 * @param database
 * @param sql
 * @return
 */
static sqlite3_stmt *this_getSQLitePreparedStatement (sqlite3 *database, const unsigned char *sql)
	{
	//========== ローカル変数 ==========
	sqlite3_stmt *statement = NULL;

	//===== 引数の確認 =====
	if (database!=NULL && sql!=NULL)
		{
		//===== プリペアドステートメントの獲得 =====
		if (sqlite3_prepare(database, sql, -1, &statement, NULL)==SQLITE_OK)
			{
			//===== SQLのパラメータをリセット =====
			sqlite3_clear_bindings(statement);
			return statement;
			}
		//===== エラー処理 =====
		else
			{
			M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_getSQLitePreparedStatement()", __LINE__, (unsigned char *)sqlite3_errmsg(database), NULL);
			return NULL;
			}
		}
	//===== 引数エラー =====
	else if (database==NULL)
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_getSQLitePreparedStatement()", __LINE__, (unsigned char *)"引数で指定されたSQLite3データベース管理オブジェクトがNULLです", NULL);
		return NULL;
		}
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_getSQLitePreparedStatement()", __LINE__, (unsigned char *)"引数で指定されたSQL文を示す文字列がNULLです", NULL);
		return NULL;
		}
	}


/**
 * CEP実行オブジェクトがメンバ変数として保持しているテーブル構築オブジェクトを<br>
 * 返す。<br>
 *
 * @param[in] self	CEP実行オブジェクト
 * @return			テーブル構築オブジェクト or NULL（エラーの場合）
 */
static M2MTableBuilder *this_getTableBuilder (const M2MCEP *self)
	{
	//===== 引数の確認 =====
	if (self!=NULL)
		{
		return self->tableBuilder;
		}
	//===== 引数エラー =====
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_getTableBuilder()", __LINE__, (unsigned char *)"引数で指定されたCEP実行オブジェクトがNULLです", NULL);
		return NULL;
		}
	}


/**
 * SQLite3データベースがバキューム処理を実行するレコード数を返す．<br>
 *
 * @param[in] self	CEP実行オブジェクト
 * @return			SQLite3データベースがバキューム処理を実行するレコード数
 */
static unsigned int this_getVacuumRecord (const M2MCEP *self)
	{
	if (self!=NULL)
		{
		return self->vacuumRecord;
		}
	else
		{
		return 0;
		}
	}


/**
 * @param[in] csv
 * @return				true : , false :
 */
static bool this_includesData (const unsigned char *csv)
	{
	//========== ローカル変数 ==========
	unsigned char *index = NULL;
	const size_t CRLF_LENGTH = M2MString_length((unsigned char *)M2MString_CRLF);

	//===== 引数の確認 =====
	if (csv!=NULL)
		{
		//===== ヘッダの改行コードを確認 =====
		if ((index=M2MString_indexOf(csv, (unsigned char *)M2MString_CRLF))!=NULL)
			{
			//===== 改行コード文字数だけポインタを進める =====
			index += CRLF_LENGTH;
			//===== データの存在を確認 =====
			if (M2MString_length(index)>0)
				{
				return true;
				}
			//===== データが含まれていなかった場合 =====
			else
				{
				return false;
				}
			}
		//===== ヘッダが含まれていなかった場合 =====
		else
			{
			return false;
			}
		}
	//===== 引数エラー =====
	else
		{
		return false;
		}
	}


/**
 * 引数で指定されたCEP実行オブジェクトを初期化する。<br>
 *
 * @param[in,out] self		CEP実行オブジェクト
 * @param[in] databaseName	ファイル上のSQLite3データベース名
 * @param[in] tableBuilder	SQLite3データベースのテーブルを構築するためのオブジェクト
 * @return					CEP構造体オブジェクト or NULL（エラーの場合）
 */
static M2MCEP *this_init (M2MCEP *self, const unsigned char *databaseName, const M2MTableBuilder *tableBuilder)
	{
	//========== ローカル変数 ==========
	const unsigned int MAX_RECORD = 50;

	//===== 引数の確認 =====
	if (self!=NULL && databaseName!=NULL && tableBuilder!=NULL)
		{
		if (this_setDatabaseName(self, databaseName)!=NULL
				&& M2MCEP_setMaxRecord(self, MAX_RECORD)!=NULL
				&& (self->record=M2MCEPRecord_new())!=NULL
				&& this_setTableBuilder(self, tableBuilder)!=NULL
				&& M2MCEP_setPersistence(self, true)!=NULL)
			{
			return self;
			}
		//===== エラー処理 =====
		else
			{
			M2MCEP_delete(&self);
			return NULL;
			}
		}
	//===== 引数エラー =====
	else
		{
		return NULL;
		}
	}


/**
 * @param[in] fileDatabase	ファイル上のSQLite3管理オブジェクト
 * @param[in] tableBuilder	テーブル構築オブジェクト
 * @param[in] tableRecord	テーブルレコード管理オブジェクト
 * @param[in] maxRecord		メモリ上のSQLite3データベーステーブルに保持しておく最大レコード数（これ以上になったら永続化を実行)
 * @param[in] persistence	永続化を実行するかどうかを示すフラグ
 * @return
 */
static void this_insertOldRecordList (sqlite3 *fileDatabase, M2MCEPRecord *tableRecord, M2MTableBuilder *tableBuilder, const unsigned int maxRecord, const bool persistence)
	{
	//========== ローカル変数 ==========
	sqlite3_stmt* statement = NULL;
	M2MColumnList *columnList = NULL;
	M2MList *oldRecordList = NULL;
	unsigned int oldRecordListLength = 0;
	M2MDataType DATA_TYPE_ARRAY[this_getMaxColumnLength(fileDatabase)];
	unsigned char *value = NULL;
	unsigned char *valueIndex = NULL;
	unsigned char *insertSQL = NULL;
	unsigned char *tableName = NULL;
	unsigned char *columnNameCSV = NULL;
	unsigned int columnIndex = 0;
	unsigned int dataTypeArrayLength = 0;
	int resultCode = 0;
	int rest = 0;
	unsigned int i = 0;
	const size_t COMMA_LENGTH = M2MString_length((unsigned char *)M2MString_COMMA);
#ifdef DEBUG
	unsigned char MESSAGE[256];
#endif // DEBUG

	//===== 引数の確認 =====
	if (tableRecord!=NULL
			&& (tableName=M2MCEPRecord_getTableName(tableRecord))!=NULL
			&& (oldRecordList=M2MList_begin(M2MCEPRecord_getOldRecordList(tableRecord)))!=NULL
			&& (oldRecordListLength=M2MList_length(oldRecordList))>0)
		{
		//===== メモリ上のSQLite3データベース挿入済みレコード数が規定のレコード数最大値を超過している場合 =====
		if ((rest=oldRecordListLength-maxRecord)>0)
			{
			//===== 過去挿入レコード永続化の場合 =====
			if (persistence==true)
				{
				//===== ファイル上のSQLite3データベースへの過去レコード挿入準備 =====
				if ((columnNameCSV=M2MCEPRecord_getColumnName(tableRecord))!=NULL
						&& (columnList=M2MColumnList_begin(M2MTableBuilder_getColumnList(tableBuilder, tableName)))!=NULL
						&& (dataTypeArrayLength=this_getDataTypeArray(columnList, columnNameCSV, DATA_TYPE_ARRAY, sizeof(DATA_TYPE_ARRAY)))>0
						&& this_createInsertSQL(tableName, columnNameCSV, M2MColumnList_length(columnList), &insertSQL)!=NULL
						&& (statement=this_getSQLitePreparedStatement(fileDatabase, insertSQL))!=NULL)
					{
#ifdef DEBUG
					memset(MESSAGE, 0, sizeof(MESSAGE));
					snprintf(MESSAGE, sizeof(MESSAGE)-1, (unsigned char *)"\"%s\"テーブル向けファイル上のSQLite3データベース挿入処理を開始します", tableName);
					M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_insertOldRecordList()", __LINE__, MESSAGE);
#endif // DEBUG
					}
				//===== エラー処理 =====
				else if (columnNameCSV==NULL)
					{
					M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_insertOldRecordList()", __LINE__, (unsigned char *)"テーブルレコード管理オブジェクトから取得したカラム名を示すCSV形式の文字列がNULLです", NULL);
					return;
					}
				else if (columnList==NULL)
					{
					M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_insertOldRecordList()", __LINE__, (unsigned char *)"テーブル構築オブジェクトから取得したテーブルのカラム構造体オブジェクトがNULLです", NULL);
					return;
					}
				else if (dataTypeArrayLength<=0)
					{
					M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_insertOldRecordList()", __LINE__, (unsigned char *)"テーブル構築オブジェクトから取得したカラムのデータ型配列がNULLです", NULL);
					return;
					}
				else if (insertSQL==NULL)
					{
					M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_insertOldRecordList()", __LINE__, (unsigned char *)"ファイル上のSQLite3データベースへレコードを挿入するためのINSERT文の作成に失敗しました", NULL);
					return;
					}
				else
					{
					M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_insertOldRecordList()", __LINE__, (unsigned char *)sqlite3_errmsg(fileDatabase), NULL);
					return;
					}
				}
			//===== 過去挿入レコード非永続化の場合 =====
			else
				{
				// 何もしない
				}
			//===== 超過分のレコード数だけ繰り返し =====
			for (i=0; i<rest; i++)
				{
				//===== 永続化する場合 =====
				if (persistence==true)
					{
					//===== INSERT文の作成 =====
					if ((value=(unsigned char *)M2MList_getValue(oldRecordList))!=NULL)
						{
						//===== フィールドの位置を初期化 =====
						columnIndex = 0;
						//===== CSV形式のカラム名で繰り返し =====
						while (columnIndex<(dataTypeArrayLength-1)
								&& value!=NULL
								&& (valueIndex=M2MString_indexOf(value, (unsigned char *)M2MString_COMMA))!=NULL)
							{
							//===== 値をセット =====
							this_setValueIntoPreparedStatement(DATA_TYPE_ARRAY[columnIndex], columnIndex+1, value, M2MString_length(value)-M2MString_length(valueIndex), statement);
							//===== フィールド位置を次へ移す =====
							columnIndex++;
							//===== 値の位置を次へ移す =====
							valueIndex += COMMA_LENGTH;
							value = valueIndex;
							}
						//===== カラム数と挿入データ数の整合性を確認 =====
						if (columnIndex==(dataTypeArrayLength-1))
							{
							//===== 最後の挿入データをセット =====
							this_setValueIntoPreparedStatement(DATA_TYPE_ARRAY[columnIndex], columnIndex+1, value, M2MString_length(value), statement);
							//===== INSERT実行 =====
							while ((resultCode=sqlite3_step(statement))==SQLITE_BUSY)
								{
								}
							//===== INSERTに成功した場合 =====
							if (resultCode==SQLITE_DONE)
								{
#ifdef DEBUG
								M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_insertOldRecordList()", __LINE__, (unsigned char *)"ファイル上のSQLite3データベースへレコードを挿入しました");
#endif // DEBUG
								}
							//===== エラー処理 =====
							else
								{
								M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_insertOldRecordList()", __LINE__, sqlite3_errmsg(fileDatabase), NULL);
								}
							}
						//===== INSERT文のパラメータをリセット =====
						sqlite3_reset(statement);
						}
					//===== INSERT文の作成エラー =====
					else
						{
						M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_insertOldRecordList()", __LINE__, (unsigned char *)"ファイル上のSQLite3データベースへレコードを挿入するためのINSERT文作成準備作業で失敗しました", NULL);
						}
					}
				//===== 永続化しない場合 =====
				else
					{
					// 何もしない
					}
				//===== データベースに挿入したレコードを削除して次のレコード情報オブジェクトへ移動 =====
				oldRecordList = M2MList_remove(oldRecordList);
				}
			//===== 永続化する場合 =====
			if (persistence==true)
				{
				//===== SQL実行オブジェクトの実行（ヒープメモリ領域を解放） =====
				if (sqlite3_finalize(statement)==SQLITE_OK)
					{
					}
				//===== エラー処理 =====
				else
					{
					M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_insertOldRecordList()", __LINE__, (unsigned char *)sqlite3_errmsg(fileDatabase), NULL);
					}
				}
			//===== 永続化しない場合 =====
			else
				{
				// 何もしない
				}
			//===== レコード情報オブジェクトの更新 =====
			tableRecord->oldRecordList = oldRecordList;
			//===== INSERT文のヒープメモリ領域を解放 =====
			M2MHeap_free(insertSQL);
#ifdef DEBUG
			memset(MESSAGE, 0, sizeof(MESSAGE));
			snprintf(MESSAGE, sizeof(MESSAGE)-1, (unsigned char *)"ファイル上のSQLite3データベースの\"%s\"テーブルへのレコード挿入処理を終了します", tableName);
			M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_insertOldRecordList()", __LINE__, MESSAGE);
#endif // DEBUG
			return;
			}
		//===== メモリ上のSQLite3データベース挿入済みレコード数が規定のレコード数最大値以内の場合 =====
		else
			{
#ifdef DEBUG
			memset(MESSAGE, 0, sizeof(MESSAGE));
			snprintf(MESSAGE, sizeof(MESSAGE)-1, (unsigned char *)"メモリ上のSQLite3データベースの\"%s\"テーブルに挿入済みレコード数(=\"%u\")が規定のレコード数最大値(=\"%u\")以内のため，レコードの削除は実行しません", tableName, oldRecordListLength, maxRecord);
			M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_insertOldRecordList()", __LINE__, MESSAGE);
#endif // DEBUG
			return;
			}
		}
	//===== エラー処理 =====
	else if (tableRecord==NULL)
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_insertOldRecordList()", __LINE__, (unsigned char *)"引数で指定されたレコード管理オブジェクトがNULLです", NULL);
		return;
		}
	else if (tableName==NULL)
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_insertOldRecordList()", __LINE__, (unsigned char *)"テーブルレコード管理オブジェクトから取得したテーブル名を示す文字列がNULLです", NULL);
		return;
		}
	else if (oldRecordList==NULL)
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_insertOldRecordList()", __LINE__, (unsigned char *)"テーブルレコード管理オブジェクトから取得した未挿入レコード情報オブジェクトがNULLです", NULL);
		return;
		}
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_insertOldRecordList()", __LINE__, (unsigned char *)"テーブルレコード管理オブジェクトから取得した未挿入レコード数が0以下です", NULL);
		return;
		}
	}


/**
 * 引数で指定されたSQLite3データベースにレコードを一括挿入する．<br>
 *
 * @param[in] database		SQLite3データベース管理オブジェクト
 * @param[in] record		レコード管理オブジェクト
 * @param[in] tableBuilder	テーブル構築オブジェクト
 * @param[in] recordList	レコード情報オブジェクト
 * @return					挿入したレコード数 or -1（エラーの場合)
 */
static int this_insertRecordList (sqlite3 *database, const M2MCEPRecord *record, M2MTableBuilder *tableBuilder, M2MList *recordList)
	{
	//========== ローカル変数 ==========
	sqlite3_stmt* statement = NULL;
	M2MColumnList *columnList = NULL;
	M2MDataType DATA_TYPE_ARRAY[this_getMaxColumnLength(database)];
	unsigned char *value = NULL;
	unsigned char *valueIndex = NULL;
	unsigned char *insertSQL = NULL;
	unsigned char *tableName = NULL;
	unsigned char *columnNameCSV = NULL;
	int numberOfRecord = 0;
	unsigned int columnIndex = 0;
	unsigned int dataTypeArrayLength = 0;
	int resultCode = 0;
	const size_t COMMA_LENGTH = M2MString_length((unsigned char *)M2MString_COMMA);

	//===== 引数の確認 =====
	if (database!=NULL && tableBuilder!=NULL && record!=NULL && (recordList=M2MList_begin(recordList))!=NULL)
		{
		//===== テーブル名、カラム名CSVと挿入対象の新規レコード情報オブジェクトの取得 =====
		if ((tableName=M2MCEPRecord_getTableName(record))!=NULL
				&& (columnNameCSV=M2MCEPRecord_getColumnName(record))!=NULL
				&& (columnList=M2MTableBuilder_getColumnList(tableBuilder, tableName))!=NULL
				&& (columnList=M2MColumnList_begin(columnList))!=NULL
				&& this_createInsertSQL(tableName, columnNameCSV, M2MColumnList_length(columnList), &insertSQL)!=NULL
				&& sqlite3_prepare(database, insertSQL, -1, &statement, NULL)==SQLITE_OK
				&& (dataTypeArrayLength=this_getDataTypeArray(columnList, columnNameCSV, DATA_TYPE_ARRAY, sizeof(DATA_TYPE_ARRAY)))>0)
			{
			//===== SQL文のパラメータをリセット =====
			sqlite3_reset(statement);
			//===== 未挿入レコード情報オブジェクトで繰り返し =====
			while (M2MList_next(recordList)!=NULL)
				{
				//===== INSERT文の作成 =====
				if ((value=(unsigned char *)M2MList_getValue(recordList))!=NULL)
					{
					//===== フィールドの位置を初期化 =====
					columnIndex = 0;
					//===== CSV形式のカラム名で繰り返し =====
					while (columnIndex<(dataTypeArrayLength-1)
							&& value!=NULL
							&& (valueIndex=M2MString_indexOf(value, (unsigned char *)M2MString_COMMA))!=NULL)
						{
						//===== 値をセット =====
						this_setValueIntoPreparedStatement(DATA_TYPE_ARRAY[columnIndex], columnIndex+1, value, M2MString_length(value)-M2MString_length(valueIndex), statement);
						//===== フィールド位置を次へ移す =====
						columnIndex++;
						//===== 値の位置を次へ移す =====
						valueIndex += COMMA_LENGTH;
						value = valueIndex;
						}
					//===== カラム数と挿入データ数の整合性を確認 =====
					if (columnIndex==(dataTypeArrayLength-1))
						{
						//===== 最後の挿入データをセット =====
						this_setValueIntoPreparedStatement(DATA_TYPE_ARRAY[columnIndex], columnIndex+1, value, M2MString_length(value), statement);
						//===== INSERT実行 =====
						while ((resultCode=sqlite3_step(statement))==SQLITE_BUSY)
							{
							}
						//===== INSERTに成功した場合 =====
						if (resultCode==SQLITE_DONE)
							{
#ifdef DEBUG
							M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_insertNewRecordList()", __LINE__, (unsigned char *)"SQLite3データベースへレコードを挿入しました");
#endif // DEBUG
							}
						//===== エラー処理 =====
						else
							{
							M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_insertNewRecordList()", __LINE__, (unsigned char *)"SQLite3データベースのINSERT処理でエラーが発生しました", NULL);
							}
						}
					//===== INSERT文のパラメータをリセット =====
					sqlite3_reset(statement);
					//===== 挿入レコード数を数える =====
					numberOfRecord++;
					}
				//===== INSERT文の作成エラー =====
				else
					{
					M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_insertNewRecordList()", __LINE__, (unsigned char *)"SQLite3データベースへレコードを挿入するためのINSERT文作成準備作業で失敗しました", NULL);
					}
				//===== 次の未挿入レコードへ進む =====
				recordList = M2MList_next(recordList);
				}
			//===== （最後の）未挿入レコードからINSERT文を作成 =====
			if ((value=(unsigned char *)M2MList_getValue(recordList))!=NULL)
				{
				columnIndex = 0;
				//===== CSV形式のカラム名で繰り返し =====
				while (columnIndex<dataTypeArrayLength
						&& value!=NULL
						&& (valueIndex=M2MString_indexOf(value, (unsigned char *)M2MString_COMMA))!=NULL)
					{
					//=====  =====
					this_setValueIntoPreparedStatement(DATA_TYPE_ARRAY[columnIndex], columnIndex+1, value, M2MString_length(value)-M2MString_length(valueIndex), statement);
					//=====  =====
					columnIndex++;
					//=====  =====
					valueIndex += COMMA_LENGTH;
					value = valueIndex;
					}
				//===== 挿入データをセット =====
				this_setValueIntoPreparedStatement(DATA_TYPE_ARRAY[columnIndex], columnIndex+1, value, M2MString_length(value), statement);
				//=====  =====
				while ((resultCode=sqlite3_step(statement))==SQLITE_BUSY)
					{
					}
				//===== INSERTに成功した場合 =====
				if (resultCode==SQLITE_DONE)
					{
#ifdef DEBUG
					M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_insertNewRecordList()", __LINE__, (unsigned char *)"SQLite3データベースへレコードを挿入しました");
#endif // DEBUG
					}
				//===== エラー処理 =====
				else
					{
					M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_insertNewRecordList()", __LINE__, (unsigned char *)"SQLite3データベースのINSERT処理でエラーが発生しました", NULL);
					}
				//===== SQL文のパラメータをリセット =====
				sqlite3_reset(statement);
				//=====  =====
				numberOfRecord++;
				}
			//===== INSERT文の作成エラー =====
			else
				{
				M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_insertNewRecordList()", __LINE__, (unsigned char *)"SQLite3データベースへレコードを挿入するためのINSERT文作成準備作業で失敗しました", NULL);
				}
			//===== SQL実行オブジェクトの実行（ヒープメモリ領域を解放） =====
			if (sqlite3_finalize(statement)==SQLITE_OK)
				{
				}
			//===== エラー処理 =====
			else
				{
				M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_insertNewRecordList()", __LINE__, (unsigned char *)sqlite3_errmsg(database), NULL);
				}
			//===== INSERT文のヒープメモリ領域を解放 =====
			M2MHeap_free(insertSQL);
			//===== 挿入したレコード数を返す =====
			return numberOfRecord;
			}
		//===== エラー処理 =====
		else if (tableName==NULL)
			{
			M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_insertNewRecordList()", __LINE__, (unsigned char *)"レコード管理オブジェクトから取得したテーブル名を示す文字列がNULLです", NULL);
			return -1;
			}
		else if (columnNameCSV==NULL)
			{
			M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_insertNewRecordList()", __LINE__, (unsigned char *)"レコード管理オブジェクトから取得したカラム名を示すCSV形式の文字列がNULLです", NULL);
			return -1;
			}
		else if (columnList==NULL)
			{
			M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_insertNewRecordList()", __LINE__, (unsigned char *)"テーブル構築オブジェクトから取得したテーブルのカラム構造体オブジェクトがNULLです", NULL);
			return -1;
			}
		else if (statement==NULL)
			{
			M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_insertNewRecordList()", __LINE__, (unsigned char *)"引数で指定されたSQLite3データベース管理オブジェクトから取得したプリペアドステートメントオブジェクトがNULLです", NULL);
			return -1;
			}
		else
			{
			M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_insertNewRecordList()", __LINE__, (unsigned char *)"テーブル構築オブジェクトから取得したカラムのデータ型配列がNULLです", NULL);
			return -1;
			}
		}
	//===== 引数エラー =====
	else if (database==NULL)
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_insertNewRecordList()", __LINE__, (unsigned char *)"引数で指定されたSQLite3データベース管理オブジェクトがNULLです", NULL);
		return -1;
		}
	else if (tableBuilder==NULL)
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_insertNewRecordList()", __LINE__, (unsigned char *)"引数で指定されたテーブル構築オブジェクトがNULLです", NULL);
		return -1;
		}
	else if (record==NULL)
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_insertNewRecordList()", __LINE__, (unsigned char *)"引数で指定されたレコード管理オブジェクトがNULLです", NULL);
		return -1;
		}
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_insertNewRecordList()", __LINE__, (unsigned char *)"レコード管理オブジェクトから取得した未挿入レコード情報オブジェクトがNULLです", NULL);
		return -1;
		}
	}


/**
 * 引数で指定されたCEP実行オブジェクトが保持する、CEPレコード管理オブジェクトの<br>
 * “メモリ上のSQLite3データベースに挿入済み”のレコードに対し、規定している最大<br>
 * レコード数を超過している場合、超過分のレコードをファイル上のSQLite3データベース<br>
 * に挿入し、永続化を行う。<br>
 *
 * @param[in,out] self	CEP実行オブジェクト
 * @return				処理に成功したCEP実行オブジェクト or NULL（挿入エラーの場合）
 */
static M2MCEP *this_insertRecordListToFileDatabase (M2MCEP *self)
	{
	//========== ローカル変数 ==========
	M2MTableBuilder *tableBuilder = NULL;
	M2MCEPRecord *tableRecord = NULL;
	unsigned int maxRecord = 0;
	bool persistence = false;

	//===== 引数の確認 =====
	if (self!=NULL)
		{
		//===== テーブルレコード管理オブジェクトを取得 =====
		if ((tableBuilder=this_getTableBuilder(self))!=NULL
				&& (tableRecord=M2MCEPRecord_begin(this_getCEPRecord(self)))!=NULL
				&& (maxRecord=this_getMaxRecord(self))>0)
			{
			//===== レコード永続化の場合 =====
			if ((persistence=this_getPersistence(self))==true)
				{
#ifdef DEBUG
				M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_insertRecordListToFileDatabase()", __LINE__, (unsigned char *)"ファイル上のSQLiteデータベースへレコードを挿入するトランザクション処理を開始します");
#endif // DEBUG
				//===== トランザクション開始 =====
				SQLRunner_beginTransaction(this_getFileDatabase(self));
				}
			//===== レコード非永続化の場合 =====
			else
				{
				// 何もしない
				}
			//===== レコード管理オブジェクトで繰り返し =====
			while (M2MCEPRecord_next(tableRecord)!=NULL)
				{
				//===== レコード管理オブジェクトの保持する過去レコード処理を実行 =====
				this_insertOldRecordList(this_getFileDatabase(self), tableRecord, tableBuilder, maxRecord, persistence);
				//===== 次のテーブルレコード管理オブジェクトへ移動 =====
				tableRecord = M2MCEPRecord_next(tableRecord);
				}
			//===== 最後のレコード管理オブジェクトの保持する過去レコード処理を実行 =====
			this_insertOldRecordList(this_getFileDatabase(self), tableRecord, tableBuilder, maxRecord, persistence);
			//===== レコード永続化の場合 =====
			if (this_getPersistence(self)==true)
				{
				//===== トランザクション終了 =====
				SQLRunner_commitTransaction(this_getFileDatabase(self));
#ifdef DEBUG
				M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_insertRecordListToFileDatabase()", __LINE__, (unsigned char *)"ファイル上のSQLiteデータベースへレコードを挿入するトランザクション処理を終了しました");
#endif // DEBUG
				}
			//===== レコード非永続化の場合 =====
			else
				{
				// 何もしない
				}
			//===== 正常終了 =====
			return self;
			}
		//===== エラー処理 =====
		else if (tableBuilder==NULL)
			{
			M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_insertRecordListToFileDatabase()", __LINE__, (unsigned char *)"引数で指定された\"CEP *\"から取得したテーブル構築オブジェクトがNULLです", NULL);
			return NULL;
			}
		else if (tableRecord==NULL)
			{
			M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_insertRecordListToFileDatabase()", __LINE__, (unsigned char *)"引数で指定された\"CEP *\"から取得したテーブルレコード管理オブジェクトがNULLです", NULL);
			return NULL;
			}
		else
			{
			M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_insertRecordListToFileDatabase()", __LINE__, (unsigned char *)"引数で指定された\"CEP *\"から取得したテーブル当たりのCEPレコード数の上限値が0以下の整数です", NULL);
			return NULL;
			}
		}
	//===== 引数エラー =====
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_insertRecordListToFileDatabase()", __LINE__, (unsigned char *)"引数で指定された\"CEP *\"がNULLです", NULL);
		return NULL;
		}
	}


/**
 * 引数で指定されたCEP実行オブジェクトのメンバ変数が保有する、メモリ上のSQLite3<br>
 * データベースに未挿入のレコード（＝CEPRecord->newRecordList）に対し、メモリ<br>
 * 上のSQLite3データベースへ一括挿入処理を行う。<br>
 *
 * @param[in] self	CEP実行オブジェクト
 * @return			挿入処理を行ったレコード数 or -1（エラーの場合）
 */
static int this_insertRecordListToMemoryDatabase (M2MCEP *self)
	{
	//========== ローカル変数 ==========
	sqlite3 *memoryDatabase = NULL;
	M2MTableBuilder *tableBuilder = NULL;
	M2MCEPRecord *record = NULL;
	int numberOfRecord = 0;
	int result = 0;

	//===== 引数の確認 =====
	if (self!=NULL)
		{
		//===== データベースとレコード管理オブジェクトの取得 =====
		if ((memoryDatabase=this_getMemoryDatabase(self))!=NULL
				&& (tableBuilder=this_getTableBuilder(self))!=NULL
				&& (record=M2MCEPRecord_begin(this_getCEPRecord(self)))!=NULL)
			{
#ifdef DEBUG
			M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_insertRecordListToMemoryDatabase()", __LINE__, (unsigned char *)"メモリ上のSQLiteデータベースへレコードを挿入する処理を開始します");
#endif // DEBUG
			//===== トランザクション開始 =====
			SQLRunner_beginTransaction(memoryDatabase);
			//===== レコード管理オブジェクトが末端に辿り着くまで繰り返し =====
			while (M2MCEPRecord_next(record)!=NULL)
				{
				//===== 同一テーブルへのレコード一括挿入 =====
				if ((result=this_insertRecordList(memoryDatabase, record, tableBuilder, M2MCEPRecord_getNewRecordList(record)))>0)
					{
					//===== 挿入済みの新規レコードを過去レコードへ移動 =====
					M2MCEPRecord_moveFromNewRecordListToOldRecordList(record);
					//===== 挿入したレコード数をカウント =====
					numberOfRecord += result;
					}
				//===== エラー処理 =====
				else
					{
					M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_insertRecordListToMemoryDatabase()", __LINE__, (unsigned char *)"メモリ上のSQLiteデータベースへのレコード挿入処理に失敗しました", NULL);
					}
				//===== 次のレコード情報オブジェクトへ進む =====
				record = M2MCEPRecord_next(record);
				}
			//===== （最後の）テーブルへのレコード一括挿入 =====
			if ((result=this_insertRecordList(memoryDatabase, record, tableBuilder, M2MCEPRecord_getNewRecordList(record)))>0)
				{
				//===== 挿入済みの新規レコードを過去レコードへ移動 =====
				M2MCEPRecord_moveFromNewRecordListToOldRecordList(record);
				//===== 挿入したレコード数をカウント =====
				numberOfRecord += result;
				}
			//===== エラー処理 =====
			else
				{
				M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_insertRecordListToMemoryDatabase()", __LINE__, (unsigned char *)"メモリ上のSQLiteデータベースへのレコード挿入処理に失敗しました", NULL);
				}
			//===== コミット =====
			SQLRunner_commitTransaction(memoryDatabase);
#ifdef DEBUG
			M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_insertRecordListToMemoryDatabase()", __LINE__, (unsigned char *)"メモリ上のSQLiteデータベースへレコードを挿入する処理を終了しました");
#endif // DEBUG
			//===== 挿入したレコード数を返す =====
			return numberOfRecord;
			}
		//===== エラー処理 =====
		else if (memoryDatabase==NULL)
			{
			M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_insertRecordListToMemoryDatabase()", __LINE__, (unsigned char *)"引数で指定されたCEP実行オブジェクトから取得したメモリ上のSQLite3管理オブジェクトがNULLです", NULL);
			return -1;
			}
		else if (tableBuilder==NULL)
			{
			M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_insertRecordListToMemoryDatabase()", __LINE__, (unsigned char *)"引数で指定されたCEP実行オブジェクトから取得したテーブル構築オブジェクトがNULLです", NULL);
			return -1;
			}
		else
			{
			M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_insertRecordListToMemoryDatabase()", __LINE__, (unsigned char *)"引数で指定されたCEP実行オブジェクトから取得したレコード管理オブジェクトがNULLです", NULL);
			return -1;
			}
		}
	//===== 引数エラー =====
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_insertRecordListToMemoryDatabase()", __LINE__, (unsigned char *)"引数で指定されたCEP実行オブジェクトがNULLです", NULL);
		return -1;
		}
	}


/**
 * 引数で指定されたパスに、同じく引数で指定されたパーミッションのディレクトリを<br>
 * 作成する。<br>
 *
 * @param[in] directoryPath		ディレクトリパスを示す文字列
 * @param[in] permission		パーミッションを示す3桁の整数文字列
 * @return						ディレクトリパスを示す文字列 or NULL（エラーの場合）
 */
static unsigned char *this_makeDirectory (const unsigned char *directoryPath, const unsigned char *permission)
	{
	if (M2MDirectory_mkdirs(directoryPath, permission)==true)
		{
		return (unsigned char *)directoryPath;
		}
	else
		{
		return NULL;
		}
	}


/**
 * 引数で指定されたデータベース名のSQLite3データベースファイルを規定ディレクトリ下<br>
 * に作成し、当該データベースオブジェクトを返す。<br>
 * 作成に失敗した場合は NULL を返す。<br>
 *
 * @param[in] databaseName	データベース名を示す文字列
 * @param[in] tableBuilder	テーブル構築用オブジェクト
 * @param[in] vacuumRecord	バキューム処理を実行するレコード数（この値が0の場合は自動バキュームを設定する)
 * @return					SQLite3データベースファイルオブジェクト or NULL（エラーの場合）
 */
static sqlite3 *this_openFileDatabase (const unsigned char *databaseName, const M2MTableBuilder *tableBuilder, const unsigned int vacuumRecord)
	{
	//========== ローカル変数 ==========
	sqlite3 *fileDatabase = NULL;
	unsigned char *databaseFilePath = NULL;
#ifdef DEBUG
	unsigned char MESSAGE[256];
#endif // DEBUG

	//===== 引数の確認 =====
	if (databaseName!=NULL && tableBuilder!=NULL)
		{
		//===== データベースファイルパス文字列の取得 =====
		if (this_getDatabaseFilePath(databaseName, &databaseFilePath)!=NULL)
			{
			//===== SQLite3データベースファイルを開く =====
			if (sqlite3_open(databaseFilePath, &fileDatabase)==SQLITE_OK)
				{
#ifdef DEBUG
				memset(MESSAGE, 0, sizeof(MESSAGE));
				snprintf(MESSAGE, sizeof(MESSAGE)-1, (unsigned char *)"ファイル上のSQLiteデータベース(=\"%s\")を開きました", databaseFilePath);
				M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_openFileDatabase()", __LINE__, MESSAGE);
#endif // DEBUG
				//===== データベースファイルパス文字列のメモリ領域を解放 =====
				M2MHeap_free(databaseFilePath);
				//===== 文字コードをUTF-8にセット =====
				this_setUTF8(fileDatabase);
#ifdef DEBUG
				M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_openFileDatabase()", __LINE__, (unsigned char *)"ファイル上のSQLiteデータベースの文字コードをUTF-8に設定しました");
#endif // DEBUG
				//===== 自動バキュームOFFの場合 =====
				if (vacuumRecord>0)
					{
					this_setAutoVacuum(fileDatabase, false);
#ifdef DEBUG
					M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_openFileDatabase()", __LINE__, (unsigned char *)"ファイル上のSQLiteデータベースの自動バキューム機能をOFFに設定しました");
#endif // DEBUG
					}
				//===== 自動バキュームONの場合 =====
				else
					{
					this_setAutoVacuum(fileDatabase, true);
#ifdef DEBUG
					M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_openFileDatabase()", __LINE__, (unsigned char *)"ファイル上のSQLiteデータベースの自動バキューム機能をONに設定しました");
#endif // DEBUG
					}
				//===== WAL(Write Ahead Logging)の設定 =====
				this_setWAL(fileDatabase, true);
				//===== テーブルの構築 =====
				M2MTableBuilder_createTable((M2MTableBuilder *)tableBuilder, fileDatabase);
#ifdef DEBUG
				M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_openFileDatabase()", __LINE__, (unsigned char *)"ファイル上のSQLiteデータベースにCEPテーブルを構築しました");
#endif // DEBUG
				//===== データベースへ返す =====
				return fileDatabase;
				}
			//===== エラー処理 =====
			else
				{
				M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_openFileDatabase()", __LINE__, (unsigned char *)sqlite3_errmsg(fileDatabase), NULL);
				sqlite3_close(fileDatabase);
				//===== データベースファイルパス文字列のメモリ領域を解放 =====
				M2MHeap_free(databaseFilePath);
				return NULL;
				}
			}
		//===== エラー処理 =====
		else
			{
			M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_openFileDatabase()", __LINE__, (unsigned char *)"ファイル上のSQLiteデータベースのファイルパスを示す文字列取得に失敗しました", NULL);
			return NULL;
			}
		}
	//===== エラー処理 =====
	else if (databaseName==NULL)
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_openFileDatabase()", __LINE__, (unsigned char *)"引数で指定されたファイル上のSQLiteデータベース名を示す文字列がNULLです", NULL);
		return NULL;
		}
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_openFileDatabase()", __LINE__, (unsigned char *)"引数で指定されたテーブル構築用オブジェクトがNULLです", NULL);
		return NULL;
		}
	}


/**
 * メモリ上のSQLite3データベースを開く。<br>
 * 当該データベースには引数で指定されたテーブルを構築する。
 *
 * @param[in] tableBuilder	テーブル構築用オブジェクト
 * @param[in] vacuumRecord	バキューム処理を実行するレコード数（この値が0の場合は自動バキュームを設定する)
 * @return					メモリ上のSQLite3データベース or NULL（エラーの場合）
 */
static sqlite3 *this_openMemoryDatabase (const M2MTableBuilder *tableBuilder, const unsigned int vacuumRecord)
	{
	//========== ローカル変数 ==========
	sqlite3 *memoryDatabase = NULL;
	const unsigned char *MEMORY_DATABASE = (unsigned char *)":memory:";

	//===== SQLite3メモリデータベースを開く =====
	if (sqlite3_open(MEMORY_DATABASE, &memoryDatabase)==SQLITE_OK)
		{
#ifdef DEBUG
		M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_openMemoryDatabase()", __LINE__, (unsigned char *)"メモリ上のSQLiteデータベースを開きました");
#endif // DEBUG
		//===== 文字コードをUTF-8にセット =====
		this_setUTF8(memoryDatabase);
#ifdef DEBUG
		M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_openMemoryDatabase()", __LINE__, (unsigned char *)"メモリ上のSQLiteデータベースの文字コードをUTF-8に設定しました");
#endif // DEBUG
		//===== 自動バキュームOFFの場合 =====
		if (vacuumRecord>0)
			{
			//===== バキューム設定をセット =====
			this_setAutoVacuum(memoryDatabase, false);
#ifdef DEBUG
			M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_openMemoryDatabase()", __LINE__, (unsigned char *)"メモリ上のSQLiteデータベースの自動バキューム機能をOFFに設定しました");
#endif // DEBUG
			}
		//===== 自動バキュームONの場合 =====
		else
			{
			//===== バキューム設定をセット =====
			this_setAutoVacuum(memoryDatabase, true);
#ifdef DEBUG
			M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_openMemoryDatabase()", __LINE__, (unsigned char *)"メモリ上のSQLiteデータベースの自動バキューム機能をONに設定しました");
#endif // DEBUG
			}
		//===== WAL(Write Ahead Logging)の設定 =====
		this_setWAL(memoryDatabase, true);
		//===== テーブルの構築 =====
		M2MTableBuilder_createTable((M2MTableBuilder *)tableBuilder, memoryDatabase);
#ifdef DEBUG
		M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_openMemoryDatabase()", __LINE__, (unsigned char *)"メモリ上のSQLiteデータベースにCEPテーブルを構築しました");
#endif // DEBUG
		//===== メモリ上のSQLite3データベースオブジェクトを返す =====
		return memoryDatabase;
		}
	//===== エラー処理 =====
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_openMemoryDatabase()", __LINE__, (unsigned char *)"メモリ上のSQLiteデータベースのオープンに失敗しました", NULL);
		sqlite3_close(memoryDatabase);
		return NULL;
		}
	}


/**
 * 引数で指定されたデータベースに対し，自動バキュームを設定する．<br>
 *
 * @param[in] database	自動バキュームを設定するSQLite3データベースオブジェクト
 * @param[in] flag		true : 自動バキュームを有効にする，false : 自動バキュームを無効にする
 */
static void this_setAutoVacuum (sqlite3 *database, const bool flag)
	{
	//========== ローカル変数 ==========
	unsigned char *PRAGMA_AUTO_VACUUM_SQL = (unsigned char *)"PRAGMA auto_vacuum = 1 ";
	unsigned char *PRAGMA_NOT_AUTO_VACUUM_SQL = (unsigned char *)"PRAGMA auto_vacuum = 0 ";

	//===== 引数の確認 =====
	if (database!=NULL)
		{
		//===== 自動バキュームを有効にする場合 =====
		if (flag==true)
			{
			//===== 自動バキュームを有効にするためのSQL文の実行 =====
			if (SQLRunner_executeUpdate(database, PRAGMA_AUTO_VACUUM_SQL)==true)
				{
				return;
				}
			//===== エラー処理 =====
			else
				{
				M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_setAutoVacuum()", __LINE__, (unsigned char *)"SQLite3データベースの自動バキュームON設定に失敗しました", NULL);
				return;
				}
			}
		//===== 自動バキュームを無効にする場合 =====
		else
			{
			//===== 自動バキュームを無効にするためのSQL文の実行 =====
			if (SQLRunner_executeUpdate(database, PRAGMA_NOT_AUTO_VACUUM_SQL)==true)
				{
				return;
				}
			//===== エラー処理 =====
			else
				{
				M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_setAutoVacuum()", __LINE__, (unsigned char *)"SQLite3データベースの自動バキュームOFF設定に失敗しました", NULL);
				return;
				}
			}
		}
	//===== 引数エラー =====
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_setAutoVacuum()", __LINE__, (unsigned char *)"引数で指定された\"sqlite3 *\"がNULLです", NULL);
		return;
		}
	}


/**
 * 引数で指定されたCEP実行オブジェクトにファイル上のSQLite3データベース名をメンバ<br>
 * 変数に設定する。<br>
 *
 * @param[in,out] self		CEP実行オブジェクト
 * @param[in] databaseName	ファイル上のSQLite3データベース名（ファイル名）
 * @return					メンバ変数が設定されたCEP実行オブジェクト or NULL（エラーの場合）
 */
static M2MCEP *this_setDatabaseName (M2MCEP *self, const unsigned char *databaseName)
	{
	//========== ローカル変数 ==========
	size_t databaseNameLength = 0;
	const unsigned char *FILE_EXTENSION = (unsigned char *)".sqlite";
	const size_t FILE_EXTENSION_LENGTH = M2MString_length(FILE_EXTENSION);
#ifdef DEBUG
	unsigned char MESSAGE[256];
#endif // DEBUG

	//===== 引数の確認 =====
	if (self!=NULL && databaseName!=NULL
			&& (databaseNameLength=M2MString_length(databaseName))>0)
		{
		//===== 初期化 =====
		this_deleteDatabaseName(self);
		//===== 規程の拡張子が付与されているかどうか確認 =====
		if (M2MString_lastIndexOf(databaseName, FILE_EXTENSION)!=NULL)
			{
			//===== ヒープメモリ領域の獲得 =====
			if ((self->databaseName=(unsigned char *)M2MHeap_malloc(databaseNameLength+1))!=NULL)
				{
				memcpy(self->databaseName, databaseName, databaseNameLength);
#ifdef DEBUG
				memset(MESSAGE, 0, sizeof(MESSAGE));
				snprintf(MESSAGE, sizeof(MESSAGE)-1, (unsigned char *)"CEP実行オブジェクトにSQLiteデータベースファイル名(=\"%s\")をセットしました", self->databaseName);
				M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_setDatabaseName()", __LINE__, MESSAGE);
#endif // DEBUG
				return self;
				}
			//===== エラー処理 =====
			else
				{
				M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_setDatabaseName()", __LINE__, (unsigned char *)"データベース名をコピーするためのヒープメモリー領域の獲得に失敗しました", NULL);
				return NULL;
				}
			}
		//===== データベース名に拡張子が付与されていなかった場合 =====
		else
			{
			//===== ヒープメモリ領域の獲得 =====
			if ((self->databaseName=(unsigned char *)M2MHeap_malloc(databaseNameLength+FILE_EXTENSION_LENGTH+1))!=NULL)
				{
				//===== データベース名をコピー =====
				memcpy(&((self->databaseName)[0]), databaseName, databaseNameLength);
				//===== 拡張子をコピー =====
				memcpy(&((self->databaseName)[databaseNameLength]), FILE_EXTENSION, FILE_EXTENSION_LENGTH);
#ifdef DEBUG
				memset(MESSAGE, 0, sizeof(MESSAGE));
				snprintf(MESSAGE, sizeof(MESSAGE)-1, (unsigned char *)"CEP実行オブジェクトにSQLiteデータベースファイル名(=\"%s\")をセットしました", self->databaseName);
				M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_setDatabaseName()", __LINE__, MESSAGE);
#endif // DEBUG
				return self;
				}
			//===== エラー処理 =====
			else
				{
				M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_setDatabaseName()", __LINE__, (unsigned char *)"データベース名をコピーするためのヒープメモリー領域の獲得に失敗しました", NULL);
				return NULL;
				}
			}
		}
	//===== 引数エラー =====
	else if (self==NULL)
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_setDatabaseName()", __LINE__, (unsigned char *)"引数で指定された\"M2MCEP *\"がNULLです", NULL);
		return NULL;
		}
	else if (databaseName==NULL)
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_setDatabaseName()", __LINE__, (unsigned char *)"引数で指定された\"databaseName\"がNULLです", NULL);
		return NULL;
		}
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_setDatabaseName()", __LINE__, (unsigned char *)"引数で指定された\"databaseName\"文字列の長さが0以下です", NULL);
		return NULL;
		}
	}


/**
 * 引数で指定されたCEP実行オブジェクトの保有するメンバー変数として、SQLite3<br>
 * ファイルデータベース管理オブジェクトをセットする。<br>
 *
 * @param[in,out] self		CEP実行オブジェクト
 * @param[in] fileDatabase	ファイル上のSQLite3データベース管理オブジェクト
 * @return					CEP実行オブジェクト or NULL（エラーの場合）
 */
static M2MCEP *this_setFileDatabase (M2MCEP *self, sqlite3 *fileDatabase)
	{
	//===== 引数の確認 =====
	if (self!=NULL && fileDatabase!=NULL)
		{
		self->fileDatabase=fileDatabase;
#ifdef DEBUG
		M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_setFileDatabase()", __LINE__, (unsigned char *)"CEP実行オブジェクトにファイル上のSQLite3データベース管理オブジェクトをセットしました");
#endif // DEBUG
		return self;
		}
	//===== 引数エラー =====
	else if (self==NULL)
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_setFileDatabase()", __LINE__, (unsigned char *)"引数で指定されたCEP実行オブジェクトがNULLです", NULL);
		return NULL;
		}
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_setFileDatabase()", __LINE__, (unsigned char *)"引数で指定されたSQLite3実行オブジェクトがNULLです", NULL);
		return NULL;
		}
	}


/**
 * 引数で指定されたCEP実行オブジェクトの保有するメンバー変数として、メモリー上の<br>
 * SQLite3データベース管理オブジェクトをセットする。<br>
 *
 * @param[in,out] self		CEP実行オブジェクト
 * @param[in] fileDatabase	メモリー上のSQLite3データベース管理オブジェクト
 * @return					CEP実行オブジェクト or NULL（エラーの場合）
 */
static M2MCEP *this_setMemoryDatabase (M2MCEP *self, sqlite3 *memoryDatabase)
	{
	//===== 引数の確認 =====
	if (self!=NULL && memoryDatabase!=NULL)
		{
		self->memoryDatabase = memoryDatabase;
#ifdef DEBUG
		M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_setMemoryDatabase()", __LINE__, (unsigned char *)"CEP実行オブジェクトにメモリー上のSQLite3データベース管理オブジェクトをセットしました");
#endif // DEBUG
		return self;
		}
	//===== 引数エラー =====
	else if (self==NULL)
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_setMemoryDatabase()", __LINE__, (unsigned char *)"引数で指定されたCEP実行オブジェクトがNULLです", NULL);
		return NULL;
		}
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_setMemoryDatabase()", __LINE__, (unsigned char *)"引数で指定されたSQLite3実行オブジェクトがNULLです", NULL);
		return NULL;
		}
	}


/**
 * 引数で指定されたCEP実行オブジェクトに対し、メンバ変数としてテーブル構築<br>
 * オブジェクトをセットする。<br>
 *
 * @param[in,out] self		CEP実行オブジェクト
 * @param[in] tableBuilder	テーブル構築オブジェクト
 * @return					テーブル構築オブジェクトをセットしたCEP実行オブジェクト
 */
static M2MCEP *this_setTableBuilder (M2MCEP *self, const M2MTableBuilder *tableBuilder)
	{
	//===== 引数の確認 =====
	if (self!=NULL && tableBuilder!=NULL)
		{
		//===== 既存のテーブル構築オブジェクトを削除 =====
		this_deleteTableBuilder(self);
		//===== テーブル構築オブジェクトをセット =====
		self->tableBuilder = (M2MTableBuilder *)tableBuilder;
#ifdef DEBUG
		M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_setTableBuilder()", __LINE__, (unsigned char *)"CEP実行オブジェクトにテーブル構築オブジェクトをセットしました");
#endif // DEBUG
		return self;
		}
	//===== 引数エラー =====
	else if (self==NULL)
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_setTableBuilder()", __LINE__, (unsigned char *)"引数で指定されたCEP実行オブジェクトがNULLです", NULL);
		return NULL;
		}
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_setTableBuilder()", __LINE__, (unsigned char *)"引数で指定されたテーブル構築オブジェクトがNULLです", NULL);
		return NULL;
		}
	}


/**
 * 引数で指定されたデータベースに対し、文字コードをUTF-8に設定する。<br>
 *
 * @param[in] database	文字コードをUTF-8に設定するSQLite3データベースオブジェクト
 */
static void this_setUTF8 (sqlite3 *database)
	{
	//========== ローカル変数 ==========
	const unsigned char *PRAGMA_ENCODING_SQL = (unsigned char *)"PRAGMA encoding = 'UTF-8'";

	//===== 引数の確認 =====
	if (database!=NULL)
		{
		//===== SQL文の実行 =====
		if (SQLRunner_executeUpdate(database, PRAGMA_ENCODING_SQL)==true)
			{
			return;
			}
		//===== エラー処理 =====
		else
			{
			M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_setUTF8()", __LINE__, (unsigned char *)"SQLite3データベースのUTF-8文字コード設定に失敗しました", NULL);
			return;
			}
		}
	//===== 引数エラー =====
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_setUTF8()", __LINE__, (unsigned char *)"引数で指定された\"sqlite3 *\"がNULLです", NULL);
		return;
		}
	}


/**
 * @param[in] dataType		フィールドのデータ型を示す列挙子
 * @param[in] index			フィールドのインデックス（>=1)
 * @param[in] value			フィールドの入力データを示す文字列
 * @param[in] valueLength	フィールドの入力データを示す文字列サイズ[Byte]
 * @param[out] statement	プリペアドステートメントオブジェクト
 */
static void this_setValueIntoPreparedStatement (const M2MDataType dataType, unsigned int index, const unsigned char *value, const size_t valueLength, sqlite3_stmt *statement)
	{
	//===== 引数の確認 =====
	if (index>0 && value!=NULL && valueLength<=M2MString_length(value) && statement!=NULL)
		{
		//===== BLOB型データの場合 =====
		if (dataType==M2M_DATA_TYPE_BLOB)
			{
			//===== 値をセット =====
			if (sqlite3_bind_blob(statement, index, value, valueLength, NULL)==SQLITE_OK)
				{
#ifdef DEBUG
				M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_setValueIntoPreparedStatement()", __LINE__, (unsigned char *)"\"BLOB\"型データをセットしました");
#endif // DEBUG
				}
			//===== エラー処理 =====
			else
				{
				M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_setValueIntoPreparedStatement()", __LINE__, (unsigned char *)"\"BLOB\"型データのセットに失敗しました", NULL);
				}
			}
		//===== BOOL型データの場合 =====
		else if (dataType==M2M_DATA_TYPE_BOOL)
			{
			}
		//===== CHAR型データの場合 =====
		else if (dataType==M2M_DATA_TYPE_CHAR)
			{
			//===== 値をセット =====
			if (sqlite3_bind_text(statement, index, value, valueLength, NULL)==SQLITE_OK)
				{
#ifdef DEBUG
				M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_setValueIntoPreparedStatement()", __LINE__, (unsigned char *)"\"BOOL\"型データをセットしました");
#endif // DEBUG
				}
			//===== エラー処理 =====
			else
				{
				M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_setValueIntoPreparedStatement()", __LINE__, (unsigned char *)"\"BOOL\"型データのセットに失敗しました", NULL);
				}
			}
		//===== DATETIME型データの場合 =====
		else if (dataType==M2M_DATA_TYPE_DATETIME)
			{
			//===== 値をセット =====
			if (sqlite3_bind_int64(statement, index, M2MString_convertFromStringToLong(value, valueLength))==SQLITE_OK)
				{
#ifdef DEBUG
				M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_setValueIntoPreparedStatement()", __LINE__, (unsigned char *)"\"DATETIME\"型データをセットしました");
#endif // DEBUG
				}
			//===== エラー処理 =====
			else
				{
				M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_setValueIntoPreparedStatement()", __LINE__, (unsigned char *)"\"DATETIME\"型データのセットに失敗しました", NULL);
				}
			}
		//===== DOUBLE型データの場合 =====
		else if (dataType==M2M_DATA_TYPE_DOUBLE)
			{
			//===== 値をセット =====
			if (sqlite3_bind_double(statement, index, M2MString_convertFromStringToDouble(value, valueLength))==SQLITE_OK)
				{
#ifdef DEBUG
				M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_setValueIntoPreparedStatement()", __LINE__, (unsigned char *)"\"DOUBLE\"型データをセットしました");
#endif // DEBUG
				}
			//===== エラー処理 =====
			else
				{
				M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_setValueIntoPreparedStatement()", __LINE__, (unsigned char *)"\"DOUBLE\"型データのセットに失敗しました", NULL);
				}
			}
		//===== ERROR型データの場合 =====
		else if (dataType==M2M_DATA_TYPE_ERROR)
			{
			}
		//===== FLOAT型データの場合 =====
		else if (dataType==M2M_DATA_TYPE_FLOAT)
			{
			//===== 値をセット =====
			if (sqlite3_bind_double(statement, index, M2MString_convertFromStringToDouble(value, valueLength))==SQLITE_OK)
				{
#ifdef DEBUG
				M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_setValueIntoPreparedStatement()", __LINE__, (unsigned char *)"\"FLOAT\"型データをセットしました");
#endif // DEBUG
				}
			//===== エラー処理 =====
			else
				{
				M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_setValueIntoPreparedStatement()", __LINE__, (unsigned char *)"\"FLOAT\"型データのセットに失敗しました", NULL);
				}
			}
		//===== INTEGER型データの場合 =====
		else if (dataType==M2M_DATA_TYPE_INTEGER)
			{
			//=====  =====
			if (sqlite3_bind_int(statement, index, M2MString_convertFromStringToInteger(value, valueLength))==SQLITE_OK)
				{
#ifdef DEBUG
				M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_setValueIntoPreparedStatement()", __LINE__, (unsigned char *)"\"INTEGER\"型データをセットしました");
#endif // DEBUG
				}
			//===== エラー処理 =====
			else
				{
				M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_setValueIntoPreparedStatement()", __LINE__, (unsigned char *)"\"INTEGER\"型データのセットに失敗しました", NULL);
				}
			}
		//===== NULL型データの場合 =====
		else if (dataType==M2M_DATA_TYPE_NULL)
			{
			//===== 値をセット =====
			if (sqlite3_bind_null(statement, index)==SQLITE_OK)
				{
#ifdef DEBUG
				M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_setValueIntoPreparedStatement()", __LINE__, (unsigned char *)"\"NULL\"型データをセットしました");
#endif // DEBUG
				}
			//===== エラー処理 =====
			else
				{
				M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_setValueIntoPreparedStatement()", __LINE__, (unsigned char *)"\"NULL\"型データのセットに失敗しました", NULL);
				}
			}
		//===== NUMERIC型データの場合 =====
		else if (dataType==M2M_DATA_TYPE_NUMERIC)
			{
			}
		//===== REAL型データの場合 =====
		else if (dataType==M2M_DATA_TYPE_REAL)
			{
			//===== 値をセット =====
			if (sqlite3_bind_double(statement, index, M2MString_convertFromStringToDouble(value, valueLength))==SQLITE_OK)
				{
#ifdef DEBUG
				M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_setValueIntoPreparedStatement()", __LINE__, (unsigned char *)"\"REAL\"型データをセットしました");
#endif // DEBUG
				}
			//===== エラー処理 =====
			else
				{
				M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_setValueIntoPreparedStatement()", __LINE__, (unsigned char *)"\"REAL\"型データのセットに失敗しました", NULL);
				}
			}
		//===== TEXT型データの場合 =====
		else if (dataType==M2M_DATA_TYPE_TEXT)
			{
			//===== 値をセット =====
			if (sqlite3_bind_text(statement, index, value, valueLength, NULL)==SQLITE_OK)
				{
#ifdef DEBUG
				M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_setValueIntoPreparedStatement()", __LINE__, (unsigned char *)"\"TEXT\"型データをセットしました");
#endif // DEBUG
				}
			//===== エラー処理 =====
			else
				{
				M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_setValueIntoPreparedStatement()", __LINE__, (unsigned char *)"\"TEXT\"型データのセットに失敗しました", NULL);
				}
			}
		//===== VARCHAR型データの場合 =====
		else if (dataType==M2M_DATA_TYPE_VARCHAR)
			{
			//===== 値をセット =====
			if (sqlite3_bind_text(statement, index, value, valueLength, NULL)==SQLITE_OK)
				{
#ifdef DEBUG
				M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_setValueIntoPreparedStatement()", __LINE__, (unsigned char *)"\"VARCHAR\"型データをセットしました");
#endif // DEBUG
				}
			//===== エラー処理 =====
			else
				{
				M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_setValueIntoPreparedStatement()", __LINE__, (unsigned char *)"\"VARCHAR\"型データのセットに失敗しました", NULL);
				}
			}
		//===== エラー処理 =====
		else
			{
			M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_setValueIntoPreparedStatement()", __LINE__, (unsigned char *)"引数で指定されたデータ型と一致するデータ型が見つかりません", NULL);
			}
		}
	//===== 引数エラー =====
	else if (index<=0)
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_setValueIntoPreparedStatement()", __LINE__, (unsigned char *)"引数で指定されたデータ値の位置を示すインデックスが0以下です", NULL);
		}
	else if (value==NULL)
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_setValueIntoPreparedStatement()", __LINE__, (unsigned char *)"引数で指定されたデータ値を示す文字列がNULLです", NULL);
		}
	else if (valueLength>M2MString_length(value))
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_setValueIntoPreparedStatement()", __LINE__, (unsigned char *)"引数で指定されたデータ値を示す文字列の長さ[Byte]が実際のサイズ以上の値です", NULL);
		}
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_setValueIntoPreparedStatement()", __LINE__, (unsigned char *)"引数で指定されたプリペアドステートメントオブジェクトがNULLです", NULL);
		}
	return;
	}


/**
 * SQLite3データベースのジャーナルモードをWAL(Write Ahead Logging)にセットする．<br>
 *
 * @param[in] database		WAL設定対象のSQLite3データベース管理オブジェクト
 * @param[in] synchronous	true : 同期モード，false : 非同期モード
 */
static void this_setWAL (sqlite3 *database, const bool synchronous)
	{
	//========== ローカル変数 ==========
	const unsigned char *PRAGMA_JOURNAL_MODE_SQL = (unsigned char *)"PRAGMA journal_mode = WAL ";
	const unsigned char *PRAGMA_SYNCHRONOUS_SQL = (unsigned char *)"PRAGMA synchronous = NORMAL ";
	const unsigned char *PRAGMA_NOT_SYNCHRONOUS_SQL = (unsigned char *)"PRAGMA synchronous = OFF ";

	//===== 引数の確認 =====
	if (database!=NULL)
		{
#ifdef DEBUG
		M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_setWAL()", __LINE__, (unsigned char *)"SQLite3データベースのジャーナルモードを設定します");
#endif // DEBUG
		//===== データベースのジャーナルモードを設定 =====
		if (SQLRunner_executeUpdate(database, PRAGMA_JOURNAL_MODE_SQL)==true)
			{
			//===== 同期モードの場合 =====
			if (synchronous==true
					&& SQLRunner_executeUpdate(database, PRAGMA_SYNCHRONOUS_SQL)==true)
				{
#ifdef DEBUG
				M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_setWAL()", __LINE__, (unsigned char *)"SQLite3データベースのジャーナルモードをWAL（同期モード)に設定しました");
#endif // DEBUG
				return;
				}
			//===== 非同期モードの場合 =====
			else if (synchronous==false
					&& SQLRunner_executeUpdate(database, PRAGMA_NOT_SYNCHRONOUS_SQL)==true)
				{
#ifdef DEBUG
				M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_setWAL()", __LINE__, (unsigned char *)"SQLite3データベースのジャーナルモードをWAL（非同期モード)に設定しました");
#endif // DEBUG
				return;
				}
			//===== エラー処理 =====
			else
				{
				M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_setWAL()", __LINE__, (unsigned char *)"SQLite3データベースの同期モード設定処理に失敗しました", NULL);
				return;
				}
			}
		//===== エラー処理 =====
		else
			{
			M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_setWAL()", __LINE__, (unsigned char *)"SQLite3データベースのWALジャーナルモード設定処理に失敗しました", NULL);
			return;
			}
		}
	//===== 引数エラー =====
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_setWAL()", __LINE__, (unsigned char *)"引数で指定された\"sqlite3 *\"がNULLです", NULL);
		}
	return;
	}


/**
 * SQLite3データベースに対してバキューム処理を実行するためのレコード数を更新する．<br>
 *
 * @param[in] self	CEP実行オブジェクト
 * @param[in] count	カウンターに付加するレコード数（>0) or 0（カウンターを初期化する場合に指定)
 */
static void this_updateRecordCounter (M2MCEP *self, const unsigned int count)
	{
	//===== 引数の確認 =====
	if (self!=NULL)
		{
		//===== 自動バキューム設定がONの場合 =====
		if (this_getVacuumRecord(self)==0)
			{
			return;
			}
		//===== 自動バキューム設定がOFFの場合 =====
		else
			{
			//===== カウンターが1以上の整数の場合 =====
			if (count>=1)
				{
				//===== カウンターを追加 =====
				self->recordCounter = self->recordCounter + count;
#ifdef DEBUG
				M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_addFileDBVacuumCounter()", __LINE__, (unsigned char *)"バキューム処理のレコードカウンターを更新しました");
#endif // DEBUG
				return;
				}
			//===== カウンターが0の場合 =====
			else
				{
				//===== カウンターを初期化 =====
				self->recordCounter = 0;
#ifdef DEBUG
				M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_addFileDBVacuumCounter()", __LINE__, (unsigned char *)"バキューム処理のレコードカウンターを初期化しました");
#endif // DEBUG
				return;
				}
			}
		}
	//===== 引数エラー =====
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_addFileDBVacuumCounter()", __LINE__, (unsigned char *)"引数で指定されたCEP実行オブジェクトがNULLです", NULL);
		return;
		}
	}


/**
 * SQLite3データベースのバキューム処理を実行する．<br>
 *
 * @param database	バキューム処理対象のSQLite3データベース管理オブジェクト
 */
static void this_vacuum (sqlite3 *database)
	{
	//========== ローカル変数 ==========
	const unsigned char *SQL = "VACUUM ";

	//===== 引数の確認 =====
	if (database!=NULL)
		{
#ifdef DEBUG
		M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_vacuum()", __LINE__, (unsigned char *)"SQLite3データベースのバキューム処理を実行します");
#endif // DEBUG
		//===== データベースのバキューム処理を実行 =====
		if (SQLRunner_executeUpdate(database, SQL)==true)
			{
#ifdef DEBUG
			M2MLogger_printDebugMessage((unsigned char *)"M2MCEP.this_vacuum()", __LINE__, (unsigned char *)"SQLite3データベースのバキューム処理を実行しました");
#endif // DEBUG
			return;
			}
		//===== エラー処理 =====
		else
			{
			M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_vacuum()", __LINE__, (unsigned char *)"SQLite3データベースのバキューム処理に失敗しました", NULL);
			return;
			}
		}
	//===== 引数エラー =====
	else
		{
		M2MLogger_printErrorMessage((unsigned char *)"M2MCEP.this_vacuum()", __LINE__, (unsigned char *)"引数で指定された\"sqlite3 *\"がNULLです", NULL);
		}
	return;
	}


/*******************************************************************************
 * 公開関数
 ******************************************************************************/
/**
 * 引数で指定されたCEP実行オブジェクトのヒープメモリ領域を解放する．<br>
 * この関数呼び出し後は引数で師弟されたCEP実行オブジェクトにアクセス出来なく<br>
 * なる事に注意する．<br>
 *
 * @param[in,out] self	メモリ領域解放対象のCEP実行オブジェクト
 */
void M2MCEP_delete (M2MCEP **self)
	{
	//========== ローカル変数 ==========
	M2MCEPRecord *record = NULL;

	//===== 引数の確認 =====
	if (self!=NULL && (*self)!=NULL)
		{
		//===== データベース名文字列のヒープメモリ領域を解放 =====
		this_deleteDatabaseName((*self));
		//===== メモリデータベースを閉じる =====
		this_closeMemoryDatabase((*self));
		//===== CEPレコード情報オブジェクト（未挿入）をファイルデータベースに挿入 =====
		this_flushCEPRecord((*self));
		//===== ファイルデータベースを閉じる =====
		this_closeFileDatabase((*self));
		//===== CEPレコード情報オブジェクトのヒープメモリ領域を解放 =====
		if ((record=this_getCEPRecord((*self)))!=NULL)
			{
			M2MCEPRecord_delete(&record);
			}
		else
			{
			}
		//===== テーブル構築オブジェクトのヒープメモリ領域を解放 =====
		this_deleteTableBuilder((*self));
		//===== CEP実行オブジェクトのヒープメモリ領域を解放 =====
		M2MHeap_free((*self));
#ifdef DEBUG
		M2MLogger_printDebugMessage((M2MString *)"M2MCEP_delete()", __LINE__, (M2MString *)"CEP実行オブジェクトのメモリ領域を解放しました");
#endif // DEBUG
		}
	//===== 引数エラー =====
	else
		{
		// 何もしない
		}
#ifdef DEBUG
	M2MLogger_printDebugMessage((M2MString *)"M2MCEP.delete()", __LINE__, (M2MString *)"********** CEPライブラリ終了 **********");
#endif // DEBUG
	return;
	}


/**
 * 引数で指定されたCSV形式の文字列データをメモリー上のSQLite3データベースのテーブル<br>
 * に挿入する．<br>
 * 内部の処理手順は下記の通りである．<br>
 * <br>
 * 1)CSV形式の文字列をパースし，CEPレコード情報オブジェクトに取り込む．<br>
 * 2)メモリー上のSQLite3データベース内の（引数で指定された）テーブルに、CEPデータを挿入する．<br>
 * 3)メモリー上のSQLite3データベースのテーブルの現在のレコード数を確認し，規定値を<br>
 *   超過した場合は古いレコードから削除する．<br>
 * 4)メモリー上のSQLite3データベースのテーブルから削除したレコードと同じデータを<br>
 *   CEPレコード情報オブジェクトから取得し，（永続化のために）ファイル上のSQLite3<br>
 *   データベースに挿入する．<br>
 * 5)CEPレコード情報オブジェクトのレコードデータから、ファイル上のSQLite3データベース<br>
 *   に挿入して永続化した超過分のレコードを削除する．<br>
 *
 * @param[in,out] self	CEP構造体オブジェクト
 * @param[in] tableName	データ挿入対象のテーブル名
 * @param[in] csv		挿入データであるCSV形式の文字列（1行目はヘッダとしてカラム名を指定する事）
 * @return				メモリー上のSQLite3データベースに挿入したレコードの行数[行] or -1（エラーの場合）
 */
int M2MCEP_insertCSV (M2MCEP *self, const M2MString *tableName, const M2MString *csv)
	{
	//========== ローカル変数 ==========
	int numberOfRecord = 0;

	//===== 引数の確認 =====
	if (self!=NULL && tableName!=NULL && csv!=NULL)
		{
		//===== レコード管理オブジェクトへCSV形式の文字列をセット =====
		if (M2MCEPRecord_setCSV(this_getCEPRecord(self), tableName, csv)>0)
			{
			//===== メモリ上のSQLite3データベースへレコード挿入 =====
			if ((numberOfRecord=this_insertRecordListToMemoryDatabase(self))>=0)
				{
				//===== バキューム処理用のレコードカウンターを更新 =====
				this_updateRecordCounter(self, numberOfRecord);
				//===== メモリ上のSQLite3データベースのレコード数を調整 =====
				this_adjustMemoryDatabaseRecord(self, tableName);
				//===== ファイル上のSQLite3データベースへレコード挿入 =====
				if (this_insertRecordListToFileDatabase(self)!=NULL)
					{
					//===== バキューム処理の有無確認 =====
					this_checkRecordCounterForVacuum(self);
					//===== レコード数を返す =====
					return numberOfRecord;
					}
				//===== エラー処理 =====
				else
					{
					M2MLogger_printErrorMessage((M2MString *)"M2MCEP_insertCSV()", __LINE__, (M2MString *)"ファイル上のSQLite3データベースへのレコード挿入処理に失敗しました", NULL);
					return -1;
					}
				}
			//===== エラー処理 =====
			else
				{
				M2MLogger_printErrorMessage((M2MString *)"M2MCEP_insertCSV()", __LINE__, (M2MString *)"メモリ上のSQLite3データベースへのレコード挿入処理に失敗しました", NULL);
				return -1;
				}
			}
		//===== エラー処理 =====
		else
			{
			M2MLogger_printErrorMessage((M2MString *)"M2MCEP_insertCSV()", __LINE__, (M2MString *)"レコード管理オブジェクトへCSV形式の文字列をセットするのに失敗しました", NULL);
			return -1;
			}
		}
	//===== 引数エラー =====
	else if (self==NULL)
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MCEP_insertCSV()", __LINE__, (M2MString *)"引数で指定されたCEP実行オブジェクトがNULLです", NULL);
		return -1;
		}
	else if (tableName==NULL)
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MCEP_insertCSV()", __LINE__, (M2MString *)"引数で指定されたテーブル名を示す文字列がNULLです", NULL);
		return -1;
		}
	else
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MCEP_insertCSV()", __LINE__, (M2MString *)"引数で指定されたCSV形式のレコードを示す文字列がNULLです", NULL);
		return -1;
		}
	}


/**
 * CEP構造体オブジェクトを新規作成し、SQLite3データベース処理のための準備を行う．<br>
 * テーブルについては、メモリ上のSQLite3データベースは"M2MCEP"オブジェクトが作成<br>
 * される度に毎回構築する必要があるため、必ず指定する事。<br>
 *
 * @param[in] databaseName	SQLite3データベース名
 * @param[in] tableBuilder	SQLite3データベースのテーブルを構築するためのオブジェクト
 * @return					CEP実行オブジェクト or NULL（エラーの場合）
 */
M2MCEP *M2MCEP_new (const M2MString *databaseName, const M2MTableBuilder *tableBuilder)
	{
	//========== ローカル変数 ==========
	M2MCEP *self = NULL;

#ifdef DEBUG
	M2MLogger_printDebugMessage((M2MString *)"M2MCEP.new()", __LINE__, (M2MString *)"********** CEPライブラリ起動 **********");
#endif // DEBUG
	//===== 引数の確認 =====
	if (databaseName!=NULL && tableBuilder!=NULL)
		{
#ifdef DEBUG
		M2MLogger_printDebugMessage((M2MString *)"M2MCEP_new()", __LINE__, (M2MString *)"CEPオブジェクトを新規作成します");
#endif // DEBUG
		//===== CEP実行オブジェクトのヒープメモリを獲得 =====
		if ((self=(M2MCEP *)M2MHeap_malloc(sizeof(M2MCEP)))!=NULL)
			{
			//===== CEP実行オブジェクトの初期化 =====
			if (this_init(self, databaseName, tableBuilder)!=NULL)
				{
#ifdef DEBUG
				M2MLogger_printDebugMessage((M2MString *)"M2MCEP_new()", __LINE__, (M2MString *)"CEPオブジェクトの新規作成処理が終了しました");
#endif // DEBUG
				return self;
				}
			//===== エラー処理 =====
			else
				{
				M2MLogger_printErrorMessage((M2MString *)"M2MCEP_new()", __LINE__, (M2MString *)"CEP実行オブジェクトの初期化処理に失敗しました", NULL);
				//===== ヒープメモリの解放 =====
				M2MCEP_delete(&self);
				return NULL;
				}
			}
		//===== エラー処理 =====
		else
			{
			M2MLogger_printErrorMessage((M2MString *)"M2MCEP_new()", __LINE__, (M2MString *)"CEP実行オブジェクトを新規作成するためのヒープメモリ領域の獲得に失敗しました", NULL);
			return NULL;
			}
		}
	//===== 引数エラー =====
	else if (databaseName==NULL)
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MCEP_new()", __LINE__, (M2MString *)"引数で指定された\"databaseName\"文字列がNULLです", NULL);
		return NULL;
		}
	else
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MCEP_new()", __LINE__, (M2MString *)"引数で指定された\"tableBuilder\"がNULLです", NULL);
		return NULL;
		}
	}


/**
 * メモリ上のSQLite3データベースに対し，引数で指定されたSELECT文を実行し，<br>
 * その結果をCSV形式の文字列で返す．<br>
 * 実行結果であるCSV形式の文字列は，1行めにカラム名文字列，2行目以降がデータとなる．<br>
 * <br>
 * 【注意事項】<br>
 * この関数を実行するためには、SQLite3のコンパイル時のオプション設定として、<br>
 * "SQLITE_ENABLE_COLUMN_METADATA"が有効になっていなければならない。<br>
 * 何故ならば、この関数はSELECT結果からテーブル情報を取得しないと、結果<br>
 * として出力するCSV形式の文字列が作成出来ないからである。<br>
 * もし、"SQLITE_ENABLE_COLUMN_METADATA"が有効でない場合、この関数にて<br>
 * コンパイルエラーとなる事に留意する事(未定義関数を呼び出しているため)。<br>
 *
 * @param[in] self		CEP構造体オブジェクト（データベース管理に使用する）
 * @param[in] sql		SELECT文を示す文字列
 * @param[out] result	実行結果であるCSV形式の文字列をコピーするためのバッファ
 * @return				バッファにコピーした、実行結果のCSV形式の文字列のポインタ or NULL（エラーの場合）
 */
M2MString *M2MCEP_select (M2MCEP *self, const M2MString *sql, M2MString **result)
	{
	//========== ローカル変数 ==========
	sqlite3 *memoryDatabase = NULL;
	sqlite3_stmt* statement = NULL;
	int resultCode = 0;
	int columnLength = -1;
	int i = 0;
	bool thisIsHeader = true;
	M2MString *columnName = NULL;
	size_t columnNameLength = 0;
	M2MString *data = NULL;
	size_t dataLength = 0;
	int sqliteColumnType = 0;

	//===== 引数の確認 =====
	if (self!=NULL && sql!=NULL && result!=NULL)
		{
		//===== SQL文をVDBE（内部実行形式）へ変換 =====
		if ((memoryDatabase=this_getMemoryDatabase(self))!=NULL
				&& sqlite3_prepare(memoryDatabase, sql, -1, &statement, NULL)==SQLITE_OK)
			{
			//===== SQL実行（結果行が末端に辿り着くまで繰り返し） =====
			while ((resultCode=sqlite3_step(statement))==SQLITE_ROW)
				{
				//===== SELECT結果の列数が未取得の場合 =====
				if (columnLength<0)
					{
					//===== SELECT結果の列数を取得 =====
					columnLength = sqlite3_column_count(statement);
					}
				//===== SELECT結果の列数を取得済みの場合 =====
				else
					{
					// 何もしない
					}
				//===== ヘッダ行を出力していない場合 =====
				if (thisIsHeader==true)
					{
					//===== 列数で繰り返し =====
					for (i=0; i<columnLength; i++)
						{
						//===== （参考コード：）エイリアスではない、本来のテーブル名(UTF-8)を取得 =====
//						sqlite3_column_table_name(statement, 1);
						//===== エイリアスではない、本来の列名（UTF-8）を取得 =====
						if ((columnName=(M2MString *)sqlite3_column_origin_name(statement, i))!=NULL
								&& (columnNameLength=M2MString_length(columnName))>0)
							{
							//===== 初回の場合 =====
							if (i==0)
								{
								// 何もしない
								}
							//===== 2回目以降の場合 =====
							else
								{
								//===== カンマ文字を付加 =====
								M2MString_append(result, M2MString_COMMA);
								}
							//===== カラム名を付加 =====
							M2MString_append(result, columnName);
							}
						//===== エラー処理 =====
						else
							{
							M2MLogger_printErrorMessage((M2MString *)"M2MCEP_select()", __LINE__, (M2MString *)"SELECT結果からカラム名を示す文字列が取得出来ません", NULL);
							}
						}
					//===== 改行コードを付加 =====
					M2MString_append(result, M2MString_CRLF);
					//===== ヘッダー行出力のフラグを下ろす =====
					thisIsHeader = false;
					}
				//===== ヘッダ行を出力済みの場合 =====
				else
					{
					// 何もしない
					}
				//===== SELECT結果データの出力を列数で繰り返し =====
				for (i=0; i<columnLength; i++)
					{
					//===== 初回の場合 =====
					if (i==0)
						{
						// 何もしない
						}
					//===== 2回目以降の場合 =====
					else
						{
						//===== カンマ文字を付加 =====
						M2MString_append(result, M2MString_COMMA);
						}
					//===== SELECT結果データの列のデータ型で分岐 =====
					if ((sqliteColumnType=sqlite3_column_type(statement, i))==SQLITE_INTEGER)
						{
						if (M2MString_convertFromIntegerToString(sqlite3_column_int(statement, i), &data)!=NULL)
							{
							//===== 整数データ文字列を付加 =====
							M2MString_append(result, data);
							//===== 整数データ文字列のメモリ領域を解放 =====
							M2MHeap_free(data);
							}
						//===== エラー処理 =====
						else
							{
							M2MLogger_printErrorMessage((M2MString *)"M2MCEP_select()", __LINE__, (M2MString *)"SELECT結果の整数データを文字列に変換出来ませんでした", NULL);
							}
						}
					//===== 浮動小数点数の場合 =====
					else if (sqliteColumnType==SQLITE_FLOAT)
						{
						if (M2MString_convertFromDoubleToString(sqlite3_column_double(statement, i), &data)!=NULL)
							{
							//===== 整数データ文字列を付加 =====
							M2MString_append(result, data);
							//===== 整数データ文字列のメモリ領域を解放 =====
							M2MHeap_free(data);
							}
						//===== エラー処理 =====
						else
							{
							M2MLogger_printErrorMessage((M2MString *)"M2MCEP_select()", __LINE__, (M2MString *)"SELECT結果の浮動小数点数データを文字列に変換出来ませんでした", NULL);
							}
						}
					//===== 文字列の場合 =====
					else if (sqliteColumnType==SQLITE_TEXT)
						{
						if ((data=(M2MString *)sqlite3_column_text(statement, i))!=NULL)
							{
							//===== 文字列を付加 =====
							M2MString_append(result, data);
							}
						//===== エラー処理 =====
						else
							{
							M2MLogger_printErrorMessage((M2MString *)"M2MCEP_select()", __LINE__, (M2MString *)"SELECT結果の文字列データを取得出来ませんでした", NULL);
							}
						}
					//===== バイナリの場合 =====
					else if (sqliteColumnType==SQLITE_BLOB)
						{
						if ((dataLength=(size_t)sqlite3_column_bytes(statement, i))>0
								&& M2MBase64_encode(sqlite3_column_blob(statement, i), dataLength, &data, false)!=NULL)
							{
							//===== バイナリデータをBase64変換した文字列を付加 =====
							M2MString_append(result, data);
							//===== 文字列のメモリ領域を解放 =====
							M2MHeap_free(data);
							}
						//===== エラー処理 =====
						else if (dataLength<=0)
							{
							M2MLogger_printErrorMessage((M2MString *)"M2MCEP_select()", __LINE__, (M2MString *)sqlite3_errmsg(memoryDatabase), NULL);
							}
						else
							{
							M2MLogger_printErrorMessage((M2MString *)"M2MCEP_select()", __LINE__, (M2MString *)sqlite3_errmsg(memoryDatabase), NULL);
							}
						}
					//===== NULLの場合 =====
					else if (sqliteColumnType==SQLITE_NULL)
						{
						M2MString_append(result, (M2MString *)"NULL");
						}
					//===== その他の場合 =====
					else
						{
						M2MLogger_printErrorMessage((M2MString *)"M2MCEP_select()", __LINE__, (M2MString *)"SQLite内部エラー！（SELECT結果のデータ型が規程の値と異なります）", NULL);
						}
					}
				//===== 改行コードを付加 =====
				M2MString_append(result, M2MString_CRLF);
				}
			sqlite3_finalize(statement);
			//===== 実行結果にデータが含まれている場合 =====
			if (this_includesData((*result))==true)
				{
				//===== そのまま実行結果を返す =====
				return (*result);
				}
			//===== 実行結果にデータが含まれていなかった場合 =====
			else
				{
				//===== ヒープメモリ領域を解放 =====
				M2MHeap_free((*result));
				return NULL;
				}
			}
		//===== エラー処理 =====
		else
			{
			M2MLogger_printErrorMessage((M2MString *)"M2MCEP_select()", __LINE__, (M2MString *)sqlite3_errmsg(memoryDatabase), NULL);
			return NULL;
			}
		}
	//===== 引数エラー =====
	else if (self==NULL)
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MCEP_select()", __LINE__, (M2MString *)"引数で指定された\"M2MCEP *\"がNULLです", NULL);
		return NULL;
		}
	else if (sql==NULL)
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MCEP_select()", __LINE__, (M2MString *)"引数で指定された\"sql\"がNULLです", NULL);
		return NULL;
		}
	else
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MCEP_select()", __LINE__, (M2MString *)"引数で指定された\"result\"がNULLです", NULL);
		return NULL;
		}
	}


/**
 * メモリに存在するSQLite3データベース内の各テーブルに格納可能なレコード数上限値<br>
 * を引数で指定された整数値に変更する．<br>
 * なお，レコード数上限値には 500 [件]以上の整数を設定する事は出来ない．<br>
 *
 * @param[in,out] self		最大レコード数変更対象のCEP実行オブジェクト
 * @param[in] maxRecord		1つのテーブルにおけるレコード数上限値を示す整数 or 0（エラーの場合）
 * @return					レコード数上限値を設定したCEP実行オブジェクト or NULL（エラーの場合）
 */
M2MCEP *M2MCEP_setMaxRecord (M2MCEP *self, const unsigned int maxRecord)
	{
	//========== ローカル変数 ==========
	const unsigned int MAX_RECORD_MAX = 500;
#ifdef DEBUG
	M2MString MESSAGE[256];
#endif // DEBUG

	//===== 引数の確認 =====
	if (self!=NULL && 0<maxRecord && maxRecord<MAX_RECORD_MAX)
		{
		//===== レコード数上限値の設定 =====
		self->maxRecord = maxRecord;
#ifdef DEBUG
		memset(MESSAGE, 0, sizeof(MESSAGE));
		snprintf(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"CEPオブジェクトにメモリ上のSQLiteデータベースのレコード数上限値(=\"%u\")をセットしました", self->maxRecord);
		M2MLogger_printDebugMessage((M2MString *)"M2MCEP.this_setDatabaseName()", __LINE__, MESSAGE);
#endif // DEBUG
		return self;
		}
	//===== 引数エラー =====
	else if (self==NULL)
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MCEP_setMaxRecord()", __LINE__, (M2MString *)"引数で指定されたCEP実行オブジェクトがNULLです", NULL);
		return NULL;
		}
	else if (maxRecord<=0)
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MCEP_setMaxRecord()", __LINE__, (M2MString *)"引数で指定されたレコード数上限値が最小設定値である0以下の整数です", NULL);
		return NULL;
		}
	else
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MCEP_setMaxRecord()", __LINE__, (M2MString *)"引数で指定されたレコード数上限値が最大設定値である500以上の整数です", NULL);
		return NULL;
		}
	}


/**
 * 共有ライブラリ（libcep.so)利用時に強制終了した場合、当該シグナルを取得して<br>
 * 終了処理を行うためにイベントハンドラーをセットする．<br>
 *
 * @param[in] self	CEP実行オブジェクト
 */
void M2MCEP_setSignalHandler (const M2MCEP *self)
	{
	//========== ローカル変数 ==========
	struct sigaction newAction;
	struct sigaction oldAction;

	//=====  =====
//	newAction.sa_handler = termination_handler;
	//=====  =====
	sigemptyset(&newAction.sa_mask);
	//=====  =====
	newAction.sa_flags = 0;
	//=====  =====
	sigaction(SIGINT, NULL, &oldAction);
	//=====  =====
	if (oldAction.sa_handler!=SIG_IGN)
		{
		sigaction (SIGINT, &newAction, NULL);
		}
	//=====  =====
	sigaction(SIGHUP, NULL, &oldAction);
	//=====  =====
	if (oldAction.sa_handler!=SIG_IGN)
		{
		sigaction(SIGHUP, &newAction, NULL);
		}
	//=====  =====
	sigaction(SIGTERM, NULL, &oldAction);
	//=====  =====
	if (oldAction.sa_handler!=SIG_IGN)
		{
		sigaction(SIGTERM, &newAction, NULL);
		}
	//=====  =====
	sigaction(SIGKILL, NULL, &oldAction);
	//=====  =====
	if (oldAction.sa_handler!=SIG_IGN)
		{
		sigaction(SIGKILL, &newAction, NULL);
		}
	return;
	}


/**
 * 引数で指定されたCEP構造体オブジェクトに対し、永続性（ファイル上のSQLite3<br>
 * データベースへの記録)の可否を示すフラグをセットする．<br>
 * true を設定した場合、M2MCEP_insertCSV() で挿入されたレコードは全て、<br>
 * ファイル上のSQLite3データベースへ記録する．<br>
 * false を設定した場合、ファイル上のSQLite3データベースへの記録は実行しない．<br>
 *
 * @param[in,out] self		CEP構造体オブジェクト
 * @param[in] persistence	永続性（ファイル上のSQLite3データベースへの記録)の可否を示すフラグ
 * @return					永続性の可否を示すフラグを設定したCEP実行オブジェクト or NULL（エラーの場合）
 */
M2MCEP *M2MCEP_setPersistence (M2MCEP *self, const bool persistence)
	{
	//===== 引数の確認 =====
	if (self!=NULL)
		{
		//===== 永続化の場合 =====
		if (persistence==true)
			{
			self->persistence = true;
#ifdef DEBUG
			M2MLogger_printDebugMessage((M2MString *)"M2MCEP_setPersistence()", __LINE__, (M2MString *)"挿入レコード永続化の設定フラグを\"ON\"にしました");
#endif // DEBUG
			return self;
			}
		//===== 非永続化の場合 =====
		else if (persistence==false)
			{
			self->persistence = false;
#ifdef DEBUG
			M2MLogger_printDebugMessage((M2MString *)"M2MCEP_setPersistence()", __LINE__, (M2MString *)"挿入レコード永続化の設定フラグを\"OFF\"にしました");
#endif // DEBUG
			return self;
			}
		//===== エラー処理 =====
		else
			{
			M2MLogger_printErrorMessage((M2MString *)"M2MCEP_setPersistence()", __LINE__, (M2MString *)"引数で指定された永続化可否のフラグが\"true\"でも\"false\"でもありません", NULL);
			return NULL;
			}
		}
	//===== 引数エラー =====
	else
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MCEP_setPersistence()", __LINE__, (M2MString *)"引数で指定されたCEP実行オブジェクトがNULLです", NULL);
		return NULL;
		}
	}


/**
 * SQLite3データベースのバキューム処理を行うレコード数を設定する．<br>
 * このバキューム処理設定については，メモリ上のSQLite3データベースとファイル上の
 * SQLite3データベース共用の設定となる．<br>
 *
 * @param self			バキューム処理のレコード数設定対象のCEP実行オブジェクト
 * @param vacuumRecord	バキューム処理のレコード数を示す整数値（0を指定した場合，自動バキューム設定となる)
 * @return				バキューム処理を行うレコード数を設定したCEP実行オブジェクト or NULL（エラーの場合）
 */
M2MCEP *M2MCEP_setVacuumRecord (M2MCEP *self, const unsigned int vacuumRecord)
	{
	//===== 引数の確認 =====
	if (self!=NULL)
		{
		self->vacuumRecord = vacuumRecord;
		return self;
		}
	//===== 引数エラー =====
	else
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MCEP_setMaxMemoryDatabaseVacuumRecord()", __LINE__, (M2MString *)"引数で指定されたCEP実行オブジェクトがNULLです", NULL);
		return NULL;
		}
	}



/* End Of File */
