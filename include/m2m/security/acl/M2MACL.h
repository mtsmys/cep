/*******************************************************************************
 * M2MACL.h : ACL(Access Control List) permission manager.
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

#ifndef M2M_SECURITY_ACL_M2MACL_H_
#define M2M_SECURITY_ACL_M2MACL_H_



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



#include "m2m/security/acl/M2MACLUser.h"
#include "m2m/security/acl/M2MACLGroup.h"


/*******************************************************************************
 * Class variable
 ******************************************************************************/
/**
 * ACL structure object manages access control list information.<br>
 *
 * @param user
 * @param group
 */
#ifndef M2MACL
typedef struct
	{
	M2MACLUser *user;
	M2MACLGroup *group;
	} M2MACL;
#endif /* M2MACL */


/*******************************************************************************
 * Public method
 ******************************************************************************/
/**
 * @param[in,out] self	ACL object
 */
void M2MACL_delete (M2MACL **self);


/**
 *
 * @param self
 * @return
 */
M2MACLGroup *M2MACL_getGroup (const M2MACL *self);


/**
 *
 * @param self
 * @return
 */
M2MACLUser *M2MACL_getUser (const M2MACL *self);


/**
 * Constructor.<br>
 *
 * @return	ACL object
 */
M2MACL *M2MACL_new ();


/**
 *
 * @param self
 * @param groupID
 * @return
 */
M2MACL *M2MACL_setGroupID (M2MACL *self, const uint64_t groupID);


/**
 *
 * @param self
 * @param userID
 * @return
 */
M2MACL *M2MACL_setUserID (M2MACL *self, const uint64_t userID);



#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2M_SECURITY_ACL_M2MACL_H_ */
