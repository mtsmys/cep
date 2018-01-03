/*******************************************************************************
 * M2MString.h
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

#ifndef M2M_LANG_M2MSTRING_H_
#define M2M_LANG_M2MSTRING_H_


#include "m2m/io/M2MHeap.h"
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



/*******************************************************************************
 * Class variable
 ******************************************************************************/
/**
 * String type (=unsigned char)
 */
#ifndef M2MString
typedef unsigned char M2MString;
#endif /* M2MString */


/**
 * COMMA (=",")
 */
#ifndef M2MString_COMMA
#define M2MString_COMMA (M2MString *)","
#endif /* M2MString_COMMA */


/**
 * Line feed and carriage return (="\r\n")
 */
#ifndef M2MString_CRLF
#define M2MString_CRLF (M2MString *)"\r\n"
#endif /* M2MString_CRLF */


/**
 * Double quotation (="\"")
 */
#ifndef M2MString_DOUBLE_QUOTATION
#define M2MString_DOUBLE_QUOTATION (M2MString *)"\""
#endif /* M2MString_DOUBLE_QUOTATION */


/**
 * Equal (="=")
 */
#ifndef M2MString_EQUAL
#define M2MString_EQUAL (M2MString *)"="
#endif /* M2MString_EQUAL */


/**
 * Line feed (="\n")
 */
#ifndef M2MString_LF
#define M2MString_LF (M2MString *)"\n"
#endif /* M2MString_LF */


/**
 * Question mark (="?")
 */
#ifndef M2MString_QUESTION_MARK
#define M2MString_QUESTION_MARK (M2MString *)"?"
#endif /* M2MString_QUESTION_MARK */


/**
 * Quotation (="'")
 */
#ifndef M2MString_QUOTATION
#define M2MString_QUOTATION (M2MString *)"'"
#endif /* M2MString_QUOTATION */


/**
 * White space (=" ")
 */
#ifndef M2MString_SPACE
#define M2MString_SPACE (M2MString *)" "
#endif /* M2MString_SPACE */



/*******************************************************************************
 * Public method
 ******************************************************************************/
/**
 * This function adds "string" into after the "self" string.<br>
 * Memory allocation is executed in this function, so caller must release<br>
 * the memory of "self" string.<br>
 *
 * @param[in,out] self	The original string or NULL("self" = "self + string")
 * @param[in] string	additional string
 * @return				Pointer of connected string or NULL (in case of error)
 */
M2MString *M2MString_append (M2MString **self, const M2MString *string);


/**
 * Add the string after the "self" string. <br>
 * The length [Byte] of the additional string is specified by argument.<br>
 *
 * @param[in,out] self		The original string to be added to the string (the string after addition is self = self + string)
 * @param[in] string		String to be added
 * @param[in] stringLength	Length of the string to be added[Byte]
 * @return					Pointer of the buffer to which the string was added or NULL (in case of error)
 */
M2MString *M2MString_appendLength (M2MString **self, const M2MString *string, const size_t stringLength);


/**
 * Compares the two strings specified by the argument and returns the result.<br>
 *
 * @param[in] self		The original string to be compared
 * @param[in] string	Another string to be compared
 * @return				0: two are equal, negative: In case of self < string, positive: In case of self > string
 */
signed int M2MString_compareTo (const M2MString *self, const M2MString *string);


/**
 * Convert double value into a string and copies it to the pointer. <br>
 * Since buffering of arrays is executed inside this function, so call <br>
 * "M2MHeap_free()" function on the caller side in order to prevent memory <br>
 * leak after using the string. <br>
 *
 * @param[in] number	Real number to be converted
 * @param[out] string	Pointer for copying the converted string (buffering is executed inside the function)
 * @return				Copied string or NULL (in case of error)
 */
M2MString *M2MString_convertFromDoubleToString (const double number, M2MString **string);


/**
 * Convert the line feed code from LF to CRLF for the argument string.<br>
 *
 * @param[in] self		The original string to convert line feed code
 * @param[out] string	Pointer to store string corrected line feed code (buffering itself is executed inside the function)
 * @return				Pointer of CSV string with corrected line feed code or NULL (in case of error)
 */
M2MString *M2MString_convertFromLFToCRLF (const M2MString *self, M2MString **string);


/**
 * Convert signed integer value into a string and copies it to the pointer. <br>
 * Since buffering of arrays is executed inside this function, so call <br>
 * "M2MHeap_free()" function on the caller side in order to prevent memory <br>
 * leak after using the string. <br>
 *
 * @param[in] number	Signed integer number to be converted
 * @param[out] string	Pointer for copying the converted string (buffering is executed inside the function)
 * @return				Copied string or NULL (in case of error)
 */
M2MString *M2MString_convertFromSignedIntegerToString (const signed int number, M2MString **string);


/**
 * Convert signed long value into a string and copies it to the pointer. <br>
 * Since buffering of arrays is executed inside this function, so call <br>
 * "M2MHeap_free()" function on the caller side in order to prevent memory <br>
 * leak after using the string. <br>
 *
 * @param[in] number	Signed long number to be converted
 * @param[out] string	Pointer for copying the converted string (buffering is executed inside the function)
 * @return				Copied string or NULL (in case of error)
 */
M2MString *M2MString_convertFromSignedLongToString (const signed long number, M2MString **string);


/**
 * Convert the argument string to double number.<br>
 *
 * @param[in] string		String indicating double integer
 * @param[in] stringLength	Size of string[Byte]
 * @return					Double converted from string
 */
double M2MString_convertFromStringToDouble (const M2MString *string, const size_t stringLength);


/**
 * Convert the argument string to long integer number.<br>
 *
 * @param[in] string		String indicating signed long integer
 * @param[in] stringLength	Size of string[Byte]
 * @return					Signed long integer converted from string
 */
long M2MString_convertFromStringToLong (const M2MString *string, const size_t stringLength);


/**
 * Convert the argument string to integer number.<br>
 *
 * @param[in] string		String indicating signed integer
 * @param[in] stringLength	Size of string[Byte]
 * @return					Signed integer converted from string
 */
signed int M2MString_convertFromStringToSignedInteger (const M2MString *string, const size_t stringLength);


/**
 * Compare the two strings and return result.<br>
 *
 * @param[in] one		String to be compared
 * @param[in] another	Another string to be compared
 * @param[in] length	Length of string to be compared [byte]
 * @return				true: When the same case, false: When they do not match
 */
bool M2MString_equals (const M2MString *one, const M2MString *another, const size_t length);


/**
 * Copy string indicating the local calendar time to the "buffer" array. <br>
 * Since buffering is not performed inside this function, so the memory <br>
 * area must be reserved on the caller side. <br>
 *
 * @param[out] buffer		A buffer for copying string indicating the time of the local calendar
 * @param[in] bufferLength	Size of the buffer [byte]
 * @return					Integer indicating the size of the local time character string copied to the buffer [bytes]
 */
unsigned int M2MString_getLocalTime (M2MString *buffer, const unsigned int bufferLength);


/**
 * Returns the pointer that the "keyword" string first appears. <br>
 * If "keyword" string isn't found, returns NULL.
 *
 * @param[in] string	Search target string
 * @param[in] keyword	Detection string
 * @return				Pointer indicating keyword start position or NULL (in case of error)
 */
M2MString *M2MString_indexOf (const M2MString *string, const M2MString *keyword);


/**
 * Returns the pointer that the "keyword" string last appears. <br>
 * If "keyword" string isn't found, returns NULL.
 *
 * @param[in] string	Search target string
 * @param[in] keyword	Detection string
 * @return				Pointer indicating keyword last position or NULL (in case of error)
 */
M2MString *M2MString_lastIndexOf (const M2MString *string, const M2MString *keyword);


/**
 * Get the length of the string. <br>
 *
 * @param[in] self	String (always be null terminated)
 * @return			Length of string or 0 (in case of error)
 */
size_t M2MString_length (const M2MString *self);


/**
 * Return dividing result of the source string (="string") by "delimiter". <br>
 * When acquiring consecutively, specify the source character string only <br>
 * for the first time, and specify NULL as the source string from the next <br>
 * time onward. <br>
 * When this process is executed, since the source character string itself <br>
 * is rewritten, designate a character string that may be broken in the <br>
 * source character string.<br>
 *
 * @param[in] string		Source string to be divided
 * @param[in] delimiter		Delimiter string
 * @param[in] savePointer	Copy buffer of divided source string (set the same variable each time)
 * @return					String corresponding to a fragment of a divided source string
 *
M2MString *M2MString_split (M2MString *string, const M2MString *delimiter, M2MString **savePoint);
*/


#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2M_LANG_M2MSTRING_H_ */
