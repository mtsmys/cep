/*******************************************************************************
 * M2MACLPermission.h
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

#ifndef M2M_LIB_SECURITY_ACL_M2MACLPERMISSION_H_
#define M2M_LIB_SECURITY_ACL_M2MACLPERMISSION_H_



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



#include "m2m/lib/lang/M2MString.h"
#include <sys/types.h>
#include <sys/stat.h>


/*******************************************************************************
 * Class variable
 ******************************************************************************/
/**
 * M2MACLPermission structure object manages access control list information.<br>
 *
 * @param mode
 */
#ifndef M2MACLPermission
typedef struct
	{
	mode_t mode;
	} M2MACLPermission;
#endif /* M2MACLPermission */


/*******************************************************************************
 * Public method
 ******************************************************************************/
/**
 * This method converts permission mode string to permission flag.<br>
 *
 * @param[in] mode	string indicating permission. ex.) "0755" or "533"(head of "0" means octal notation)
 * @return			permission number
 */
mode_t M2MACLPermission_convertStringToNumber (const M2MString *mode);


/**
 * This method converts permission flag to permission mode string.<br>
 *
 * @param[in] mode		permission number
 * @param[out] buffer	buffer for copying permission string(must be 4[Byte])
 * @return				pointer of permission string or NULL(means error)
 */
M2MString *M2MACLPermission_convertNumberToString (const mode_t mode, M2MString buffer[4]);


/**
 * Destructor.<br>
 *
 * @param[in,out] self	ACL permission object
 */
void M2MACLPermission_delete (M2MACLPermission **self);


/**
 *
 * @param[in] self	ACL permission object
 * @return			mode value of permission
 */
mode_t M2MACLPermission_getMode (const M2MACLPermission *self);


/**
 * Constructor.<br>
 *
 * @return	ACL permission object
 */
M2MACLPermission *M2MACLPermission_new ();


/**
 *
 * @param[in,out] self	ACL permission object
 * @param[in] mode		mode value of permission
 * @return
 */
M2MACLPermission *M2MACLPermission_setMode (M2MACLPermission *self, const mode_t mode);


/**
 * This method validates argument "mode" number is suitable or not.<br>
 *
 * @param[in] mode	permission number
 * @return			true : validate, false : invalid
 */
bool M2MACLPermission_validateNumber (const mode_t mode);



#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2M_SECURITY_ACL_M2MACLPERMISSION_H_ */
