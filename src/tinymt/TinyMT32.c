/*******************************************************************************
 * @file TinyMT32.c
 *
 * @brief Tiny Mersenne Twister only 127 bit internal state
 *
 * @author Mutsuo Saito (Hiroshima University)
 * @author Makoto Matsumoto (The University of Tokyo)
 *
 * Copyright (c) 2011, 2013 Mutsuo Saito, Makoto Matsumoto,
 * Hiroshima University and The University of Tokyo.
 * All rights reserved.
 * 
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of the Hiroshima University nor the names of
 *       its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written
 *       permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

#include "tinymt/TinyMT32.h"


/*******************************************************************************
 * Private function
 ******************************************************************************/
/**
 * This function represents a function used in the initialization
 * by init_by_array
 * 
 * @param x 32-bit integer
 * @return 32-bit integer
 */
static uint32_t this_ini_func1 (uint32_t x)
	{
	return (x ^ (x >> 27)) * UINT32_C(1664525);
	}


/**
 * This function represents a function used in the initialization
 * by init_by_array
 * 
 * @param x 32-bit integer
 * @return 32-bit integer
 */
static uint32_t this_ini_func2 (uint32_t x)
	{
	return (x ^ (x >> 27)) * UINT32_C(1566083941);
	}


/**
 * This function certificate the period of 2^127-1.
 * 
 * @param self	 tinymt state vector.
 */
static void this_period_certification (TinyMT32 *self)
	{
	if ((self->status[0] & TINYMT32_MASK) == 0 
			&& self->status[1] == 0 
			&& self->status[2] == 0 
			&& self->status[3] == 0)
		{
		self->status[0] = 'T';
		self->status[1] = 'I';
		self->status[2] = 'N';
		self->status[3] = 'Y';
		}
	}



/*******************************************************************************
 * Public function
 ******************************************************************************/
/**
 * This function initializes the internal state array with a 32-bit
 * unsigned integer seed.
 * 
 * @param[in] self	tinymt state vector.
 * @param[in] seed	a 32-bit unsigned integer used as a seed.
 */
void TinyMT32_init (TinyMT32 *self, const uint32_t seed)
	{
	//========== Variable ==========
	int i = 0;

	//===== Check argument =====
	if (self!=NULL && seed>0)
		{
		//===== Initialize structure =====
		memset(self, 0, sizeof(TinyMT32));
		//=====  =====
		self->status[0] = seed;
		self->status[1] = self->mat1;
		self->status[2] = self->mat2;
		self->status[3] = self->tmat;
		//=====  =====
		for (i=1; i<TINYMT32_MIN_LOOP; i++)
			{
			self->status[i & 3] ^= i + UINT32_C(1812433253) * (self->status[(i - 1) & 3] ^ (self->status[(i - 1) & 3] >> 30));
			}
		//=====  =====
		this_period_certification(self);
		//=====  =====
		for (i=0; i<TINYMT32_PRE_LOOP; i++)
			{
			TinyMT32_next_state(self);
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		}
	else
		{
		}
	return;
	}


/**
 * This function initializes the internal state array,
 * with an array of 32-bit unsigned integers used as seeds
 * 
 * @param[in] self			tinymt state vector.
 * @param[in] init_key		the array of 32-bit integers, used as a seed.
 * @param[in] key_length	the length of init_key.
 */
void TinyMT32_init_by_array (TinyMT32 *self, uint32_t init_key[], int key_length)
	{
	//========== Variable ==========
	int i = 0;
	int j = 0;
	int count = 0;
	uint32_t r = 0;
	uint32_t * st = &self->status[0];
	const int LAG = 1;
	const int MID = 1;
	const int SIZE = 4;

	st[0] = 0;
	st[1] = self->mat1;
	st[2] = self->mat2;
	st[3] = self->tmat;
	if (key_length + 1 > TINYMT32_MIN_LOOP)
		{
		count = key_length + 1;
		}
	else
		{
		count = TINYMT32_MIN_LOOP;
		}
	r = this_ini_func1(st[0] ^ st[MID % SIZE] ^ st[(SIZE - 1) % SIZE]);
	st[MID % SIZE] += r;
	r += key_length;
	st[(MID + LAG) % SIZE] += r;
	st[0] = r;
	count--;
	for (i = 1, j = 0; (j < count) && (j < key_length); j++)
		{
		r = this_ini_func1(st[i % SIZE] ^ st[(i + MID) % SIZE] ^ st[(i + SIZE - 1) % SIZE]);
		st[(i + MID) % SIZE] += r;
		r += init_key[j] + i;
		st[(i + MID + LAG) % SIZE] += r;
		st[i % SIZE] = r;
		i = (i + 1) % SIZE;
		}
	for (; j < count; j++)
		{
		r = this_ini_func1(st[i % SIZE] ^ st[(i + MID) % SIZE] ^ st[(i + SIZE - 1) % SIZE]);
		st[(i + MID) % SIZE] += r;
		r += i;
		st[(i + MID + LAG) % SIZE] += r;
		st[i % SIZE] = r;
		i = (i + 1) % SIZE;
		}
	for (j = 0; j < SIZE; j++)
		{
		r = this_ini_func2(st[i % SIZE] + st[(i + MID) % SIZE] + st[(i + SIZE - 1) % SIZE]);
		st[(i + MID) % SIZE] ^= r;
		r -= i;
		st[(i + MID + LAG) % SIZE] ^= r;
		st[i % SIZE] = r;
		i = (i + 1) % SIZE;
		}
	this_period_certification(self);
	for (i = 0; i < TINYMT32_PRE_LOOP; i++)
		{
		TinyMT32_next_state(self);
		}
	}


// End Of File
