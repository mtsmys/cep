/*******************************************************************************
 * @file tinymt32.c
 *
 * @brief Tiny Mersenne Twister only 127 bit internal state
 *
 * @author Mutsuo Saito (Hiroshima University)
 * @author Makoto Matsumoto (The University of Tokyo)
 *
 * Copyright (C) 2011 Mutsuo Saito, Makoto Matsumoto,
 * Hiroshima University and The University of Tokyo.
 * All rights reserved.
 *
 * The 3-clause BSD License is applied to this software, see
 * LICENSE.txt
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
 * @param self		tinymt state vector.
 * @param seed		a 32-bit unsigned integer used as a seed.
 */
void TinyMT32_init (TinyMT32 *self, uint32_t seed)
	{
	self->status[0] = seed;
	self->status[1] = self->mat1;
	self->status[2] = self->mat2;
	self->status[3] = self->tmat;
	for (int i = 1; i < TINYMT32_MIN_LOOP; i++)
		{
		self->status[i & 3] ^= i + UINT32_C(1812433253) * (self->status[(i - 1) & 3] ^ (self->status[(i - 1) & 3] >> 30));
		}
	this_period_certification(self);
	for (int i = 0; i < TINYMT32_PRE_LOOP; i++)
		{
		TinyMT32_next_state(self);
		}
	}


/**
 * This function initializes the internal state array,
 * with an array of 32-bit unsigned integers used as seeds
 * 
 * @param self			tinymt state vector.
 * @param init_key		the array of 32-bit integers, used as a seed.
 * @param key_length	the length of init_key.
 */
void TinyMT32_init_by_array (TinyMT32 *self, uint32_t init_key[], int key_length)
	{
	const int lag = 1;
	const int mid = 1;
	const int size = 4;
	int i, j;
	int count;
	uint32_t r;
	uint32_t * st = &self->status[0];

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
	r = this_ini_func1(st[0] ^ st[mid % size] ^ st[(size - 1) % size]);
	st[mid % size] += r;
	r += key_length;
	st[(mid + lag) % size] += r;
	st[0] = r;
	count--;
	for (i = 1, j = 0; (j < count) && (j < key_length); j++)
		{
		r = this_ini_func1(st[i % size] ^ st[(i + mid) % size] ^ st[(i + size - 1) % size]);
		st[(i + mid) % size] += r;
		r += init_key[j] + i;
		st[(i + mid + lag) % size] += r;
		st[i % size] = r;
		i = (i + 1) % size;
		}
	for (; j < count; j++)
		{
		r = this_ini_func1(st[i % size] ^ st[(i + mid) % size] ^ st[(i + size - 1) % size]);
		st[(i + mid) % size] += r;
		r += i;
		st[(i + mid + lag) % size] += r;
		st[i % size] = r;
		i = (i + 1) % size;
		}
	for (j = 0; j < size; j++)
		{
		r = this_ini_func2(st[i % size] + st[(i + mid) % size] + st[(i + size - 1) % size]);
		st[(i + mid) % size] ^= r;
		r -= i;
		st[(i + mid + lag) % size] ^= r;
		st[i % size] = r;
		i = (i + 1) % size;
		}
	this_period_certification(self);
	for (i = 0; i < TINYMT32_PRE_LOOP; i++)
		{
		TinyMT32_next_state(self);
		}
	}


// End Of File
