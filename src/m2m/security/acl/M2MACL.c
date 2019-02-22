/*******************************************************************************
 * M2MACL.c :
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

#include "m2m/security/acl/M2MACL.h"


/*******************************************************************************
 * Private method
 ******************************************************************************/
/**
 *
 * @param[in,out] self
 */
static void this_deleteGroup (M2MACL *self)
	{
	//========== Variable ==========
	M2MACLGroup *group = NULL;

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Check existence of ACLGroup object =====
		if ((group=M2MACL_getGroup(self))!=NULL)
			{
			M2MACLGroup_delete(&group);
			}
		else
			{
			}
		}
	//===== Argument error =====
	else
		{
		}
	return;
	}


/**
 *
 * @param[in,out] self
 */
static void this_deleteUser (M2MACL *self)
	{
	//========== Variable ==========
	M2MACLUser *user = NULL;

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Check existence of ACLUser object =====
		if ((user=M2MACL_getUser(self))!=NULL)
			{
			M2MACLUser_delete(&user);
			}
		else
			{
			}
		}
	//===== Argument error =====
	else
		{
		}
	return;
	}


/**
 *
 * @param self
 * @param group
 * @return
 */
static M2MACL *this_setGroup (M2MACL *self, M2MACLGroup *group)
	{
	//===== Check argument =====
	if (self!=NULL && group!=NULL)
		{
		self->group = group;
		return self;
		}
	//===== Argument error =====
	else
		{
		return NULL;
		}
	}


/**
 *
 * @param self
 * @param user
 * @return
 */
static M2MACL *this_setUser (M2MACL *self, M2MACLUser *user)
	{
	//===== Check argument =====
	if (self!=NULL && user!=NULL)
		{
		self->user = user;
		return self;
		}
	//===== Argument error =====
	else
		{
		return NULL;
		}
	}



/*******************************************************************************
 * Public method
 ******************************************************************************/
/**
 * Destructor.<br>
 * This method removes allocated ACL object in ACL_new().<br>
 *
 * @param[in,out] self	ACL object
 */
void M2MACL_delete (M2MACL **self)
	{
	//===== Check argument =====
	if (self!=NULL && (*self)!=NULL)
		{
		this_deleteUser((*self));
		this_deleteGroup((*self));
		M2MHeap_free((*self));
		}
	//===== Argument error =====
	else
		{
		// do nothing
		}
	return;
	}


/**
 *
 * @param self
 * @return
 */
M2MACLGroup *M2MACL_getGroup (const M2MACL *self)
	{
	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Check existence of ACLGroup object =====
		if (self->group!=NULL)
			{
			return self->group;
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
 *
 * @param self
 * @return
 */
M2MACLUser *M2MACL_getUser (const M2MACL *self)
	{
	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Check existence of ACLUser object =====
		if (self->user!=NULL)
			{
			return self->user;
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
 * Constructor.<br>
 * This method allocates new memory for creating ACL structure object.<br>
 *
 * @return	ACL object
 */
M2MACL *M2MACL_new ()
	{
	//========== Variable ==========
	M2MACL *self = NULL;

	//===== Allocate new memory =====
	if ((self=(M2MACL *)M2MHeap_malloc(sizeof(M2MACL)))!=NULL)
		{
		return self;
		}
	//===== Error handling =====
	else
		{
		return NULL;
		}
	}


/**
 *
 * @param self
 * @param groupID
 * @return
 */
M2MACL *M2MACL_setGroupID (M2MACL *self, const uint64_t groupID)
	{
	//========== Variable ==========
	M2MACLGroup *group = NULL;

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Allocate new memory =====
		if ((group=M2MACLGroup_new())!=NULL)
			{
			//===== Set group ID =====
			if (M2MACLGroup_setID(group, groupID)!=NULL
					&& this_setGroup(self, group)!=NULL)
				{
				return self;
				}
			//===== Error handling =====
			else
				{
				M2MACLGroup_delete(&group);
				return NULL;
				}
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
 *
 * @param self
 * @param userID
 * @return
 */
M2MACL *M2MACL_setUserID (M2MACL *self, const uint64_t userID)
	{
	//========== Variable ==========
	M2MACLUser *user = NULL;

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Allocate new memory =====
		if ((user=M2MACLUser_new())!=NULL)
			{
			//===== Set user ID =====
			if (M2MACLUser_setID(user, userID)!=NULL
					&& this_setUser(self, user)!=NULL)
				{
				return self;
				}
			//===== Error handling =====
			else
				{
				M2MACLUser_delete(&user);
				return NULL;
				}
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



/* End Of File */
