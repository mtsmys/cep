/*******************************************************************************
 * M2MNode.h
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

#pragma once

#ifndef M2M_GRAPH_M2MNODE_H_
#define M2M_GRAPH_M2MNODE_H_



#include "m2m/lib/db/M2MSQLite.h"
#include "m2m/lib/lang/M2MString.h"
#include "m2m/lib/log/M2MFileAppender.h"
#include "m2m/lib/time/M2MDate.h"
#include "m2m/lib/util/list/M2MList.h"
#include "tinymt/TinyMT32.h"
#include <inttypes.h>
#include <sqlite3.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



/*******************************************************************************
 * Definition
 ******************************************************************************/
/**
 * Table name of managing properties of node relationships.<br>
 */
#ifndef M2MNode_TABLE_NAME
#define M2MNode_TABLE_NAME (M2MString *)"m2m_node"
#endif /* M2MNode_TABLE_NAME */


/**
 * Identifier column in m2m_node table.<br>
 * Data type of this column is "hexadecimal number string" because SQLite3 <br>
 * doesn't allow the size of unsigned integer number. <br>
 */
#ifndef M2MNode_COLUMN_ID
#define M2MNode_COLUMN_ID (M2MString *)"id"
#endif /* M2MNode_COLUMN_ID */


/**
 * Column "name" of the name of node which is unique in "m2m_node" table.<br>
 * Data type of this column is string.<br>
 */
#ifndef M2MNode_COLUMN_NAME
#define M2MNode_COLUMN_NAME (M2MString *)"name"
#endif /* M2MNode_COLUMN_NAME */


/**
 * Column name of the "property" information of node in "m2m_node" table.<br>
 * Data type of this column is string.<br>
 */
#ifndef M2MNode_COLUMN_PROPERTY
#define M2MNode_COLUMN_PROPERTY (M2MString *)"property"
#endif /* M2MNode_COLUMN_PROPERTY */


/**
 * Column name of the Nested Set Model information of node in "m2m_node" table.<br>
 * Data type of this column is "NUMERIC" which is defined in SQLite3.<br>
 */
#ifndef M2MNode_COLUMN_LEFT
#define M2MNode_COLUMN_LEFT (M2MString *)"lft"
#endif /* M2MNode_COLUMN_LEFT */


/**
 * Column name of the Nested Set Model information of node in "m2m_node" table.<br>
 * Data type of this column is "NUMERIC" which is defined in SQLite3.<br>
 */
#ifndef M2MNode_COLUMN_RIGHT
#define M2MNode_COLUMN_RIGHT (M2MString *)"rgt"
#endif /* M2MNode_COLUMN_RIGHT */


/**
 * Length of node ID which is hexadecimal string indicating unsigned integer (=32[bit]).<br>
 * Hexadecimal string is 2[Byte] in 1[Byte] data, so this variable is twice of 4[Byte].<br>
 */
#ifndef M2MNode_ID_LENGTH
#define M2MNode_ID_LENGTH 8
#endif /* M2MNode_ID_LENGTH */




/*******************************************************************************
 * Public function
 ******************************************************************************/
/**
 * Constructor.<br>
 * Set a new record as 1 node into the "m2m_node" table.<br>
 *
 * @param[in] database	SQLite3 database object
 * @param[in] name		String indicating unique node name in "m2m_node" table
 * @param[in] property	String indicating property belonging to the node or NULL
 * @param[in] logger	Logging object
 * @param[out] id		Buffer for storing 8[Byte] node ID string which is unique in "m2m_node" table
 * @param[in] idLength	Size of buffer for storing 8[Byte] node ID
 * @return				Pointer of stored node ID or NULL (in case of error)
 */
M2MString *M2MNode_add (sqlite3 *database, const M2MString *name, const M2MString *property, const M2MFileAppender *logger, M2MString id[], const size_t idLength);


/**
 * Destructor.<br>
 * Delete a record indicated with ID in the "m2m_node" table.
 *
 * @param[in] database	SQLite3 database object
 * @param[in] id		Node ID string which is unique in "m2m_node" table
 * @param[in] logger	Logging object
 */
void M2MNode_delete (sqlite3 *database, const M2MString *id, const M2MFileAppender *logger);


/**
 * @param[in] database	SQLite3 database object
 * @param[in] name		String indicating node name
 * @param[in] logger	Logging object
 * @param[out] id		Buffer for storing node ID string
 * @param[in] idLength	Size of buffer for storing 8[Byte] node ID
 * @return				Pointer of stored node ID or NULL (in case of error)
 */
M2MString *M2MNode_getID (sqlite3 *database, const M2MString *name, const M2MFileAppender *logger, M2MString id[], const size_t idLength);


/**
 * @param[in] database	SQLite3 database object
 * @param[in] logger	Logging object
 * @param[out] idList	List object for storing ID number strings (allocation is executed in this function, so caller must release this memory)
 * @return				List object stored strings indicating node IDs which are unique in "m2m_node" table
 */
M2MList *M2MNode_getIDList (sqlite3 *database, const M2MFileAppender *logger, M2MList **idList);


/**
 * @param[in] database	SQLite3 database object
 * @param[in] id		Number indicating node ID which is unique in "m2m_node" table
 * @param[in] logger	Logging object
 * @param[out] name		Pointer to copying the node name (buffering is executed inside this function)
 * @return				String indicating node name or NULL (in case of error)
 */
M2MString *M2MNode_getName (sqlite3 *database, const M2MString *id, const M2MFileAppender *logger, M2MString **name);


/**
 * @param[in] database	SQLite3 database object
 * @param[in] id		Number indicating node ID which is unique inside "m2m_node" table
 * @param[in] logger	Logging object
 * @param[out] property	Pointer to copying property belonging to the node
 * @return				Pointer of stored node property string
 */
M2MString *M2MNode_getProperty (sqlite3 *database, const M2MString *id, const M2MFileAppender *logger, M2MString **property);


/**
 * Set left & right parameters for Nested Sets Model into a node.<br>
 * If initialize them, set 0 as their arguments in the same time.<br>
 *
 * @param[in] database	SQLite3 database object
 * @param[in] id		Node ID string which is unique in "m2m_node" table
 * @param[in] left		Nested Sets Model parameter (>=1) or 0 (in case of initialization)
 * @param[in] right		Nested Sets Model parameter (>=1) or 0 (in case of initialization)
 * @param[in] logger	Logging object
 * @return				String indicating node ID which was set Nested Sets Model parameter or NULL (in case of error)
 */
M2MString *M2MNode_setNestedSetsModel (sqlite3 *database, M2MString *id, const uint32_t left, const uint32_t right, const M2MFileAppender *logger);



#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2M_GRAPH_M2MNODE_H_ */
