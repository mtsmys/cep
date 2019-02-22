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
		return 0UL;
		}
	}


/**
 * This method copies local time string into indicated "buffer" memory.<br>
 * Output string format is "yyyy/MM/dd HH:mm:ss.SSS";
 * This method doesn't allocation, so caller needs to prepare memory<br>
 * before call this method.<br>
 *
 * @param[out] buffer		memory buffer for copying local time string
 * @param[in] bufferLength	memory buffer length(max size)
 * @return					length of local time string or 0(means error)
 */
size_t M2MDate_getLocalTimeString (M2MString *buffer, const size_t bufferLength)
	{
	//========== Variable ==========
	struct timeval currentTime;
	struct tm *localCalendar = NULL;
	size_t miliSecondLength = 0;
	M2MString *miliSecond = NULL;
	M2MString second[8];
	const M2MString *FORMAT = (M2MString *)"%Y-%m-%d %H:%M:%S.";

	//===== Check argument =====
	if (buffer!=NULL && bufferLength>0)
		{
		//===== Initialize buffer =====
		memset(buffer, 0, bufferLength);
		//===== Get current time =====
		if (gettimeofday(&currentTime, NULL)==0
				&& (localCalendar=localtime(&(currentTime.tv_sec)))!=NULL)
			{
			//===== Convert time to string =====
			strftime(buffer, bufferLength-1, FORMAT, localCalendar);
			//===== Convert millisecond to string =====
			if (M2MString_convertFromSignedLongToString((signed long)(currentTime.tv_usec/1000UL), &miliSecond)!=NULL
					&& (miliSecondLength=M2MString_length(miliSecond))>0)
				{
				//===== In the case of digit number of millisecond is 1 =====
				if (miliSecondLength==1)
					{
					memset(second, 0, sizeof(second));
					//===== Convert millisecond into second format =====
					M2MString_format(second, sizeof(second)-1, (M2MString *)"00%s", miliSecond);
					M2MHeap_free(miliSecond);
					}
				//===== In the case of digit number of millisecond is 2 =====
				else if (miliSecondLength==2)
					{
					memset(second, 0, sizeof(second));
					//===== Convert millisecond into second format =====
					M2MString_format(second, sizeof(second)-1, (M2MString *)"0%s", miliSecond);
					M2MHeap_free(miliSecond);
					}
				//===== In the case of digit number of millisecond is 3 =====
				else if (miliSecondLength==3)
					{
					memset(second, 0, sizeof(second));
					//===== Convert millisecond into second format =====
					M2MString_format(second, sizeof(second)-1, (M2MString *)"%s", miliSecond);
					M2MHeap_free(miliSecond);
					}
				//===== Error handling =====
				else
					{
					//===== Initialize buffer =====
					memset(buffer, 0, bufferLength);
					M2MHeap_free(miliSecond);
					return 0;
					}
				//===== Check buffer length for copying millisecond string =====
				if (M2MString_length(second)<(bufferLength-M2MString_length(buffer)-1))
					{
					//===== Copy millisecond string =====
					memcpy(&(buffer[M2MString_length(buffer)]), second, M2MString_length(second));
					//===== Release allocated memory =====
					M2MHeap_free(miliSecond);
					return M2MString_length(buffer);
					}
				//===== Error handling =====
				else
					{
					//===== Initialize buffer =====
					M2MHeap_free(miliSecond);
					memset(buffer, 0, bufferLength);
					return 0;
					}
				}
			//===== Error handling =====
			else
				{
				//===== Initialize buffer =====
				M2MHeap_free(miliSecond);
				memset(buffer, 0, bufferLength);
				return 0;
				}
			}
		//===== Error handling =====
		else
			{
			return 0;
			}
		}
	//===== Error handling =====
	else
		{
		return 0;
		}
	}



/* End Of File */
