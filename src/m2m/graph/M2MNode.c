/*******************************************************************************
 * M2MNode.c
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

#include "m2m/graph/M2MNode.h"



/*******************************************************************************
 * Private variable
 ******************************************************************************/
/**
 * Initialize variable for random seed.<br>
 */
static bool this_initSeed = true;



/*******************************************************************************
 * Definition of private function
 ******************************************************************************/
/**
 * Return initialized seed variable.<br>
 *
 * @param[in] x	A seed variable for Xorshift algorithm
 * @return		Initialized seed variable
 */
static uint32_t this_initSeedVariable (const uint32_t x);



/*******************************************************************************
 * Private function
 ******************************************************************************/
/**
 * Get randomized 32[bit] unsigned integer number.<br>
 *
 * @return	Randomized 32[bit] unsigned integer number
 */
static uint32_t this_createNewNodeID ()
	{
	//========== Variable ==========
	static uint32_t x = 123456789UL;
	static uint32_t y = 362436069UL;
	static uint32_t z = 521288629UL;
	static uint32_t w = 88675123UL;
	uint32_t t = 0UL;
	uint32_t nodeID = 0UL;

	//===== Initialize seed (once executed) =====
	x = this_initSeedVariable(x);
	//===== Get Xorshift random number =====
	t = x ^ (x << 11);
	x = y;
	y = z;
	z = w;
	nodeID = (w = (w ^ (w >> 19)) ^ (t ^ (t >> 8)));
	return nodeID;
	}


/**
 * Only when called in the first time, multiply the input value by a random number. <br>
 *
 * @param[in] x	Input number
 * @return		Randomized number or intact input value
 */
static uint32_t this_initSeedVariable (const uint32_t x)
	{
	//========== Variable ==========
	float random = 0.0;

	//===== In case of first time =====
	if (this_initSeed==true)
		{
		//===== Lower the flag =====
		this_initSeed = false;
		//===== Get random number =====
		srand((unsigned int) time(0));
		random = (rand() + 0.5) / (RAND_MAX + 1);
		//===== Return the randomized number =====
		return (uint32_t)(random * x);
		}
	//===== In case of the second and subsequent =====
	else
		{
		return x;
		}
	}


/**
 * @param[in] database	SQLite3 database object
 * @return				true: , false:
 */
static bool this_createNewTable (sqlite3 *database)
	{
	//========== Variable ==========
	M2MString buffer[512];
	const M2MString *SQL_FORMAT = (M2MString *)"CREATE TABLE %s (%s TEXT PRIMARY KEY  NOT NULL  UNIQUE, %s TEXT NOT NULL  UNIQUE, %s TEXT, %s NUMERIC, %s NUMERIC) ";

	//===== Check argument =====
	if (database!=NULL)
		{
		//===== Check the existence of the table =====
		if (M2MSQLiteConfig_isExistingTable(database, M2MNode_TABLE_NAME)==true)
			{
			return true;
			}
		//===== In case of not existing the table =====
		else
			{
			//===== Create new SQL string =====
			memset(buffer, 0, sizeof(buffer));
			snprintf(buffer,
					sizeof(buffer)-1,
					SQL_FORMAT,
					M2MNode_TABLE_NAME,
					M2MNode_COLUMN_ID,
					M2MNode_COLUMN_NAME,
					M2MNode_COLUMN_PROPERTY,
					M2MNode_COLUMN_LEFT,
					M2MNode_COLUMN_RIGHT);
			//===== Startup transaction =====
			M2MSQLRunner_beginTransaction(database);
			//===== Execute SQL =====
			if (M2MSQLRunner_executeUpdate(database, buffer)==true)
				{
				}
			//===== Error handling =====
			else
				{
				}
			//===== Shutdown transaction =====
			M2MSQLRunner_commitTransaction(database);
			return true;
			}
		}
	//===== Argument error =====
	else
		{
		return false;
		}
	}


/**
 * @param[in] database	SQLite3 database object
 * @param[in] name		String indicating node name
 * @param[in] property	String indicating property belonging to the node or NULL
 * @return				Number indicating node ID which is unique in "m2m_node" table or 0 (in case of error)
 */
static uint32_t this_setRecord (sqlite3 *database, const M2MString *name, const M2MString *property)
	{
	//========== Variable ==========
	size_t nameLength = 0;
	size_t propertyLength = 0;
	uint32_t nodeID = 0;
	M2MString hexadecimalString[16];
	M2MString buffer[512];
	sqlite3_stmt *statement = NULL;
	int result = 0;
	const M2MString *SQL_FORMAT = (M2MString *)"INSERT INTO %s (%s,%s,%s) VALUES (?,?,?) ";

	//===== Check argument =====
	if (database!=NULL
			&& name!=NULL && (nameLength=M2MString_length(name))>0)
		{
		memset(hexadecimalString, 0, sizeof(hexadecimalString));
		//===== Create new node ID =====
		if ((nodeID=this_createNewNodeID())>0
				&& M2MString_convertFromUnsignedLongToHexadecimalString(nodeID, hexadecimalString, sizeof(hexadecimalString))!=NULL)
			{
			memset(buffer, 0, sizeof(buffer));
			snprintf(buffer, sizeof(buffer)-1, SQL_FORMAT,
					M2MNode_TABLE_NAME,
					M2MNode_COLUMN_ID,
					M2MNode_COLUMN_NAME,
					M2MNode_COLUMN_PROPERTY);
			//=====  =====
			if ((statement=M2MSQLiteConfig_getPreparedStatement(database, buffer))!=NULL)
				{
				M2MSQLiteConfig_setValueIntoPreparedStatement(M2MDataType_TEXT, 1, hexadecimalString, M2MString_length(hexadecimalString), statement);
				M2MSQLiteConfig_setValueIntoPreparedStatement(M2MDataType_TEXT, 2, name, nameLength, statement);
				//===== In the case of existing node property information =====
				if (property!=NULL && (propertyLength=M2MString_length(property))>0)
					{
					//=====  =====
					M2MSQLiteConfig_setValueIntoPreparedStatement(M2MDataType_TEXT, 3, property, propertyLength, statement);
					}
				//=====  =====
				else
					{
					//=====  =====
					M2MSQLiteConfig_setValueIntoPreparedStatement(M2MDataType_NULL, 3, property, 0, statement);
					}
				//===== Set a new record indicating a node =====
				if ((result=M2MSQLiteConfig_next(statement))==SQLITE_ROW || result==SQLITE_DONE)
					{
					//===== Release prepared statement object =====
					sqlite3_finalize(statement);
					return nodeID;
					}
				//===== Error handling =====
				else
					{
					//===== Release prepared statement object =====
					sqlite3_finalize(statement);
					return 0;
					}
				}
			//===== Error handling =====
			else
				{
				return 0;
				}
			}
		//===== Error handling =====
		else if (nodeID<=0)
			{
			return 0;
			}
		else
			{
			return 0;
			}
		}
	//===== Argument error =====
	else if (database==NULL)
		{
		return 0;
		}
	else
		{
		return 0;
		}
	}


/**
 * @param[in] database	SQLite3 database object
 * @param[in] nodeID	Hexadecimal string indicating node ID number
 * @param[in] left
 * @param[in] right
 * @return
 */
static bool this_updateNestedSetsModel (sqlite3 *database, const M2MString *nodeID, const M2MString *left, const M2MString *right)
	{
	//========== Variable ==========
	size_t nodeIDLength = 0;
	size_t leftLength = 0;
	size_t rightLength = 0;
	M2MString buffer[512];
	sqlite3_stmt *statement = NULL;
	int result = 0;
	const M2MString *SQL_FORMAT = (M2MString *)"UPDATE %s SET %s = ?, %s = ? WHERE %s = ? ";

	//===== Check argument =====
	if (database!=NULL
			&& nodeID!=NULL && (nodeIDLength=M2MString_length(nodeID))>0)
		{
		//=====  =====
		memset(buffer, 0, sizeof(buffer));
		snprintf(buffer, sizeof(buffer)-1, SQL_FORMAT,
				M2MNode_TABLE_NAME,
				M2MNode_COLUMN_LEFT,
				M2MNode_COLUMN_RIGHT,
				M2MNode_COLUMN_ID);
		//=====  =====
		if ((statement=M2MSQLiteConfig_getPreparedStatement(database, buffer))!=NULL)
			{
			//===== In case of setting Nested Sets Model values =====
			if (left!=NULL && (leftLength=M2MString_length(left))>0
					&& right!=NULL && (rightLength=M2MString_length(right))>0)
				{
				//===== Set values into statement =====
				if (M2MSQLiteConfig_setValueIntoPreparedStatement(M2MDataType_NUMERIC, 1, left, leftLength, statement)==true
						&& M2MSQLiteConfig_setValueIntoPreparedStatement(M2MDataType_NUMERIC, 2, right, rightLength, statement)==true
						&& M2MSQLiteConfig_setValueIntoPreparedStatement(M2MDataType_TEXT, 3, nodeID, nodeIDLength, statement)==true
						&& ((result=M2MSQLiteConfig_next(statement))==SQLITE_ROW || result==SQLITE_DONE))
					{
					sqlite3_finalize(statement);
					return true;
					}
				//===== Error handling =====
				else
					{
					sqlite3_finalize(statement);
					return false;
					}
				}
			//===== In case of initializing values =====
			else if (left==NULL && right==NULL)
				{
				//===== Set values into statement =====
				if (M2MSQLiteConfig_setValueIntoPreparedStatement(M2MDataType_NULL, 1, NULL, 0, statement)==true
						&& M2MSQLiteConfig_setValueIntoPreparedStatement(M2MDataType_NULL, 2, NULL, 0, statement)==true
						&& M2MSQLiteConfig_setValueIntoPreparedStatement(M2MDataType_TEXT, 3, nodeID, nodeIDLength, statement)==true
						&& ((result=M2MSQLiteConfig_next(statement))==SQLITE_ROW || result==SQLITE_DONE))
					{
					sqlite3_finalize(statement);
					return true;
					}
				//===== Error handling =====
				else
					{
					sqlite3_finalize(statement);
					return false;
					}
				}
			//===== Error handling =====
			else
				{
				sqlite3_finalize(statement);
				return false;
				}
			}
		//===== Error handling =====
		else
			{
			return false;
			}
		}
	//===== Argument error =====
	else if (database==NULL)
		{
		return false;
		}
	else if (nodeID==NULL || M2MString_length(nodeID)<=0)
		{
		return false;
		}
	else
		{
		return false;
		}
	}



/*******************************************************************************
 * Public function
 ******************************************************************************/
/**
 * Destructor.<br>
 * Delete a record indicated with ID in the "m2m_node" table.
 *
 * @param[in] database	SQLite3 database object
 * @param[in] nodeID	Number indicating node ID which is unique in "m2m_node" table
 */
void M2MNode_delete (sqlite3 *database, const uint32_t nodeID)
	{
	//========== Variable ==========
	M2MString hexadecimalString[16];
	M2MString buffer[512];
	sqlite3_stmt *statement = NULL;
	int result = 0;
	const M2MString *SQL_FORMAT = (M2MString *)"DELETE FROM %s WHERE %s = ? ";

	//===== Check argument =====
	if (database!=NULL)
		{
		memset(hexadecimalString, 0, sizeof(hexadecimalString));
		//===== Convert ID number into hexadecimal string =====
		if (M2MString_convertFromUnsignedLongToHexadecimalString(nodeID, hexadecimalString, sizeof(hexadecimalString))!=NULL)
			{
			//===== Prepare SQL statement =====
			memset(buffer, 0, sizeof(buffer));
			snprintf(buffer, sizeof(buffer)-1, SQL_FORMAT, M2MNode_TABLE_NAME, M2MNode_COLUMN_ID);
			//===== Get prepared statement =====
			if ((statement=M2MSQLiteConfig_getPreparedStatement(database, buffer))!=NULL)
				{
				//===== Execute SQL statement =====
				if (M2MSQLiteConfig_setValueIntoPreparedStatement(M2MDataType_TEXT, 1, hexadecimalString, M2MString_length(hexadecimalString), statement)==true
						&& ((result=M2MSQLiteConfig_next(statement))==SQLITE_ROW || result==SQLITE_DONE))
					{
					}
				//===== Error handling =====
				else
					{
					}
				//===== Release prepared statement object =====
				sqlite3_finalize(statement);
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
		}
	return;
	}


/**
 * @param[in] database	SQLite3 database object
 * @param[in] name		String indicating node name
 * @return				Number indicating node ID which is unique in "m2m_node" table
 */
uint32_t M2MNode_getID (sqlite3 *database, const M2MString *name)
	{
	//========== Variable ==========
	size_t nameLength = 0;
	M2MString buffer[512];
	sqlite3_stmt *statement = NULL;
	uint32_t nodeID = 0UL;
	int result = 0;
	M2MString *text = NULL;
	const M2MString *SQL_FORMAT = (M2MString *)"SELECT %s FROM %s WHERE %s = ? ";

	//===== Check argument =====
	if (database!=NULL
			&& name!=NULL && (nameLength=M2MString_length(name))>0)
		{
		//===== Prepare SQL statement =====
		memset(buffer, 0, sizeof(buffer));
		snprintf(buffer, sizeof(buffer)-1, SQL_FORMAT, M2MNode_COLUMN_ID, M2MNode_TABLE_NAME, M2MNode_COLUMN_NAME);
		//===== Get prepared statement =====
		if ((statement=M2MSQLiteConfig_getPreparedStatement(database, buffer))!=NULL)
			{
			//===== Execute SQL =====
			if (M2MSQLiteConfig_setValueIntoPreparedStatement(M2MDataType_TEXT, 1, name, nameLength, statement)==true
					&& ((result=M2MSQLiteConfig_next(statement))==SQLITE_ROW || result==SQLITE_DONE)
					&& (text=(M2MString *)sqlite3_column_text(statement, 0))!=NULL
					&& (nodeID=M2MString_convertFromHexadecimalStringToUnsignedLong(text, M2MString_length(text)))>0UL)
				{
				//===== Release prepared statement object =====
				sqlite3_finalize(statement);
				return nodeID;
				}
			//===== Error handling =====
			else
				{
				//===== Release prepared statement object =====
				sqlite3_finalize(statement);
				return 0UL;
				}
			}
		//===== Error handling =====
		else
			{
			return 0UL;
			}
		}
	//===== Argument error =====
	else if (database==NULL)
		{
		return 0UL;
		}
	else
		{
		return 0UL;
		}
	}


/**
 * @param[in] database	SQLite3 database object
 * @param[in] nodeID	Number indicating node ID which is unique in "m2m_node" table
 * @param[out] name		Pointer to copying the node name (buffering is executed inside this function)
 * @return				String indicating node name or NULL (in case of error)
 */
M2MString *M2MNode_getName (sqlite3 *database, const uint32_t nodeID, M2MString **name)
	{
	//========== Variable ==========
	M2MString hexadecimalString[16];
	M2MString buffer[512];
	sqlite3_stmt *statement = NULL;
	int result = 0;
	const M2MString *SQL_FORMAT = (M2MString *)"SELECT %s FROM %s WHERE %s = ? ";

	//===== Check argument =====
	if (database!=NULL)
		{
		memset(hexadecimalString, 0, sizeof(hexadecimalString));
		//===== Convert ID number into hexadecimal string =====
		if (M2MString_convertFromUnsignedLongToHexadecimalString(nodeID, hexadecimalString, sizeof(hexadecimalString))!=NULL)
			{
			memset(buffer, 0, sizeof(buffer));
			snprintf(buffer, sizeof(buffer)-1, SQL_FORMAT, M2MNode_COLUMN_NAME, M2MNode_TABLE_NAME, M2MNode_COLUMN_ID);
			//===== Get SQLite3 prepared statement =====
			if ((statement=M2MSQLiteConfig_getPreparedStatement(database, buffer))!=NULL)
				{
				//=====  =====
				if (M2MSQLiteConfig_setValueIntoPreparedStatement(M2MDataType_TEXT, 1, hexadecimalString, M2MString_length(hexadecimalString), statement)==true
						&& ((result=M2MSQLiteConfig_next(statement))==SQLITE_ROW || result==SQLITE_DONE)
						&& M2MString_append(name, sqlite3_column_text(statement, 0))!=NULL)
					{
					//===== Release prepared statement object =====
					sqlite3_finalize(statement);
					return (*name);
					}
				//===== Error handling =====
				else
					{
					//===== Release prepared statement object =====
					sqlite3_finalize(statement);
					return NULL;
					}
				}
			//===== Error handling =====
			else
				{
				return NULL;
				}
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


/**
 * @param[in] database	SQLite3 database object
 * @param[in] nodeID	Number indicating node ID which is unique inside "m2m_node" table
 * @param[out] property	Pointer to copying property belonging to the node
 * @return				String indicating node name or NULL (in case of error)
 */
M2MString *M2MNode_getProperty (sqlite3 *database, const uint32_t nodeID, M2MString **property)
	{
	//========== Variable ==========
	M2MString hexadecimalString[16];
	M2MString buffer[512];
	sqlite3_stmt *statement = NULL;
	int result = 0;
	const M2MString *SQL_FORMAT = (M2MString *)"SELECT %s FROM %s WHERE %s = ? ";

	//===== Check argument =====
	if (database!=NULL)
		{
		memset(hexadecimalString, 0, sizeof(hexadecimalString));
		//===== Convert ID number into hexadecimal string =====
		if (M2MString_convertFromUnsignedLongToHexadecimalString(nodeID, hexadecimalString, sizeof(hexadecimalString))!=NULL)
			{
			memset(buffer, 0, sizeof(buffer));
			snprintf(buffer, sizeof(buffer)-1, SQL_FORMAT, M2MNode_COLUMN_PROPERTY, M2MNode_TABLE_NAME, M2MNode_COLUMN_ID);
			//===== Get SQLite3 prepared statement =====
			if ((statement=M2MSQLiteConfig_getPreparedStatement(database, buffer))!=NULL)
				{
				//=====  =====
				if (M2MSQLiteConfig_setValueIntoPreparedStatement(M2MDataType_TEXT, 1, hexadecimalString, M2MString_length(hexadecimalString), statement)==true
						&& ((result=M2MSQLiteConfig_next(statement))==SQLITE_ROW || result==SQLITE_DONE)
						&& M2MString_append(property, sqlite3_column_text(statement, 0))!=NULL)
					{
					//===== Release prepared statement object =====
					sqlite3_finalize(statement);
					return (*property);
					}
				//===== Error handling =====
				else
					{
					//===== Release prepared statement object =====
					sqlite3_finalize(statement);
					return NULL;
					}
				}
			//===== Error handling =====
			else
				{
				return NULL;
				}
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


/**
 * Constructor.<br>
 * Set a new record into the "m2m_node" table.<br>
 *
 * @param[in] database	SQLite3 database object
 * @param[in] name		String indicating unique node name in "m2m_node" table
 * @param[in] property	String indicating property belonging to the node or NULL
 * @return				Number indicating node ID which is unique in "m2m_node" table
 */
uint32_t M2MNode_new (sqlite3 *database, const M2MString *name, const M2MString *property)
	{
	//========== Variable ==========
	size_t nameLength = 0;

	//===== Check argument =====
	if (name!=NULL && (nameLength=M2MString_length(name))>0)
		{
		//===== Check the existence of table =====
		if (this_createNewTable(database)==true)
			{
			//===== Set a new record into table =====
			return this_setRecord(database, name, property);
			}
		//===== Error handling =====
		else
			{
			return 0UL;
			}
		}
	//===== Argument error =====
	else if (database==NULL)
		{
		return 0UL;
		}
	else if (name==NULL || nameLength<=0)
		{
		return 0UL;
		}
	else
		{
		return 0UL;
		}
	}


/**
 * Set left & right parameters for Nested Sets Model into a node.<br>
 * If initialize them, set 0 as their arguments in the same time.<br>
 *
 * @param[in] database	SQLite3 database object
 * @param[in] nodeID	Number indicating node ID which is unique in "m2m_node" table
 * @param[in] left		Nested Sets Model parameter (>=1) or 0 (in case of initialization)
 * @param[in] right		Nested Sets Model parameter (>=1) or 0 (in case of initialization)
 * @return				Number indicating node ID which was set Nested Sets Model parameter or 0 (in case of error)
 */
uint32_t M2MNode_setNestedSetsModel (sqlite3 *database, const uint32_t nodeID, const uint32_t left, const uint32_t right)
	{
	//========== Variable ==========
	M2MString hexadecimalString[16];
	M2MString leftString[16];
	M2MString rightString[16];

	//===== Check argument =====
	if (database!=NULL && nodeID>0)
		{
		memset(hexadecimalString, 0, sizeof(hexadecimalString));
		//===== Convert node ID into hexadecimal string =====
		if (M2MString_convertFromUnsignedLongToHexadecimalString(nodeID, hexadecimalString, sizeof(hexadecimalString))!=NULL)
			{
			//===== In case of setting parameters =====
			if (left>0 && right>0 && left!=right
					&& M2MString_convertFromUnsignedLongToString(left, leftString, sizeof(leftString))!=NULL
					&& M2MString_convertFromUnsignedLongToString(right, rightString, sizeof(rightString))!=NULL)
				{
				//===== Update a record =====
				if (this_updateNestedSetsModel(database, hexadecimalString, leftString, rightString)==true)
					{
					return nodeID;
					}
				//===== Error handling =====
				else
					{
					return 0UL;
					}
				}
			//===== In case of initialize parameters =====
			else if (left==0 && right==0)
				{
				//===== Update a record =====
				if (this_updateNestedSetsModel(database, hexadecimalString, NULL, NULL)==true)
					{
					return nodeID;
					}
				//===== Error handling =====
				else
					{
					return 0UL;
					}
				}
			//===== Error handling =====
			else
				{
				return 0UL;
				}
			}
		//===== Error handling =====
		else
			{
			return 0UL;
			}
		}
	//===== Argument error =====
	else if (database==NULL)
		{
		return 0UL;
		}
	else
		{
		return 0UL;
		}
	}



/* End Of File */
