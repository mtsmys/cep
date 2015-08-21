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

#include "m2m/io/M2MHeap.h"


/*******************************************************************************
 * 公開関数
 ******************************************************************************/
/**
 * “calloc()”関数を使って新規にメモリをアロケートして返す。<br>
 *
 * @param[in] count	基本サイズ（＝base[Byte]）の繰り返し回数[回]
 * @param[in] base	アロケーションの基本サイズ[Byte]
 * @return			アロケートしたメモリ（= count * base[Byte]） or NULL(エラーの場合)
 */
void *M2MHeap_calloc (const size_t count, const size_t base)
	{
	return calloc(count, base);
	}


/**
 * “malloc()”関数を使って新規にメモリをアロケートして返す。<br>
 *
 * @param[in] size	アロケートするメモリサイズ[Byte]
 * @return			アロケートしたメモリ or NULL(エラーの場合)
 */
void *M2MHeap_malloc (const size_t size)
	{
	//========== ローカル変数 ==========
	void *memory = NULL;

	//===== ヒープメモリの取得 =====
	if ((memory=malloc(size))!=NULL)
		{
		//===== メモリ領域の初期化 =====
		memset(memory, 0, size);
		return memory;
		}
	//===== エラー処理 =====
	else
		{
		return NULL;
		}
	}



/* End Of File */
