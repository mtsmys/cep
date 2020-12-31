/*******************************************************************************
 * M2MSQLiteConfigTest.c
 *
 * Copyright (c) 2018, Akihisa Yasuda
 * All rights reserved.
 ******************************************************************************/

#include "m2m/lib/db/M2MSQLite.h"
#include "m2m/lib/lang/M2MString.h"
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
 * Test function for "M2MSQLiteConfig_closeDatabase()"
 */
static void this_testCloseDatabase ()
	{
	M2MSQLiteConfig_closeDatabase(M2MSQLiteConfig_openDatabase(this_getTestTablePath()));

	M2MSQLiteConfig_closeDatabase(NULL);

	return;
	}


/**
 * Test function for "M2MSQLiteConfig_getPreparedStatement()"
 */
static void this_testGetPreparedStatement ()
	{
	}


/**
 * Test function for "M2MSQLiteConfig_getTableInfoSQL()"
 */
static void this_testGetTableInfoSQL ()
	{
	//========== Variable ==========
	M2MString *sql = NULL;
	const M2MString *TABLE_NAME = "hoge";

	//=====  =====
	M2MSQLiteConfig_getTableInfoSQL(TABLE_NAME, &sql);
	CU_ASSERT_PTR_NOT_NULL(sql);
	M2MHeap_free(sql);

	//=====  =====
	CU_ASSERT_PTR_NULL(M2MSQLiteConfig_getTableInfoSQL(TABLE_NAME, NULL));

	//=====  =====
	CU_ASSERT_PTR_NULL(M2MSQLiteConfig_getTableInfoSQL(NULL, &sql));

	return;
	}


/**
 * Test function for "M2MSQLiteConfig_isExistingTable()"
 */
static void this_testIsExistingTable ()
	{
	//========== Variable ==========
	sqlite3 *database = NULL;
	bool result = false;

	database = M2MSQLiteConfig_openDatabase(this_getTestTablePath());
	result = M2MSQLiteConfig_isExistingTable(database, "hoge");
	CU_ASSERT_FALSE(result);
	M2MSQLiteConfig_closeDatabase(database);

	return;
	}


/**
 * Test function for "M2MSQLiteConfig_openDatabase()"
 */
static void this_testOpenDatabase ()
	{
	//========== Variable ==========
	sqlite3 *database = NULL;

	database = M2MSQLiteConfig_openDatabase(this_getTestTablePath());
	CU_ASSERT_PTR_NOT_NULL(database);
	M2MSQLiteConfig_closeDatabase(database);
	database = NULL;

	database = M2MSQLiteConfig_openDatabase(NULL);
	CU_ASSERT_PTR_NULL(database);
	M2MSQLiteConfig_closeDatabase(database);
	database = NULL;

	database = M2MSQLiteConfig_openDatabase((M2MString *)"");
	CU_ASSERT_PTR_NULL(database);
	M2MSQLiteConfig_closeDatabase(database);
	database = NULL;

	return;
	}


/**
 * Test function for "M2MSQLiteConfig_setAutoVacuum()"
 */
static void this_testSetAutoVacuum ()
	{
	}


/**
 * Test function for "M2MSQLiteConfig_setSynchronous()"
 */
static void this_testSetSynchronous ()
	{
	}


/**
 * Test function for "M2MSQLiteConfig_setUTF8()"
 */
static void this_testSetUTF8 ()
	{
	}


/**
 * Test function for "M2MSQLiteConfig_vacuum()"
 */
static void this_testVacuum ()
	{
	}


/**
 * Test function for "M2MSQLiteConfig_next()"
 */
static void this_testNext ()
	{
	}


/**
 * Test function for "M2MSQLiteConfig_setValueIntoPreparedStatement()"
 */
static void this_testSetValueIntoPreparedStatement ()
	{
	}



/*******************************************************************************
 * Public function
 ******************************************************************************/
/**
 * Entry point for CUnit test program.<br>
 *
 * @param argc
 * @param argv
 * @return
 *
int main (int argc, char **argv)
	{
	//========== Variable ==========
	CU_pSuite M2MSQLiteConfigTest;

	//=====  =====
	CU_initialize_registry();
	//=====  =====
	M2MSQLiteConfigTest = CU_add_suite("M2MSQLiteConfigTest", NULL, NULL);
	//=====  =====
	CU_add_test(M2MSQLiteConfigTest, "testM2MSQLiteConfig_openDatabase()", this_testOpenDatabase);
	CU_add_test(M2MSQLiteConfigTest, "testM2MSQLiteConfig_closeDatabase()", this_testCloseDatabase);
	CU_add_test(M2MSQLiteConfigTest, "testM2MSQLiteConfig_getPreparedStatement()", this_testGetPreparedStatement);
	CU_add_test(M2MSQLiteConfigTest, "testM2MSQLiteConfig_isExistingTable()", this_testIsExistingTable);
	CU_add_test(M2MSQLiteConfigTest, "testM2MSQLiteConfig_getTableInfoSQL()", this_testGetTableInfoSQL);
	CU_add_test(M2MSQLiteConfigTest, "testM2MSQLiteConfig_next()", this_testNext);
	CU_add_test(M2MSQLiteConfigTest, "testM2MSQLiteConfig_setAutoVacuum()", this_testSetAutoVacuum);
	CU_add_test(M2MSQLiteConfigTest, "testM2MSQLiteConfig_setSynchronous()", this_testSetSynchronous);
	CU_add_test(M2MSQLiteConfigTest, "testM2MSQLiteConfig_setUTF8()", this_testSetUTF8);
	CU_add_test(M2MSQLiteConfigTest, "testM2MSQLiteConfig_setValueIntoPreparedStatement()", this_testSetValueIntoPreparedStatement);
	CU_add_test(M2MSQLiteConfigTest, "testM2MSQLiteConfig_vacuum()", this_testVacuum);
	//=====  =====
	CU_console_run_tests();
	//=====  =====
	CU_cleanup_registry();

	return 0;
	}
*/


/* End Of File */
