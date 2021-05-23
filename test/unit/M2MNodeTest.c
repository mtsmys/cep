/*******************************************************************************
 * M2MNodeTest.c
 *
 * Copyright (c) 2018, Akihisa Yasuda
 * All rights reserved.
 ******************************************************************************/

#include "m2m/lib/db/M2MSQLite.h"
#include "m2m/graph/M2MNode.h"
#include <inttypes.h>
#include <CUnit/CUnit.h>
#include <CUnit/Console.h>



/*******************************************************************************
 * Private function
 ******************************************************************************/
/**
 * Return a string indicating database file path for test.<br>
 *
 * @return	String indicating database file path
 */
static M2MString *this_getTestTablePath ()
	{
	return (M2MString *)"/tmp/hoge.sqlite";
	}


/**
 * Test function for "M2MNode_delete()"
 */
static void this_testDelete ()
	{
	return;
	}


/**
 * Test function for "M2MNode_getID()"
 */
static void this_testGetID ()
	{
	return;
	}


/**
 * Test function for "M2MNode_getName()"
 */
static void this_testGetName ()
	{
	return;
	}


/**
 * Test function for "M2MNode_getProperty()"
 */
static void this_testGetProperty ()
	{
	return;
	}


/**
 * Test function for "M2MNode_new()"
 */
static void this_testNew ()
	{
	//========== Variable ==========
	sqlite3 *database = NULL;
	M2MString name[256];
	const M2MString *property = (M2MString *)"";

	database = M2MSQLiteConfig_openDatabase(this_getTestTablePath());

	memset(name, 0, sizeof(name));
	M2MString_getLocalTime(name, sizeof(name));

	CU_ASSERT(M2MNode_new(database, name, property));

	CU_ASSERT(M2MNode_new(database, NULL, property));

	CU_ASSERT(M2MNode_new(database, name, NULL));

	M2MSQLiteConfig_closeDatabase(database);

	return;
	}


/**
 * Test function for "M2MNode_setNestedSetsModel()"
 */
static void this_testSetNestedSetsModel ()
	{
	//========== Variable ==========
	sqlite3 *database = NULL;
	M2MString name[256];
	uint32_t nodeID = 0UL;
	uint32_t left = 1UL;
	uint32_t right = 255UL;
	const M2MString *property = (M2MString *)"";
/*
	database = M2MSQLiteConfig_openDatabase(this_getTestTablePath());

	memset(name, 0, sizeof(name));
	M2MString_getLocalTime(name, sizeof(name));

	if ((nodeID=M2MNode_getID(database, name))>0)
		{
		}
	else
		{
		nodeID = M2MNode_new(database, name, property);
		}

	CU_ASSERT_EQUAL(nodeID, M2MNode_setNestedSetsModel(database, nodeID, left, right));
	CU_ASSERT_NOT_EQUAL(nodeID, M2MNode_setNestedSetsModel(database, nodeID, left, left));
	CU_ASSERT_EQUAL(nodeID, M2MNode_setNestedSetsModel(database, nodeID, 0, 0));

	M2MSQLiteConfig_closeDatabase(database);
*/
	return;
	}



/*******************************************************************************
 * Public function
 ******************************************************************************/
/**
 * @param argc
 * @param argv
 * @return
 *
int main (int argc, char **argv)
	{
	//========== Variable ==========
	CU_pSuite M2MNodeTest;

	//=====  =====
	CU_initialize_registry();
	//=====  =====
	M2MNodeTest = CU_add_suite("M2MNodeTest", NULL, NULL);
	//=====  =====
	CU_add_test(M2MNodeTest, "M2MNode_new()", this_testNew);
	CU_add_test(M2MNodeTest, "M2MNode_delete()", this_testDelete);
	CU_add_test(M2MNodeTest, "M2MNode_getID()", this_testGetID);
	CU_add_test(M2MNodeTest, "M2MNode_getProperty()", this_testGetProperty);
	CU_add_test(M2MNodeTest, "M2MNode_getName()", this_testGetName);
	CU_add_test(M2MNodeTest, "M2MNode_setNestedSetsModel()", this_testSetNestedSetsModel);
	//=====  =====
	CU_console_run_tests();
	//=====  =====
	CU_cleanup_registry();

	return 0;
	}
*/


/* End Of File */
