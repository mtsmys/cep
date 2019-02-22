/*******************************************************************************
 * M2MCharacterEncoding.h
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

#pragma once

#ifndef M2M_LANG_M2MCHARACTERENCODING_H_
#define M2M_LANG_M2MCHARACTERENCODING_H_



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



/*******************************************************************************
 * Class variable
 ******************************************************************************/
/**
 * This is a character encoding capable of encoding all possible characters in Unicode.
 */
#ifndef M2MCharacterEncoding_UTF8
#define M2MCharacterEncoding_UTF8 (unsigned char *)"UTF-8"
#endif /* M2MCharacterEncoding_UTF8 */


/**
 *
 */
#ifndef M2MCharacterEncoding_SJIS
#define M2MCharacterEncoding_SJIS (unsigned char *)"SHIFT_JIS"
#endif /* M2MCharacterEncoding_SJIS */


/**
 *
 */
#ifndef M2MCharacterEncoding_CP932
#define M2MCharacterEncoding_CP932 (unsigned char *)"CP932"
#endif /* M2MCharacterEncoding_CP932 */



#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2M_LANG_CHARACTERENCODING_H_ */
