/*******************************************************************************
 * M2MACLUser.h
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

#ifndef M2M_SECURITY_ACL_M2MACLUSER_H_
#define M2M_SECURITY_ACL_M2MACLUSER_H_



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



#include "m2m/lang/M2MString.h"
#include "m2m/security/acl/M2MACLPermission.h"
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>



/*******************************************************************************
 * Class variable
 ******************************************************************************/
/**
 * @param id
 * @param name
 * @param permission
 */
#ifndef M2MACLUser
typedef struct
	{
	uint64_t id;
	M2MString *name;
	M2MACLPermission *permission;
	} M2MACLUser;
#endif /* M2MACLUser */



/*******************************************************************************
 * Public method
 ******************************************************************************/
/**
 *
 * @param[in,out] self	ACL user object
 */
void M2MACLUser_delete (M2MACLUser **self);


/**
 * This method detects user name string with user ID.<br>
 *
 * @param[in] id		user ID(=number of 12[bit])
 * @param[out] buffer	pointer for copying user name string(buffering is executed in this method)
 * @return				copied user name string or NULL(means error)
 */
unsigned char *M2MACLUser_detectName (const uid_t id, M2MString **buffer);


/**
 * This method returns owner ID number owned by ACL user.<br>
 *
 * @param[in] self	ACL user object
 * @return			owner ID number which handles access control
 */
uint64_t M2MACLUser_getID (const M2MACLUser *self);


/**
 *
 * @param[in] self	ACL user object
 * @return			ALC user name string or NULL(means error)
 */
unsigned char *M2MACLUser_getName (const M2MACLUser *self);


/**
 *
 * @return
 */
M2MACLUser *M2MACLUser_new ();


/**
 *
 * @param[in,out] self	ACL user object
 * @param[in] id		user ID
 * @return
 */
M2MACLUser *M2MACLUser_setID (M2MACLUser *self, const uint64_t id);



#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2M_SECURITY_ACL_M2MACLUSER_H_ */
