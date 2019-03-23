/*******************************************************************************
 * M2MACLGroup.c
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

#include "m2m/lib/security/acl/M2MACLGroup.h"


/*******************************************************************************
 * Private method
 ******************************************************************************/
/**
 * @param self
 */
static void this_deleteName (M2MACLGroup *self)
	{
	M2MString *name = NULL;

	if (self!=NULL)
		{
		if ((name=M2MACLGroup_getName(self))!=NULL)
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
void M2MACLGroup_delete (M2MACLGroup **self)
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
 * This method detects group name string with group ID(=number).<br>
 *
 * @param[in] id		group ID number
 * @param[out] buffer	pointer of buffer(allocation is executed in this method)
 * @return				pointer of group name string or NULL(means error)
 */
unsigned char *M2MACLGroup_detectName (const gid_t id, M2MString **buffer)
	{
	//========== Variable ==========
	size_t bufferLength = 0;
	M2MString *tmpBuffer = NULL;
	struct group grp;
	struct group *result = NULL;

	//===== Get buffer size for constructing "passwd" structure =====
	if ((bufferLength=sysconf(_SC_GETGR_R_SIZE_MAX))!=-1)
		{
		//===== Allocate new memory =====
		if ((tmpBuffer=(M2MString *)M2MHeap_malloc(bufferLength))!=NULL)
			{
			//===== Get "passwd" structure depend on userID =====
			if (getgrgid_r(id, &grp, tmpBuffer, bufferLength, &result)==0)
				{
				//=====  =====
				if (M2MString_append(buffer, (M2MString *)(result->gr_name))!=NULL)
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
 * This method returns group ID number owned by ACL.<br>
 *
 * @param[in] self	ACL object
 * @return			group ID number which handles access control or 0(means error)
 */
uint64_t M2MACLGroup_getID (const M2MACLGroup *self)
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
 * @return			ALC group name string or NULL(means error)
 */
M2MString *M2MACLGroup_getName (const M2MACLGroup *self)
	{
	if (self!=NULL)
		{
		return self->name;
		}
	else
		{
		return NULL;
		}
	}


/**
 * Constructor.<br>
 * This method creates new ACL group object.<br>
 *
 * @return	new allocated ACL group object
 */
M2MACLGroup *M2MACLGroup_new ()
	{
	//========== Variable ==========
	M2MACLGroup *self = NULL;

	//===== Allocate new memory =====
	if ((self=(M2MACLGroup *)M2MHeap_malloc(sizeof(M2MACLGroup)))!=NULL)
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
 * @param self
 * @param id
 * @return
 */
M2MACLGroup *M2MACLGroup_setID (M2MACLGroup *self, const uint64_t id)
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
