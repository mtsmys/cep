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

#ifndef M2M_LIB_LANG_M2MSTRING_H_
#define M2M_LIB_LANG_M2MSTRING_H_


#include "m2m/M2MSystem.h"
#include "m2m/lib/io/M2MHeap.h"
#include <ctype.h>
#include <errno.h>
#include <iconv.h>
#include <inttypes.h>
#include <limits.h>
#include <locale.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
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
 *
 */
#ifndef M2MString_COLON
#define M2MString_COLON (M2MString *)":"
#endif /* M2MString_COLON */


/**
 * COMMA (=",")
 */
#ifndef M2MString_COMMA
#define M2MString_COMMA (M2MString *)","
#endif /* M2MString_COMMA */


/**
 * Carriage return (="\r")
 */
#ifndef M2MString_CR
#define M2MString_CR (M2MString *)"\r"
#endif /* M2MString_CR */


/**
 * Carriage return and line feed (="\r\n")
 */
#ifndef M2MString_CRLF
#define M2MString_CRLF (M2MString *)"\r\n"
#endif /* M2MString_CRLF */


/**
 *
 */
#ifndef M2MString_DOT
#define M2MString_DOT (M2MString *)"."
#endif /* M2MString_DOT */


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
 *
 */
#ifndef M2MString_HYPHEN
#define M2MString_HYPHEN (M2MString *)"-"
#endif /* M2MString_HYPHEN */


/**
 *
 */
#ifndef M2MString_LEFT_CURLY_BRACKET
#define M2MString_LEFT_CURLY_BRACKET (M2MString *)"{"
#endif /* M2MString_LEFT_CURLY_BRACKET */


/**
 *
 */
#ifndef M2MString_LEFT_SQUARE_BRACKET
#define M2MString_LEFT_SQUARE_BRACKET (M2MString *)"["
#endif /* M2MString_LEFT_SQUARE_BRACKET */


/**
 * Line feed (="\n")
 */
#ifndef M2MString_LF
#define M2MString_LF (M2MString *)"\n"
#endif /* M2MString_LF */


/**
 * Null-Terminated Byte String
 */
#ifndef M2MString_NTBS
#define M2MString_NTBS (M2MString *)'\0'
#endif /* M2MString_NTBS */


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
 *
 */
#ifndef M2MString_RIGHT_CURLY_BRACKET
#define M2MString_RIGHT_CURLY_BRACKET (M2MString *)"}"
#endif /* M2MString_RIGHT_CURLY_BRACKET */


/**
 *
 */
#ifndef M2MString_RIGHT_SQUARE_BRACKET
#define M2MString_RIGHT_SQUARE_BRACKET (M2MString *)"]"
#endif /* M2MString_RIGHT_SQUARE_BRACKET */


/**
 *
 */
#ifndef M2MString_SEMI_COLON
#define M2MString_SEMI_COLON (M2MString *)";"
#endif /* M2MString_SEMI_COLON */


/**
 *
 */
#ifndef M2MString_SLASH
#define M2MString_SLASH (M2MString *)"/"
#endif /* M2MString_SLASH */


/**
 * White space (=" ")
 */
#ifndef M2MString_SPACE
#define M2MString_SPACE (M2MString *)" "
#endif /* M2MString_SPACE */

/**
 *
 */
#ifndef M2MString_TAB
#define M2MString_TAB (M2MString *)"\t"
#endif /* M2MString_TAB */


/**
 *
 */
#ifndef M2MString_TILDE
#define M2MString_TILDE (M2MString *)"~"
#endif /* M2MString_TILDE */


/**
 *
 */
#ifndef M2MString_TWO_DOT_LEADER
#define M2MString_TWO_DOT_LEADER (M2MString *)".."
#endif /* M2MString_TWO_DOT_LEADER */


/**
 *
 */
#ifndef M2MString_UNDER_LINE
#define M2MString_UNDER_LINE (M2MString *)"_"
#endif /* M2MString_UNDER_LINE */


/**
 *
 */
#ifndef M2MString_ZENKAKU_SPACE
#define M2MString_ZENKAKU_SPACE (M2MString *)"　"
#endif /* M2MString_ZENKAKU_SPACE */



/*******************************************************************************
 * Public function
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
int32_t M2MString_compareTo (const M2MString *self, const M2MString *string);


/**
 * This method converts character code with "iconv".<br>
 *
 * @param fromString			Conversion target string
 * @param fromCharacterSetName	Character set name of original string
 * @param toCharacterSetName	Convert character set name
 * @param toString				Pointer for copying converted string(buffering is executed in this method)
 * @return						Pointer of converted string or NULL(means error)
 */
M2MString *M2MString_convertCharacterSet (const M2MString *fromString, const M2MString *fromCharacterSetName, const M2MString *toCharacterSetName, M2MString **toString);


/**
 * 
 * @param binary
 * @param binaryLength
 * @param buffer
 * @return
 */
M2MString *M2MString_convertFromBinaryToHexadecimalString (unsigned char *binary, const size_t binaryLength, M2MString **buffer);


/**
 * @param[in] boolean
 * @param[out] buffer
 * @param[in] bufferLength
 */
M2MString *M2MString_convertFromBooleanToString (const bool boolean, M2MString *buffer, const size_t bufferLength);


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
 * M2MString converter from hexadecimal string into binary data.<br>
 *
 * @param[in] hexadecimalString		Hexadecimal string
 * @param[out] buffer				Buffer for storing converted binary data
 * @return							Pointer of converted binary data or NULL (in case of error)
 */
unsigned char *M2MString_convertFromHexadecimalStringToBinary (const M2MString *hexadecimalString, unsigned char **buffer);


/**
 * M2MString converter from hexadecimal string into long number.<br>
 *
 * @param self			Hexadecimal string
 * @param selfLength	Length of hexadecimal string[Byte]
 * @return				Converted number from hexadecimal string
 */
uint32_t M2MString_convertFromHexadecimalStringToUnsignedLong (const M2MString *self, const size_t selfLength);


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
M2MString *M2MString_convertFromSignedIntegerToString (const int32_t number, M2MString **string);


/**
 * Convert signed long value into a string and copies it to the pointer. <br>
 * Since buffering of arrays is executed inside this function, so call <br>
 * "M2MHeap_free()" function on the caller side in order to prevent memory <br>
 * leak after using the string. <br>
 *
 * @param[in] number	Signed long number to be converted
 * @param[out] string	Pointer for copying the converted string (buffering is executed inside this function)
 * @return				Copied string or NULL (in case of error)
 */
M2MString *M2MString_convertFromSignedLongToString (const signed long number, M2MString **string);


/**
 * @param[in] string
 * @return
 */
bool M2MString_convertFromStringToBoolean (const M2MString *string);


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
int32_t M2MString_convertFromStringToSignedLong (const M2MString *string, const size_t stringLength);


/**
 * Convert the argument string to integer number.<br>
 *
 * @param[in] string		String indicating signed integer
 * @param[in] stringLength	Size of string[Byte]
 * @return					Signed integer converted from string
 */
int32_t M2MString_convertFromStringToSignedInteger (const M2MString *string, const size_t stringLength);


/**
 * This method convert from string to 64bit integer number.<br>
 *
 * @param[in] string		String indicating signed long
 * @param[in] stringLength	Size of string[Byte]
 * @return					Signed 64bit integer number converted from string
 */
int64_t M2MString_convertFromStringToSignedLongLong (const M2MString *string, const size_t stringLength);


/**
 * This method convert from string to 32bit unsigned integer number.<br>
 *
 * @param[in] string		String indicating signed long
 * @param[in] stringLength	Size of string[Byte]
 * @return					Unsigned 32bit integer number converted from string
 */
uint32_t M2MString_convertFromStringToUnsignedInteger (const M2MString *string, const size_t stringLength);


/**
 * This method convert from string to 32bit unsigned long number.<br>
 *
 * @param[in] string		String indicating signed long
 * @param[in] stringLength	Size of string[Byte]
 * @return					32bit unsigned long number converted from string
 */
uint32_t M2MString_convertFromStringToUnsignedLong (const M2MString *string, const size_t stringLength);


/**
 * This method converts from 32bit unsigned integer to string.<br>
 * Caller must provide enough buffers as "buffer" argument.<br>
 *
 * @param[in] number		conversion target number
 * @param[out] buffer		Buffer for copying unsigned integer string
 * @param[in] bufferLength	Length of Buffer[Byte]
 */
M2MString *M2MString_convertFromUnsignedIntegerToString (const uint32_t number, M2MString *buffer, const size_t bufferLength);


/**
 * This method converts from unsigned long to string.<br>
 * Caller must provide enough buffers as "buffer" argument.<br>
 *
 * @param[in] number		Conversion target number
 * @param[out] buffer		Array for copying integer string
 * @param[in] bufferLength	Length of array[Byte]
 * @return					Converted hexadecimal number string or NULL (in case of error)
 */
M2MString *M2MString_convertFromUnsignedLongToHexadecimalString (const uint32_t number, M2MString *buffer, const size_t bufferLength);


/**
 * This method converts from unsigned long to string.<br>
 * Caller must provide enough buffers as "buffer" argument.<br>
 *
 * @param[in] number		Conversion target number
 * @param[out] buffer		Array for copying integer string
 * @param[in] bufferLength	Length of array[Byte]
 */
M2MString *M2MString_convertFromUnsignedLongToString (const uint32_t number, M2MString *buffer, const size_t bufferLength);


/**
 * Converts a UTF-16 string to UTF-8. Returns a new string that must be freed<br>
 * or NULL if no conversion was needed.<br>
 *
 * @param[in,out] string
 * @param[in] stringLength
 * @return
 */
M2MString *M2MString_convertFromUTF16ToUTF8 (M2MString **string, size_t *stringLenngth);


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
 * This method converts variables into string in the indicated format.<br>
 *
 * @param buffer		Buffer for copying translated strings
 * @param bufferLength	Length of buffer[Byte]
 * @param format		Format for translation into string
 * @return				Length of converted strings[Byte] or -1(means error)
 */
int32_t M2MString_format (M2MString *buffer, const size_t bufferLength, const M2MString *format, ...);


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
 * @param self
 * @return
 */
bool M2MString_isNumber (const M2MString self);


/**
 * @param self
 * @return
 */
bool M2MString_isSpace (const M2MString *self);


/**
 * @param self
 * @return
 */
bool M2MString_isUTF (const M2MString *self);


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
 * This method replaces each substring of this string.<br>
 * It matches the literal target sequence with the specified literal <br>
 * replacement sequence.<br>
 *
 * @param[in] self			original string
 * @param[in] target		sequence of character values to be replaced
 * @param[in] replacement	replacement sequence of character values
 * @param[out] buffer		buffer for copying replaced string
 * @param[in] bufferLength	length of buffer[Byte]
 * @return					replaced string or NULL(means error)
 */
M2MString *M2MString_replace (const M2MString *self, const M2MString *target, const M2MString *replacement, M2MString *buffer, const size_t bufferLength);


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
 */
M2MString *M2MString_split (M2MString *string, const M2MString *delimiter, M2MString **savePoint);


/**
 * @param self
 * @param buffer
 * @param bufferLength
 * @return
 */
M2MString *M2MString_toLowerCase (const M2MString *self, M2MString *buffer, const size_t bufferLength);


/**
 * @param self
 * @param buffer
 * @param bufferLength
 * @return
 */
M2MString *M2MString_toUpperCase (const M2MString *self, M2MString *buffer, const size_t bufferLength);



#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2M_LANG_M2MSTRING_H_ */
