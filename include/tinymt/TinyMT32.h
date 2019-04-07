/*******************************************************************************
 * @file TinyMT32.h
 *
 * @brief Tiny Mersenne Twister only 127 bit internal state
 *
 * @author Mutsuo Saito (Hiroshima University)
 * @author Makoto Matsumoto (University of Tokyo)
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

#pragma once

#ifndef TINYMT_TINYMT32_H_
#define TINYMT_TINYMT32_H_



#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>



#if defined(__cplusplus)
extern "C" {
#endif



/*******************************************************************************
 * Definition
 ******************************************************************************/
#ifndef TINYMT32_MASK
#define TINYMT32_MASK UINT32_C(0x7fffffff)
#endif /* TINYMT32_MASK */


#ifndef TINYMT32_MEXP
#define TINYMT32_MEXP 127
#endif /* TINYMT32_MEXP */


#ifndef TINYMT32_MIN_LOOP
#define TINYMT32_MIN_LOOP 8
#endif /* TINYMT32_MIN_LOOP */


#ifndef TINYMT32_MUL
#define TINYMT32_MUL (1.0f / 16777216.0f)
#endif /* TINYMT32_MUL */


#ifndef TINYMT32_PRE_LOOP
#define TINYMT32_PRE_LOOP 8
#endif /* TINYMT32_PRE_LOOP */


#ifndef TINYMT32_SH0
#define TINYMT32_SH0 1
#endif /* TINYMT32_SH0 */


#ifndef TINYMT32_SH1
#define TINYMT32_SH1 10
#endif /* TINYMT32_SH1 */


#ifndef TINYMT32_SH8
#define TINYMT32_SH8 8
#endif /* TINYMT32_SH8 */


/**
 * tinymt32 internal state vector and parameters
 */
#ifndef TinyMT32
typedef struct
	{
	uint32_t status[4];
	uint32_t mat1;
	uint32_t mat2;
	uint32_t tmat;
	} TinyMT32;
#endif /* TinyMT32 */



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
void TinyMT32_init (TinyMT32 *self, const uint32_t seed);


/**
 * This function initializes the internal state array,
 * with an array of 32-bit unsigned integers used as seeds
 * 
 * @param self			tinymt state vector.
 * @param init_key		the array of 32-bit integers, used as a seed.
 * @param key_length	the length of init_key.
 */
void TinyMT32_init_by_array (TinyMT32 *self, uint32_t init_key[], int key_length);


#if defined(__GNUC__)
/**
 * This function always returns 127
 * 
 * @param self		not used
 * @return			always 127
 */
inline static int TinyMT32_get_mexp (TinyMT32 *self  __attribute__((unused)))
	{
	return TINYMT32_MEXP;
	}
#else
inline static int TinyMT32_get_mexp (TinyMT32 *self)
	{
	return TINYMT32_MEXP;
	}
#endif


/**
 * This function changes internal state of tinymt32.
 * Users should not call this function directly.
 * 
 * @param self	tinymt internal status
 */
inline static void TinyMT32_next_state (TinyMT32 *self)
	{
	uint32_t x;
	uint32_t y;

	y = self->status[3];
	x = (self->status[0] & TINYMT32_MASK) ^ self->status[1] ^ self->status[2];
	x ^= (x << TINYMT32_SH0);
	y ^= (y >> TINYMT32_SH0) ^ x;
	self->status[0] = self->status[1];
	self->status[1] = self->status[2];
	self->status[2] = x ^ (y << TINYMT32_SH1);
	self->status[3] = y;
	self->status[1] ^= -((int32_t)(y & 1)) & self->mat1;
	self->status[2] ^= -((int32_t)(y & 1)) & self->mat2;
	}


/**
 * This function outputs 32-bit unsigned integer from internal state.
 * Users should not call this function directly.
 * 
 * @param self	tinymt internal status
 * @return			32-bit unsigned pseudo random number
 */
inline static uint32_t TinyMT32_temper (TinyMT32 *self)
	{
	uint32_t t0, t1;
	t0 = self->status[3];
#if defined(LINEARITY_CHECK)
	t1 = self->status[0] ^ (self->status[2] >> TINYMT32_SH8);
#else
	t1 = self->status[0] + (self->status[2] >> TINYMT32_SH8);
#endif
	t0 ^= t1;
	t0 ^= -((int32_t)(t1 & 1)) & self->tmat;
	return t0;
	}


/**
 * This function outputs floating point number from internal state.
 * Users should not call this function directly.
 * 
 * @param self		tinymt internal status
 * @return			floating point number r (1.0 <= r < 2.0)
 */
inline static float TinyMT32_temper_conv (TinyMT32 *self)
	{
	uint32_t t0, t1;
	union {
		uint32_t u;
		float f;
	} conv;

	t0 = self->status[3];
#if defined(LINEARITY_CHECK)
	t1 = self->status[0] ^ (self->status[2] >> TINYMT32_SH8);
#else
	t1 = self->status[0] + (self->status[2] >> TINYMT32_SH8);
#endif
	t0 ^= t1;
	conv.u = ((t0 ^ (-((int32_t)(t1 & 1)) & self->tmat)) >> 9) | UINT32_C(0x3f800000);
	return conv.f;
	}


/**
 * This function outputs floating point number from internal state.
 * Users should not call this function directly.
 * 
 * @param self		tinymt internal status
 * @return			floating point number r (1.0 < r < 2.0)
 */
inline static float TinyMT32_temper_conv_open (TinyMT32 *self)
	{
	uint32_t t0, t1;
	union {
		uint32_t u;
		float f;
	} conv;

	t0 = self->status[3];
#if defined(LINEARITY_CHECK)
	t1 = self->status[0] ^ (self->status[2] >> TINYMT32_SH8);
#else
	t1 = self->status[0] + (self->status[2] >> TINYMT32_SH8);
#endif
	t0 ^= t1;
	conv.u = ((t0 ^ (-((int32_t)(t1 & 1)) & self->tmat)) >> 9) | UINT32_C(0x3f800001);
	return conv.f;
	}


/**
 * This function outputs 32-bit unsigned integer from internal state.
 * 
 * @param self		tinymt internal status
 * @return			32-bit unsigned integer r (0 <= r < 2^32)
 */
inline static uint32_t TinyMT32_generate_uint32 (TinyMT32 *self)
	{
	TinyMT32_next_state(self);
	return TinyMT32_temper(self);
	}


/**
 * This function outputs floating point number from internal state.
 * This function is implemented using multiplying by (1 / 2^24).
 * floating point multiplication is faster than using union trick in
 * my Intel CPU.
 * 
 * @param self		tinymt internal status
 * @return			floating point number r (0.0 <= r < 1.0)
 */
inline static float TinyMT32_generate_float (TinyMT32 *self)
	{
	TinyMT32_next_state(self);
	return (TinyMT32_temper(self) >> 8) * TINYMT32_MUL;
	}


/**
 * This function outputs floating point number from internal state.
 * This function is implemented using union trick.
 * 
 * @param self	tinymt internal status
 * @return			floating point number r (1.0 <= r < 2.0)
 */
inline static float TinyMT32_generate_float12 (TinyMT32 * self)
	{
	TinyMT32_next_state(self);
	return TinyMT32_temper_conv(self);
	}


/**
 * This function outputs floating point number from internal state.
 * This function is implemented using union trick.
 * 
 * @param self		tinymt internal status
 * @return			floating point number r (0.0 <= r < 1.0)
 */
inline static float TinyMT32_generate_float01 (TinyMT32 *self)
	{
	TinyMT32_next_state(self);
	return TinyMT32_temper_conv(self) - 1.0f;
	}


/**
 * This function outputs floating point number from internal state.
 * This function may return 1.0 and never returns 0.0.
 * 
 * @param self		tinymt internal status
 * @return			floating point number r (0.0 < r <= 1.0)
 */
inline static float TinyMT32_generate_floatOC (TinyMT32 *self)
	{
	TinyMT32_next_state(self);
	return 1.0f - TinyMT32_generate_float(self);
	}


/**
 * This function outputs floating point number from internal state.
 * This function returns neither 0.0 nor 1.0.
 * 
 * @param self		tinymt internal status
 * @return			floating point number r (0.0 < r < 1.0)
 */
inline static float TinyMT32_generate_floatOO (TinyMT32 *self)
	{
	TinyMT32_next_state(self);
	return TinyMT32_temper_conv_open(self) - 1.0f;
	}


/**
 * This function outputs double precision floating point number from
 * internal state. The returned value has 32-bit precision.
 * In other words, this function makes one double precision floating point
 * number from one 32-bit unsigned integer.
 * 
 * @param self		tinymt internal status
 * @return			floating point number r (0.0 <= r < 1.0)
 */
inline static double TinyMT32_generate_32double (TinyMT32 *self)
	{
	TinyMT32_next_state(self);
	return TinyMT32_temper(self) * (1.0 / 4294967296.0);
	}



#if defined(__cplusplus)
}
#endif



#endif
