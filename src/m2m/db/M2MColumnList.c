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

#include "m2m/db/M2MColumnList.h"


/*******************************************************************************
 * Private function
 ******************************************************************************/
/**
 *
 * @param[in] self	Column information list object
 */
static void this_deleteColumn (M2MColumnList *self)
	{
	//========== Variable ==========
	M2MColumn *column = NULL;
	const M2MString *METHOD_NAME = (M2MString *)"M2MColumnList.this_deleteColumn()";

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Get column information structure =====
		if ((column=M2MColumnList_getColumn(self))!=NULL)
			{
			//===== Delete =====
			M2MColumn_delete(&column);
			}
		//===== In the case of having no column structure =====
		else
			{
			// do nothing
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MColumnList\" object is NULL", NULL);
		}
	return;
	}


/**
 * Search and detect a list object matching column name from the column list.<br>
 *
 * @param[in,out] self			Column information list object
 * @param[in] columnName		String indicating column name
 * @param[in] columnNameLength	Length of column name string[Byte]
 * @return						Column information list object matching the column name
 */
static M2MColumnList *this_detect (M2MColumnList *self, const M2MString *columnName, const size_t columnNameLength)
	{
	//========== Variable ==========
	M2MColumn *column = NULL;
	const M2MString *METHOD_NAME = (M2MString *)"M2MColumnList.this_detect()";

	//===== Check argument =====
	if (self!=NULL
			&& columnName!=NULL
			&& columnNameLength<=M2MString_length(columnName))
		{
		//===== Get begin node =====
		if ((self=(M2MColumnList_begin(self)))!=NULL)
			{
			//===== Loop until getting at the end node =====
			while (M2MColumnList_next(self)!=NULL)
				{
				//===== Check the name of column =====
				if ((column=M2MColumnList_getColumn(self))!=NULL
						&& M2MString_equals(M2MColumn_getName(column), columnName, columnNameLength)==true)
					{
					return self;
					}
				//===== Error handling =====
				else if (column==NULL)
					{
					M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"The medium node of \"M2MColumnList\" object is NULL", NULL);
					return NULL;
					}
				//===== Move to next node =====
				else
					{
					self = M2MColumnList_next(self);
					}
				}
			//===== Check the name of column =====
			if ((column=M2MColumnList_getColumn(self))!=NULL
					&& M2MString_equals(M2MColumn_getName(column), columnName, columnNameLength)==true)
				{
				return self;
				}
			//===== Error handling =====
			else if (column==NULL)
				{
				M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"The end node of \"M2MColumnList\" object is NULL", NULL);
				return NULL;
				}
			//===== In case of not existing a node matching the name =====
			else
				{
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"The begin node of \"M2MColumnList\" object is NULL", NULL);
			return NULL;
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MColumnList\" object is NULL", NULL);
		return NULL;
		}
	else if (columnName==NULL)
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"columnName\" string is NULL", NULL);
		return NULL;
		}
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"columnNameLength\" is longer than the length of \"columnName\" string", NULL);
		return NULL;
		}
	}


/**
 *
 * @param[in,out] self	Column information list object
 * @param[in] column
 * @return
 */
static M2MColumnList *this_setColumn (M2MColumnList *self, M2MColumn *column)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MColumnList.this_setColumn()";

	//===== Check argument =====
	if (self!=NULL && column!=NULL)
		{
		this_deleteColumn(self);
		self->column = column;
		return self;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MColumnList\" object is NULL", NULL);
		return NULL;
		}
	}


/**
 * @param[in,out] self	Column information list object
 * @param[in] next
 * @return
 */
static M2MColumnList *this_setNext (M2MColumnList *self, M2MColumnList *next)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MColumnList.this_setNext()";

	//===== Check argument =====
	if (self!=NULL)
		{
		self->next = next;
		return self;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MColumnList\" object is NULL", NULL);
		return NULL;
		}
	}


/**
 * @param[in,out] self	Column information list object
 * @param[in] previous
 * @return
 */
static M2MColumnList *this_setPrevious (M2MColumnList *self, M2MColumnList *previous)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MColumnList.this_setPrevious()";

	//===== Check argument =====
	if (self!=NULL)
		{
		self->previous = previous;
		return self;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MColumnList\" object is NULL", NULL);
		return NULL;
		}
	}


/*******************************************************************************
 * Public function
 ******************************************************************************/
/**
 * Add a new node to the link of the column information list object.<br>
 *
 * @param[in,out] self		Column information list object
 * @param[in] columnName	String indicating column name
 * @param[in] dataType		Data type of the column
 * @param[in] primaryKey	Flag for primary key activation
 * @param[in] autoIncrement	Flag for auto increment enable
 * @param[in] allowNULL		Flag for NULL activation
 * @param[in] unique		Flag of uniqueness validation
 * @return					Newly added information list object or NULL (in case of error)
 */
M2MColumnList *M2MColumnList_add (M2MColumnList *self, const M2MString *columnName, const M2MDataType dataType, const bool primaryKey, const bool autoIncrement, const bool allowNULL, const bool unique)
	{
	//========== Variable ==========
	M2MColumnList *detectedNode = NULL;
	M2MColumnList *newNode = NULL;
	M2MColumn *column = NULL;
	const M2MString *METHOD_NAME = (M2MString *)"M2MColumnList_add()";

	//===== Check argument =====
	if (self!=NULL && columnName!=NULL && M2MString_length(columnName)>0)
		{
		//===== In case of same column is existing =====
		if ((detectedNode=this_detect(self, columnName, M2MString_length(columnName)))!=NULL)
			{
			return detectedNode;
			}
		//===== In case of adding a newly column node =====
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
					//===== Error handling =====
					else
						{
						M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to set the column information specified in the argument to the column information list object", NULL);
						M2MColumn_delete(&column);
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
					//===== Error handling =====
					else
						{
						M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to set the column information specified in the argument to the column information list object", NULL);
						M2MColumn_delete(&column);
						M2MColumnList_delete(newNode);
						return NULL;
						}
					}
				//===== Error handling =====
				else if (self==NULL)
					{
					M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to get end node from \"M2 MColumnList\" object specified by argument", NULL);
					return NULL;
					}
				else if (newNode==NULL)
					{
					M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to get heap memory to create a new column list object", NULL);
					return NULL;
					}
				else
					{
					M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to get heap memory to create a new column information object", NULL);
					M2MColumnList_delete(newNode);
					return NULL;
					}
				}
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MColumnList\" object is NULL", NULL);
		return NULL;
		}
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"columnName\" string is NULL or vacant", NULL);
		return NULL;
		}
	}


/**
 * Get the node located one column ahead of the link of the column list object.<br>
 *
 * @param[in] self	Column information list object
 * @return			Column information list object which is first node of link or NULL (in case of error)
 */
M2MColumnList *M2MColumnList_begin (M2MColumnList *self)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MColumnList_begin()";

	//===== Check argument =====
	if (self!=NULL)
		{
		while (M2MColumnList_previous(self)!=self)
			{
			self = M2MColumnList_previous(self);
			}
		return self;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MColumnList\" object is NULL", NULL);
		return NULL;
		}
	}


/**
 * Release the memory of all nodes of the link of the column list object.<br>
 *
 * @param[in,out] self	Column information list object
 */
void M2MColumnList_delete (M2MColumnList *self)
	{
	//========== Variable ==========
	M2MColumnList *next = NULL;
	const M2MString *METHOD_NAME = (M2MString *)"M2MColumnList_delete()";

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Get the first node =====
		if ((self=M2MColumnList_begin(self))!=NULL)
			{
			//===== Repeat until reaching the end node =====
			while (M2MColumnList_next(self)!=NULL)
				{
				//===== Temporarily get the pointer of the node behind =====
				next = M2MColumnList_next(self);
				//===== Detach node to be deleted from link =====
				this_setNext(self, NULL);
				this_setPrevious(next, next);
				//===== Release heap memory of column structure object =====
				this_deleteColumn(self);
				M2MHeap_free(self);
				//===== Move to next node =====
				self = next;
				}
			//===== Confirm existence of end node =====
			if (self!=NULL)
				{
				//===== Release heap memory of column structure object =====
				this_deleteColumn(self);
				M2MHeap_free(self);
				}
			//===== Error handling =====
			else
				{
				M2MLogger_printErrorMessage((M2MString *)"M2MColumnList_delete()", __LINE__, (M2MString *)"End node of column list object link is NULL", NULL);
				}
			}
		//===== Error handling =====
		else
			{
			M2MLogger_printErrorMessage((M2MString *)"M2MColumnList_delete()", __LINE__, (M2MString *)"Failed to get first node of the column list object", NULL);
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MColumnList\" object is NULL", NULL);
		}
	return;
	}


/**
 * 引数で指定されたカラム構造体オブジェクトのリンクの末端に位置するノードを取得する。<br>
 *
 * @param[in,out] self	Column information list object
 * @return				カラム構造体オブジェクトのリンクの末端ノード or NULL（エラーの場合）
 */
M2MColumnList *M2MColumnList_end (M2MColumnList *self)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MColumnList_end()";

	//===== Check argument =====
	if (self!=NULL)
		{
		while (M2MColumnList_next(self)!=NULL)
			{
			self = M2MColumnList_next(self);
			}
		return self;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MColumnList\" object is NULL", NULL);
		return NULL;
		}
	}


/**
 * 引数で指定されたカラム構造体オブジェクトからカラム情報オブジェクトを取得して<br>
 * 返す。<br>
 *
 * @param[in] self	Column information list object
 * @return			カラム情報オブジェクト
 */
M2MColumn *M2MColumnList_getColumn (const M2MColumnList *self)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MColumnList_getColumn()";

	//===== Check argument =====
	if (self!=NULL)
		{
		return self->column;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MColumnList\" object is NULL", NULL);
		return NULL;
		}
	}


/**
 * 引数で指定されたカラム構造体オブジェクトが1つ以上の値を持っているかどうかを判定<br>
 * する。<br>
 *
 * @param[in] self	Column information list object
 * @return			true : 値が1つも存在しない, false : 値を1つ以上保有している
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
 * @param[in] self	Column information list object
 * @return			リンクされているカラム構造体オブジェクトのノード数を示す整数
 */
unsigned int M2MColumnList_length (M2MColumnList *self)
	{
	//========== Variable ==========
	unsigned int length = 0;
	const M2MString *METHOD_NAME = (M2MString *)"M2MColumnList_length()";

	//===== Check argument =====
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
		//===== Error handling =====
		else
			{
			return 0;
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MColumnList\" object is NULL", NULL);
		return 0;
		}
	}


/**
 * 引数で指定されたカラム構造体オブジェクトの1つ後ろに位置するカラム構造体<br>
 * オブジェクトを返す。<br>
 *
 * @param[in] self	Column information list object
 * @return next 	1つ後ろに位置するカラム構造体オブジェクト（引数ノードが末端の場合はNULLを返す）
 */
M2MColumnList *M2MColumnList_next (const M2MColumnList *self)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MColumnList_next()";

	//===== Check argument =====
	if (self!=NULL)
		{
		return self->next;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MColumnList\" object is NULL", NULL);
		return NULL;
		}
	}


/**
 * Get heap memory and create a new M2MColumnList structure object.<br>
 *
 * @return	Created new column information list object
 */
M2MColumnList *M2MColumnList_new ()
	{
	//========== Variable ==========
	M2MColumnList *self = NULL;
	const M2MString *METHOD_NAME = (M2MString *)"M2MColumnList_new()";

	//===== Get heap memory for creating new object =====
	if ((self=(M2MColumnList *)M2MHeap_malloc(sizeof(M2MColumnList)))!=NULL)
		{
		//===== Initialize column information list object =====
		if (this_setPrevious(self, self)!=NULL
				&& this_setNext(self, NULL)!=NULL)
			{
			return self;
			}
		//===== Error handling =====
		else
			{
			M2MColumnList_delete(self);
			M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to initialize \"M2MColumnList\" object", NULL);
			return NULL;
			}
		}
	//===== Error handling =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to get heap memory for creating a new \"M2MColumnList\" object", NULL);
		return NULL;
		}
	}


/**
 * Get and return the column object existing one before the argument. <br>
 * If the column structure object is the first node, it indicates the <br>
 * same pointer.<br>
 *
 * @param[in] self	Column information list object
 * @return			Column structure object which exists one before or NULL (in case of error)
 */
M2MColumnList *M2MColumnList_previous (const M2MColumnList *self)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MColumnList_previous()";

	//===== Check argument =====
	if (self!=NULL)
		{
		return self->previous;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MColumnList\" object is NULL", NULL);
		return NULL;
		}
	}


/**
 * Search and detect an object matching column name from the column list.<br>
 *
 * @param[in] self				Column information list object
 * @param[in] columnName		String indicating column name
 * @param[in] columnNameLength	Length of column name string[Byte]
 * @return						Column information object matching the column name
 */
M2MColumn *M2MColumnList_search (M2MColumnList *self, const M2MString *columnName, const size_t columnNameLength)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MColumnList_search()";

	//===== Check argument =====
	if (self!=NULL
			&& columnName!=NULL
			&& columnNameLength<=M2MString_length(columnName))
		{
		//===== Detected a matching node =====
		if ((self=this_detect(self, columnName, columnNameLength))!=NULL)
			{
			return M2MColumnList_getColumn(self);
			}
		//===== Failed to detect =====
		else
			{
			return NULL;
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MColumnList\" object is NULL", NULL);
		return NULL;
		}
	else if (columnName==NULL)
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"columnName\" string is NULL", NULL);
		return NULL;
		}
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"columnNameLength\" is longer than the length of \"columnName\" string", NULL);
		return NULL;
		}
	}



/* End Of File */
