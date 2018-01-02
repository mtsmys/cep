/*******************************************************************************
 * M2MCEPRecord.c
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

#include "m2m/cep/M2MCEPRecord.h"


/*******************************************************************************
 * 内部関数
 ******************************************************************************/
/**
 * @param[in,out] self	レコード管理オブジェクト
 */
static void this_deleteColumnName (M2MCEPRecord *self)
	{
	//===== フィールド名リストの存在を確認 =====
	if (M2MCEPRecord_getColumnName(self)!=NULL)
		{
		//===== ヒープメモリ領域を解放 =====
		M2MHeap_free(self->columnName);
		}
	//===== Error handling =====
	else
		{
		}
	return;
	}


/**
 * @param[in,out] self	レコード管理オブジェクト
 */
static void this_deleteNewRecordList (M2MCEPRecord *self)
	{
	//===== 新規レコード管理リストの存在を確認 =====
	if (M2MCEPRecord_getNewRecordList(self)!=NULL)
		{
		//===== ヒープメモリ領域を解放（構造体内のポインタなので解放可能） =====
		M2MList_delete(self->newRecordList);
		}
	//===== Error handling =====
	else
		{
		}
	return;
	}


/**
 * @param[in,out] self	レコード管理オブジェクト
 */
static void this_deleteOldRecordList (M2MCEPRecord *self)
	{
	//===== 過去のレコード管理リストの存在を確認 =====
	if (M2MCEPRecord_getOldRecordList(self)!=NULL)
		{
		//===== ヒープメモリ領域を解放（構造体内のポインタなので解放可能） =====
		M2MList_delete(self->oldRecordList);
		}
	//===== Error handling =====
	else
		{
		}
	return;
	}


/**
 * 引数で指定されたレコード管理オブジェクトが保有しているテーブル名文字列の<br>
 * メモリ領域を解放する。<br>
 *
 * @param[in,out] self	レコード管理オブジェクト
 */
static void this_deleteTableName (M2MCEPRecord *self)
	{
	//===== テーブル名文字列の存在を確認 =====
	if (M2MCEPRecord_getTableName(self)!=NULL)
		{
		//===== ヒープメモリ領域を解放（構造体内のポインタなので解放可能） =====
		M2MHeap_free(self->tableName);
		}
	//===== Error handling =====
	else
		{
		}
	return;
	}


/**
 * 引数で指定されたレコード管理オブジェクト（群）の中から, 指定されたテーブル名<br>
 * と一致するノードを取得して返す。<br>
 * 見つからなかった場合はNULLを返す。<br>
 *
 * @param[in] self			レコード管理オブジェクト
 * @param[in] tableName		ノード検索に利用するテーブル名文字列
 * @return					テーブル名が一致したレコード管理オブジェクト or NULL（エラーの場合）
 */
static M2MCEPRecord *this_detectM2MCEPRecord (M2MCEPRecord *self, const M2MString *tableName)
	{
	//========== Variable ==========
#ifdef DEBUG
	M2MString MESSAGE[256];
#endif // DEBUG
	const M2MString *METHOD_NAME = (M2MString *)"M2MCEPRecord.this_detectM2MCEPRecord()";

	//===== Check argument =====
	if (self!=NULL && tableName!=NULL)
		{
#ifdef DEBUG
		memset(MESSAGE, 0, sizeof(MESSAGE));
		snprintf(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"レコード管理オブジェクトの中に引数で指定されたテーブル名(=\"%s\")と一致するノードが存在するかどうか探索します", tableName);
		M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, MESSAGE);
#endif // DEBUG
		//===== レコード管理オブジェクトの先頭ノードを取得 =====
		if ((self=M2MCEPRecord_begin(self))!=NULL)
			{
			//===== 末端ノードに辿り着くまで繰り返し =====
			while (M2MCEPRecord_next(self)!=NULL)
				{
				//===== ノードが保有するテーブル名と一致した場合 =====
				if (M2MString_compareTo(M2MCEPRecord_getTableName(self), tableName)==0)
					{
#ifdef DEBUG
					memset(MESSAGE, 0, sizeof(MESSAGE));
					snprintf(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"レコード管理オブジェクトのテーブル名(=\"%s\")と引数で指定されたテーブル名)(=\"%s\")が一致しました", M2MCEPRecord_getTableName(self), tableName);
					M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, MESSAGE);
					M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, (M2MString *)"探索処理を終了します");
#endif // DEBUG
					return self;
					}
				//===== テーブル名が異なる場合 =====
				else
					{
#ifdef DEBUG
					memset(MESSAGE, 0, sizeof(MESSAGE));
					snprintf(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"レコード管理オブジェクトのテーブル名(=\"%s\")と引数で指定されたテーブル名(=\"%s\")は異なります", M2MCEPRecord_getTableName(self), tableName);
					M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, MESSAGE);
#endif // DEBUG
					//===== 次のノードへ移動 =====
					self = M2MCEPRecord_next(self);
					}
				}
			//===== 末端ノードが保有するテーブル名と一致した場合 =====
			if (M2MString_compareTo(M2MCEPRecord_getTableName(self), tableName)==0)
				{
#ifdef DEBUG
				memset(MESSAGE, 0, sizeof(MESSAGE));
				snprintf(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"レコード管理オブジェクトのテーブル名(=\"%s\")と引数で指定されたテーブル名)(=\"%s\")が一致しました", M2MCEPRecord_getTableName(self), tableName);
				M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, MESSAGE);
				M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, (M2MString *)"探索処理を終了します");
#endif // DEBUG
				return self;
				}
			//===== 末端ノードが保有するテーブル名が異なる場合 =====
			else
				{
#ifdef DEBUG
				memset(MESSAGE, 0, sizeof(MESSAGE));
				snprintf(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"レコード管理オブジェクトのテーブル名(=\"%s\")と引数で指定されたテーブル名(=\"%s\")は異なります", M2MCEPRecord_getTableName(self), tableName);
				M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, MESSAGE);
#endif // DEBUG
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"引数で指定された\"M2MCEPRecord *\"から取得した先頭ノードがNULLです", NULL);
			return NULL;
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"引数で指定された\"M2MCEPRecord *\"がNULLです", NULL);
		return NULL;
		}
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"引数で指定された\"tableName\"がNULLです", NULL);
		return NULL;
		}
	}


/**
 * @param[in] self	レコード管理オブジェクト
 * @return			レコード管理オブジェクトの末端ノード or NULL（エラーの場合）
 */
static M2MCEPRecord *this_end (M2MCEPRecord *self)
	{
	//===== Check argument =====
	if (self!=NULL)
		{
		//===== 末端ノードに辿り着くまで繰り返し =====
		while (M2MCEPRecord_next(self)!=NULL)
			{
			self = M2MCEPRecord_next(self);
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
 * @param[in] self
 */
static void this_init (M2MCEPRecord *self)
	{
	//========== Variable ==========
	M2MList *newRecordList = NULL;
	M2MList *oldRecordList = NULL;
	const M2MString *METHOD_NAME = (M2MString *)"M2MCEPRecord.this_init()";

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== メンバ変数の初期化 =====
		this_deleteTableName(self);
		this_deleteColumnName(self);
		this_deleteNewRecordList(self);
		this_deleteOldRecordList(self);
		//===== 新旧レコード情報オブジェクトの新規作成 =====
		if ((newRecordList=M2MList_new())!=NULL
				&& (oldRecordList=M2MList_new())!=NULL)
			{
			//===== 新規に挿入するレコード情報を格納するためのオブジェクトをセット =====
			self->newRecordList = newRecordList;
			//===== 過去に挿入したレコード情報を格納するためのオブジェクトをセット =====
			self->oldRecordList = oldRecordList;
			//===== 正常終了 =====
			return;
			}
		//===== Error handling =====
		else if (newRecordList==NULL)
			{
			M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"新規に挿入したレコード情報を格納するためのリスト構造オブジェクトの作成に失敗しました", NULL);
			return;
			}
		else
			{
			M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"過去に挿入したレコード情報を格納するためのリスト構造オブジェクトの作成に失敗しました", NULL);
			this_deleteNewRecordList(self);
			return;
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"引数で指定された\"M2MCEPRecord *\"がNULLです", NULL);
		return;
		}
	}


/**
 * @param[in] self
 * @return
 */
static unsigned int this_length (M2MCEPRecord *self)
	{
	//========== Variable ==========
	unsigned int length = 0;
	M2MString *tableName = NULL;
	const M2MString *METHOD_NAME = (M2MString *)"M2MCEPRecord.this_length()";

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== 先頭ノードの取得 =====
		if ((self=M2MCEPRecord_begin(self))!=NULL)
			{
			//===== テーブル名とCSV形式のカラム名の存在を確認 =====
			if ((tableName=M2MCEPRecord_getTableName(self))!=NULL
					&& M2MCEPRecord_getColumnName(self)!=NULL)
				{
				//=====  =====
				length++;
				}
			//===== Error handling =====
			else
				{
				}
			//=====  =====
			while (M2MCEPRecord_next(self)!=NULL)
				{
				//===== テーブル名とCSV形式のカラム名の存在を確認 =====
				if ((tableName=M2MCEPRecord_getTableName(self))!=NULL
						&& M2MCEPRecord_getColumnName(self)!=NULL)
					{
					length++;
					}
				//===== Error handling =====
				else
					{
					}
				}
			return length;
			}
		//===== Error handling =====
		else
			{
			M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"引数で指定されたレコード管理オブジェクトから先頭ノードの取得に失敗しました", NULL);
			return 0;
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"引数で指定されたレコード管理オブジェクトがNULLです", NULL);
		return 0;
		}
	}


/**
 * @param[in,out] self		レコード管理オブジェクト
 * @param[in] csv			CSV1行目のカラム名を記述した文字列
 * @param[in] lineLength	CSV1行目のカラム名を記述した文字列の長さ[バイト]
 * @return					カラム名文字列がセットされたレコード管理オブジェクト or NULL（エラーの場合）
 */
static M2MCEPRecord *this_setColumnName (M2MCEPRecord *self, const M2MString *csv, const size_t lineLength)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MCEPRecord.this_setColumnName()";

	//===== Check argument =====
	if (self!=NULL && csv!=NULL && lineLength>0 && lineLength<=M2MString_length(csv))
		{
		//===== 初期化（ヒープメモリ領域の解放） =====
		this_deleteColumnName(self);
		//===== ヒープメモリ領域の獲得 =====
		if ((self->columnName=(M2MString *)M2MHeap_malloc(lineLength+1))!=NULL)
			{
			//===== カラム名を示すCSV形式の文字列をコピー =====
			memcpy(self->columnName, csv, lineLength);
			return self;
			}
		//===== Error handling =====
		else
			{
			M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"CSV形式のカラム名を示す文字列をコピーするための, ヒープメモリ領域の獲得に失敗しました", NULL);
			return NULL;
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"引数で指定された\"M2MCEPRecord *\"がNULLです", NULL);
		return NULL;
		}
	else if (csv==NULL)
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"引数で指定された\"csv\"がNULLです", NULL);
		return NULL;
		}
	else if (lineLength<=0)
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"引数で指定された\"lineLength\"が0以下の整数です", NULL);
		return NULL;
		}
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"引数で指定された\"lineLength\"が\"csv\"の文字列数よりも大きい値です", NULL);
		return NULL;
		}
	}


/**
 * 引数で指定された新規レコード情報オブジェクトにCSV形式のレコードをセットする．<br>
 *
 * @param[in] newRecordList	新規レコード情報オブジェクト
 * @param[in] csv			CSV形式の文字列を示すレコード
 * @return					セットしたレコード数 or -1（エラーの場合)
 */
static int this_setCSVIntoNewRecordList (M2MList *newRecordList, const M2MString *csv)
	{
	//========== Variable ==========
	int numberOfRecord = 0;
	M2MString *lineHead = NULL;
	M2MString *lineEnd = NULL;
	size_t lineLength = 0;
#ifdef DEBUG
	M2MString MESSAGE[256];
#endif // DEBUG
	const unsigned int CRLF_LENGTH = M2MString_length(M2MString_CRLF);
	const M2MString *METHOD_NAME = (M2MString *)"M2MCEPRecord.this_setCSVIntoNewRecordList()";

	//===== Check argument =====
	if (newRecordList!=NULL && csv!=NULL && M2MString_length(csv)>0)
		{
		//===== 1行目のヘッダ行を無視する =====
		if ((lineEnd=M2MString_indexOf(csv, M2MString_CRLF))!=NULL
				&& (lineEnd+=CRLF_LENGTH)!=NULL)
			{
			//===== 先頭の位置を取得 =====
			lineHead = lineEnd;
			//===== 最終行に辿り着くまで繰り返し =====
			while ((lineEnd=M2MString_indexOf(lineHead, M2MString_CRLF))!=NULL)
				{
				//===== CSV1行のデータサイズ取得 =====
				if ((lineLength=M2MString_length(lineHead)-M2MString_length(lineEnd))>0)
					{
					//===== CSV1行を新規レコード情報オブジェクトへコピー =====
					if (M2MList_add(newRecordList, lineHead, lineLength)!=NULL)
						{
						//===== コピーしたレコード行数をインクリメント =====
						numberOfRecord++;
						}
					//===== Error handling =====
					else
						{
						M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"CSV形式のレコードをレコード情報オブジェクトへ格納する処理に失敗しました．．．処理を継続します", NULL);
						}
					}
				//===== Error handling =====
				else
					{
//					M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"CSV形式の文字列の文字列数が0[Byte]以下です．．．処理を継続します", NULL);
					}
				//===== 次の行の先頭へ移動 =====
				lineEnd += CRLF_LENGTH;
				lineHead = lineEnd;
				}
			//===== CSV最終行のコピー =====
			if ((lineLength=M2MString_length(lineHead))>0)
				{
				//===== CSV1行を新規レコード情報オブジェクトへコピー =====
				if (M2MList_add(newRecordList, lineHead, lineLength)!=NULL)
					{
					//===== コピーしたレコード行数をインクリメント =====
					numberOfRecord++;
#ifdef DEBUG
					memset(MESSAGE, 0, sizeof(MESSAGE)-1l);
					snprintf(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"引数で指定されたCSV形式の文字列からレコード情報を\"%u\"[件]取得しました", numberOfRecord);
					M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, MESSAGE);
#endif // DEBUG
					return numberOfRecord;
					}
				//===== Error handling =====
				else
					{
					M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"CSV形式の文字列をレコード情報オブジェクトへ格納する処理に失敗しました", NULL);
					//===== セットしたレコード数が1以上の場合 =====
					if (numberOfRecord>=1)
						{
						return numberOfRecord;
						}
					//===== セットしたレコード数が0以下の場合 =====
					else
						{
						return -1;
						}
					}
				}
			//===== Error handling =====
			else
				{
//				M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"CSV形式の文字列の文字列数が0[Byte]以下です", NULL);
				//===== セットしたレコード数が1以上の場合 =====
				if (numberOfRecord>=1)
					{
					return numberOfRecord;
					}
				//===== セットしたレコード数が0以下の場合 =====
				else
					{
					return -1;
					}
				}
			}
		//===== データ行が存在しない場合 =====
		else
			{
			M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"引数で指定されたCSV形式の文字列は1行目のヘッダ行しか存在しない不正なデータです", NULL);
			return -1;
			}
		}
	//===== Argument error =====
	else if (newRecordList==NULL)
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"引数で指定された新規挿入用のレコード情報オブジェクトがNULLです", NULL);
		return -1;
		}
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"引数で指定されたCSV形式の文字列がNULL, もしくは空白です", NULL);
		return -1;
		}
	}


/**
 * 引数で指定されたレコード管理オブジェクトにテーブル名文字列をセットする。<br>
 *
 * @param[in,out] self		レコード管理オブジェクト
 * @param[in] tableName		テーブル名を示す文字列
 * @return					テーブル名のセット（もしくは更新）に成功したレコード管理オブジェクト
 */
static M2MCEPRecord *this_setTableName (M2MCEPRecord *self, const M2MString *tableName)
	{
	//========== Variable ==========
	unsigned int tableNameLength = 0;

	//===== Check argument =====
	if (self!=NULL && tableName!=NULL && (tableNameLength=M2MString_length(tableName))>0)
		{
		//===== テーブル名の初期化 =====
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
			return NULL;
			}
		}
	//===== Argument error =====
	else
		{
		return NULL;
		}
	}



/*******************************************************************************
 * Public function
 ******************************************************************************/
/**
 * 引数で指定されたレコード管理オブジェクトから先頭のレコード管理オブジェクト<br>
 * を取得して返す．<br>
 * 【注意】<br>
 * 先頭のレコード管理オブジェクトは1つ前のポインタが自分と同じアドレスを示す．<br>
 *
 * @param[in] self	レコード管理オブジェクト
 * @return			先頭のレコード管理オブジェクト or NULL（エラーの場合）
 */
M2MCEPRecord *M2MCEPRecord_begin (M2MCEPRecord *self)
	{
	//===== Check argument =====
	if (self!=NULL)
		{
		//===== 先頭ノードに辿り着くまで繰り返し =====
		while (self!=NULL && M2MCEPRecord_previous(self)!=self)
			{
			//===== 1つ前のノードへ移動 =====
			self = M2MCEPRecord_previous(self);
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
 * 引数で指定されたレコード管理オブジェクトのメモリ領域を全て解放する．<br>
 * <br>
 * 【注意】<br>
 * レコード管理オブジェクトはメンバ変数として, リンク構造となっているポインタ<br>
 * を保持している。そのため, ヒープメモリの解放の際, ポインタのポインタとする必要<br>
 * は無く, 内部で保持しているポインタを通して正しいアドレスを取得する事が出来る。<br>
 *
 * @param[in,out] self	メモリ領域解放対象のレコード管理オブジェクト
 */
void M2MCEPRecord_delete (M2MCEPRecord **self)
	{
	//========== Variable ==========
	M2MCEPRecord *next = NULL;
	const M2MString *METHOD_NAME = (M2MString *)"M2MCEPRecord_delete()";

	//===== Check argument =====
	if (self!=NULL && (*self)!=NULL)
		{
		//===== 先頭のM2MList構造体オブジェクトの取得 =====
		if (((*self)=M2MCEPRecord_begin((*self)))!=NULL)
			{
			//===== 後方のM2MList構造体オブジェクトの有無を確認 =====
			while ((*self)!=NULL && (next=M2MCEPRecord_next((*self)))!=NULL)
				{
				//===== M2MList構造体オブジェクトをリンクから外す =====
				M2MCEPRecord_setNextRecord((*self), NULL);
				M2MCEPRecord_setPreviousRecord(next, next);
				//===== M2MList構造体オブジェクトのメモリ領域を解放 =====
				this_deleteTableName((*self));
				this_deleteColumnName((*self));
				this_deleteNewRecordList((*self));
				this_deleteOldRecordList((*self));
				M2MHeap_free((*self));
				//===== 次のM2MList構造体オブジェクトへ移る =====
				(*self) = next;
				}
			//===== M2MList構造体オブジェクトのメモリ領域を解放 =====
			if ((*self)!=NULL)
				{
				this_deleteTableName((*self));
				this_deleteColumnName((*self));
				this_deleteNewRecordList((*self));
				this_deleteOldRecordList((*self));
				M2MHeap_free((*self));
				}
			else
				{
				M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"内部エラー！引数で指定された\"M2MCEPRecord *\"のノードの中にNULLが混入しています", NULL);
				}
			}
		//===== Error handling =====
		else
			{
			M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"引数で指定された\"M2MCEPRecord *\"の先頭ノードの取得に失敗しました", NULL);
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"引数で指定された\"M2MCEPRecord *\"がNULLです", NULL);
		}
	return;
	}


/**
 * 引数で指定されたレコード管理オブジェクトがメンバ変数として保持するカラム名<br>
 * を示すCSV形式の文字列を返す．<br>
 *
 * @param[in] self		レコード管理オブジェクト
 * @return				引数で指定されたレコード管理オブジェクトが保持するカラム名を示すCSV形式の文字列 or NULL（エラーの場合）
 */
M2MString *M2MCEPRecord_getColumnName (const M2MCEPRecord *self)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MCEPRecord_getColumnName()";

	//===== Check argument =====
	if (self!=NULL)
		{
		return self->columnName;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"レコード管理オブジェクトが保持するCSV形式のカラム名がNULLです", NULL);
		return NULL;
		}
	}


/**
 * 引数で指定されたレコード管理オブジェクトがメンバ変数として保持する, 新規に<br>
 * 挿入されたレコードリストを返す．<br>
 *
 * @param[in] self		レコード管理オブジェクト
 * @return				引数で指定されたレコード管理オブジェクトが保持する, 新規に挿入された（未だメモリーDB未挿入の）レコードリスト
 */
M2MList *M2MCEPRecord_getNewRecordList (const M2MCEPRecord *self)
	{
	if (self!=NULL)
		{
		return self->newRecordList;
		}
	else
		{
		return NULL;
		}
	}


/**
 * 引数で指定されたレコード管理オブジェクトがメンバ変数として保持する, 過去に<br>
 * 挿入されたレコードリストを返す．<br>
 *
 * @param[in] self		レコード管理オブジェクト
 * @return				引数で指定されたレコード管理オブジェクトが保持する, 過去に挿入された（メモリーDB挿入済みの）レコードリスト
 */
M2MList *M2MCEPRecord_getOldRecordList (const M2MCEPRecord *self)
	{
	//===== Check argument =====
	if (self!=NULL)
		{
		return self->oldRecordList;
		}
	//===== Argument error =====
	else
		{
		return NULL;
		}
	}


/**
 * 引数で指定されたレコード管理オブジェクトがメンバ変数として保持するテーブル名<br>
 * を返す．<br>
 *
 * @param[in] self		レコード管理オブジェクト
 * @return				引数で指定されたレコード管理オブジェクトが保持するテーブル名
 */
M2MString *M2MCEPRecord_getTableName (const M2MCEPRecord *self)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MCEPRecord_getTableName()";

	//===== Check argument =====
	if (self!=NULL)
		{
		return self->tableName;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"レコード管理オブジェクトが保持するテーブル名がNULLです", NULL);
		return NULL;
		}
	}


/**
 * 引数で指定されたレコード管理オブジェクトの“新規に挿入したレコード情報”<br>
 * 構造体オブジェクトのレコード情報を, “過去に挿入されたレコード情報”構造体<br>
 * オブジェクトへコピーする。<br>
 * この操作は, メモリ上のSQLite3データベースへレコードを挿入した後, ファイル上の<br>
 * SQLite3データベースへ永続化のためデータを整理するために実行する。<br>
 *
 * @param[in,out] self
 */
void M2MCEPRecord_moveFromNewRecordListToOldRecordList (M2MCEPRecord *self)
	{
	//========== Variable ==========
	M2MList *newRecordList = NULL;
	M2MList *oldRecordList = NULL;
	void *value = NULL;
	size_t valueLength = 0;
	unsigned int counter = 0;
#ifdef DEBUG
	M2MString MESSAGE[256];
#endif // DEBUG
	const M2MString *METHOD_NAME = (M2MString *)"M2MCEPRecord_moveFromNewRecordListToOldRecordList()";

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== 新旧レコード情報オブジェクトの取得 =====
		if ((newRecordList=M2MCEPRecord_getNewRecordList(self))!=NULL
				&& (oldRecordList=M2MCEPRecord_getOldRecordList(self))!=NULL)
			{
			//===== 新規レコード情報オブジェクトの先頭ノードを取得 =====
			if ((newRecordList=M2MList_begin(newRecordList))!=NULL)
				{
				//===== 末端のノードに辿り着くまで繰り返し =====
				while (newRecordList!=NULL && M2MList_next(newRecordList)!=NULL)
					{
					//===== 新規レコード情報オブジェクトのレコードの存在を確認 =====
					if ((value=M2MList_getValue(newRecordList))!=NULL
							&& (valueLength=M2MList_getValueLength(newRecordList))>0)
						{
						//===== 新規レコード情報オブジェクトのレコードを過去のレコード情報オブジェクトへコピー =====
						if (M2MList_add(oldRecordList, value, valueLength)!=NULL)
							{
							counter++;
							}
						//===== Error handling =====
						else
							{
							M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"新規レコード情報オブジェクトを過去のレコード情報オブジェクトへコピーする処理でエラーが発生しました", NULL);
							}
						}
					//===== Error handling =====
					else
						{
						M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"新規レコード情報オブジェクトのレコード値がNULL, もしくはレコードサイズが0[Byte]以下です", NULL);
						}
					//===== 新規レコード情報オブジェクトから移動済みレコードを削除して次のノードへ移動 =====
					newRecordList = M2MList_remove(newRecordList);
					}
				//===== 新規レコード情報オブジェクトのレコードの存在を確認 =====
				if ((value=M2MList_getValue(newRecordList))!=NULL
						&& (valueLength=M2MList_getValueLength(newRecordList))>0)
					{
					//===== 末端ノードのレコードを過去のレコード情報オブジェクトへコピー =====
					if (M2MList_add(oldRecordList, value, valueLength)!=NULL)
						{
						counter++;
						}
					//===== Error handling =====
					else
						{
						M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"新規レコード情報オブジェクトを過去のレコード情報オブジェクトへコピーする処理でエラーが発生しました", NULL);
						}
					}
				//===== Error handling =====
				else
					{
					M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"新規レコード情報オブジェクトのレコード値がNULL, もしくはレコードサイズが0[Byte]以下です", NULL);
					}
				//===== 新規レコード情報オブジェクトから移動済みレコードを削除 =====
				self->newRecordList = M2MList_remove(newRecordList);
#ifdef DEBUG
				memset(MESSAGE, 0, sizeof(MESSAGE));
				snprintf(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"\"%s\"テーブルの新規レコード情報オブジェクトから過去のレコード情報オブジェクトへ\"%u\"個のレコードを移動しました", M2MCEPRecord_getTableName(self), counter);
				M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, MESSAGE);
#endif // DEBUG
				return;
				}
			//===== Error handling =====
			else
				{
				M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"引数で指定された\"M2MCEPRecord *\"から取得した\"newRecordList\"の先頭ノードがNULLです", NULL);
				return;
				}
			}
		//===== Error handling =====
		else if (newRecordList==NULL)
			{
			M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"引数で指定された\"M2MCEPRecord *\"から取得した\"newRecordList\"がNULLです", NULL);
			return;
			}
		else
			{
			M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"引数で指定された\"M2MCEPRecord *\"から取得した\"oldRecordList\"がNULLです", NULL);
			return;
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"引数で指定された\"M2MCEPRecord *\"がNULLです", NULL);
		return;
		}
	}


/**
 * ヒープメモリ領域を新規に獲得し, レコード管理オブジェクトを生成する．<br>
 * このレコード管理オブジェクトの前方ノードは自分自身のポインタ, 後方ノードは<br>
 * NULLに初期化されている点に留意する事。<br>
 *
 * @return	新規に生成したレコード管理オブジェクト
 */
M2MCEPRecord *M2MCEPRecord_new ()
	{
	//========== Variable ==========
	M2MCEPRecord *self = NULL;
	const M2MString *METHOD_NAME = (M2MString *)"M2MCEPRecord_new()";

	//===== ヒープメモリ領域の獲得 =====
	if ((self=(M2MCEPRecord *)M2MHeap_malloc(sizeof(M2MCEPRecord)))!=NULL)
		{
		if ((self->newRecordList=M2MList_new())!=NULL)
			{
			if ((self->oldRecordList=M2MList_new())!=NULL)
				{
				//===== メンバ変数の初期化 =====
				M2MCEPRecord_setPreviousRecord(self, self);
				M2MCEPRecord_setNextRecord(self, NULL);
#ifdef DEBUG
				M2MLogger_printDebugMessage(METHOD_NAME, __LINE__, (M2MString *)"CEPレコード管理オブジェクトを新規作成しました");
#endif // DEBUG
				return self;
				}
			//===== Error handling =====
			else
				{
				M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"挿入済みレコード情報を管理するオブジェクトの作成に失敗しました", NULL);
				this_deleteNewRecordList(self);
				this_deleteColumnName(self);
				M2MHeap_free(self);
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"新規レコード情報を管理するオブジェクトの作成に失敗しました", NULL);
			this_deleteColumnName(self);
			M2MHeap_free(self);
			return NULL;
			}
		}
	//===== Error handling =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"レコード管理オブジェクトの作成に失敗しました", NULL);
		return NULL;
		}
	}


/**
 * 引数で指定されたレコード管理オブジェクトがメンバ変数として保持する, 1つ後ろの<br>
 * レコード管理オブジェクトを返す．<br>
 *
 * @param[in] self		レコード管理オブジェクト
 * @return				引数で指定されたレコード管理オブジェクトが保持する, 1つ後ろのレコード管理オブジェクト
 */
M2MCEPRecord *M2MCEPRecord_next (const M2MCEPRecord *self)
	{
	if (self!=NULL)
		{
		return self->next;
		}
	else
		{
		return NULL;
		}
	}


/**
 * 引数で指定されたレコード管理オブジェクトがメンバ変数として保持する, 1つ手前の<br>
 * レコード管理オブジェクトを返す．<br>
 *
 * @param[in] self		レコード管理オブジェクト
 * @return				引数で指定されたレコード管理オブジェクトが保持する, 1つ手前のレコード管理オブジェクト
 */
M2MCEPRecord *M2MCEPRecord_previous (const M2MCEPRecord *self)
	{
	if (self!=NULL)
		{
		return self->previous;
		}
	else
		{
		return NULL;
		}
	}


/**
 * 引数で指定されたテーブル名に該当するレコード管理オブジェクトだけ, メモリ領域<br>
 * を解放する（ポインタで接続された他のレコード管理オブジェクトは存続する）．<br>
 *
 * @param[in,out] self
 * @param[in] tableName
 */
void M2MCEPRecord_remove (M2MCEPRecord *self, const M2MString *tableName)
	{
	//========== Variable ==========
	M2MCEPRecord *record = NULL;
	M2MCEPRecord *previous = NULL;
	M2MCEPRecord *next = NULL;

	//===== Check argument =====
	if (self!=NULL && tableName!=NULL)
		{
		//===== レコード管理オブジェクトノードを取得 =====
		if ((record=this_detectM2MCEPRecord(self, tableName))!=NULL)
			{
			//===== メンバ変数のヒープメモリ領域を解放 =====
			this_deleteTableName(record);
			this_deleteColumnName(record);
			this_deleteNewRecordList(record);
			this_deleteOldRecordList(record);
			//===== 指定のレコード管理オブジェクトの前方／後方ノードの取得 =====
			previous = M2MCEPRecord_previous(record);
			next = M2MCEPRecord_next(record);
			//===== 指定のレコード管理オブジェクトが先頭ノードの場合 =====
			if (previous==record)
				{
				//===== 指定のレコード管理オブジェクトに後続ノードが存在する場合 =====
				if (next!=NULL)
					{
					//===== ノード間の連結を切り離し =====
					M2MCEPRecord_setPreviousRecord(next, next);
					//===== ノードのヒープメモリ領域を解放 =====
					M2MHeap_free(record);
					}
				//===== 指定のレコード管理オブジェクトに後続ノードが存在しない（1つしか存在しない）場合 =====
				else
					{
					// 何もしない
					}
				}
			//===== 指定のレコード管理オブジェクトが末端ノードの場合（但し, 前方ノードは必ず存在する） =====
			else if (next==NULL)
				{
				//===== ノード間の連結を切り離し =====
				M2MCEPRecord_setNextRecord(previous, NULL);
				//===== ノードのヒープメモリ領域を解放 =====
				M2MHeap_free(record);
				}
			//===== 指定のレコード管理オブジェクトの前方／後方ノードが存在する場合 =====
			else
				{
				//===== ノード間の連結を切り離し =====
				M2MCEPRecord_setNextRecord(previous, next);
				M2MCEPRecord_setPreviousRecord(next, previous);
				//===== ノードのヒープメモリ領域を解放 =====
				M2MHeap_free(record);
				}
			return;
			}
		//===== Error handling =====
		else
			{
			return;
			}
		}
	//===== Argument error =====
	else
		{
		return;
		}
	}


/**
 * 引数で指定されたテーブル名をメンバ変数に持つレコード管理オブジェクトに対し, <br>
 * CSV形式の文字列データをリスト構造体オブジェクトに格納する．<br>
 * テーブル名をメンバ変数に持つレコード管理オブジェクトが存在しなかった場合, <br>
 * レコード管理オブジェクトを新たに生成し, CSV形式の文字列データをセットして<br>
 * 引数で指定されたレコード管理オブジェクトに追加する。<br>
 * <br>
 * CSV形式の文字列は以下の仕様に従っている事。<br>
 * ・文字コード：UTF-8<br>
 * ・改行コード："\r\n"<br>
 * ・1行目にカラム名を示すヘッダ, 2行目以降がデータとなる<br>
 * <br>
 * [引数にセットするCSV形式の文字列例]<br>
 * date,temperature,humidity\r\n	← ヘッダ行<br>
 * 1395984160,23.8,46\r\n			← レコード1行目<br>
 * 1395984254,24.0,45\r\n			← レコード2行目<br>
 * ・・・<br>
 *
 * @param[in,out] self		レコード管理オブジェクト
 * @param[in] tableName		テーブル名を示す文字列
 * @param[in] csv			CSV形式の文字列データ
 * @return					オブジェクトに格納したレコード数[個] or -1（エラーの場合）
 */
int M2MCEPRecord_setCSV (M2MCEPRecord *self, const M2MString *tableName, const M2MString *csv)
	{
	//========== メンバ変数 ==========
	M2MCEPRecord *record = NULL;
	M2MList *newRecordList = NULL;
	M2MString *lineEnd = NULL;
	size_t lineLength = 0;
	int numberOfRecord = -1;

	//===== Check argument =====
	if (self!=NULL
			&& tableName!=NULL && M2MString_length(tableName)>0
			&& csv!=NULL && M2MString_length(csv)>0)
		{
		//===== テーブル名が既にセットされていた場合（既存ノードが存在する場合） =====
		if ((record=this_detectM2MCEPRecord(self, tableName))!=NULL)
			{
			//===== 新規レコード情報管理オブジェクトを取得 =====
			if ((newRecordList=M2MCEPRecord_getNewRecordList(record))!=NULL)
				{
				//===== 新規レコード情報管理オブジェクトにCSV形式のレコードをセット =====
				return this_setCSVIntoNewRecordList(newRecordList, csv);
				}
			//===== Error handling =====
			else
				{
				M2MLogger_printErrorMessage((M2MString *)"M2MCEPRecord_setCSV()", __LINE__, (M2MString *)"引数で指定されたレコード管理オブジェクトから新規レコード情報リストが取得出来ません", NULL);
				return -1;
				}
			}
		//===== 指定されたテーブル名を持つノードが存在しなかった場合 =====
		else
			{
			//===== レコード管理オブジェクトに値を持つノードが1つも存在していない場合 =====
			if (this_length(self)==0)
				{
				//===== レコード管理オブジェクトの1つ目のノードを設定（1つ目のノードは"必ず存在する"事に留意) =====
				if (this_setTableName(self, tableName)!=NULL
						&& (newRecordList=M2MCEPRecord_getNewRecordList(self))!=NULL)
					{
					//===== テーブルのカラム名をレコード管理オブジェクトにセット =====
					if ((lineEnd=M2MString_indexOf(csv, M2MString_CRLF))!=NULL
							&& (lineLength=M2MString_length(csv)-M2MString_length(lineEnd))>0
							&& this_setColumnName(self, (M2MString *)csv, lineLength)!=NULL)
						{
						//===== 新規レコード情報管理オブジェクトにCSV形式のレコードをセット =====
						if ((numberOfRecord=this_setCSVIntoNewRecordList(newRecordList, csv))>=1)
							{
							return numberOfRecord;
							}
						//===== Error handling =====
						else
							{
							M2MLogger_printErrorMessage((M2MString *)"M2MCEPRecord_setCSV()", __LINE__, (M2MString *)"CSV形式のレコードをレコード情報オブジェクトセットするのに失敗しました", NULL);
							//===== レコード管理オブジェクトのノードを初期化（テーブル名と新規レコード情報オブジェクトを削除) =====
							this_init(self);
							return -1;
							}
						}
					//===== Error handling =====
					else
						{
						M2MLogger_printErrorMessage((M2MString *)"M2MCEPRecord_setCSV()", __LINE__, (M2MString *)"引数で指定されたCSV形式の文字列の1行目のヘッダ行取得に失敗しました", NULL);
						//===== 生成したレコード管理オブジェクトを削除 =====
						this_init(self);
						return -1;
						}
					}
				//===== Error handling =====
				else
					{
					M2MLogger_printErrorMessage((M2MString *)"M2MCEPRecord_setCSV()", __LINE__, (M2MString *)"新規レコード情報オブジェクトの作成に失敗しました", NULL);
					//===== 生成したレコード管理オブジェクトを削除 =====
					this_init(self);
					return -1;
					}
				}
			//===== レコード管理オブジェクトにノードが1つ以上存在している場合 =====
			else
				{
				//===== レコード管理オブジェクトのノードを新規生成 =====
				if ((self=this_end(self))!=NULL
						&& (record=M2MCEPRecord_new())!=NULL
						&& this_setTableName(record, tableName)!=NULL
						&& (newRecordList=M2MCEPRecord_getNewRecordList(record))!=NULL)
					{
					//===== テーブルのカラム名をレコード管理オブジェクトにセット =====
					if ((lineEnd=M2MString_indexOf(csv, M2MString_CRLF))!=NULL
							&& (lineLength=M2MString_length(csv)-M2MString_length(lineEnd))>0
							&& this_setColumnName(record, (M2MString *)csv, lineLength)!=NULL)
						{
						//===== 新規レコード情報管理オブジェクトにCSV形式のレコードをセット =====
						if ((numberOfRecord=this_setCSVIntoNewRecordList(newRecordList, csv))>0)
							{
							//===== 既存のレコード管理オブジェクトと連結 =====
							M2MCEPRecord_setNextRecord(self, record);
							M2MCEPRecord_setPreviousRecord(record, self);
							//===== セットしたレコード数を返す =====
							return numberOfRecord;
							}
						//===== Error handling =====
						else
							{
							M2MLogger_printErrorMessage((M2MString *)"M2MCEPRecord_setCSV()", __LINE__, (M2MString *)"レコード管理オブジェクトへCSV形式のレコードをセットするのに失敗しました", NULL);
							//===== 生成したレコード管理オブジェクトを削除 =====
							M2MCEPRecord_delete(&record);
							return -1;
							}
						}
					//===== Error handling =====
					else
						{
						M2MLogger_printErrorMessage((M2MString *)"M2MCEPRecord_setCSV()", __LINE__, (M2MString *)"レコード管理オブジェクトにカラム名をセットするのに失敗しました", NULL);
						//===== 生成したレコード管理オブジェクトを削除 =====
						M2MCEPRecord_delete(&record);
						return -1;
						}
					}
				//===== Error handling =====
				else
					{
					M2MLogger_printErrorMessage((M2MString *)"M2MCEPRecord_setCSV()", __LINE__, (M2MString *)"新規レコード情報リストの作成処理に失敗しました", NULL);
					//===== 生成したレコード管理オブジェクトを削除 =====
					M2MCEPRecord_delete(&record);
					return -1;
					}
				}
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MCEPRecord_setCSV()", __LINE__, (M2MString *)"引数で指定されたレコード管理オブジェクトがNULLです", NULL);
		return -1;
		}
	else if (tableName==NULL)
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MCEPRecord_setCSV()", __LINE__, (M2MString *)"引数で指定されたテーブル名を示す文字列がNULLです", NULL);
		return -1;
		}
	else
		{
		M2MLogger_printErrorMessage((M2MString *)"M2MCEPRecord_setCSV()", __LINE__, (M2MString *)"引数で指定されたCSV形式のレコードを示す文字列がNULLです", NULL);
		return -1;
		}
	}


/**
 * 引数で指定されたレコード管理オブジェクトに対し, 1つ後ろのレコード管理<br>
 * オブジェクトをセットする．<br>
 *
 * @param[in] self			レコード管理オブジェクト
 * @param[in] nextRecord	1つ後ろのレコード管理オブジェクト
 */
void M2MCEPRecord_setNextRecord (M2MCEPRecord *self, M2MCEPRecord *nextRecord)
	{
	if (self!=NULL)
		{
		self->next = nextRecord;
		}
	else
		{
		}
	return;
	}


/**
 * 引数で指定されたレコード管理オブジェクトに対し, 1つ手前のレコード管理<br>
 * オブジェクトをセットする．<br>
 *
 * @param[in,out] self			レコード管理オブジェクト
 * @param[in] previousRedord	1つ手前のレコード管理オブジェクト
 */
void M2MCEPRecord_setPreviousRecord (M2MCEPRecord *self, M2MCEPRecord *previousRecord)
	{
	if (self!=NULL)
		{
		self->previous = previousRecord;
		}
	else
		{
		}
	return;
	}


/**
 * 引数で指定されたレコード管理オブジェクトの要素数[個]を返す。<br>
 *
 * @param[in] self	レコード管理オブジェクト
 * @return			レコード管理オブジェクトの要素数[個]
 */
unsigned int M2MCEPRecord_size (M2MCEPRecord *self)
	{
	//========== Variable ==========
	unsigned int size = 0;

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== 先頭ノードの取得 =====
		if ((self=M2MCEPRecord_begin(self))!=NULL)
			{
			size++;
			//===== 末端ノードに辿り着くまで繰り返し =====
			while ((self=M2MCEPRecord_next(self))!=NULL)
				{
				size++;
				}
			//===== 要素数を返す =====
			return size;
			}
		//===== Error handling =====
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



/* End Of File */
