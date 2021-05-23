/*******************************************************************************
 * M2MCEPTest.c
 *
 * Copyright (c) 2019, Akihisa Yasuda
 * All rights reserved.
 ******************************************************************************/

#include "m2m/cep/M2MCEP.h"
#include <inttypes.h>
#include <CUnit/CUnit.h>
#include <CUnit/Console.h>



/*******************************************************************************
 * Private function
 ******************************************************************************/
/**
 * Test function for "M2MCEP_delete()"
 */
static void this_testDelete ()
	{
	}


/**
 * Test function for "M2MCEP_getDatabaseName()"
 */
static void this_testGetDatabaseName ()
	{
	}


/**
 * Test function for "M2MCEP_getFileDatabase()"
 */
static void this_testGetFileDatabase ()
	{
	}


/**
 * Test function for "M2MCEP_getMemoryDatabase()"
 */
static void this_testGetMemoryDatabase ()
	{
	}


/**
 * Test function for "M2MCEP_getVersion()"
 */
static void this_testGetVersion ()
	{
	}


/**
 * Test function for "M2MCEP_insertCSV()"
 */
static void this_testInsertCSV ()
	{
	}


/**
 * Test function for "M2MCEP_new()"
 */
static void this_testNew ()
	{
	}


/**
 * Test function for "M2MCEP_select()"
 */
static void this_testSelect ()
	{
	}


/**
 * Test function for "M2MCEP_setMaxRecord()"
 */
static void this_testSetMaxRecord ()
	{
	}


/**
 * Test function for "M2MCEP_setPersistence()"
 */
static void this_testSetPersistence ()
	{
	}


/**
 * Test function for "M2MCEP_setSignalHandler()"
 */
static void this_testSetSignalHandler ()
	{
	}


/**
 * Test function for "M2MCEP_setVacuumRecord()"
 */
static void this_testSetVacuumRecord ()
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
	CU_pSuite M2MCEPTest;

	//=====  =====
	CU_initialize_registry();
	//=====  =====
	M2MCEPTest = CU_add_suite("M2MCEPTest", NULL, NULL);
	//=====  =====
	CU_add_test(M2MCEPTest, "M2MCEP_new()", this_testNew);
	CU_add_test(M2MCEPTest, "M2MCEP_getDatabaseName()", this_testGetDatabaseName);
	CU_add_test(M2MCEPTest, "M2MCEP_getFileDatabase()", this_testGetFileDatabase);
	CU_add_test(M2MCEPTest, "M2MCEP_getMemoryDatabase()", this_testGetMemoryDatabase);
	CU_add_test(M2MCEPTest, "M2MCEP_getVersion()", this_testGetVersion);
	CU_add_test(M2MCEPTest, "M2MCEP_insertCSV()", this_testInsertCSV);
	CU_add_test(M2MCEPTest, "M2MCEP_select()", this_testSelect);
	CU_add_test(M2MCEPTest, "M2MCEP_setMaxRecord()", this_testSetMaxRecord);
	CU_add_test(M2MCEPTest, "M2MCEP_setPersistence()", this_testSetPersistence);
	CU_add_test(M2MCEPTest, "M2MCEP_setSignalHandler()", this_testSetSignalHandler);
	CU_add_test(M2MCEPTest, "M2MCEP_setVacuumRecord()", this_testSetVacuumRecord);
	CU_add_test(M2MCEPTest, "M2MCEP_delete()", this_testDelete);
	//=====  =====
	CU_console_run_tests();
	//=====  =====
	CU_cleanup_registry();

	return 0;
	}
*/



/* End Of File */
