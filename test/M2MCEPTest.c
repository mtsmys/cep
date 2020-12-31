/*******************************************************************************
 * M2MCEPTest.c
 *
 * Copyright (c) 2019, Akihisa Yasuda
 * All rights reserved.
 ******************************************************************************/

#include "m2m/app/cep/M2MCEP.h"
#include "m2m/lib/db/M2MTableManager.h"
#include "m2m/lib/db/M2MColumnList.h"
#include "m2m/lib/lang/M2MString.h"
#include "m2m/lib/log/M2MFileAppender.h"
#include "m2m/lib/io/M2MHeap.h"
#include <stdbool.h>


/**
 * Entry point.<br>
 *
 * @param argc
 * @param argv
 * @return
 *
int main (int argc, char **argv)
	{
	//========== Variable ==========
	M2MCEP *cep = NULL;
	M2MTableManager *tableBuilder = NULL;
	M2MColumnList *columnList = NULL;
	unsigned int i = 0;
	M2MString *result = NULL;
	M2MString CSV[256];
	const bool PERSISTENCE = true;
	const unsigned int MAX_REPEAT = (unsigned int)1010;
//	const unsigned int VACUUM_RECORD = (unsigned int)1000;
	const M2MString *TABLE_NAME = (M2MString *)"daily";
	const M2MString *DATABASE_NAME = (M2MString *)"monthly";
	const M2MString *METHOD_NAME = (M2MString *)"CEPTest.main()";

	//===== カラム構造体オブジェクトの新規作成 =====
	if ((columnList=M2MColumnList_new())!=NULL)
		{
		//===== テーブル構築オブジェクトの新規作成 =====
		if (M2MColumnList_add(columnList, (M2MString *)"date", M2MSQLiteDataType_DATETIME, false, false, false, false)!=NULL
				&& M2MColumnList_add(columnList, (M2MString *)"name", M2MSQLiteDataType_TEXT, false, false, false, false)!=NULL
				&& M2MColumnList_add(columnList, (M2MString *)"value", M2MSQLiteDataType_DOUBLE, false, false, false, false)!=NULL
				&& (tableBuilder=M2MTableManager_new())!=NULL)
			{
			//===== CEP実行オブジェクトの新規作成 =====
			if (M2MTableManager_setConfig(tableBuilder, TABLE_NAME, columnList)!=NULL
					&& (cep=M2MCEP_new(DATABASE_NAME, tableBuilder))!=NULL)
				{
				//===== レコードの永続化を設定 =====
				M2MCEP_setPersistence(cep, PERSISTENCE);
				//===== バキューム実行レコード数を設定 =====
//				M2MCEP_setVacuumRecord(cep, VACUUM_RECORD);
				M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"挿入処理を開始します");
				//===== 規程回数だけ繰り返し =====
				for (i=0; i<MAX_REPEAT; i++)
					{
					memset(CSV, 0, sizeof(CSV));
					snprintf(CSV, sizeof(CSV)-1, (M2MString *)"date,name,value\r\n%u,食べ物,%f", i, (double)i);
					M2MCEP_insertCSV(cep, TABLE_NAME, CSV);
					}
				M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"挿入処理を終了します");
				M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"検索処理を開始します");
				//=====  =====
				if (M2MCEP_select(cep, (M2MString *)"SELECT * FROM daily ORDER BY date ", &result)!=NULL)
					{
//					M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, result);
					M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"検索処理を終了します");
					M2MHeap_free(result);
					}
				else
					{
					M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"検索処理に失敗しました");
					}
				//===== CEP実行オブジェクトの削除 =====
				M2MCEP_delete(&cep);
				return 0;
				}
			//===== エラー処理 =====
			else
				{
				//===== テーブル構築オブジェクトの削除 =====
				M2MTableManager_delete(&tableBuilder);
				//===== カラム構造体オブジェクトの削除 =====
				M2MColumnList_delete(columnList);
				return 1;
				}
			}
		//===== エラー処理 =====
		else
			{
			//===== カラム構造体オブジェクトの削除 =====
			M2MColumnList_delete(columnList);
			return 1;
			}
		}
	//===== エラー処理 =====
	else
		{
		return 1;
		}
	}
*/


/* End Of File */
