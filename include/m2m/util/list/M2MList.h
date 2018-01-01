/*******************************************************************************
 * M2MList.h : Imitation of STL List Container
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
 *******************************************************************************/

#pragma once

#ifndef M2M_UTIL_LIST_M2MLIST_H_
#define M2M_UTIL_LIST_M2MLIST_H_


#include "m2m/io/M2MHeap.h"
#include <stdio.h>
#include <stdbool.h>



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



/*******************************************************************************
 * Definition
 ******************************************************************************/
/**
 * A list structure object (dynamic array)<br>
 *
 * @param previous		A list structure object located before (indicating its own pointer in the case of the head)
 * @param next			A list structure object located behind (indicating NULL in the case of the end)
 * @param value			Pointer to "value" stored in list structure object
 * @param valueLength	Integer indicating the size of "value"[Byte]
 */
#ifndef M2MList
typedef struct M2MList
	{
	struct M2MList *previous;
	struct M2MList *next;
	void *value;
	size_t valueLength;
	} M2MList;
#endif /* M2MList */



/*******************************************************************************
 * Public function
 ******************************************************************************/
/**
 * Add a new node to the next position of argument list structure object.<br>
 * Then, as the member variable of the new node, the value is copied <br>
 * (deep copy). <br>
 * Therefore, to delete the added node, use the M2MList_remove() function.<br>
 *
 * @param[in,out] self		List structure object
 * @param[in] value			The value of added node stored in list structure object
 * @param[in] valueLength	Size of value[Byte]
 * @return					Pointer of added list structure object or NULL (In case of error)
 */
M2MList *M2MList_add (M2MList *self, const void *value, const size_t valueLength);


/**
 * Get the node located ahead of the link of the list structure object.<br>
 *
 * @param[in] self	List structure object
 * @return			First node of link of list structure object or NULL (in case of error)
 */
M2MList *M2MList_begin (M2MList *self);


/**
 * Release the memory of all nodes of list structure object.<br>
 *
 * @param[in,out] self	List structure object
 */
void M2MList_delete (M2MList *self);


/**
 * Returns the list structure object corresponding to the index.<br>
 *
 * @param[in] self		List structure object
 * @param[in] index		An integer indicating an index (0 to n-1)
 * @return 				List structure object or NULL (in case of error)
 */
M2MList *M2MList_detect (M2MList *self, const unsigned int index);


/**
 * Get the node located at the end of the list structure object.<br>
 *
 * @param[in,out] self	List structure object
 * @return				End node of the list structure object or NULL (in case of error)
 */
M2MList *M2MList_end (M2MList *self);


/**
 * Return value of the list structure object existing at the index position.<br>
 *
 * @param[in] self	List structure object
 * @return			Value stored in list structure object or NULL (in case of error)
 */
void *M2MList_getValue (M2MList *self);


/**
 * Return value length of the list structure object at the index position.<br>
 *
 * @param[in] self	List structure object
 * @return			Value length stored in list structure object or NULL (in case of error)
 */
size_t M2MList_getValueLength (M2MList *self);


/**
 * Check whether the list structure object has at least one value.<br>
 *
 * @param[in] self	List structure object
 * @return			true: In case of no value, false: In case of having values
 */
bool M2MList_isEmpty (M2MList *self);


/**
 * Return number indicating length of nodes in the list structure object.<br>
 *
 * @param[in] self	List structure object
 * @return			The number of nodes of the linked list structure object
 */
unsigned int M2MList_length (M2MList *self);


/**
 * Return a list structure object located after the argument one.<br>
 *
 * @param[in] self	List structure object
 * @return next 	List structure object located behind or NULL (if the position of argument node is end)
 */
M2MList *M2MList_next (const M2MList *self);


/**
 * Get heap memory and create a new M2MList structure object.<br>
 *
 * @return	Created M2MList structure object
 */
M2MList *M2MList_new ();


/**
 * Get the list structure object existing one before the argument.<br>
 * If the argument object is the first node, returns the same pointer.<br>
 *
 * @param[in] self	List structure object
 * @return			List structure object that located before or NULL (in case of argument error)
 */
M2MList *M2MList_previous (const M2MList *self);


/**
 * Deletes a list structure object existing at the position of argument. <br>
 * If there is only one, delete only the value itself, not the object itself. <br>
 * <br>
 * [Caution]<br>
 * When calling this method, the caller must always receive the return <br>
 * value. <br>
 * This is because if the memory area of the node itself of the list <br>
 * structure object given by the argument is released, there is no way <br>
 * for receiving the link of the remaining node, resulting in an error.<br>
 *
 * @param[in,out] self	List structure objects to be deleted
 */
M2MList *M2MList_remove (M2MList *self);


/**
 * Copy (deep copy) argument value as the variable of the list structure object.<br>
 * Therefore, even if releasing the heap memory with the "value" after <br>
 * calling the function, it does not affect the member variables of the <br>
 * list structure object at all.<br>
 *
 * @param[in,out] self		List structure object
 * @param[in] value			The value of set to the list structure object
 * @param[in] valueLength	Size of value[Byte]
 * @return					Pointer of list structure object set value or NULL (In case of error)
 */
M2MList *M2MList_set (M2MList *self, const void *value, const size_t valueLength);



#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2M_UTIL_LIST_M2MLIST_H_ */
