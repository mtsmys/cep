/*******************************************************************************
 * M2MDataFrame.c
 *
 * Copyright (c) 2018, Akihisa Yasuda
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

#include "m2m/app/cep/M2MDataFrame.h"


/*******************************************************************************
 * Private function
 ******************************************************************************/
/**
 * Delete the culumn name string owned by argument as a member variable.<br>
 *
 * @param[in,out] self	Record management object
 */
static void this_deleteColumnName (M2MDataFrame *self)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MDataFrame.this_deleteColumnName()";

	//===== Confirm existence of field name list =====
	if (M2MDataFrame_getColumnName(self)!=NULL)
		{
		//===== Release heap memory area =====
		M2MHeap_free(self->columnName);
		}
	//===== Error handling =====
	else
		{
		M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MDataFrame *\" object has no column name");
		}
	return;
	}


/**
 * Delete the newRecordList object owned by argument as a member variable.<br>
 *
 * @param[in,out] self	Record management object
 */
static void this_deleteNewRecordList (M2MDataFrame *self)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MDataFrame.this_deleteNewRecordList()";

	//===== Confirm existence of new record management list =====
	if (M2MDataFrame_getNewRecordList(self)!=NULL)
		{
		//===== Release heap memory area (it is a pointer in the structure so it can be released) =====
		M2MList_delete(self->newRecordList);
		}
	//===== Error handling =====
	else
		{
		M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MDataFrame *\" object has no \"newRecordList\" object");
		}
	return;
	}


/**
 * Delete the oldRecordList object owned by argument as a member variable.<br>
 *
 * @param[in,out] self	Record management object
 */
static void this_deleteOldRecordList (M2MDataFrame *self)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MDataFrame.this_deleteOldRecordList()";

	//===== Confirm existence of record management list in the past =====
	if (M2MDataFrame_getOldRecordList(self)!=NULL)
		{
		//===== Release heap memory area (it is a pointer in the structure so it can be released) =====
		M2MList_delete(self->oldRecordList);
		}
	//===== Error handling =====
	else
		{
		M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MDataFrame *\" object has no \"oldRecordList\" object");
		}
	return;
	}


/**
 * Releases the memory area of the table name string held by the argument.<br>
 *
 * @param[in,out] self	Record management object
 */
static void this_deleteTableName (M2MDataFrame *self)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MDataFrame.this_deleteTableName()";

	//===== Confirm the existence of table name string =====
	if (M2MDataFrame_getTableName(self)!=NULL)
		{
		//===== Free heap memory area (it is a pointer in the structure so it can be released) =====
		M2MHeap_free(self->tableName);
		}
	//===== Error handling =====
	else
		{
		M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MDataFrame *\" object has not table name string");
		}
	return;
	}


/**
 * Get and return the node that matches the table name specified by the argument.<br>
 * If not found, return NULL.<br>
 *
 * @param[in] self			Record management object
 * @param[in] tableName		Table name string used for node search
 * @return					Record management object with matching table name or NULL (in case of error)
 */
static M2MDataFrame *this_detectM2MDataFrame (M2MDataFrame *self, const M2MString *tableName)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MDataFrame.this_detectM2MDataFrame()";

	//===== Check argument =====
	if (self!=NULL && tableName!=NULL)
		{
		//===== Get the begin node of record management object =====
		if ((self=M2MDataFrame_begin(self))!=NULL)
			{
			//===== Repeat until reaching the end node =====
			while (M2MDataFrame_next(self)!=NULL)
				{
				//===== When it matches the table name held by the node =====
				if (M2MString_compareTo(M2MDataFrame_getTableName(self), tableName)==0)
					{
					return self;
					}
				//===== When table names are different =====
				else
					{
					//===== Go to next node =====
					self = M2MDataFrame_next(self);
					}
				}
			//===== When matches the table name possessed by the terminal node =====
			if (M2MString_compareTo(M2MDataFrame_getTableName(self), tableName)==0)
				{
				return self;
				}
			//===== When the table name owned by the terminal node is different =====
			else
				{
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"The first node obtained from \"M2MDataFrame *\" argument object is NULL");
			return NULL;
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MDataFrame *\" object is NULL");
		return NULL;
		}
	else
		{
		M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"tableName\" string is NULL or vacant");
		return NULL;
		}
	}


/**
 * Get the end node of record management object.<br>
 *
 * @param[in] self	Record management object
 * @return			The end node of record management object or NULL (in case of error)
 */
static M2MDataFrame *this_end (M2MDataFrame *self)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MDataFrame.this_end()";

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Repeat until reaching the end node =====
		while (M2MDataFrame_next(self)!=NULL)
			{
			self = M2MDataFrame_next(self);
			}
		return self;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MDataFrame *\" object is NULL");
		return NULL;
		}
	}


/**
 * @param[in] self	Record management object
 */
static void this_init (M2MDataFrame *self)
	{
	//========== Variable ==========
	M2MList *newRecordList = NULL;
	M2MList *oldRecordList = NULL;
	const M2MString *METHOD_NAME = (M2MString *)"M2MDataFrame.this_init()";

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Initialization of member variables =====
		this_deleteTableName(self);
		this_deleteColumnName(self);
		this_deleteNewRecordList(self);
		this_deleteOldRecordList(self);
		//===== Create old and new record information objects =====
		if ((newRecordList=M2MList_new())!=NULL
				&& (oldRecordList=M2MList_new())!=NULL)
			{
			//===== Set an object to store newly inserted record information =====
			self->newRecordList = newRecordList;
			//===== Set an object for storing record information inserted in the past =====
			self->oldRecordList = oldRecordList;
			//===== Successful completion =====
			return;
			}
		//===== Error handling =====
		else if (newRecordList==NULL)
			{
			M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"Failed to create a list structure object to store newly inserted record information");
			return;
			}
		else
			{
			M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"Failed to create a list structure object for storing record information inserted in the past");
			this_deleteNewRecordList(self);
			return;
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MDataFrame *\" object is NULL");
		return;
		}
	}


/**
 * Get the number of record management object.<br>
 *
 * @param[in] self	Record management object
 * @return			The number of record management object
 */
static unsigned int this_length (M2MDataFrame *self)
	{
	//========== Variable ==========
	unsigned int length = 0;
	M2MString *tableName = NULL;
	M2MString *columnNameCSV = NULL;
	const M2MString *METHOD_NAME = (M2MString *)"M2MDataFrame.this_length()";

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Get the first node =====
		if ((self=M2MDataFrame_begin(self))!=NULL)
			{
			//===== Confirm existence of table name and CSV column name string =====
			if ((tableName=M2MDataFrame_getTableName(self))!=NULL
					&& M2MString_length(tableName)>0
					&& (columnNameCSV=M2MDataFrame_getColumnName(self))!=NULL
					&& M2MString_length(columnNameCSV)>0)
				{
				//=====  =====
				length++;
				}
			//===== Error handling =====
			else if (tableName==NULL || M2MString_length(tableName)<=0)
				{
				M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"\"tableName\" string owned by argument is NULL or vacant");
				}
			else
				{
				M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"\"columnName\" CSV string owned by argument is NULL or vacant");
				}
			//=====  =====
			while (M2MDataFrame_next(self)!=NULL)
				{
				//===== Confirm existence of table name and column name in CSV format =====
				if ((tableName=M2MDataFrame_getTableName(self))!=NULL
						&& M2MString_length(tableName)>0
						&& (columnNameCSV=M2MDataFrame_getColumnName(self))!=NULL
						&& M2MString_length(columnNameCSV)>0)
					{
					length++;
					}
				//===== Error handling =====
				else if (tableName==NULL || M2MString_length(tableName)<=0)
					{
					M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"\"tableName\" string owned by argument is NULL or vacant");
					}
				else
					{
					M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"\"columnName\" CSV string owned by argument is NULL or vacant");
					}
				}
			//===== Return the number of record management object =====
			return length;
			}
		//===== Error handling =====
		else
			{
			M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"Failed to get the first node from the argument object");
			return 0;
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MDataFrame *\" object is NULL");
		return 0;
		}
	}


/**
 * @param[in,out] self		Record management object
 * @param[in] csv			CSV string describing the column name of the first line
 * @param[in] lineLength	CSV Length of string describing column name of line 1 [byte]
 * @return					Column name record management object with character string set or NULL (in case of error)
 */
static M2MDataFrame *this_setColumnName (M2MDataFrame *self, const M2MString *csv, const size_t lineLength)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MDataFrame.this_setColumnName()";

	//===== Check argument =====
	if (self!=NULL
			&& csv!=NULL && lineLength>0 && lineLength<=M2MString_length(csv))
		{
		//===== Initialization (release of heap memory area) =====
		this_deleteColumnName(self);
		//===== Get the heap memory area =====
		if ((self->columnName=(M2MString *)M2MHeap_malloc(lineLength+1))!=NULL)
			{
			//===== Copy CSV string indicating column name =====
			memcpy(self->columnName, csv, lineLength);
			return self;
			}
		//===== Error handling =====
		else
			{
			M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"Failed to get of the heap memory area to copy the string indicating the column name in CSV format");
			return NULL;
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MDataFrame *\" object is NULL");
		return NULL;
		}
	else if (csv==NULL)
		{
		M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"csv\" string is NULL");
		return NULL;
		}
	else if (lineLength<=0)
		{
		M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"Argument error! \"lineLength\" number is less than or equal to 0");
		return NULL;
		}
	else
		{
		M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"Argument error! \"lineLength\" number is larger than the number of \"csv\" string");
		return NULL;
		}
	}


/**
 * Set a record in CSV format to the new record information object specified by the argument.<br>
 *
 * @param[in] newRecordList	New record information object
 * @param[in] csv			A record indicating a character string in CSV format
 * @return					Number of set records or -1 (in case of error)
 */
static int this_setCSVIntoNewRecordList (M2MList *newRecordList, const M2MString *csv)
	{
	//========== Variable ==========
	int numberOfRecord = 0;
	M2MString *lineHead = NULL;
	M2MString *lineEnd = NULL;
	size_t lineLength = 0;
	const unsigned int CRLF_LENGTH = M2MString_length(M2MString_CRLF);
	const M2MString *METHOD_NAME = (M2MString *)"M2MDataFrame.this_setCSVIntoNewRecordList()";

	//===== Check argument =====
	if (newRecordList!=NULL
			&& csv!=NULL && M2MString_length(csv)>0)
		{
		//===== Ignore the header line of the first line =====
		if ((lineEnd=M2MString_indexOf(csv, M2MString_CRLF))!=NULL
				&& (lineEnd+=CRLF_LENGTH)!=NULL)
			{
			//===== Get begin position of CSV data area =====
			lineHead = lineEnd;
			//===== Repeat until reaching the last line =====
			while ((lineEnd=M2MString_indexOf(lineHead, M2MString_CRLF))!=NULL)
				{
				//===== Get data size of CSV 1 row =====
				if ((lineLength=M2MString_length(lineHead)-M2MString_length(lineEnd))>0)
					{
					//===== Copy CSV 1 line to new record information object =====
					if (M2MList_add(newRecordList, lineHead, lineLength)!=NULL)
						{
						//===== Increment the number of copied record lines =====
						numberOfRecord++;
						}
					//===== Error handling =====
					else
						{
						M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"Failed to store record in CSV format to record information object. . . Continue processing");
						}
					}
				//===== Error handling =====
				else
					{
					M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"The number of character strings in the CSV format character string is 0 [Byte] or less. . . Continue processing");
					}
				//===== Move to the beginning of the next line =====
				lineEnd += CRLF_LENGTH;
				lineHead = lineEnd;
				}
			//===== Copy CSV last line =====
			if ((lineLength=M2MString_length(lineHead))>0)
				{
				//===== Copy CSV 1 line to new record information object =====
				if (M2MList_add(newRecordList, lineHead, lineLength)!=NULL)
					{
					//===== Increment the number of copied record lines =====
					numberOfRecord++;
					return numberOfRecord;
					}
				//===== Error handling =====
				else
					{
					M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"Failed to store CSV format character string in record information object");
					//===== When the number of records set is 1 or more =====
					if (numberOfRecord>=1)
						{
						return numberOfRecord;
						}
					//===== When the number of set records is 0 or less =====
					else
						{
						return -1;
						}
					}
				}
			//===== Error handling =====
			else
				{
				M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"The number of character strings in CSV format string is 0 [Byte] or less");
				//===== When the number of records set is 1 or more =====
				if (numberOfRecord>=1)
					{
					return numberOfRecord;
					}
				//===== When the number of records set is 0 or less =====
				else
					{
					return -1;
					}
				}
			}
		//===== When there is no data row =====
		else
			{
			M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"The CSV format character string specified by argument is illegal data with only the header line of the first line");
			return -1;
			}
		}
	//===== Argument error =====
	else if (newRecordList==NULL)
		{
		M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"Argument error! The record information object for new insertion specified by argument is NULL");
		return -1;
		}
	else
		{
		M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"Argument error! The CSV format string specified by argument is NULL or blank");
		return -1;
		}
	}


/**
 * Set the table name string in the record management object specified by the argument.<br>
 *
 * @param[in,out] self		Record management object
 * @param[in] tableName		String indicating table name
 * @return					A record management object that succeeded in setting (or updating) the table name
 */
static M2MDataFrame *this_setTableName (M2MDataFrame *self, const M2MString *tableName)
	{
	//========== Variable ==========
	unsigned int tableNameLength = 0;
	const M2MString *METHOD_NAME = (M2MString *)"M2MDataFrame.this_setTableName()";

	//===== Check argument =====
	if (self!=NULL
			&& tableName!=NULL && (tableNameLength=M2MString_length(tableName))>0)
		{
		//===== Initialize the table name string =====
		this_deleteTableName(self);
		//===== Get the heap memory area =====
		if ((self->tableName=(M2MString *)M2MHeap_malloc(tableNameLength+1))!=NULL)
			{
			//===== Copy of the table name string =====
			memcpy(self->tableName, tableName, tableNameLength);
			return self;
			}
		//===== Error handling =====
		else
			{
			M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"Failed to get heap memory area for copying \"tableName\" string");
			return NULL;
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MDataFrame *\" object is NULL");
		return NULL;
		}
	else
		{
		M2MLogger_error(NULL, METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"tableName\" string is NULL or vacant");
		return NULL;
		}
	}



/*******************************************************************************
 * Public function
 ******************************************************************************/
/**
 * Get the first record management object from the argument and returns it.<br>
 * <br>
 * [Attention!]<br>
 * The first record indicates the same address as the previous pointer to itself.<br>
 *
 * @param[in] self	Record management object
 * @return			First record management object
 */
M2MDataFrame *M2MDataFrame_begin (M2MDataFrame *self)
	{
	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Repeat until reaching the first node =====
		while (self!=NULL && M2MDataFrame_previous(self)!=self)
			{
			//===== Move to previous node =====
			self = M2MDataFrame_previous(self);
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
 * Free all memory area of record management object specified by argument.<br>
 * <br>
 * [Attention!]<br>
 * The record management object holds a pointer which is a link structure as a <br>
 * member variable.
 * Therefore, when releasing the heap memory, it is unnecessary to use it as a <br>
 * pointer pointer, and it is possible to acquire the correct address through <br>
 * the pointer held internally.<br>
 *
 * @param[in,out] self	Record management object to release memory area
 */
void M2MDataFrame_delete (M2MDataFrame **self)
	{
	//========== Variable ==========
	M2MDataFrame *next = NULL;

	//===== Check argument =====
	if (self!=NULL && (*self)!=NULL)
		{
		//===== Get the first M2MList structure object =====
		if (((*self)=M2MDataFrame_begin((*self)))!=NULL)
			{
			//===== Confirm existence of backward M2MList structure object =====
			while ((*self)!=NULL && (next=M2MDataFrame_next((*self)))!=NULL)
				{
				//===== Remove the M2MList structure object from the link =====
				M2MDataFrame_setNextRecord((*self), NULL);
				M2MDataFrame_setPreviousRecord(next, next);
				//===== Free memory area of M2MList structure object =====
				this_deleteTableName((*self));
				this_deleteColumnName((*self));
				this_deleteNewRecordList((*self));
				this_deleteOldRecordList((*self));
				M2MHeap_free((*self));
				//===== Move to the next M2MList structure object =====
				(*self) = next;
				}
			//===== Free the last memory area of M2MList structure object =====
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
				M2MLogger_error(NULL, __func__, __LINE__, (M2MString *)"Internal error! NULL is included in the node of \"M2MDataFrame *\" specified by argument");
				}
			}
		//===== Error handling =====
		else
			{
			M2MLogger_error(NULL, __func__, __LINE__, (M2MString *)"Failed to get the first node of \"M2MDataFrame *\" specified by argument");
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(NULL, __func__, __LINE__, (M2MString *)"\"M2MDataFrame *\" specified by argument is NULL");
		}
	return;
	}


/**
 * Returns a CSV format string indicating the column name held by the argument <br>
 * as a member variable.<br>
 *
 * @param[in] self		Record management object
 * @return				CSV format string indicating the column name held by record management object specified by argument or NULL (in case of error)
 */
M2MString *M2MDataFrame_getColumnName (const M2MDataFrame *self)
	{
	//===== Check argument =====
	if (self!=NULL)
		{
		return self->columnName;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(NULL, __func__, __LINE__, (M2MString *)"Argument error! Indicated \"M2MDataFrame *\" object is NULL");
		return NULL;
		}
	}


/**
 * Returns a newly inserted record list that the argument holds as a member variable.<br>
 *
 * @param[in] self		Record management object
 * @return				A record list newly inserted (not yet inserted in the memory DB) held by the record management object specified by the argument
 */
M2MList *M2MDataFrame_getNewRecordList (const M2MDataFrame *self)
	{
	//===== Check argument =====
	if (self!=NULL)
		{
		return self->newRecordList;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(NULL, __func__, __LINE__, (M2MString *)"Argument error! Indicated \"M2MDataFrame *\" object is NULL");
		return NULL;
		}
	}


/**
 * Returns a record list inserted in the past that the argument holds as a member variable.<br>
 *
 * @param[in] self		Record management object
 * @return				Record list (inserted in the memory DB) inserted in the past that is held by the record management object specified by the argument
 */
M2MList *M2MDataFrame_getOldRecordList (const M2MDataFrame *self)
	{
	//===== Check argument =====
	if (self!=NULL)
		{
		return self->oldRecordList;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(NULL, __func__, __LINE__, (M2MString *)"Argument error! Indicated \"M2MDataFrame *\" object is NULL");
		return NULL;
		}
	}


/**
 * Returns the table name string held by the argument as a member variable.<br>
 *
 * @param[in] self		Record management object
 * @return				Table name held by record management object specified by argument
 */
M2MString *M2MDataFrame_getTableName (const M2MDataFrame *self)
	{
	//===== Check argument =====
	if (self!=NULL)
		{
		return self->tableName;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(NULL, __func__, __LINE__, (M2MString *)"Argument error! Indicated \"M2MDataFrame *\" object is NULL");
		return NULL;
		}
	}


/**
 * Copy the record information of the "newly inserted record information" structure <br>
 * object of the record management object designated by the argument to the "record <br>
 * information inserted in the past" structure object.<br>
 * This operation is executed to insert data into the SQLite 3 database in memory <br>
 * and then organize the data for persistence into the SQLite 3 database on the file.<br>
 *
 * @param[in,out] self	Record management object
 */
void M2MDataFrame_moveFromNewRecordListToOldRecordList (M2MDataFrame *self)
	{
	//========== Variable ==========
	M2MList *newRecordList = NULL;
	M2MList *oldRecordList = NULL;
	void *value = NULL;
	size_t valueLength = 0;
	unsigned int counter = 0;

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Retrieve old and new record information objects =====
		if ((newRecordList=M2MDataFrame_getNewRecordList(self))!=NULL
				&& (oldRecordList=M2MDataFrame_getOldRecordList(self))!=NULL)
			{
			//===== Get the begin node of the new record information object =====
			if ((newRecordList=M2MList_begin(newRecordList))!=NULL)
				{
				//===== Repeat until reaching the terminal node =====
				while (newRecordList!=NULL && M2MList_next(newRecordList)!=NULL)
					{
					//===== Confirm existence of record of new record information object =====
					if ((value=M2MList_getValue(newRecordList))!=NULL
							&& (valueLength=M2MList_getValueLength(newRecordList))>0)
						{
						//===== Copy record of new record information object to past record information object =====
						if (M2MList_add(oldRecordList, value, valueLength)!=NULL)
							{
							counter++;
							}
						//===== Error handling =====
						else
							{
							M2MLogger_error(NULL, __func__, __LINE__, (M2MString *)"An error occurred during the process of copying the new record information object to the past record information object");
							}
						}
					//===== Error handling =====
					else
						{
						M2MLogger_error(NULL, __func__, __LINE__, (M2MString *)"The record value of the new record information object is NULL or the record size is 0 [byte] or less");
						}
					//===== Delete the moved record from the new record information object and move to the next node =====
					newRecordList = M2MList_remove(newRecordList);
					}
				//===== Confirm existence of record of new record information object =====
				if ((value=M2MList_getValue(newRecordList))!=NULL
						&& (valueLength=M2MList_getValueLength(newRecordList))>0)
					{
					//===== Copy the record of the terminal node to the past record information object =====
					if (M2MList_add(oldRecordList, value, valueLength)!=NULL)
						{
						counter++;
						}
					//===== Error handling =====
					else
						{
						M2MLogger_error(NULL, __func__, __LINE__, (M2MString *)"An error occurred during the process of copying the new record information object to the past record information object");
						}
					}
				//===== Error handling =====
				else
					{
					M2MLogger_error(NULL, __func__, __LINE__, (M2MString *)"The record value of the new record information object is NULL or the record size is 0 [byte] or less");
					}
				//===== Delete moved record from new record information object =====
				self->newRecordList = M2MList_remove(newRecordList);
				return;
				}
			//===== Error handling =====
			else
				{
				M2MLogger_error(NULL, __func__, __LINE__, (M2MString *)"The first node of \"newRecordList\" obtained from \"M2MDataFrame *\" specified by argument is NULL");
				return;
				}
			}
		//===== Error handling =====
		else if (newRecordList==NULL)
			{
			M2MLogger_error(NULL, __func__, __LINE__, (M2MString *)"\"NewRecordList\" obtained from \"M2MDataFrame *\" specified by argument is NULL");
			return;
			}
		else
			{
			M2MLogger_error(NULL, __func__, __LINE__, (M2MString *)"\"OldRecordList\" obtained from \"M2MDataFrame *\" specified by argument is NULL");
			return;
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(NULL, __func__, __LINE__, (M2MString *)"Argument error! Indicated \"M2MDataFrame *\" object is NULL");
		return;
		}
	}


/**
 * A heap memory area is newly acquired, and a record management object is generated.<br>
 * Keep in mind that the forward node of this record management object is its own <br>
 * pointer and the backward node is initialized to NULL.<br>
 *
 * @return	A newly created record management object
 */
M2MDataFrame *M2MDataFrame_new ()
	{
	//========== Variable ==========
	M2MDataFrame *self = NULL;

	//===== Get the heap memory area =====
	if ((self=(M2MDataFrame *)M2MHeap_malloc(sizeof(M2MDataFrame)))!=NULL)
		{
		if ((self->newRecordList=M2MList_new())!=NULL)
			{
			if ((self->oldRecordList=M2MList_new())!=NULL)
				{
				//===== Initialization of member variables =====
				M2MDataFrame_setPreviousRecord(self, self);
				M2MDataFrame_setNextRecord(self, NULL);
				return self;
				}
			//===== Error handling =====
			else
				{
				M2MLogger_error(NULL, __func__, __LINE__, (M2MString *)"Failed to create an object to manage inserted record information");
				this_deleteNewRecordList(self);
				this_deleteColumnName(self);
				M2MHeap_free(self);
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			M2MLogger_error(NULL, __func__, __LINE__, (M2MString *)"Failed to create an object to manage new record information");
			this_deleteColumnName(self);
			M2MHeap_free(self);
			return NULL;
			}
		}
	//===== Error handling =====
	else
		{
		M2MLogger_error(NULL, __func__, __LINE__, (M2MString *)"Failed to create a record management object");
		return NULL;
		}
	}


/**
 * Returns the record management object after the argument is held as a member variable.<br>
 *
 * @param[in] self		Record management object
 * @return				The record management object immediately following the record management object specified by the argument
 */
M2MDataFrame *M2MDataFrame_next (const M2MDataFrame *self)
	{
	//===== Check argument =====
	if (self!=NULL)
		{
		return self->next;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(NULL, __func__, __LINE__, (M2MString *)"Argument error! Indicated \"M2MDataFrame *\" object is NULL");
		return NULL;
		}
	}


/**
 * Returns the record management object immediately before which the argument is <br>
 * held as a member variable.<br>
 *
 * @param[in] self		Record management object
 * @return				The record management object immediately before the record management object specified by the argument
 */
M2MDataFrame *M2MDataFrame_previous (const M2MDataFrame *self)
	{
	//===== Check argument =====
	if (self!=NULL)
		{
		return self->previous;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(NULL, __func__, __LINE__, (M2MString *)"Argument error! Indicated \"M2MDataFrame *\" object is NULL");
		return NULL;
		}
	}


/**
 * Only the record management object corresponding to the table name specified by <br>
 * the argument releases the memory area (other record management objects connected <br>
 * by the pointer will survive).<br>
 *
 * @param[in,out] self		Record management object
 * @param[in] tableName		String indicating the table name possessed by the record management object to be deleted
 */
void M2MDataFrame_remove (M2MDataFrame *self, const M2MString *tableName)
	{
	//========== Variable ==========
	M2MDataFrame *record = NULL;
	M2MDataFrame *previous = NULL;
	M2MDataFrame *next = NULL;
	M2MString MESSAGE[256];

	//===== Check argument =====
	if (self!=NULL
			&& tableName!=NULL && M2MString_length(tableName)>0)
		{
		//===== Get records management object node =====
		if ((record=this_detectM2MDataFrame(self, tableName))!=NULL)
			{
			//===== Free heap memory area of member variable =====
			this_deleteTableName(record);
			this_deleteColumnName(record);
			this_deleteNewRecordList(record);
			this_deleteOldRecordList(record);
			//===== Get forward / backward node of designated record management object =====
			previous = M2MDataFrame_previous(record);
			next = M2MDataFrame_next(record);
			//===== When the designated record management object is the first node =====
			if (previous==record)
				{
				//===== If there is a succeeding node in the specified record management object =====
				if (next!=NULL)
					{
					//===== Detach the connection between the nodes =====
					M2MDataFrame_setPreviousRecord(next, next);
					//===== Free heap memory area of node =====
					M2MHeap_free(record);
					}
				//===== If there is no subsequent node in the specified record management object (only one exists) =====
				else
					{
					// do nothing
					}
				}
			//===== If the specified record management object is an end node (however, the front node always exists) =====
			else if (next==NULL)
				{
				//===== Detach the connection between the nodes =====
				M2MDataFrame_setNextRecord(previous, NULL);
				//===== Free heap memory area of node =====
				M2MHeap_free(record);
				}
			//===== If there is a forward / backward node of the specified record management object =====
			else
				{
				//===== Detach the connection between the nodes =====
				M2MDataFrame_setNextRecord(previous, next);
				M2MDataFrame_setPreviousRecord(next, previous);
				//===== Free heap memory area of node =====
				M2MHeap_free(record);
				}
			return;
			}
		//===== Error handling =====
		else
			{
			memset(MESSAGE, 0, sizeof(MESSAGE));
			snprintf(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"There is not a table matched the name(=\"%s\")", tableName);
			M2MLogger_error(NULL, __func__, __LINE__, MESSAGE);
			return;
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_error(NULL, __func__, __LINE__, (M2MString *)"Argument error! Indicated \"M2MDataFrame *\" object is NULL");
		return;
		}
	else
		{
		M2MLogger_error(NULL, __func__, __LINE__, (M2MString *)"Argument error! Indicated \"tableName\" string is NULL or vacant");
		return;
		}
	}


/**
 * String data in CSV format is stored in the list structure object for the <br>
 * record management object having the table name designated by the argument as <br>
 * the member variable.<br>
 * If no record management object having a table name as a member variable exists, <br>
 * a record management object is newly generated, CSV format character string <br>
 * data is set and added to the record management object specified by the argument.<br>
 * <br>
 * Strings in CSV format must comply with the following specifications.<br>
 * - Character code: UTF-8<br>
 * - Line feed code: "\r\n"<br>
 * - The header showing the column name in the first line, the data after the second line<br>
 * <br>
 * [Example of CSV format string set as argument]<br>
 * date, temperature, humidity\r\n	Header row<br>
 * 1395984160, 23.8, 46\r\n			Record 1st line<br>
 * 1395984254, 24.0, 45\r\n			Record 2nd line<br>
 * ...<br>
 *
 * @param[in,out] self		Record management object
 * @param[in] tableName		String indicating table name
 * @param[in] csv			String data in CSV format (header showing column name in the first row, data after the second row)
 * @return					Number of records stored in object [number] or -1 (in case of error)
 */
int M2MDataFrame_setCSV (M2MDataFrame *self, const M2MString *tableName, const M2MString *csv)
	{
	//========== Variable ==========
	M2MDataFrame *record = NULL;
	M2MList *newRecordList = NULL;
	M2MString *lineEnd = NULL;
	size_t lineLength = 0;
	int numberOfRecord = -1;

	//===== Check argument =====
	if (self!=NULL
			&& tableName!=NULL && M2MString_length(tableName)>0
			&& csv!=NULL && M2MString_length(csv)>0)
		{
		//===== If the table name has already been set (if an existing node exists) =====
		if ((record=this_detectM2MDataFrame(self, tableName))!=NULL)
			{
			//===== Get new Record Information Management Object =====
			if ((newRecordList=M2MDataFrame_getNewRecordList(record))!=NULL)
				{
				//===== Set record of CSV format to new record information management object =====
				return this_setCSVIntoNewRecordList(newRecordList, csv);
				}
			//===== Error handling =====
			else
				{
				M2MLogger_error(NULL, __func__, __LINE__, (M2MString *)"A new record information list can not be acquired from the record management object specified by the argument");
				return -1;
				}
			}
		//===== If there is no node with the specified table name =====
		else
			{
			//===== When there is no node having a value in the record management object =====
			if (this_length(self)==0)
				{
				//===== Set the first node of the record management object (keep in mind that the first node "always exists") =====
				if (this_setTableName(self, tableName)!=NULL
						&& (newRecordList=M2MDataFrame_getNewRecordList(self))!=NULL)
					{
					//===== Set column name of table to record management object =====
					if ((lineEnd=M2MString_indexOf(csv, M2MString_CRLF))!=NULL
							&& (lineLength=M2MString_length(csv)-M2MString_length(lineEnd))>0
							&& this_setColumnName(self, (M2MString *)csv, lineLength)!=NULL)
						{
						//===== Set record of CSV format to new record information management object =====
						if ((numberOfRecord=this_setCSVIntoNewRecordList(newRecordList, csv))>=1)
							{
							return numberOfRecord;
							}
						//===== Error handling =====
						else
							{
							M2MLogger_error(NULL, __func__, __LINE__, (M2MString *)"Failed to set record information object in CSV format record");
							//===== Initialize node of record management object (delete table name and new record information object) =====
							this_init(self);
							return -1;
							}
						}
					//===== Error handling =====
					else
						{
						M2MLogger_error(NULL, __func__, __LINE__, (M2MString *)"Failed to get the header line of the first line of the CSV format character string specified by the argument");
						//===== Delete generated record management object =====
						this_init(self);
						return -1;
						}
					}
				//===== Error handling =====
				else
					{
					M2MLogger_error(NULL, __func__, __LINE__, (M2MString *)"Failed to create new record information object");
					//===== Delete generated record management object =====
					this_init(self);
					return -1;
					}
				}
			//===== When there is at least one node in the record management object =====
			else
				{
				//===== Generate new record management object nodes =====
				if ((self=this_end(self))!=NULL
						&& (record=M2MDataFrame_new())!=NULL
						&& this_setTableName(record, tableName)!=NULL
						&& (newRecordList=M2MDataFrame_getNewRecordList(record))!=NULL)
					{
					//===== Set column name of table to record management object =====
					if ((lineEnd=M2MString_indexOf(csv, M2MString_CRLF))!=NULL
							&& (lineLength=M2MString_length(csv)-M2MString_length(lineEnd))>0
							&& this_setColumnName(record, (M2MString *)csv, lineLength)!=NULL)
						{
						//===== Set record of CSV format to new record information management object =====
						if ((numberOfRecord=this_setCSVIntoNewRecordList(newRecordList, csv))>0)
							{
							//===== Concatenate with existing record management objects =====
							M2MDataFrame_setNextRecord(self, record);
							M2MDataFrame_setPreviousRecord(record, self);
							//===== Returns the number of records set =====
							return numberOfRecord;
							}
						//===== Error handling =====
						else
							{
							M2MLogger_error(NULL, __func__, __LINE__, (M2MString *)"Failed to set record in CSV format to record management object");
							//===== Delete generated record management object =====
							M2MDataFrame_delete(&record);
							return -1;
							}
						}
					//===== Error handling =====
					else
						{
						M2MLogger_error(NULL, __func__, __LINE__, (M2MString *)"Failed to set column name in record management object");
						//===== Delete generated record management object =====
						M2MDataFrame_delete(&record);
						return -1;
						}
					}
				//===== Error handling =====
				else
					{
					M2MLogger_error(NULL, __func__, __LINE__, (M2MString *)"Failed to create new record information list");
					//===== Delete generated record management object =====
					M2MDataFrame_delete(&record);
					return -1;
					}
				}
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_error(NULL, __func__, __LINE__, (M2MString *)"Argument error! Indicated \"M2MDataFrame *\" object is NULL");
		return -1;
		}
	else if (tableName==NULL || M2MString_length(tableName)<=0)
		{
		M2MLogger_error(NULL, __func__, __LINE__, (M2MString *)"Argument error! The string indicating the table name is NULL");
		return -1;
		}
	else
		{
		M2MLogger_error(NULL, __func__, __LINE__, (M2MString *)"Argument error! The string indicating the CSV format record is NULL");
		return -1;
		}
	}


/**
 * Set the record management object one record behind the argument.<br>
 *
 * @param[in] self			Record management object
 * @param[in] nextRecord	The record management object one behind
 */
void M2MDataFrame_setNextRecord (M2MDataFrame *self, M2MDataFrame *nextRecord)
	{
	//===== Check argument =====
	if (self!=NULL)
		{
		self->next = nextRecord;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(NULL, __func__, __LINE__, (M2MString *)"Argument error! Indicated \"M2MDataFrame *\" object is NULL");
		}
	return;
	}


/**
 * Set the record management object one before the argument.<br>
 *
 * @param[in,out] self			Record management object
 * @param[in] previousRedord	Record management object one before
 */
void M2MDataFrame_setPreviousRecord (M2MDataFrame *self, M2MDataFrame *previousRecord)
	{
	//===== Check argument =====
	if (self!=NULL)
		{
		self->previous = previousRecord;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(NULL, __func__, __LINE__, (M2MString *)"Argument error! Indicated \"M2MDataFrame *\" object is NULL");
		}
	return;
	}


/**
 * Returns the number of elements of record management object specified by argument.<br>
 *
 * @param[in] self	Record management object
 * @return			The number of element
 */
unsigned int M2MDataFrame_size (M2MDataFrame *self)
	{
	//========== Variable ==========
	unsigned int size = 0;

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Get the first node =====
		if ((self=M2MDataFrame_begin(self))!=NULL)
			{
			size++;
			//===== Repeat until reaching the end node =====
			while ((self=M2MDataFrame_next(self))!=NULL)
				{
				size++;
				}
			//===== Returns the number of elements =====
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
		M2MLogger_error(NULL, __func__, __LINE__, (M2MString *)"Argument error! Indicated \"M2MDataFrame *\" object is NULL");
		return 0;
		}
	}



/* End Of File */
