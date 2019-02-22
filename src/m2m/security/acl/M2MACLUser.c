/*******************************************************************************
 * M2MACLUser.c
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

#include "m2m/security/acl/M2MACLUser.h"


/*******************************************************************************
 * Private method
 ******************************************************************************/
/**
 *
 * @param self
 */
static void this_deleteName (M2MACLUser *self)
	{
	M2MString *name = NULL;

	if (self!=NULL)
		{
		if ((name=M2MACLUser_getName(self))!=NULL)
			{
			M2MHeap_free(name);
			}
		else
			{
			}
		}
	else
		{
		}
	return;
	}


/*******************************************************************************
 * Public method
 ******************************************************************************/
/**
 *
 * @param self
 */
void M2MACLUser_delete (M2MACLUser **self)
	{
	if (self!=NULL && (*self)!=NULL)
		{
		this_deleteName((*self));
		M2MHeap_free((*self));
		}
	else
		{
		}
	return;
	}


/**
 * This method detects user name string from user ID.<br>
 *
 * @param id		User ID
 * @param buffer	pointer for copying user name string(buffering is executed in this method)
 * @return			copied user name string or NULL(means error)
 */
M2MString *ACLUser_detectName (const uid_t id, M2MString **buffer)
	{
	//========== Variable ==========
	size_t bufferLength = 0;
	M2MString *tmpBuffer = NULL;
	struct passwd pwd;
	struct passwd *result = NULL;

	//===== Get buffer size for constructing "passwd" structure =====
	if ((bufferLength=sysconf(_SC_GETPW_R_SIZE_MAX))!=-1)
		{
		//===== Allocate new memory =====
		if ((tmpBuffer=(M2MString *)M2MHeap_malloc(bufferLength))!=NULL)
			{
			//===== Get "passwd" structure depend on userID =====
			if (getpwuid_r(id, &pwd, tmpBuffer, bufferLength, &result)==0)
				{
				//=====  =====
				if (M2MString_append(buffer, (M2MString *)(result->pw_name))!=NULL)
					{
					//===== Release allocated memory =====
					M2MHeap_free(tmpBuffer);
					return (*buffer);
					}
				//===== Error handling =====
				else
					{
					//===== Release allocated memory =====
					M2MHeap_free(tmpBuffer);
					return NULL;
					}
				}
			//===== Error handling =====
			else
				{
				//===== Release allocated memory =====
				M2MHeap_free(tmpBuffer);
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			return NULL;
			}
		}
	//===== Error handling =====
	else
		{
		return NULL;
		}
	}


/**
 * This method returns user ID number owned by ACL.<br>
 *
 * @param[in] self	ACL object
 * @return			user ID number which handles access control or -1(means error)
 */
uint64_t M2MACLUser_getID (const M2MACLUser *self)
	{
	if (self!=NULL)
		{
		return self->id;
		}
	else
		{
		return 0;
		}
	}


/**
 *
 * @param[in] self
 * @return			ALC user name string or NULL(means error)
 */
unsigned char *M2MACLUser_getName (const M2MACLUser *self)
	{
	if (self!=NULL)
		{
		if (self->name!=NULL)
			{
			return self->name;
			}
		else
			{
			return NULL;
			}
		}
	else
		{
		return NULL;
		}
	}


/**
 * Constructor.<br>
 * This method creates new ACL user object.<br>
 *
 * @return	new allocated ACL user object
 */
M2MACLUser *M2MACLUser_new ()
	{
	//========== Variable ==========
	M2MACLUser *self = NULL;

	//===== Allocate new memory =====
	if ((self=(M2MACLUser *)M2MHeap_malloc(sizeof(M2MACLUser)))!=NULL)
		{
		//===== Initialization =====
		self->id = 0;
		self->name = NULL;
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
 * @param[in,out] self
 * @param[in] id
 * @return
 */
M2MACLUser *M2MACLUser_setID (M2MACLUser *self, const uint64_t id)
	{
	//===== Check argument =====
	if (self!=NULL)
		{
		self->id = id;
		return self;
		}
	//===== Argument error =====
	else
		{
		return NULL;
		}
	}



/* End Of File */
