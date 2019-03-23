/*******************************************************************************
 * M2MACLGroup.h
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

#ifndef M2M_LIB_SECURITY_ACL_M2MACLGROUP_H_
#define M2M_LIB_SECURITY_ACL_M2MACLGROUP_H_



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



#include "m2m/lib/lang/M2MString.h"
#include "m2m/lib/security/acl/M2MACLPermission.h"
#include <sys/types.h>
#include <unistd.h>
#include <grp.h>


/*******************************************************************************
 * Class variable
 ******************************************************************************/
/**
 * @param id
 * @param name
 * @param permission
 */
#ifndef M2MACLGroup
typedef struct
	{
	uint64_t id;
	M2MString *name;
	M2MACLPermission *permission;
	} M2MACLGroup;
#endif /* M2MACLGroup */



/*******************************************************************************
 * Public method
 ******************************************************************************/
/**
 * Destructor.<br>
 * This method releases allocated memory for ACL group object.<br>
 *
 * @param[in,out] self	ACL group object for releasing allocated memory
 */
void M2MACLGroup_delete (M2MACLGroup **self);


/**
 * This method detects group name string with group ID(=number).<br>
 *
 * @param[in] id		ACL group ID(number of 12[bit])
 * @param[out] buffer	pointer of buffer(allocation is executed in this method)
 * @return				pointer of group name string or NULL(means error)
 */
M2MString *M2MACLGroup_detectName (const gid_t id, M2MString **buffer);


/**
 * This method returns group ID number owned by ACLGroup.<br>
 *
 * @param[in] self	ACL group object
 * @return			group ID number which handles access control or 0(means error)
 */
uint64_t M2MACLGroup_getID (const M2MACLGroup *self);


/**
 *
 * @param[in] self	ACL group object
 * @return			ALC group name string or NULL(means error)
 */
M2MString *M2MACLGroup_getName (const M2MACLGroup *self);


/**
 * Constructor.<br>
 * This method creates new ACL group object.<br>
 *
 * @return	new allocated ACL group object
 */
M2MACLGroup *M2MACLGroup_new ();


/**
 *
 * @param[in,out] self	ACL group object
 * @param[in] id		ACL group ID(number of 12[bit])
 * @return
 */
M2MACLGroup *M2MACLGroup_setID (M2MACLGroup *self, const uint64_t id);



#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* M2M_SECURITY_ACL_M2MACLGROUP_H_ */
