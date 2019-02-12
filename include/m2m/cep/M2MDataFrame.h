/*******************************************************************************
 * M2MDataFrame.h
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

#ifndef M2M_CEP_M2MDATAFRAME_H_
#define M2M_CEP_M2MDATAFRAME_H_


#include "m2m/lang/M2MString.h"
#include "m2m/log/M2MLogger.h"
#include "m2m/util/list/M2MList.h"



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



/*******************************************************************************
 * Definition
 ******************************************************************************/
/**
 * Record management object for CEP(Complex Event Processing).<br>
 * Divide nodes for each table name (to be inserted into records) and concatenate <br>
 * them as a list structure.<br>
 * Note that the maximum number of structure objects showing record information of <br>
 * the same table name can be held by member variables of the M2MCEP structure.<br>
 *
 * @param previousRecord	A pointer of the record management object one before (indicating its own pointer in the case of the head)
 * @param nextRecord		A pointer of the record management object immediately after (indicating NULL in the case of the end)
 * @param tableName			String indicating table name
 * @param columnName		CSV format string (= [field name, field name, field name ···]) indicating the field name of the table
 * @param newRecordList 	A structure object (= [CSV 1 line -> CSV 1 line -> CSV 1 line ····)] indicating newly inserted record information (character string))
 * @param newRecordList 	A structure object (= [CSV 1 line -> CSV 1 line -> CSV 1 line ...)] indicating the record information (character string) inserted in the past. Ascending order from past data
 */
#ifndef M2MDataFrame
typedef struct M2MDataFrame
	{
	struct M2MDataFrame *previous;
	struct M2MDataFrame *next;
	M2MString *tableName;
	M2MString *columnName;
	M2MList *newRecordList;
	M2MList *oldRecordList;
	} M2MDataFrame;
#endif /* M2MDataFrame */



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
M2MDataFrame *M2MDataFrame_begin (M2MDataFrame *self);


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
void M2MDataFrame_delete (M2MDataFrame **self);


/**
 * Returns a CSV format string indicating the column name held by the argument <br>
 * as a member variable.<br>
 *
 * @param[in] self		Record management object
 * @return				CSV format string indicating the column name held by record management object specified by argument or NULL (in case of error)
 */
M2MString *M2MDataFrame_getColumnName (const M2MDataFrame *self);


/**
 * Returns a newly inserted record list that the argument holds as a member variable.<br>
 *
 * @param[in] self		Record management object
 * @return				A record list newly inserted (not yet inserted in the memory DB) held by the record management object specified by the argument
 */
M2MList *M2MDataFrame_getNewRecordList (const M2MDataFrame *self);


/**
 * Returns a record list inserted in the past that the argument holds as a member variable.<br>
 *
 * @param[in] self		Record management object
 * @return				Record list (inserted in the memory DB) inserted in the past that is held by the record management object specified by the argument
 */
M2MList *M2MDataFrame_getOldRecordList (const M2MDataFrame *self);


/**
 * Returns the table name string held by the argument as a member variable.<br>
 *
 * @param[in] self		Record management object
 * @return				Table name held by record management object specified by argument
 */
M2MString *M2MDataFrame_getTableName (const M2MDataFrame *self);


/**
 * Copy the record information of the "newly inserted record information" structure <br>
 * object of the record management object designated by the argument to the "record <br>
 * information inserted in the past" structure object.<br>
 * This operation is executed to insert data into the SQLite 3 database in memory <br>
 * and then organize the data for persistence into the SQLite 3 database on the file.<br>
 *
 * @param[in,out] self	Record management object
 */
void M2MDataFrame_moveFromNewRecordListToOldRecordList (M2MDataFrame *self);


/**
 * A heap memory area is newly acquired, and a record management object is generated.<br>
 * Keep in mind that the forward node of this record management object is its own <br>
 * pointer and the backward node is initialized to NULL.<br>
 *
 * @return	A newly created record management object
 */
M2MDataFrame *M2MDataFrame_new ();


/**
 * Returns the record management object after the argument is held as a member variable.<br>
 *
 * @param[in] self		Record management object
 * @return				The record management object immediately following the record management object specified by the argument
 */
M2MDataFrame *M2MDataFrame_next (const M2MDataFrame *self);


/**
 * Returns the record management object immediately before which the argument is <br>
 * held as a member variable.<br>
 *
 * @param[in] self		Record management object
 * @return				The record management object immediately before the record management object specified by the argument
 */
M2MDataFrame *M2MDataFrame_previous (const M2MDataFrame *self);


/**
 * Only the record management object corresponding to the table name specified by <br>
 * the argument releases the memory area (other record management objects connected <br>
 * by the pointer will survive).<br>
 *
 * @param[in,out] self		Record management object
 * @param[in] tableName		String indicating the table name possessed by the record management object to be deleted
 */
void M2MDataFrame_remove (M2MDataFrame *self, const M2MString *tableName);


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
 * date, temperature, humidity\r\n ← Header row<br>
 * 1395984160, 23.8, 46\r\n ← Record 1st line<br>
 * 1395984254, 24.0, 45\r\n ← record second line<br>
 * ...<br>
 *
 * @param[in,out] self		Record management object
 * @param[in] tableName		String indicating table name
 * @param[in] csv			String data in CSV format (header showing column name in the first row, data after the second row)
 * @return					Number of records stored in object [number] or -1 (in case of error)
 */
int M2MDataFrame_setCSV (M2MDataFrame *self, const M2MString *tableName, const M2MString *csv);


/**
 * Set the record management object one record behind the argument.<br>
 *
 * @param[in] self			Record management object
 * @param[in] nextRecord	The record management object one behind
 */
void M2MDataFrame_setNextRecord (M2MDataFrame *self, M2MDataFrame *nextRecord);


/**
 * Set the record management object one before the argument.<br>
 *
 * @param[in,out] self			Record management object
 * @param[in] previousRedord	Record management object one before
 */
void M2MDataFrame_setPreviousRecord (M2MDataFrame *self, M2MDataFrame *previousRecord);


/**
 * Returns the number of elements of record management object specified by argument.<br>
 *
 * @param[in] self	Record management object
 * @return			The number of element
 */
unsigned int M2MDataFrame_size (M2MDataFrame *self);



#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2M_CEP_M2MDATAFRAME_H_ */
