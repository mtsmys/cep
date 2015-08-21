/*******************************************************************************
 * SQLRunner.h
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

#ifndef SQLRUNNER_H_
#define SQLRUNNER_H_



#include "m2m/util/M2MLogger.h"
#include <sqlite3.h>



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



/*******************************************************************************
 * 公開メソッド
 ******************************************************************************/
/**
 * 指定されたSQLite3データベース管理オブジェクトに対し、トランザクションを開始する．<br>
 *
 * @param[in] database	SQLite3データベース管理オブジェクト
 * @return				true : 成功，false : 失敗
 */
bool SQLRunner_beginTransaction (sqlite3 *database);


/**
 * 指定されたSQLite3データベース管理オブジェクトのトランザクションをコミットする．<br>
 *
 * @param[in] database	SQLite3データベース
 * @return				true : 成功，false : 失敗
 */
bool SQLRunner_commitTransaction (sqlite3 *database);


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
bool SQLRunner_executeUpdate (sqlite3 *database, const M2MString *sql);




#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* SQLRUNNER_H_ */
