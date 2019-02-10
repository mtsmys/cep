/*******************************************************************************
 * M2MDate.c
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

#include "m2m/time/M2MDate.h"


/*******************************************************************************
 * Public function
 ******************************************************************************/
/**
 * This method returns milliseconds number of current time.<br>
 *
 * @return	Milliseconds number of current time or 0 (in case of error)
 */
uint32_t M2MDate_getCurrentTimeMillis ()
	{
	//========== Variable ==========
	struct timeval currentTime;
	const M2MString *METHOD_NAME = (M2MString *)"M2MDate_getCurrentTimeMillis()";

	//===== Initialize object =====
	memset(&currentTime, 0, sizeof(currentTime));
	//===== Get local calendar =====
	if (gettimeofday(&currentTime, NULL)==0)
		{
		//===== Return milliseconds of current time =====
		return (uint32_t)(currentTime.tv_sec * 1000UL) + (uint32_t)(currentTime.tv_usec / 1000UL);
		}
	//===== Error handling =====
	else
		{
		M2MLogger_printErrorMessage(METHOD_NAME, __LINE__, (M2MString *)"Failed to get current time structure object", NULL);
		return 0UL;
		}
	}



/* End Of File */
