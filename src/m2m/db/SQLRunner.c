/*******************************************************************************
 * SQLRunner.c
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

#include "m2m/db/SQLRunner.h"


/*******************************************************************************
 * 公開メソッド
 ******************************************************************************/
/**
 * 指定されたSQLite3データベース管理オブジェクトに対し、トランザクションを開始する．<br>
 *
 * @param[in] database	SQLite3データベース管理オブジェクト
 * @return				true : 成功，false : 失敗
 */
bool SQLRunner_beginTransaction (sqlite3 *database)
	{
	//========== ローカル変数 ==========
	const M2MString *BEGIN_SQL = (M2MString *)"BEGIN ";

	//===== 引数の確認 =====
	if (database!=NULL)
		{
		//===== SQL実行 =====
		if (SQLRunner_executeUpdate(database, BEGIN_SQL)==true)
			{
#ifdef DEBUG
			M2MLogger_printDebugMessage((M2MString *)"SQLRunner_beginTransaction()", __LINE__, (M2MString *)"SQLite3データベースに対してトランザクションを開始しました");
#endif // DEBUG
			return true;
			}
		//===== エラー処理 =====
		else
			{
			M2MLogger_printErrorMessage((M2MString *)"SQLRunner_beginTransaction()", __LINE__, (M2MString *)"SQLite3データベースに対してトランザクションの開始に失敗しました", NULL);
			return false;
			}
		}
	//===== 引数エラー =====
	else
		{
		M2MLogger_printErrorMessage((M2MString *)"SQLRunner_beginTransaction()", __LINE__, (M2MString *)"引数で指定された\"sqlite3 *\"がNULLです", NULL);
		return false;
		}
	}


/**
 * 指定されたSQLite3データベース管理オブジェクトのトランザクションをコミットする．<br>
 *
 * @param[in] database	SQLite3データベース
 * @return				true : 成功，false : 失敗
 */
bool SQLRunner_commitTransaction (sqlite3 *database)
	{
	//========== ローカル変数 ==========
	const M2MString *COMMIT_SQL = (M2MString *)"COMMIT ";

	//===== 引数の確認 =====
	if (database!=NULL)
		{
		//===== コミット =====
		if (SQLRunner_executeUpdate(database, COMMIT_SQL)==true)
			{
#ifdef DEBUG
			M2MLogger_printDebugMessage((M2MString *)"SQLRunner_commitTransaction()", __LINE__, (M2MString *)"SQLite3データベースに対する操作をコミットしました");
#endif // DEBUG
			return true;
			}
		//===== エラー処理 =====
		else
			{
			M2MLogger_printErrorMessage((M2MString *)"SQLRunner_commitTransaction()", __LINE__, (M2MString *)"SQLite3データベースに対するコミットに失敗しました", NULL);
			return false;
			}
		}
	//===== 引数エラー =====
	else
		{
		M2MLogger_printErrorMessage((M2MString *)"SQLRunner_commitTransaction()", __LINE__, (M2MString *)"引数で指定された\"sqlite3 *\"がNULLです", NULL);
		return false;
		}
	}


/**
 * 引数で指定されたSQLite3データベース管理オブジェクトに対してSQL文を実行する．<br>
 * <br>
 * 【注意】<br>
 * この処理は完了まで時間が掛かるため，通常の"INSERT"や"UPDATE"の目的では利用<br>
 * しない事．<br>
 *
 * @param database	SQLite3データベース管理オブジェクト
 * @param sql		SQL文を示す文字列
 * @return			true : 成功，false : 失敗
 */
bool SQLRunner_executeUpdate (sqlite3 *database, const M2MString *sql)
	{
	//========== ローカル変数 ==========
	sqlite3_stmt *statement = NULL;

	//===== 引数の確認 =====
	if (database!=NULL && sql!=NULL)
		{
		//===== SQL文をVDBE（内部実行形式）へ変換 =====
		if (sqlite3_prepare(database, sql, -1, &statement, NULL)==SQLITE_OK)
			{
			//===== SQL実行 =====
			while (sqlite3_step(statement)==SQLITE_BUSY)
				{
				}
			if (sqlite3_finalize(statement)==SQLITE_OK)
				{
				return true;
				}
			//===== エラー処理 =====
			else
				{
				M2MLogger_printErrorMessage((M2MString *)"SQLRunner_executeUpdate()", __LINE__, (M2MString *)sqlite3_errmsg(database), NULL);
				return false;
				}
			}
		//===== エラー処理 =====
		else
			{
			M2MLogger_printErrorMessage((M2MString *)"SQLRunner_executeUpdate()", __LINE__, (M2MString *)sqlite3_errmsg(database), NULL);
			return false;
			}
		}
	//===== 引数エラー =====
	else if (database==NULL)
		{
		M2MLogger_printErrorMessage((M2MString *)"SQLRunner_executeUpdate()", __LINE__, (M2MString *)"引数で指定されたSQLite3データベース管理オブジェクトがNULLです", NULL);
		return false;
		}
	else
		{
		M2MLogger_printErrorMessage((M2MString *)"SQLRunner_executeUpdate()", __LINE__, (M2MString *)"引数で指定されたSQL文を示す文字列がNULLです", NULL);
		return false;
		}
	}



/* End Of File */
