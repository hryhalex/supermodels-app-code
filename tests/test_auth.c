#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "../include/auth.h"

sqlite3 *test_db;

int init_suite(void) {
    sqlite3_open(":memory:", &test_db);
    auth_create_table(test_db);
    return 0;
}

int clean_suite(void) {
    sqlite3_close(test_db);
    return 0;
}

void test_auth_register(void) {
    int rc = auth_register(test_db, "testuser", "testpass", "customer");
    CU_ASSERT_EQUAL(rc, SQLITE_OK);
}

void test_auth_login_success(void) {
    User user;
    auth_register(test_db, "logintest", "loginpass", "customer");
    int rc = auth_login(test_db, "logintest", "loginpass", &user);
    CU_ASSERT_EQUAL(rc, SQLITE_OK);
    CU_ASSERT_STRING_EQUAL(user.username, "logintest");
}

void test_auth_login_fail(void) {
    User user;
    auth_register(test_db, "failtest", "correctpass", "customer");
    int rc = auth_login(test_db, "failtest", "wrongpass", &user);
    CU_ASSERT_NOT_EQUAL(rc, SQLITE_OK);
}

int main() {
    CU_initialize_registry();
    CU_pSuite suite = CU_add_suite("Auth Tests", init_suite, clean_suite);
    CU_add_test(suite, "test_auth_register", test_auth_register);
    CU_add_test(suite, "test_auth_login_success", test_auth_login_success);
    CU_add_test(suite, "test_auth_login_fail", test_auth_login_fail);
    
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return 0;
}
