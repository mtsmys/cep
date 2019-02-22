/*******************************************************************************
 * M2MHeap.h
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

#pragma once

#ifndef M2M_IO_M2MHEAP_H_
#define M2M_IO_M2MHEAP_H_


#include <stddef.h>
#include <stdlib.h>
#include <string.h>



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



/*******************************************************************************
 * Public method
 ******************************************************************************/
/**
 * This method allocate new memory with using "calloc()".<br>
 *
 * @param[in] count	Repeat number of base size
 * @param[in] base	Base size of allocation(allocation size = count * base[Byte])[Byte]
 * @return			Allocated memory or NULL (In case of error)
 */
void *M2MHeap_calloc (const size_t count, const size_t base);


/**
 * This method delete indicated heap memory with using "free()".<br>
 * If indicated memory is NULL, do nothing.<br>
 *
 * @param[in,out] var	Memory release target variable
 */
#ifndef M2MHeap_free
#define M2MHeap_free( var ) {if( var != NULL ) free( var); var = NULL;}
#endif /* M2MHeap_free */


/**
 * This method allocate new memory with using "malloc()".<br>
 *
 * @param[in] size	Allocation size[Byte]
 * @return			Allocated memory or NULL (In case of error)
 */
void *M2MHeap_malloc (const size_t size);


/**
 * This method changes the size of the allocation pointed to by ptr to size.<br>
 *
 * @param ptr	Allocated memory pointer
 * @param size	Memory size for allocation[Byte]
 * @return		Reallocated memory pointer or NULL(means error)
 */
void *M2MHeap_realloc (void *ptr, const size_t size);



#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2M_IO_M2MHEAP_H_ */
