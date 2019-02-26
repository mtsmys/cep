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
		M2MLogger_error(M2MColumnList_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MColumnList\" object is NULL");
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
					M2MLogger_error(M2MColumnList_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"The medium node of \"M2MColumnList\" object is NULL");
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
				M2MLogger_error(M2MColumnList_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"The end node of \"M2MColumnList\" object is NULL");
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
			M2MLogger_error(M2MColumnList_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"The begin node of \"M2MColumnList\" object is NULL");
			return NULL;
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_error(M2MColumnList_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MColumnList\" object is NULL");
		return NULL;
		}
	else if (columnName==NULL)
		{
		M2MLogger_error(M2MColumnList_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"columnName\" string is NULL");
		return NULL;
		}
	else
		{
		M2MLogger_error(M2MColumnList_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"columnNameLength\" is longer than the length of \"columnName\" string");
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
		M2MLogger_error(M2MColumnList_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MColumnList\" object is NULL");
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
		M2MLogger_error(M2MColumnList_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MColumnList\" object is NULL");
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
		M2MLogger_error(M2MColumnList_getLogger(self), METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MColumnList\" object is NULL");
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
						M2MLogger_error(M2MColumnList_getLogger(self), __func__, __LINE__, (M2MString *)"Failed to set the column information specified in the argument to the column information list object");
						M2MColumn_delete(&column);
						return NULL;
						}
					}
				//===== Error handling =====
				else
					{
					M2MLogger_error(M2MColumnList_getLogger(self), __func__, __LINE__, (M2MString *)"Failed to get heap memory to create a new column information object");
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
						M2MLogger_error(M2MColumnList_getLogger(self), __func__, __LINE__, (M2MString *)"Failed to set the column information specified in the argument to the column information list object");
						M2MColumn_delete(&column);
						M2MColumnList_delete(newNode);
						return NULL;
						}
					}
				//===== Error handling =====
				else if (self==NULL)
					{
					M2MLogger_error(M2MColumnList_getLogger(self), __func__, __LINE__, (M2MString *)"Failed to get end node from \"M2 MColumnList\" object specified by argument");
					return NULL;
					}
				else if (newNode==NULL)
					{
					M2MLogger_error(M2MColumnList_getLogger(self), __func__, __LINE__, (M2MString *)"Failed to get heap memory to create a new column list object");
					return NULL;
					}
				else
					{
					M2MLogger_error(M2MColumnList_getLogger(self), __func__, __LINE__, (M2MString *)"Failed to get heap memory to create a new column information object");
					M2MColumnList_delete(newNode);
					return NULL;
					}
				}
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_error(M2MColumnList_getLogger(self), __func__, __LINE__, (M2MString *)"Argument error! Indicated \"M2MColumnList\" object is NULL");
		return NULL;
		}
	else
		{
		M2MLogger_error(M2MColumnList_getLogger(self), __func__, __LINE__, (M2MString *)"Argument error! Indicated \"columnName\" string is NULL or vacant");
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
		M2MLogger_error(M2MColumnList_getLogger(self), __func__, __LINE__, (M2MString *)"Argument error! Indicated \"M2MColumnList\" object is NULL");
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
				M2MLogger_error(M2MColumnList_getLogger(self), (M2MString *)"M2MColumnList_delete()", __LINE__, (M2MString *)"End node of column list object link is NULL");
				}
			}
		//===== Error handling =====
		else
			{
			M2MLogger_error(M2MColumnList_getLogger(self), (M2MString *)"M2MColumnList_delete()", __LINE__, (M2MString *)"Failed to get first node of the column list object");
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(M2MColumnList_getLogger(self), __func__, __LINE__, (M2MString *)"Argument error! Indicated \"M2MColumnList\" object is NULL");
		}
	return;
	}


/**
 * Get the node located at the end of the link of the column list object.<br>
 *
 * @param[in,out] self	Column information list object
 * @return				End node of link of column list object or NULL (in case of error)
 */
M2MColumnList *M2MColumnList_end (M2MColumnList *self)
	{
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
		M2MLogger_error(M2MColumnList_getLogger(self), __func__, __LINE__, (M2MString *)"Argument error! Indicated \"M2MColumnList\" object is NULL");
		return NULL;
		}
	}


/**
 * Get column information object possessed by column list object.<br>
 *
 * @param[in] self	Column information list object
 * @return			Column information object
 */
M2MColumn *M2MColumnList_getColumn (const M2MColumnList *self)
	{
	//===== Check argument =====
	if (self!=NULL)
		{
		return self->column;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(M2MColumnList_getLogger(self), __func__, __LINE__, (M2MString *)"Argument error! Indicated \"M2MColumnList\" object is NULL");
		return NULL;
		}
	}


/**
 * Return Logger object held by column information object.<br>
 *
 * @param[in] self	Column information list object
 * @return			Logger object owned by argument or NULL (in case of error)
 */
M2MFileAppender *M2MColumnList_getLogger (const M2MColumnList *self)
	{
	//========== Variable ==========
	M2MColumn *column = NULL;

	//===== Check argument =====
	if (self!=NULL && (column=M2MColumnList_getColumn(self))!=NULL)
		{
		return M2MColumn_getLogger(column);
		}
	//===== Argument error =====
	else
		{
		return NULL;
		}
	}


/**
 * Check whether or not the column structure object has one or more values.<br>
 *
 * @param[in] self	Column information list object
 * @return			true: There is no value, false: Have one or more values
 */
bool M2MColumnList_isEmpty (M2MColumnList *self)
	{
	//===== Check the length of list =====
	if (M2MColumnList_length(self)>0)
		{
		return false;
		}
	//===== In case of no nodes in list =====
	else
		{
		return true;
		}
	}


/**
 * Get the number of nodes in the column structure list object.<br>
 *
 * @param[in] self	Column information list object
 * @return			The number of nodes of linked column list object
 */
unsigned int M2MColumnList_length (M2MColumnList *self)
	{
	//========== Variable ==========
	unsigned int length = 0;

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Get the first node =====
		if ((self=M2MColumnList_begin(self))!=NULL
				&& M2MColumnList_getColumn(self)!=NULL)
			{
			length++;
			//===== Repeat until reaching the end node =====
			while (M2MColumnList_next(self)!=NULL)
				{
				//===== Get column name =====
				if (M2MColumnList_getColumn(self)!=NULL)
					{
					length++;
					}
				//===== When column name is not set yet =====
				else
					{
					// do nothing
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
		M2MLogger_error(M2MColumnList_getLogger(self), __func__, __LINE__, (M2MString *)"Argument error! Indicated \"M2MColumnList\" object is NULL");
		return 0;
		}
	}


/**
 * Get a column structure object positioned after the argument object.<br>
 *
 * @param[in] self	Column information list object
 * @return next 	Column structure object located one behind or NULL (in case of the argument node is end)
 */
M2MColumnList *M2MColumnList_next (const M2MColumnList *self)
	{
	//===== Check argument =====
	if (self!=NULL)
		{
		return self->next;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(M2MColumnList_getLogger(self), __func__, __LINE__, (M2MString *)"Argument error! Indicated \"M2MColumnList\" object is NULL");
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
			M2MLogger_error(M2MColumnList_getLogger(self), __func__, __LINE__, (M2MString *)"Failed to initialize \"M2MColumnList\" object");
			return NULL;
			}
		}
	//===== Error handling =====
	else
		{
		M2MLogger_error(M2MColumnList_getLogger(self), __func__, __LINE__, (M2MString *)"Failed to get heap memory for creating a new \"M2MColumnList\" object");
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
	//===== Check argument =====
	if (self!=NULL)
		{
		return self->previous;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(M2MColumnList_getLogger(self), __func__, __LINE__, (M2MString *)"Argument error! Indicated \"M2MColumnList\" object is NULL");
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
		M2MLogger_error(M2MColumnList_getLogger(self), __func__, __LINE__, (M2MString *)"Argument error! Indicated \"M2MColumnList\" object is NULL");
		return NULL;
		}
	else if (columnName==NULL)
		{
		M2MLogger_error(M2MColumnList_getLogger(self), __func__, __LINE__, (M2MString *)"Argument error! Indicated \"columnName\" string is NULL");
		return NULL;
		}
	else
		{
		M2MLogger_error(M2MColumnList_getLogger(self), __func__, __LINE__, (M2MString *)"Argument error! Indicated \"columnNameLength\" is longer than the length of \"columnName\" string");
		return NULL;
		}
	}



/* End Of File */
