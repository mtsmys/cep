/*******************************************************************************
 * M2MFileAppender.c :
 *
 * Copyright (c) 2019, Akihisa Yasuda
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

#include "m2m/log/M2MFileAppender.h"



/*******************************************************************************
 * Private function
 ******************************************************************************/
/**
 * Get the length of the string. <br>
 *
 * @param[in] self	String (always be null terminated)
 * @return			Length of string or 0 (in case of error)
 */
static size_t this_getStringlength (const unsigned char *string)
	{
	//===== Check argument =====
	if (string!=NULL)
		{
		return strlen(string);
		}
	//===== Argument error =====
	else
		{
		return 0;
		}
	}



/*******************************************************************************
 * Public function
 ******************************************************************************/
/**
 *
 * @return Integer number of thread ID.<br>
 */
int32_t M2MSystem_getThreadID ()
	{
	return syscall(SYS_gettid);
	}


/**
 *
 * @param[in,out] buffer
 * @param[in] bufferLength
 * @return
 */
unsigned char *M2MSystem_getThreadIDString (unsigned char *buffer, const size_t bufferLength)
	{
	//===== Check argument =====
	if (buffer!=NULL && bufferLength>0)
		{
		//===== Initialize array =====
		memset(buffer, 0, bufferLength);
		//===== Convert from long to string =====
		if (snprintf(buffer, bufferLength-1, "%d", M2MSystem_getThreadID())>=0
				&& this_getStringlength(buffer)>0)
			{
			return buffer;
			}
		//===== Error handling =====
		else
			{
			return NULL;
			}
		}
	//===== Argument error =====
	else
		{
		return NULL;
		}
	}


/**
 * Print out a line to the "standard" output stream.<br>
 *
 * @param[in] data	Output data
 */
void M2MSystem_println (const unsigned char *data)
	{
	//===== Check argument =====
	if (data!=NULL && this_getStringlength(data)>0)
		{
		//===== Print out message =====
		fprintf(stderr, "%s\n", data);
		}
	//===== Argument error =====
	else
		{
		}
	return;
	}



/* End Of File */
