/*******************************************************************************
 * M2MHeap.c
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

#include "m2m/lib/io/M2MHeap.h"


/*******************************************************************************
 * Public function
 ******************************************************************************/
/**
 * This method allocate new memory with using "calloc()".<br>
 *
 * @param[in] count	Repeat number of base size
 * @param[in] base	Base size of allocation(allocation size = count * base[Byte])[Byte]
 * @return			Allocated memory or NULL (In case of error)
 */
void *M2MHeap_calloc (const size_t count, const size_t base)
	{
	return calloc(count, base);
	}


/**
 * This method allocate new memory with using "malloc()".<br>
 *
 * @param[in] size	Allocation size[Byte]
 * @return			Allocated memory or NULL (In case of error)
 */
void *M2MHeap_malloc (const size_t size)
	{
	//========== Variable ==========
	void *memory = NULL;

	//===== Get heap memory =====
	if ((memory=malloc(size))!=NULL)
		{
		//===== Initialize heap memory =====
		memset(memory, 0, size);
		return memory;
		}
	//===== Error handling =====
	else
		{
		return NULL;
		}
	}


/**
 * This method changes the size of the allocation pointed to by ptr to size.<br>
 *
 * @param ptr	Allocated memory pointer
 * @param size	Memory size for allocation[Byte]
 * @return		Reallocated memory pointer or NULL(means error)
 */
void *M2MHeap_realloc (void *ptr, const size_t size)
	{
	return realloc(ptr, size);
	}



/* End Of File */
