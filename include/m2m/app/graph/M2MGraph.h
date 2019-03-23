/*******************************************************************************
 * M2MGraph.h
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

#ifndef M2M_APP_GRAPH_M2MGRAPH_H_
#define M2M_APP_GRAPH_M2MGRAPH_H_



#include "m2m/app/graph/M2MNode.h"
#include "m2m/lib/lang/M2MString.h"
#include "m2m/lib/log/M2MFileAppender.h"
#include "m2m/lib/io/M2MHeap.h"
#include <sqlite3.h>



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



/*******************************************************************************
 * Definition
 ******************************************************************************/
/**
 * @param database			Handler of SQLite3 database connection
 * @param logger			Logging object
 * @param sqliteFilePath	String indicating SQLite3 database file pathname (if you want to use in-memory database, please set ":memory:" string)
 */
#ifndef M2MGraph
typedef struct
	{
	sqlite3 *database;
	M2MFileAppender *logger;
	M2MString *sqliteFilePath;
	} M2MGraph;
#endif /* M2MGraph */



/*******************************************************************************
 * Public function
 ******************************************************************************/
/**
 * Set a new record as node into the "m2m_node" table.<br>
 *
 * @param[in] self		M2MGraph structure object
 * @param[in] name		String indicating unique node name in "m2m_node" table
 * @param[in] property	String indicating property belonging to the node or NULL
 * @return				Number indicating node ID which is unique in "m2m_node" table
 */
uint32_t M2MGraph_addNode (M2MGraph *self, const M2MString *name, const M2MString *property);


/**
 * Connect nodes specified by arguments with an edge.<br>
 * Actually maintain relationships in a "Nested Sets Model" in the table <br>
 * of SQLite 3 database.<br>
 *
 * @param[in] self			M2MGraph structure object
 * @param[in] nodeID		Node ID of the connection source to be connected at edge
 * @param[in] anotherNodeID	Node ID of connection destination to be connected by edge
 * @return					M2MGraph structure object relationships updated or NULL (in case of error)
 */
M2MGraph *M2MGraph_connect (const M2MGraph *self, const uint32_t nodeID, const uint32_t anotherNodeID);


/**
 * Destructor.<br>
 * Release the heap memory for M2MGraph structure object.<br>
 *
 * @param[in,out] self	M2MGraph structure object which is to be released memory
 */
void M2MGraph_delete (M2MGraph **self);


/**
 * Get the SQLite connection object on file or memory.<br>
 *
 * @param[in] self		M2MGraph structure object
 * @return				SQLite3 database object
 */
sqlite3 *M2MGraph_getDatabase (M2MGraph *self);


/**
 * Get logging object owned by the argument M2MGraph structure object.<br>
 *
 * @param[in] self	M2MGraph structure object
 * @return			Logging object owned by M2MGraph structure object
 */
M2MFileAppender *M2MGraph_getLogger (const M2MGraph *self);


/**
 * Get string indicating the database file pathname possessed by the M2MGraph object.<br>
 *
 * @param[in] self	M2MGraph structure object
 * @return			String indicating database file name or NULL (in case of error)
 */
M2MString *M2MGraph_getSQLiteFilePath (const M2MGraph *self);


/**
 * Constructor.<br>
 * This function creates new M2MGraph structure object.<br>
 * When creating, this function executes memory allocation, so caller must <br>
 * release the memory for avoiding memory leak.<br>
 *
 * @param[in] sqliteFilePath	String indicating SQLite3 file pathname (if you want to use in-memory database, please set ":memory:" string)
 * @return						Created new M2MGraph structure object or NULL (in case of error)
 */
M2MGraph *M2MGraph_new (const M2MString *sqliteFilePath);



#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2M_DB_GRAPH_M2MGRAPH_H_ */
