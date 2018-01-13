/*******************************************************************************
 * M2MColumn.c
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

#include "m2m/db/M2MColumn.h"


/*******************************************************************************
 * Private function
 ******************************************************************************/
/**
 * Release heap memory of string indicating the column name.<br>
 *
 * @param[in,out] self	Column information object
 */
static void this_deleteName (M2MColumn *self)
	{
	//========== Variable ==========
	M2MString *name = NULL;
	const M2MString *METHOD_NAME = (M2MString *)"M2MColumn.this_deleteName()";

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Check existence of column name =====
		if ((name=M2MColumn_getName(self))!=NULL)
			{
			//===== Release the heap memory =====
			M2MHeap_free(name);
			}
		//===== Error handling =====
		else
			{
			M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to get column name string", NULL);
			}
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MColumn\" object is NULL", NULL);
		}
	return;
	}



/*******************************************************************************
 * Public function
 ******************************************************************************/
/**
 * Destructor.<br>
 * Release the heap memory of column information object.<br>
 *
 * @param[in,out] self	Column information object
 */
void M2MColumn_delete (M2MColumn **self)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MColumn_delete()";

	//===== Check argument =====
	if (self!=NULL && (*self)!=NULL)
		{
		//=====  =====
		this_deleteName((*self));
		M2MHeap_free((*self));
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MColumn\" object is NULL", NULL);
		}
	return;
	}


/**
 * Return string indicating the column name held by column information object.<br>
 *
 * @param[in] self	Column information object
 * @return			String indicating the column name or NULL (in case of error)
 */
unsigned char *M2MColumn_getName (const M2MColumn *self)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MColumn_getName()";

	//===== Check argument =====
	if (self!=NULL)
		{
		return self->name;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MColumn\" object is NULL", NULL);
		return NULL;
		}
	}


/**
 * Return the data type held by column information object.<br>
 *
 * @param[in] self	Column information object
 */
M2MDataType M2MColumn_getDataType (const M2MColumn *self)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MColumn_getDataType()";

	//===== Check argument =====
	if (self!=NULL)
		{
		return self->dataType;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MColumn\" object is NULL", NULL);
		return M2M_DATA_TYPE_ERROR;
		}
	}


/**
 * Return flag for primary key activation held by column information object.<br>
 *
 * @param[in] self	Column information object
 */
bool M2MColumn_getPrimaryKey (const M2MColumn *self)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MColumn_getPrimaryKey()";

	//===== Check argument =====
	if (self!=NULL)
		{
		return self->primaryKey;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MColumn\" object is NULL", NULL);
		return false;
		}
	}


/**
 * Return flag for auto increment enable held by column information object.<br>
 *
 * @param[in] self	Column information object
 */
bool M2MColumn_getAutoIncrement (const M2MColumn *self)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MColumn_getAutoIncrement()";

	//===== Check argument =====
	if (self!=NULL)
		{
		return self->autoIncrement;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MColumn\" object is NULL", NULL);
		return false;
		}
	}


/**
 * Return flag for NULL activation held by column information object.<br>
 *
 * @param[in] self	Column information object
 */
bool M2MColumn_getAllowNULL (const M2MColumn *self)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MColumn_getAllowNULL()";

	//===== Check argument =====
	if (self!=NULL)
		{
		return self->allowNULL;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MColumn\" object is NULL", NULL);
		return false;
		}
	}


/**
 * Return flag of uniqueness validation held by column information object.<br>
 *
 * @param[in] self	Column information object
 */
bool M2MColumn_getUnique (const M2MColumn *self)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MColumn_getUnique()";

	//===== Check argument =====
	if (self!=NULL)
		{
		return self->unique;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MColumn\" object is NULL", NULL);
		return false;
		}
	}


/**
 * Constructor.<br>
 * Get the heap memory and create a new column information object.
 *
 * @return	Created new column information object or NULL (in case of error)
 */
M2MColumn *M2MColumn_new ()
	{
	//========== Variable ==========
	M2MColumn *self = NULL;
	const M2MString *METHOD_NAME = (M2MString *)"M2MColumn_new()";

	//===== Get heap memory =====
	if ((self=(M2MColumn *)M2MHeap_malloc(sizeof(M2MColumn)))!=NULL)
		{
		//===== Initialize column information object =====
		if (M2MColumn_setAllowNULL(self, false)!=NULL
				&& M2MColumn_setAutoIncrement(self, false)!=NULL
				&& M2MColumn_setDataType(self, M2M_DATA_TYPE_ERROR)!=NULL
				&& M2MColumn_setPrimaryKey(self, false)!=NULL
				&& M2MColumn_setUnique(self, false)!=NULL)
			{
			return self;
			}
		//===== Error handling =====
		else
			{
			M2MColumn_delete(&self);
			M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to initialize a new column information object", NULL);
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


/**
 * Set flag whether auto increment is enabled or not in column information object.<br>
 *
 * @param[in,out] self		Column information object
 * @param[in] autoIncrement	Flag indicating whether auto increment
 * @return					Column information object with automatic increment information set or NULL (in case of error)
 */
M2MColumn *M2MColumn_setAutoIncrement (M2MColumn *self, const bool autoIncrement)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MColumn_setAutoIncrement()";

	//===== Check argument =====
	if (self!=NULL)
		{
		self->autoIncrement = autoIncrement;
		return self;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MColumn\" object is NULL", NULL);
		return NULL;
		}
	}


/**
 * Set a flag indicating whether NULL is allowed or not in the record.<br>
 *
 * @param[in,out] self	Column information object
 * @param[in] allowNULL	Flag indicating whether NULL is allowed for record
 * @return				Column information object set with flag indicating NULL availability or NULL (in case of error)
 */
M2MColumn *M2MColumn_setAllowNULL (M2MColumn *self, const bool allowNULL)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MColumn_setAllowNULL()";

	//===== Check argument =====
	if (self!=NULL)
		{
		self->allowNULL = allowNULL;
		return self;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MColumn\" object is NULL", NULL);
		return NULL;
		}
	}


/**
 * Set an enumerator indicating the data type of the column information object.<br>
 *
 * @param[in,out] self	Column information object
 * @param[in] dataType	Enumerator for data type
 * @return				Column information object with enumerator indicating data type or NULL (in case of error)
 */
M2MColumn *M2MColumn_setDataType (M2MColumn *self, const M2MDataType dataType)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MColumn_setDataType()";

	//===== Check argument =====
	if (self!=NULL)
		{
		self->dataType = dataType;
		return self;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MColumn\" object is NULL", NULL);
		return NULL;
		}
	}


/**
 * Set string indicating a column name in column information object.<br>
 *
 * @param[in,out] self	Column information object
 * @param[in] name		String indicating the column name
 * @return				Column information object with column name set or NULL (in case of error)
 */
M2MColumn *M2MColumn_setName (M2MColumn *self, const M2MString *name)
	{
	//========== Variable ==========
	size_t nameLength = 0;
	const M2MString *METHOD_NAME = (M2MString *)"M2MColumn_setName()";

	//===== Check argument =====
	if (self!=NULL && name!=NULL && (nameLength=M2MString_length(name))>0)
		{
		//===== Initialize variable =====
		this_deleteName(self);
		//===== Copy the column name into member variable =====
		if (M2MString_appendLength(&(self->name), name, nameLength)!=NULL)
			{
			return self;
			}
		//===== Error handling =====
		else
			{
			M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to get heap memory for copying the column name string", NULL);
			return NULL;
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MColumn\" object is NULL", NULL);
		return NULL;
		}
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"name\" string is NULL or vacant", NULL);
		return NULL;
		}
	}


/**
 * Set a flag indicating whether or not it is a primary key.<br>
 *
 * @param[in,out] self		Column information object
 * @param[in] primaryKey	Flag indicating whether or not it is a primary key
 * @return					Column information object with flag set or NULL (in case of error)
 */
M2MColumn *M2MColumn_setPrimaryKey (M2MColumn *self, const bool primaryKey)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MColumn_setPrimaryKey()";

	//===== Check argument =====
	if (self!=NULL)
		{
		self->primaryKey = primaryKey;
		return self;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MColumn\" object is NULL", NULL);
		return NULL;
		}
	}


/**
 * Set a flag indicating the uniqueness of the record in column information object.<br>
 *
 * @param[in,out] self	Column information object
 * @param[in] unique	Flag indicating whether or not the record data has uniqueness
 * @return				Column information object or flag with flag indicating whether uniqueness is set or NULL (in case of error)
 */
M2MColumn *M2MColumn_setUnique (M2MColumn *self, const bool unique)
	{
	//========== Variable ==========
	const M2MString *METHOD_NAME = (M2MString *)"M2MColumn_setUnique()";

	//===== Check argument =====
	if (self!=NULL)
		{
		self->unique = unique;
		return self;
		}
	//===== Argument error =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"M2MColumn\" object is NULL", NULL);
		return NULL;
		}
	}



/* End Of File */
