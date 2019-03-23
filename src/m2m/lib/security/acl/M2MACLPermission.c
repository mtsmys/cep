/*******************************************************************************
 * M2MACLPermission.c
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

#include "m2m/lib/security/acl/M2MACLPermission.h"


/*******************************************************************************
 * Private method
 ******************************************************************************/
/**
 * This method converts from number to group permission flag.<br>
 *
 * @param[in] number	numeric notation of group permission(=1-7)
 * @return				permission flag
 */
static mode_t this_convertGroupPermission (const uint32_t number)
	{
	if (number==1)
		{
		return S_IXGRP;
		}
	else if (number==2)
		{
		return S_IWGRP;
		}
	else if (number==3)
		{
		return S_IWGRP | S_IXGRP;
		}
	else if (number==4)
		{
		return S_IRGRP;
		}
	else if (number==5)
		{
		return S_IRGRP | S_IXGRP;
		}
	else if (number==6)
		{
		return S_IRGRP | S_IWGRP;
		}
	else if (number==7)
		{
		return S_IRGRP | S_IWGRP | S_IXGRP;
		}
	//===== Argument error =====
	else
		{
		return 0;
		}
	}


/**
 * This method converts from number to others permission flag.<br>
 *
 * @param[in] number	numeric notation of others permission(=1-7)
 * @return				permission flag
 */
static mode_t this_convertOthersPermission (const uint32_t number)
	{
	if (number==1)
		{
		return S_IXOTH;
		}
	else if (number==2)
		{
		return S_IWOTH;
		}
	else if (number==3)
		{
		return S_IWOTH | S_IXOTH;
		}
	else if (number==4)
		{
		return S_IROTH;
		}
	else if (number==5)
		{
		return S_IROTH | S_IXOTH;
		}
	else if (number==6)
		{
		return S_IROTH | S_IWOTH;
		}
	else if (number==7)
		{
		return S_IROTH | S_IWOTH | S_IXOTH;
		}
	//===== Argument error =====
	else
		{
		return 0;
		}
	}


/**
 * This method converts from number to user permission flag.<br>
 *
 * @param[in] number	numeric notation of user permission(=1-7)
 * @return				permission flag
 */
static mode_t this_convertOwnerPermission (const uint32_t number)
	{
	if (number==1)
		{
		return S_IXUSR;
		}
	else if (number==2)
		{
		return S_IWUSR;
		}
	else if (number==3)
		{
		return S_IWUSR | S_IXUSR;
		}
	else if (number==4)
		{
		return S_IRUSR;
		}
	else if (number==5)
		{
		return S_IRUSR | S_IXUSR;
		}
	else if (number==6)
		{
		return S_IRUSR | S_IWUSR;
		}
	else if (number==7)
		{
		return S_IRUSR | S_IWUSR | S_IXUSR;
		}
	//===== Argument error =====
	else
		{
		return 0;
		}
	}


/**
 * This method returns default permission(=0421).<br>
 *
 * @return	default permission number(=12[bit])
 */
static mode_t this_getDefaultPermission ()
	{
	return S_IRUSR | S_IWUSR | S_IXUSR;
	}



/*******************************************************************************
 * Public method
 ******************************************************************************/
/**
 * This method converts permission mode string to permission flag.<br>
 *
 * @param[in] mode	string indicating permission. ex.) "0755" or "533"
 * @return			permission number
 */
mode_t M2MACLPermission_convertStringToNumber (const M2MString *mode)
	{
	//========== Variable ==========
	M2MString number[2];
	uint32_t modeLength = 0;
	uint32_t userPermission = 0;
	uint32_t groupPermission = 0;
	uint32_t othersPermission = 0;

	//===== Check argument =====
	if (mode!=NULL && ((modeLength=M2MString_length(mode))==3 || modeLength==4))
		{
		//===== In the case of length of permission mode is 4 =====
		if (modeLength==4)
			{
			//===== Check code of number string =====
			if (M2MString_isNumber(mode[1])==true
					&& M2MString_isNumber(mode[2])==true
					&& M2MString_isNumber(mode[3])==true)
				{
				memset(number, 0, sizeof(number));
				memcpy(number, &(mode[1]), 1);
				userPermission = M2MString_convertFromStringToUnsignedInteger(number, 1);
				memset(number, 0, sizeof(number));
				memcpy(number, &(mode[2]), 1);
				groupPermission = M2MString_convertFromStringToUnsignedInteger(number, 1);
				memset(number, 0, sizeof(number));
				memcpy(number, &(mode[3]), 1);
				othersPermission = M2MString_convertFromStringToUnsignedInteger(number, 1);
				}
			//===== Error handling =====
			else if (M2MString_isNumber(mode[1])==false)
				{
				return this_getDefaultPermission();
				}
			else if (M2MString_isNumber(mode[2])==false)
				{
				return this_getDefaultPermission();
				}
			else
				{
				return this_getDefaultPermission();
				}
			}
		//===== In the case of length of permission mode is 3 =====
		else
			{
			//===== Check code of number string =====
			if (M2MString_isNumber(mode[0])==true
					&& M2MString_isNumber(mode[1])==true
					&& M2MString_isNumber(mode[2])==true )
				{
				memset(number, 0, sizeof(number));
				memcpy(number, &(mode[0]), 1);
				userPermission = M2MString_convertFromStringToUnsignedInteger(number, 1);
				memset(number, 0, sizeof(number));
				memcpy(number, &(mode[1]), 1);
				groupPermission = M2MString_convertFromStringToUnsignedInteger(number, 1);
				memset(number, 0, sizeof(number));
				memcpy(number, &(mode[2]), 1);
				othersPermission = M2MString_convertFromStringToUnsignedInteger(number, 1);
				}
			//===== Error handling =====
			else if (M2MString_isNumber(mode[0])==false)
				{
				return this_getDefaultPermission();
				}
			else if (M2MString_isNumber(mode[1])==false)
				{
				return this_getDefaultPermission();
				}
			else
				{
				return this_getDefaultPermission();
				}
			}
		//===== Get permission mode =====
		return this_convertOwnerPermission(userPermission) | this_convertGroupPermission(groupPermission) | this_convertOthersPermission(othersPermission);
		}
	//===== Argument error =====
	else
		{
		return this_getDefaultPermission();
		}
	}


/**
 * This method converts permission flag to permission mode string.<br>
 *
 * @param[in] mode		permission number
 * @param[out] buffer	buffer for copying permission string(must be 4[Byte])
 * @return				pointer of permission string or NULL(means error)
 */
unsigned char *M2MACLPermission_convertNumberToString (const mode_t mode, M2MString buffer[4])
	{
	if (M2MACLPermission_validateNumber(mode)==true)
		{
		memset(buffer, 0, 4);
		//=====  =====

		return buffer;
		}
	else
		{
		return NULL;
		}
	}


/**
 * Destructor.<br>
 *
 * @param[in,out] self	ACL permission object
 */
void M2MACLPermission_delete (M2MACLPermission **self)
	{
	if (self!=NULL && (*self)!=NULL)
		{
		M2MHeap_free((*self));
		}
	else
		{
		}
	return;
	}


/**
 *
 * @param[in] self	ACL permission object
 * @return			mode value of permission
 */
mode_t M2MACLPermission_getMode (const M2MACLPermission *self)
	{
	if (self!=NULL && M2MACLPermission_validateNumber(self->mode)==true)
		{
		return self->mode;
		}
	else
		{
		return this_getDefaultPermission();
		}
	}


/**
 * Constructor.<br>
 *
 * @return	ACL permission object
 */
M2MACLPermission *M2MACLPermission_new ()
	{
	M2MACLPermission *self = NULL;

	if ((self=(M2MACLPermission *)M2MHeap_malloc(sizeof(M2MACLPermission)))!=NULL)
		{
		return self;
		}
	else
		{
		return NULL;
		}
	}


/**
 *
 * @param[in,out] self	ACL permission object
 * @param[in] mode		mode value of permission
 * @return
 */
M2MACLPermission *M2MACLPermission_setMode (M2MACLPermission *self, const mode_t mode)
	{
	if (self!=NULL && M2MACLPermission_validateNumber(mode)==true)
		{
		self->mode = mode;
		return self;
		}
	else
		{
		return NULL;
		}
	}


/**
 * This method validates argument "mode" number is suitable or not.<br>
 *
 * @param[in] mode	permission number
 * @return			true : validate, false : invalid
 */
bool M2MACLPermission_validateNumber (const mode_t mode)
	{
	return true;
	}



// End Of File
