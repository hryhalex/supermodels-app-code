#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "../include/db_helper.h"

sqlite3 *test_db;

int init_suite(void) {
    sqlite3_open(":memory:", &test_db);
    return 0;
}

int clean_suite(void) {
    sqlite3_close(test_db);
    return 0;
}

void test_db_execute(void) {
    int rc = db_execute(test_db, "CREATE TABLE test (id INTEGER);");
    CU_ASSERT_EQUAL(rc, SQLITE_OK);
}

void test_db_table_exists(void) {
    db_execute(test_db, "CREATE TABLE test_table (id INTEGER);");
    int exists = db_table_exists(test_db, "test_table");
    CU_ASSERT_EQUAL(exists, 1);
    
    int not_exists = db_table_exists(test_db, "nonexistent");
    CU_ASSERT_EQUAL(not_exists, 0);
}

void test_db_transaction(void) {
    int rc = db_begin_transaction(test_db);
    CU_ASSERT_EQUAL(rc, SQLITE_OK);
    
    rc = db_commit_transaction(test_db);
    CU_ASSERT_EQUAL(rc, SQLITE_OK);
    
    rc = db_begin_transaction(test_db);
    CU_ASSERT_EQUAL(rc, SQLITE_OK);
    
    rc = db_rollback_transaction(test_db);
    CU_ASSERT_EQUAL(rc, SQLITE_OK);
}

int main() {
    CU_initialize_registry();
    CU_pSuite suite = CU_add_suite("DB Helper Tests", init_suite, clean_suite);
    CU_add_test(suite, "test_db_execute", test_db_execute);
    CU_add_test(suite, "test_db_table_exists", test_db_table_exists);
    CU_add_test(suite, "test_db_transaction", test_db_transaction);
    
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return 0;
}
