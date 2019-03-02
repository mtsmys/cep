/*******************************************************************************
 * M2MList.c : Imitation of STL M2MList Container
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

#include "m2m/util/list/M2MList.h"


/*******************************************************************************
 * Declaration of private function
 ******************************************************************************/
/**
 * Initialize "errorno" variable.<br>
 */
static void this_initErrorNumber ();



/*******************************************************************************
 * Private function
 ******************************************************************************/
/**
 * Set the size of the value held in the list structure.<br>
 *
 * @param[in,out] self	List structure object
 * @param[in] length	Size of value held by the list structure object[byte]
 * @return				List structure object or NULL (In case of error)
 */
static M2MList *this_setValueLength (M2MList *self, const size_t valueLength);


/*******************************************************************************
 * Private function
 ******************************************************************************/
/**
 * Copy the log message to the argument "buffer" pointer.<br>
 * Buffering of array for copying is executed inside the function.<br>
 * Therefore, it is necessary for caller to call the "M2MHeap_free()" function <br>
 * in order to prevent memory leak after using the variable.<br>
 *
 * @param[in] functionName		String indicating function name
 * @param[in] lineNumber		Line number in source file (can be embedded with "__LINE__")
 * @param[in] message			Message string
 * @param[out] buffer			Buffer to copy the created log message
 * @return						The pointer of "buffer" copied the created log message string or NULL (in case of error)
 */
static M2MString *this_createNewLogMessage (const M2MString *functionName, const uint32_t lineNumber, const M2MString *message, M2MString **buffer)
	{
	//========== Variable ==========
	M2MString *logLevelString = (M2MString *)"ERROR";
	M2MString time[64];
	M2MString lineNumberString[16];
	M2MString errnoMessage[256];
	M2MString threadID[128];
	size_t functionNameLength = 0;
	size_t messageLength = 0;

	//===== Check argument =====
	if (functionName!=NULL && (functionNameLength=M2MString_length(functionName))>0
			&& message!=NULL && (messageLength=M2MString_length(message))>0
			&& buffer!=NULL)
		{
		//===== Get line number string =====
		memset(lineNumberString, 0, sizeof(lineNumberString));
		snprintf(lineNumberString, sizeof(lineNumberString)-1, (M2MString *)"%d", lineNumber);
		//===== Initialize array =====
		memset(time, 0, sizeof(time));
		//===== Get current time string from local calendar ======
		if (M2MDate_getLocalTimeString(time, sizeof(time))>0
				&& M2MSystem_getThreadIDString(threadID, sizeof(threadID))!=NULL)
			{
			//===== In the case of existing error number =====
			if (errno!=0 && strerror_r(errno, errnoMessage, sizeof(errnoMessage))==0)
				{
				//===== Create new log message string =====
				if (M2MString_append(buffer, M2MString_LEFT_SQUARE_BRACKET)!=NULL
						&& M2MString_append(buffer, time)!=NULL
						&& M2MString_append(buffer, M2MString_RIGHT_SQUARE_BRACKET)!=NULL

						&& M2MString_append(buffer, M2MString_LEFT_SQUARE_BRACKET)!=NULL
						&& M2MString_append(buffer, logLevelString)!=NULL
						&& M2MString_append(buffer, M2MString_RIGHT_SQUARE_BRACKET)!=NULL

						&& M2MString_append(buffer, M2MString_LEFT_SQUARE_BRACKET)!=NULL
						&& M2MString_append(buffer, (M2MString *)"tid=")!=NULL
						&& M2MString_append(buffer, threadID)!=NULL
						&& M2MString_append(buffer, M2MString_RIGHT_SQUARE_BRACKET)!=NULL

						&& M2MString_append(buffer, M2MString_LEFT_SQUARE_BRACKET)!=NULL
						&& M2MString_append(buffer, functionName)!=NULL
						&& M2MString_append(buffer, M2MString_COLON)!=NULL
						&& M2MString_append(buffer, lineNumberString)!=NULL
						&& M2MString_append(buffer, (M2MString *)"l")!=NULL
						&& M2MString_append(buffer, M2MString_RIGHT_SQUARE_BRACKET)!=NULL

						&& M2MString_append(buffer, M2MString_LEFT_SQUARE_BRACKET)!=NULL
						&& M2MString_append(buffer, errnoMessage)!=NULL
						&& M2MString_append(buffer, M2MString_COLON)!=NULL
						&& M2MString_append(buffer, M2MString_SPACE)!=NULL
						&& M2MString_append(buffer, message)!=NULL
						&& M2MString_append(buffer, M2MString_RIGHT_SQUARE_BRACKET)!=NULL
						)
					{
					//===== Initialize error number =====
					this_initErrorNumber();
					//===== Return created log message string =====
					return (*buffer);
					}
				//===== Error handling =====
				else if ((*buffer)!=NULL)
					{
					//===== Release allocated memory =====
					M2MHeap_free((*buffer));
					//===== Initialize error number =====
					this_initErrorNumber();
					return NULL;
					}
				else
					{
					//===== Initialize error number =====
					this_initErrorNumber();
					return NULL;
					}
				}
			//===== In the case of not existing error number =====
			else
				{
				//===== Create new log message string =====
				if (M2MString_append(buffer, M2MString_LEFT_SQUARE_BRACKET)!=NULL
						&& M2MString_append(buffer, time)!=NULL
						&& M2MString_append(buffer, M2MString_RIGHT_SQUARE_BRACKET)!=NULL

						&& M2MString_append(buffer, M2MString_LEFT_SQUARE_BRACKET)!=NULL
						&& M2MString_append(buffer, logLevelString)!=NULL
						&& M2MString_append(buffer, M2MString_RIGHT_SQUARE_BRACKET)!=NULL

						&& M2MString_append(buffer, M2MString_LEFT_SQUARE_BRACKET)!=NULL
						&& M2MString_append(buffer, (M2MString *)"tid=")!=NULL
						&& M2MString_append(buffer, threadID)!=NULL
						&& M2MString_append(buffer, M2MString_RIGHT_SQUARE_BRACKET)!=NULL

						&& M2MString_append(buffer, M2MString_LEFT_SQUARE_BRACKET)!=NULL
						&& M2MString_append(buffer, functionName)!=NULL
						&& M2MString_append(buffer, M2MString_COLON)!=NULL
						&& M2MString_append(buffer, lineNumberString)!=NULL
						&& M2MString_append(buffer, (M2MString *)"l")!=NULL
						&& M2MString_append(buffer, M2MString_RIGHT_SQUARE_BRACKET)!=NULL

						&& M2MString_append(buffer, M2MString_LEFT_SQUARE_BRACKET)!=NULL
						&& M2MString_append(buffer, message)!=NULL
						&& M2MString_append(buffer, M2MString_RIGHT_SQUARE_BRACKET)!=NULL
						)
					{
					//===== Initialize error number =====
					this_initErrorNumber();
					//===== Return created log message string =====
					return (*buffer);
					}
				//===== Error handling =====
				else if ((*buffer)!=NULL)
					{
					//===== Release allocated memory =====
					M2MHeap_free((*buffer));
					//===== Initialize error number =====
					this_initErrorNumber();
					return NULL;
					}
				else
					{
					//===== Initialize error number =====
					this_initErrorNumber();
					return NULL;
					}
				}
			}
		//===== Error handling =====
		else
			{
			//===== Initialize error number =====
			this_initErrorNumber();
			return NULL;
			}
		}
	//===== Argument error =====
	else if (logLevelString==NULL)
		{
		//===== Initialize error number =====
		this_initErrorNumber();
		return NULL;
		}
	else if (functionName==NULL || functionNameLength<=0)
		{
		//===== Initialize error number =====
		this_initErrorNumber();
		return NULL;
		}
	else if (message==NULL || messageLength<=0)
		{
		//===== Initialize error number =====
		this_initErrorNumber();
		return NULL;
		}
	else
		{
		//===== Initialize error number =====
		this_initErrorNumber();
		return NULL;
		}
	}


/**
 * Releases the heap memory of value held by list structure object.<br>
 *
 * @param[in,out] self	List structure object
 */
static void this_deleteValue (M2MList *self)
	{
	//===== Check the existence of value =====
	if (M2MList_getValue(self)!=NULL)
		{
		//===== Release heap memory =====
		M2MHeap_free(self->value);
		//===== Initialize value length =====
		this_setValueLength(self, 0);
		}
	//===== Error handling =====
	else
		{
		}
	return;
	}


/**
 * Initialize "errorno" variable.<br>
 */
static void this_initErrorNumber ()
	{
	errno = 0;
	return;
	}


/**
 * Print out error message to standard error output.<br>
 *
 * @param[in] functionName	String indicating function name
 * @param[in] lineNumber	Line number in source file (can be embedded with "__LINE__")
 * @param[in] message		Message string indicating error content
 */
static void this_printErrorMessage (const M2MString *functionName, const uint32_t lineNumber, const M2MString *message)
	{
	//========== Variable ==========
	M2MString *logMessage = NULL;

	//===== Create new log message =====
	if (this_createNewLogMessage(functionName, lineNumber, message, &logMessage)!=NULL)
		{
		//===== Print out log =====
		M2MSystem_println(logMessage);
		//===== Release allocated memory =====
		M2MHeap_free(logMessage);
		}
	//===== Error handling =====
	else
		{
		}
	return;
	}


/**
 * Set the size of the value held in the list structure.<br>
 *
 * @param[in,out] self	List structure object
 * @param[in] length	Size of value held by the list structure object[byte]
 * @return				List structure object or NULL (In case of error)
 */
static M2MList *this_setValueLength (M2MList *self, const size_t valueLength)
	{
	//========== Variable ==========
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MList.this_setValueLength()";

	//===== Check argument =====
	if (self!=NULL)
		{
		self->valueLength = valueLength;
		return self;
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"self\" object is NULL");
		return NULL;
		}
	}


/**
 * Set a list structure object located next position of the argument.<br>
 *
 * @param[in,out] self	List structure object
 * @param[in] next		List structure object for setting
 * @return				List structure object or NULL (In case of error)
 */
static M2MList *this_setNext (M2MList *self, M2MList *next)
	{
	//========== Variable ==========
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MList.this_setNext()";

	//===== Check argument =====
	if (self!=NULL)
		{
		self->next = next;
		return self;
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"self\" object is NULL");
		return NULL;
		}
	}


/**
 * Set a list structure object located behind position of the argument.<br>
 *
 * @param[in,out] self	List structure object
 * @param[in] previous	List structure object for setting
 * @return				List structure object or NULL (In case of error)
 */
static M2MList *this_setPrevious (M2MList *self, M2MList *previous)
	{
	//========== Variable ==========
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MList.this_setPrevious()";

	//===== Check argument =====
	if (self!=NULL && previous!=NULL)
		{
		self->previous = previous;
		return self;
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		this_printErrorMessage(FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"self\" object is NULL");
		return NULL;
		}
	else
		{
		this_printErrorMessage(FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"previous\" object is NULL");
		return NULL;
		}
	}



/*******************************************************************************
 * Public function
 ******************************************************************************/
/**
 * Add a new node to the next position of argument list structure object.<br>
 * Then, as the member variable of the new node, the value is copied <br>
 * (deep copy). <br>
 * Therefore, to delete the added node, use the M2MList_remove() function.<br>
 *
 * @param[in,out] self		List structure object
 * @param[in] value			The value of added node stored in list structure object
 * @param[in] valueLength	Size of value[Byte]
 * @return					Pointer of added list structure object or NULL (In case of error)
 */
M2MList *M2MList_add (M2MList *self, const void *value, const size_t valueLength)
	{
	//========== Variable ==========
	M2MList *node = NULL;
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MList_add()";

	//===== Check argument =====
	if (self!=NULL && value!=NULL && 0<valueLength)
		{
		//===== Get the first node =====
		if ((self=M2MList_begin(self))!=NULL)
			{
			//===== In the case of existing a value in the first node =====
			if (M2MList_getValue(self)!=NULL)
				{
				//===== Move to end node =====
				self = M2MList_end(self);
				//===== Create new node =====
				node = M2MList_new();
				this_setPrevious(node, self);
				this_setNext(node, NULL);
				M2MList_set(node, value, valueLength);
				//===== Add a new node to the end of the list structure =====
				this_setNext(self, node);
				return node;
				}
			//===== In the case of not existing a value in the first node =====
			else
				{
				return M2MList_set(self, value, valueLength);
				}
			}
		//===== Error handling =====
		else
			{
			this_printErrorMessage(FUNCTION_NAME, __LINE__, (M2MString *)"Failed to get the begin node of M2MList object");
			return NULL;
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		this_printErrorMessage(FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"self\" object is NULL");
		return NULL;
		}
	else if (value==NULL)
		{
		this_printErrorMessage(FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"value\" object is NULL");
		return NULL;
		}
	else
		{
		this_printErrorMessage(FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"valueLength\" is negative");
		return NULL;
		}
	}


/**
 * Get the node located ahead of the link of the list structure object.<br>
 *
 * @param[in] self	List structure object
 * @return			First node of link of list structure object or NULL (in case of error)
 */
M2MList *M2MList_begin (M2MList *self)
	{
	//========== Variable ==========
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MList_begin()";

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== When it is not the first node =====
		while (M2MList_previous(self)!=self)
			{
			//===== Move to the previous node =====
			self = M2MList_previous(self);
			}
		return self;
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"self\" object is NULL");
		return NULL;
		}
	}


/**
 * Release the memory of all nodes of list structure object.<br>
 *
 * @param[in,out] self	List structure object
 */
void M2MList_delete (M2MList *self)
	{
	//========== Variable ==========
	M2MList *next = NULL;
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MList_delete()";

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Get first node =====
		if ((self=M2MList_begin(self))!=NULL)
			{
			//===== Repeat until reaching the end node =====
			while ((next=M2MList_next(self))!=NULL)
				{
				//===== Delete a node of list structure object =====
				this_deleteValue(self);
				M2MHeap_free(self);
				//===== Move to the next node =====
				self = next;
				}
			//===== Delete the end node =====
			this_deleteValue(self);
			M2MHeap_free(self);
			}
		//===== Error handling =====
		else
			{
			this_printErrorMessage(FUNCTION_NAME, __LINE__, (M2MString *)"Failed to get the begin node of M2MList object");
			}
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"self\" object is NULL");
		}
	return;
	}


/**
 * Returns the list structure object corresponding to the index.<br>
 *
 * @param[in] self		List structure object
 * @param[in] index		An integer indicating an index (0 to n-1)
 * @return 				List structure object or NULL (in case of error)
 */
M2MList *M2MList_detect (M2MList *self, const unsigned int index)
	{
	//========== Variable ==========
	unsigned int position = 0;
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MList_detect()";

	//===== Check argument =====
	if (self!=NULL && index<M2MList_length(self))
		{
		//===== Get the first node of the list structure object =====
		if ((self=M2MList_begin(self))!=NULL)
			{
			//===== Repeat until reaching the end node =====
			while (self!=NULL)
				{
				//===== Check index number matching =====
				if (index==position)
					{
					return self;
					}
				//===== When the index does not match =====
				else
					{
					//===== Move to next node of list structure =====
					self = M2MList_next(self);
					//===== Increment index number =====
					position++;
					}
				}
			return self;
			}
		//===== Error handling =====
		else
			{
			this_printErrorMessage(FUNCTION_NAME, __LINE__, (M2MString *)"Failed to get the begin node of M2MList object");
			return NULL;
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		this_printErrorMessage(FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"self\" object is NULL");
		return NULL;
		}
	else
		{
		this_printErrorMessage(FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"index\" number is invalid");
		return NULL;
		}
	}


/**
 * Get the node located at the end of the list structure object.<br>
 *
 * @param[in,out] self	List structure object
 * @return				End node of the list structure object or NULL (in case of error)
 */
M2MList *M2MList_end (M2MList *self)
	{
	//========== Variable ==========
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MList_end()";

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Repeat until reaching the end node =====
		while (M2MList_next(self)!=NULL)
			{
			self = M2MList_next(self);
			}
		//===== Return the end node =====
		return self;
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"self\" object is NULL");
		return NULL;
		}
	}


/**
 * Return value of the list structure object existing at the index position.<br>
 *
 * @param[in] self	List structure object
 * @return			Value stored in list structure object or NULL (in case of error)
 */
void *M2MList_getValue (M2MList *self)
	{
	//========== Variable ==========
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MList_getValue()";

	//===== Check argument =====
	if (self!=NULL)
		{
		return self->value;
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"self\" object is NULL");
		return NULL;
		}
	}


/**
 * Return value length of the list structure object at the index position.<br>
 *
 * @param[in] self	List structure object
 * @return			Value length[Byte] stored in list structure object or NULL (in case of error)
 */
size_t M2MList_getValueLength (M2MList *self)
	{
	//========== Variable ==========
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MList_getValueLength()";

	//===== Check argument =====
	if (self!=NULL)
		{
		return self->valueLength;
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"self\" object is NULL");
		return 0;
		}
	}


/**
 * Check whether the list structure object has at least one value.<br>
 *
 * @param[in] self	List structure object
 * @return			true: In case of no value, false: In case of having values
 */
bool M2MList_isEmpty (M2MList *self)
	{
	//========== Variable ==========
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MList_isEmpty()";

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Get begin node =====
		if ((self=M2MList_begin(self))!=NULL)
			{
			//===== Repeat until moving to the end node =====
			while (M2MList_next(self)!=NULL)
				{
				//===== Confirm existence of value =====
				if (M2MList_getValue(self)!=NULL)
					{
					return false;
					}
				//===== When the value is not stored =====
				else
					{
					//===== Move to next node =====
					self = M2MList_next(self);
					}
				}
			//===== Confirm existence of value of end node =====
			if (M2MList_getValue(self)!=NULL)
				{
				return false;
				}
			//===== In the case of end node hasn't value =====
			else
				{
				return true;
				}
			}
		//===== Error handling =====
		else
			{
			this_printErrorMessage(FUNCTION_NAME, __LINE__, (M2MString *)"Failed to get the begin node of M2MList object");
			return true;
			}
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"self\" object is NULL");
		return true;
		}
	}


/**
 * Return number indicating length of nodes in the list structure object.<br>
 *
 * @param[in] self	List structure object
 * @return			The number of nodes of the linked list structure object
 */
unsigned int M2MList_length (M2MList *self)
	{
	//========== Variable ==========
	unsigned int length = 0;
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MList_length()";

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== Get begin node =====
		if ((self=M2MList_begin(self))!=NULL)
			{
			//===== Confirm existence of value =====
			if (M2MList_getValue(self)!=NULL)
				{
				//===== Count the number of nodes =====
				length++;
				}
			//===== When the value is not held =====
			else
				{
				// do nothing
				}
			//===== Confirm existence of value =====
			while (M2MList_next(self)!=NULL)
				{
				//===== Confirm existence of value =====
				if (M2MList_getValue(self)!=NULL)
					{
					//===== Count the number of nodes =====
					length++;
					}
				//===== When the value is not held =====
				else
					{
					// do nothing
					}
				//===== Move to next node =====
				self = M2MList_next(self);
				}
			//===== Return the number of list structure objects =====
			return length;
			}
		//===== Error handling =====
		else
			{
			this_printErrorMessage(FUNCTION_NAME, __LINE__, (M2MString *)"Failed to get the begin node of M2MList object");
			return 0;
			}
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"self\" object is NULL");
		return 0;
		}
	}


/**
 * Return a list structure object located after the argument one.<br>
 *
 * @param[in] self	List structure object
 * @return next 	List structure object located behind or NULL (if the position of argument node is end)
 */
M2MList *M2MList_next (const M2MList *self)
	{
	//========== Variable ==========
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MList_next()";

	//===== Check argument =====
	if (self!=NULL)
		{
		return self->next;
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"self\" object is NULL");
		return NULL;
		}
	}


/**
 * Get heap memory and create a new M2MList structure object.<br>
 *
 * @return	Created M2MList structure object or NULL (in case of error)
 */
M2MList *M2MList_new ()
	{
	//========== Variable ==========
	M2MList *self = NULL;
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MList_new()";

	//===== Get heap memory =====
	if ((self=(M2MList *)M2MHeap_malloc(sizeof(M2MList)))!=NULL)
		{
		//===== Initialize created object =====
		if (this_setPrevious(self, self)!=NULL)
			{
			//===== Initialize created object =====
			if (this_setNext(self, NULL)!=NULL)
				{
				return self;
				}
			//===== Error handling =====
			else
				{
				M2MList_delete(self);
				this_printErrorMessage(FUNCTION_NAME, __LINE__, (M2MString *)"Failed to set next pointer of newly created M2MList object");
				return NULL;
				}
			}
		//===== Error handling =====
		else
			{
			M2MList_delete(self);
			this_printErrorMessage(FUNCTION_NAME, __LINE__, (M2MString *)"Failed to set previous pointer of newly created M2MList object");
			return NULL;
			}
		}
	//===== Error handling =====
	else
		{
		this_printErrorMessage(FUNCTION_NAME, __LINE__, (M2MString *)"Failed to get heap memory for creating M2MList object");
		return NULL;
		}
	}


/**
 * Get the list structure object existing one before the argument.<br>
 * If the argument object is the first node, returns the same pointer.<br>
 *
 * @param[in] self	List structure object
 * @return			List structure object that located before or NULL (in case of argument error)
 */
M2MList *M2MList_previous (const M2MList *self)
	{
	//========== Variable ==========
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MList_previous()";

	//===== Check argument =====
	if (self!=NULL)
		{
		return self->previous;
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"self\" object is NULL");
		return NULL;
		}
	}


/**
 * Deletes a list structure object existing at the position of argument. <br>
 * If there is only one, delete only the value itself, not the object itself. <br>
 * <br>
 * [Caution]<br>
 * When calling this method, the caller must always receive the return <br>
 * value. <br>
 * This is because if the memory area of the node itself of the list <br>
 * structure object given by the argument is released, there is no way <br>
 * for receiving the link of the remaining node, resulting in an error.<br>
 *
 * @param[in,out] self	List structure objects to be deleted
 */
M2MList *M2MList_remove (M2MList *self)
	{
	//========== Variable ==========
	unsigned int length = 0;
	M2MList *previous = NULL;
	M2MList *next = NULL;
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MList_remove()";

	//===== Check argument =====
	if (self!=NULL)
		{
		//===== When there is no node having a value =====
		if ((length=M2MList_length(self))==0)
			{
			//===== Initialize previous and next link information =====
			this_setPrevious(self, self);
			this_setNext(self, NULL);
			return self;
			}
		//===== When there is only one node having value =====
		else if (length==1)
			{
			//===== Delete the value =====
			this_deleteValue(self);
			//===== Initialize previous and next link information =====
			this_setPrevious(self, self);
			this_setNext(self, NULL);
			return self;
			}
		//===== When there are two or more nodes with values =====
		else
			{
			//===== In the case of first node =====
			if (M2MList_previous(self)==self)
				{
				//===== Get next node =====
				if ((next=M2MList_next(self))!=NULL)
					{
					//===== Remove the first node from the link =====
					this_setPrevious(next, next);
					//===== Delete the node =====
					this_deleteValue(self);
					M2MHeap_free(self);
					return next;
					}
				//=====  =====
				else
					{
					return self;
					}
				}
			//===== When it is an end node =====
			else if (M2MList_next(self)==NULL)
				{
				//===== Get the previous node =====
				if ((previous=M2MList_previous(self))!=NULL)
					{
					//===== Replace the end node =====
					this_setNext(previous, NULL);
					//===== Delete the node =====
					this_deleteValue(self);
					M2MHeap_free(self);
					return previous;
					}
				//=====  =====
				else
					{
					return self;
					}
				}
			//===== When it is an intermediate node =====
			else
				{
				//===== Get previous and next node =====
				if ((previous = M2MList_previous(self))!=NULL
						&& (next = M2MList_next(self))!=NULL)
					{
					//===== Connect previous node and next node each other =====
					this_setNext(previous, next);
					this_setPrevious(next, previous);
					//===== Delete the node =====
					this_deleteValue(self);
					M2MHeap_free(self);
					return previous;
					}
				//=====  =====
				else
					{
					return self;
					}
				}
			}
		}
	//===== Argument error =====
	else
		{
		this_printErrorMessage(FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"self\" object is NULL");
		return self;
		}
	}


/**
 * Copy (deep copy) argument value as the variable of the list structure object.<br>
 * Therefore, even if releasing the heap memory with the "value" after <br>
 * calling the function, it does not affect the member variables of the <br>
 * list structure object at all.<br>
 *
 * @param[in,out] self		List structure object
 * @param[in] value			The value of set to the list structure object
 * @param[in] valueLength	Size of value[Byte]
 * @return					Pointer of list structure object set value or NULL (In case of error)
 */
M2MList *M2MList_set (M2MList *self, const void *value, const size_t valueLength)
	{
	//========== Variable ==========
	const M2MString *FUNCTION_NAME = (M2MString *)"M2MList_set()";

	//===== Check argument =====
	if (self!=NULL && value!=NULL && 0<valueLength)
		{
		//===== Initialize list node =====
		this_deleteValue(self);
		//===== Get heap memory =====
		if ((self->value=M2MHeap_malloc(valueLength+1))!=NULL)
			{
			//===== Set value of list node =====
			memcpy(self->value, value, valueLength);
			this_setValueLength(self, valueLength);
			return self;
			}
		//===== Error handling =====
		else
			{
			this_printErrorMessage(FUNCTION_NAME, __LINE__, (M2MString *)"Failed to get heap memory for copying a value");
			return NULL;
			}
		}
	//===== Argument error =====
	else if (self==NULL)
		{
		this_printErrorMessage(FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"self\" object is NULL");
		return NULL;
		}
	else if (value==NULL)
		{
		this_printErrorMessage(FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"value\" object is NULL");
		return NULL;
		}
	else
		{
		this_printErrorMessage(FUNCTION_NAME, __LINE__, (M2MString *)"Argument error! Indicated \"valueLength\" number is invalid");
		return NULL;
		}
	}



/* End of File */
