/*******************************************************************************
 * M2MGraphTest.c
 *
 * Copyright (c) 2018, Akihisa Yasuda
 * All rights reserved.
 ******************************************************************************/

#include "m2m/app/graph/M2MGraph.h"
#include <inttypes.h>
#include <CUnit/CUnit.h>
#include <CUnit/Console.h>



/*******************************************************************************
 * Private function
 ******************************************************************************/
/**
 * @return
 */
static M2MString *this_getDtabaseName ()
	{
	return (M2MString *)"hoge";
	}


/**
 * Test function for "M2MGraph_connect()"
 */
static void this_testConnect ()
	{
	return;
	}


/**
 * Test function for "M2MGraph_delete()"
 */
static void this_testDelete ()
	{
	return;
	}

/**
 * Test function for "M2MGraph_getDatabaseName()"
 */
static void this_testGetDatabaseName ()
	{
	return;
	}

/**
 * Test function for "M2MGraph_getFileDatabase()"
 */
static void this_testGetFileDatabase ()
	{
	return;
	}

/**
 * Test function for "M2MGraph_new()"
 */
static void this_testNew ()
	{
	M2MGraph *graph = NULL;

	CU_ASSERT_PTR_NOT_NULL(graph = M2MGraph_new(this_getDtabaseName()));
	CU_ASSERT_PTR_NULL(M2MGraph_new(NULL));

	M2MGraph_delete(&graph);

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
	CU_pSuite M2MGraphTest;

	//=====  =====
	CU_initialize_registry();
	//=====  =====
	M2MGraphTest = CU_add_suite("M2MGraphTest", NULL, NULL);
	//=====  =====
	CU_add_test(M2MGraphTest, "M2MGraph_new()", this_testNew);
	CU_add_test(M2MGraphTest, "M2MGraph_delete()", this_testDelete);
	CU_add_test(M2MGraphTest, "M2MGraph_getDatabaseName()", this_testGetDatabaseName);
	CU_add_test(M2MGraphTest, "M2MGraph_getFileDatabase()", this_testGetFileDatabase);
	CU_add_test(M2MGraphTest, "M2MGraph_connect()", this_testConnect);
	//=====  =====
	CU_console_run_tests();
	//=====  =====
	CU_cleanup_registry();

	return 0;
	}
*/


/* End Of File */
