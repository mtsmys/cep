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

#include "m2m/lib/db/M2MTableManager.h"


/*******************************************************************************
 * Definition of private function
 ******************************************************************************/
/**
 * Returns a column structure object that the table construction object <br>
 * specified by the argument holds as a member variable.<br>
 *
 * @param[in] self	Table construction object
 * @return			Column structure object or NULL (in case of error)
 */
static M2MColumnList *this_getColumnList (const M2MTableManager *self);


/**
 * Get logger object owned by the argument "M2MTableManager" object.<br>
 *
 * @param[in] self	Table construction object
 * @return			Logger object
 */
static M2MFileAppender *this_getLogger (const M2MTableManager *self);


/**
 * Get the table name string held by the table construction object.<br>
 *
 * @param[in] self	Table construction object
 * @return			String indicating table name
 */
static unsigned char *this_getTableName (const M2MTableManager *self);


/**
 * Returns the object before the table construction object.<br>
 *
 * @param[in] self	Table construction object
 * @return			Node immediately before the table construction object or NULL (in case of error)
 */
static M2MTableManager *this_previous (const M2MTableManager *self);



/*******************************************************************************
 * Private function
 ******************************************************************************/
/**
 * Get the first node of the table construction object.<br>
 *
 * @param[in] self	Table construction object
 * @return			First node of the table construction object
 */
static M2MTableManager *this_begin (M2MTableManager *self)
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
		return NULL;
		}
	}


/**
 * Release the heap memory of column list held by table construction object.<br>
 *
 * @param[in,out] self	Table construction object
 */
static void this_deleteColumnList (M2MTableManager *self)
	{
	//========== Variable ==========
	M2MColumnList *columnList = NULL;
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MTableManager.this_deleteColumnList()";

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Check existence of column structure object =====
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
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MTableManager\" object is NULL");
		}
	return;
	}


/**
 * Release the heap memory of string indicating the table name.<br>
 *
 * @param[in] self	Table construction object
 */
static void this_deleteTableName (M2MTableManager *self)
	{
	//========== Variable ==========
	M2MString *tableName = NULL;
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MTableManager.this_deleteTableName()";

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Check existence of table name string =====
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
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MTableManager\" object is NULL");
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
	M2MSQLiteDataType dataType = 0;
	M2MString *dataTypeString = NULL;
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MTableManager.this_getColumnDefinition()";

	//===== Check argument =====
	if (columnList!=NULL && columnDefinition!=NULL)
		{
		//===== Get a column information object =====
		if ((column=M2MColumnList_getColumn(columnList))!=NULL
				&& (columnName=M2MColumn_getName(column))!=NULL
				&& (dataTypeString=M2MSQLiteDataType_toString(dataType=M2MColumn_getDataType(column)))!=NULL)
			{
			//===== Set column names =====
			M2MString_append(columnDefinition, M2MString_QUOTATION);
			M2MString_append(columnDefinition, columnName);
			M2MString_append(columnDefinition, (M2MString *)"' ");
			//===== Set data type =====
			M2MString_append(columnDefinition, dataTypeString);
			M2MString_append(columnDefinition, (M2MString *)M2MString_SPACE);
			//===== When NULL is allowed =====
			if (M2MColumn_getAllowNULL(column)==true)
				{
				}
			//===== When NULL is not permitted =====
			else
				{
				M2MString_append(columnDefinition, (M2MString *)"NOT NULL ");
				}
			//===== For primary key =====
			if (M2MColumn_getPrimaryKey(column)==true)
				{
				//===== Set primary key =====
				M2MString_append(columnDefinition, (M2MString *)"PRIMARY KEY ");
				//===== When the data type is an integer and auto increment is set =====
				if (dataType==M2MSQLiteDataType_INTEGER
						&& M2MColumn_getAutoIncrement(column)==true)
					{
					//===== Set auto increment =====
					M2MString_append(columnDefinition, (M2MString *)"AUTOINCREMENT ");
					}
				//===== When it is not the automatic increment setting =====
				else
					{
					// do nothing
					}
				}
			//===== When it is not a primary key =====
			else
				{
				// do nothing
				}
			//===== When the value is unique =====
			if (M2MColumn_getUnique(column)==true)
				{
				//===== Set unique setting =====
				M2MString_append(columnDefinition, (M2MString *)"UNIQUE ");
				}
			//===== When the value is not unique =====
			else
				{
				// do nothing
				}
			//===== Returns a string indicating the column definition =====
			return (*columnDefinition);
			}
		//===== Error handling =====
		else if (column==NULL)
			{
			M2MLogger_error(M2MColumnList_getLogger(columnList), FUNCTION_NAME, __LINE__, (M2MString *)"\"M2 MColumn *\" obtained from \"M2 MColumnList *\" specified as argument is NULL");
			return NULL;
			}
		else if (columnName==NULL)
			{
			M2MLogger_error(M2MColumnList_getLogger(columnList), FUNCTION_NAME, __LINE__, (M2MString *)"Column name which is a member variable of \"M2 MColumn *\" obtained from \"M2 MColumnList *\" specified as argument is NULL");
			return NULL;
			}
		else
			{
			M2MLogger_error(M2MColumnList_getLogger(columnList), FUNCTION_NAME, __LINE__, (M2MString *)"The string indicating the data type of \"M2MColumn *\" obtained from \"M2MColumnList *\" specified by argument is NULL");
			return NULL;
			}
		}
	//===== Argument error =====
	else if (columnList==NULL)
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"\"M2MColumnList *\" specified as argument is NULL");
		return NULL;
		}
	else
		{
		M2MLogger_error(M2MColumnList_getLogger(columnList), FUNCTION_NAME, __LINE__, (M2MString *)"\"ColumnDefinition\" specified as argument is NULL");
		return NULL;
		}
	}


/**
 * Returns a column structure object that the table construction object holds as a member variable.<br>
 *
 * @param[in] self	Table construction object
 * @return			Column structure object or NULL (in case of error)
 */
static M2MColumnList *this_getColumnList (const M2MTableManager *self)
	{
	//========== Variable ==========
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MTableManager.this_getColumnList()";

	//===== Check argument =====
	if (self!=NULL)
		{
		return self->columnList;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MTableManager\" object is NULL");
		return NULL;
		}
	}


/**
 * Get logger object owned by the argument "M2MTableManager" object.<br>
 *
 * @param[in] self	Table construction object
 * @return			Logger object
 */
static M2MFileAppender *this_getLogger (const M2MTableManager *self)
	{
	//========== Variable ==========
	M2MColumnList *columnList = NULL;
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MTableManager.this_getLogger()";

	//===== Check argument =====
	if (self!=NULL
			&& (columnList=this_getColumnList(this_begin((M2MTableManager *)self)))!=NULL)
		{
		//===== Return logger object owned by M2MColumnList object  =====
		return M2MColumnList_getLogger(columnList);
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MTableManager\" object is NULL");
		return NULL;
		}
	else
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MTableManager\" object hasn't \"M2MColumnList\" object as member variable");
		return NULL;
		}
	}


/**
 * @param[in] self	Table construction object
 * @param[out] sql	A buffer for copying string indicating a SQL statement for building a table (buffering itself is carried out within this function, so the caller only designates a pointer)
 * @return			Pointer of string copied to "sql" given as argument or NULL (in case of error)
 */
static M2MString *this_getTableCreateSQL (const M2MTableManager *self, M2MString **sql)
	{
	//========== Variable ==========
	M2MString *tableName = NULL;
	M2MColumnList *columnList = NULL;
	unsigned int columnListLength = 0;
	unsigned int i = 0;
	M2MString *columnDefinition = NULL;
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MTableManager.this_getTableCreateSQL()";

	//===== Check argument =====
	if (self!=NULL && sql!=NULL)
		{
		//===== Get table information =====
		if ((tableName=this_getTableName(self))!=NULL
				&& (columnList=M2MColumnList_begin(this_getColumnList(self)))!=NULL
				&& (columnListLength=M2MColumnList_length(columnList))>0)
			{
			//===== Set SQL command and table name =====
			M2MString_append(sql, (M2MString *)"CREATE TABLE '");
			M2MString_append(sql, tableName);
			//===== Prepare column definitions =====
			M2MString_append(sql, (M2MString *)"' (");
			//===== Repeat as many columns =====
			for (i=0; i<columnListLength; i++)
				{
				//===== Get column definition string =====
				if (this_getColumnDefinition(columnList, &columnDefinition)!=NULL)
					{
					//===== Set column definition string =====
					M2MString_append(sql, columnDefinition);
					//===== When it is not the last column definition =====
					if (i!=columnListLength-1)
						{
						//===== Add comma character =====
						M2MString_append(sql, (M2MString *)", ");
						}
					//===== In the case of the last column definition =====
					else
						{
						// do nothing
						}
					//===== Free heap memory area of column definition string =====
					M2MHeap_free(columnDefinition);
					}
				//===== When acquisition of column definition character string failed =====
				else
					{
					M2MLogger_error(this_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"An attempt to create a column definition character string has failed");
					//===== Free heap memory area =====
					M2MHeap_free(columnDefinition);
					M2MHeap_free((*sql));
					return NULL;
					}
				//===== Move to the next column =====
				columnList = M2MColumnList_next(columnList);
				}
			//===== Finish the creation of SQL statement =====
			M2MString_append(sql, (M2MString *)") ");
			//===== Return a SQL statement =====
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
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MTableManager\" object is NULL");
		return NULL;
		}
	else
		{
		M2MLogger_error(this_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"sql\" pointer is NULL");
		return NULL;
		}
	}


/**
 * Get the table name string held by the table construction object specified by the argument.<br>
 *
 * @param[in] self	Table construction object
 * @return			String indicating table name
 */
static M2MString *this_getTableName (const M2MTableManager *self)
	{
	//========== Variable ==========
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MTableManager.this_getTableName()";

	//===== Check argument =====
	if (self!=NULL)
		{
		return self->tableName;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MTableManager\" object is NULL");
		return NULL;
		}
	}


/**
 * Returns the next node of the table construction object specified by the argument.<br>
 *
 * @param[in] self	Table construction object
 * @return			Next node of the table construction object specified by argument or NULL (in case of error)
 */
static M2MTableManager *this_next (const M2MTableManager *self)
	{
	//========== Variable ==========
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MTableManager.this_next()";

	//===== Check argument =====
	if (self!=NULL)
		{
		return self->next;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MTableManager\" object is NULL");
		return NULL;
		}
	}


/**
 * Returns the previous node of the table construction object specified by the argument.<br>
 *
 * @param[in] self	Table construction object
 * @return			Node before the table construction object specified by argument or NULL (in case of error)
 */
static M2MTableManager *this_previous (const M2MTableManager *self)
	{
	//========== Variable ==========
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MTableManager.this_previous()";

	//===== Check argument =====
	if (self!=NULL)
		{
		return self->previous;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MTableManager\" object is NULL");
		return NULL;
		}
	}


/**
 * Set the column structure object as member variable of the table construction object.<br>
 *
 * @param[in] self			Table construction object
 * @param[in] columnList	Column structure object
 * @return					Table manager object or NULL (in case of error)
 */
static M2MTableManager *this_setColumnList (M2MTableManager *self, M2MColumnList *columnList)
	{
	//========== Variable ==========
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MTableManager.this_setColumnList()";

	//===== Check argument =====
	if (self!=NULL && columnList!=NULL)
		{
		self->columnList = columnList;
		return self;
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MTableManager\" object is NULL");
		return NULL;
		}
	else
		{
		M2MLogger_error(this_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MColumnList\" object is NULL");
		return NULL;
		}
	}


/**
 * Set the node of the table manager object located one behind the object specified as argument.<br>
 *
 * @param[in,out] self	Table construction object
 * @param[in] next		Table construction object
 * @return				Table construction object or NULL (in case of error)
 */
static M2MTableManager *this_setNext (M2MTableManager *self, M2MTableManager *next)
	{
	//========== Variable ==========
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MTableManager.this_setNext()";

	//===== Check argument =====
	if (self!=NULL)
		{
		self->next = next;
		return self;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MTableManager\" object is NULL");
		return NULL;
		}
	}


/**
 * Set the node of the table building object located immediately before the table <br>
 * building object specified by the argument.<br>
 *
 * @param[in,out] self	Table construction object
 * @param[in] previous	Table construction object
 * @return				Table construction object or NULL (in case of error)
 */
static M2MTableManager *this_setPrevious (M2MTableManager *self, M2MTableManager *previous)
	{
	//========== Variable ==========
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MTableManager.this_setPrevious()";

	//===== Check argument =====
	if (self!=NULL && previous!=NULL)
		{
		self->previous = previous;
		return self;
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MTableManager\" object is NULL");
		return NULL;
		}
	else
		{
		M2MLogger_error(this_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"previous\" object is NULL");
		return NULL;
		}
	}


/**
 * Set a string indicating the table name as a member variable of the table construction object.<br>
 *
 * @param[in,out] self		Table construction object
 * @param[in] tableName		Table construction object
 * @return					Table construction object or NULL (in case of error)
 */
static M2MTableManager *this_setTableName (M2MTableManager *self, const M2MString *tableName)
	{
	//========== Variable ==========
	size_t tableNameLength = 0;
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MTableManager.this_setTableName()";

	//===== Check argument =====
	if (self!=NULL && tableName!=NULL)
		{
		//===== Get length of string indicating table name =====
		if ((tableNameLength=M2MString_length(tableName))>0)
			{
			//===== Initialization of member variable indicating table name =====
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
				M2MLogger_error(this_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Failed to get the heap memory area for copying the character string indicating the table name");
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			M2MLogger_error(this_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"The length of the string of \"tableName\" specified by argument is 0 or less");
			return NULL;
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MTableManager\" object is NULL");
		return NULL;
		}
	else
		{
		M2MLogger_error(this_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"tableName\" string is NULL");
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
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MTableManager_createTable()";

	//===== Check argument =====
	if (self!=NULL && database!=NULL)
		{
		//===== Get the head node of table building object =====
		if ((self=this_begin(self))!=NULL)
			{
			//===== Repeat until reaching the end node =====
			while ((next=this_next(self))!=NULL)
				{
				//===== When a table with the same name exists =====
				if (M2MSQLite_isExistingTable(database, this_getTableName(self))==true)
					{
					// do nothing
					}
				//===== When there is no table with the same name =====
				else
					{
					//===== When executing SQL statement for table construction =====
					if (executeUpdate==true)
						{
						// do nothing
						}
					//===== When the SQL statement for table construction has not been executed yet =====
					else
						{
						M2MLogger_debug(this_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Start the construction process of CEP table");
						//===== Start transaction =====
						M2MSQLite_beginTransaction(database);
						//===== Set a flag =====
						executeUpdate = true;
						}
					//===== Get string of SQL statement for building table =====
					if (this_getTableCreateSQL(self, &sql)!=NULL)
						{
						//===== Execute SQL =====
						if (M2MSQLite_executeUpdate(database, sql)==true)
							{
							M2MLogger_debug(this_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"CREATE statement for CEP table construction was executed");
							}
						//===== Error handling =====
						else
							{
							M2MLogger_error(this_getLogger(self), FUNCTION_NAME, __LINE__, M2MSQLite_getErrorMessage(database));
							}
						//===== Free heap memory area of SQL statement =====
						M2MHeap_free(sql);
						}
					//===== Error handling =====
					else
						{
						memset(MESSAGE, 0, sizeof(MESSAGE));
						snprintf(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Failed to retrieve SQL statement for building table(= \"%s\")", this_getTableName(self));
						M2MLogger_error(this_getLogger(self), FUNCTION_NAME, __LINE__, MESSAGE);
						}
					}
				//===== Proceed to the next node =====
				self = next;
				}
			//===== When a table with the same name exists =====
			if (M2MSQLite_isExistingTable(database, this_getTableName(self))==true)
				{
				// do nothing
				}
			//===== When there is no table with the same name =====
			else
				{
				//===== When executing SQL statement for table construction =====
				if (executeUpdate==true)
					{
					// do nothing
					}
				//===== When the SQL statement for table construction has not been executed yet =====
				else
					{
					M2MLogger_debug(this_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Start the construction process of CEP table");
					//===== Start transaction =====
					M2MSQLite_beginTransaction(database);
					//===== Set a flag =====
					executeUpdate = true;
					}
				//===== Retrieve the character string of the SQL statement for constructing the table of the terminal node =====
				if (this_getTableCreateSQL(self, &sql)!=NULL)
					{
					//===== Execute SQL =====
					if (M2MSQLite_executeUpdate(database, sql)==true)
						{
						M2MLogger_debug(this_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"CREATE statement for CEP table construction was executed");
						}
					//===== Error handling =====
					else
						{
						M2MLogger_error(this_getLogger(self), FUNCTION_NAME, __LINE__, M2MSQLite_getErrorMessage(database));
						}
					//===== Free heap memory area of SQL statement =====
					M2MHeap_free(sql);
					}
				//===== Error handling =====
				else
					{
					memset(MESSAGE, 0, sizeof(MESSAGE));
					snprintf(MESSAGE, sizeof(MESSAGE)-1, (M2MString *)"Failed to retrieve SQL statement for building table(=\"%s\")", this_getTableName(self));
					M2MLogger_error(this_getLogger(self), FUNCTION_NAME, __LINE__, MESSAGE);
					}
				}
			//===== When executing SQL statement for table construction =====
			if (executeUpdate==true)
				{
				//===== End transaction =====
				M2MSQLite_commitTransaction(database);
				M2MLogger_debug(this_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Finish CEP table construction processing");
				}
			//===== When the SQL statement for table construction has not been executed yet =====
			else
				{
				// do nothing
				}
			//===== Successful completion =====
			return;
			}
		//===== Error handling =====
		else
			{
			M2MLogger_error(this_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Failed to get the first node from \"M2MTableManager *\" specified by argument");
			return;
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MTableManager\" object is NULL");
		return;
		}
	else
		{
		M2MLogger_error(this_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"database\" object is NULL");
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
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MTableManager_delete()";

	//===== Check argument =====
	if (self!=NULL && (*self)!=NULL)
		{
		//===== Get the head node of the table construction object =====
		if (((*self)=this_begin((*self)))!=NULL)
			{
			//===== Repeat until reaching the end node =====
			while ((*self)!=NULL && (next=this_next((*self)))!=NULL)
				{
				//===== Free heap memory area of table building object =====
				this_deleteTableName((*self));
				this_deleteColumnList((*self));
				M2MHeap_free((*self));
				//===== Move to the next node =====
				(*self) = next;
				}
			//===== Confirm (end) table building object =====
			if ((*self)!=NULL)
				{
				//===== Releases heap memory area of (end) table building object =====
				this_deleteTableName((*self));
				this_deleteColumnList((*self));
				M2MHeap_free((*self));
				}
			//===== Error handling =====
			else
				{
				}
			}
		//===== Error handling =====
		else
			{
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MTableManager\" object is NULL");
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
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MTableManager_getColumnList()";

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
			M2MLogger_error(this_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Failed to get begin node from indicated \"M2MTableManager\" object");
			return NULL;
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MTableManager\" object is NULL");
		return NULL;
		}
	else
		{
		M2MLogger_error(this_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"tableName\" string is NULL");
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
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MTableManager_new()";

	//===== Get heap memory =====
	if ((self=(M2MTableManager *)M2MHeap_malloc(sizeof(M2MTableManager)))!=NULL)
		{
		//===== Set member variables =====
		this_setPrevious(self, self);
		this_setNext(self, NULL);
		return self;
		}
	//===== Error handling =====
	else
		{
		M2MLogger_error(this_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Failed to allocate new memory for creating \"M2MTableManager\" structure object");
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
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MTableManager_setConfig()";

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
			M2MLogger_error(this_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Failed to set table name or column structure object in \"M2MTableManager\" object");
			return NULL;
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_error(NULL, FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MTableManager\" object is NULL");
		return NULL;
		}
	else if (tableName==NULL)
		{
		M2MLogger_error(this_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"tableName\" string is NULL");
		return NULL;
		}
	else
		{
		M2MLogger_error(this_getLogger(self), FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MColumnList\" object is NULL");
		return NULL;
		}
	}



/* End Of File */
