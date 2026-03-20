#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "../include/order.h"

sqlite3 *test_db;

int init_suite(void) {
    sqlite3_open(":memory:", &test_db);
    order_create_table(test_db);
    return 0;
}

int clean_suite(void) {
    sqlite3_close(test_db);
    return 0;
}

void test_order_add(void) {
    Order o = {0, "Иванов Иван", "Букет невесты", 2, "2025-03-20", "", "pending"};
    int rc = order_add(test_db, &o);
    CU_ASSERT_EQUAL(rc, SQLITE_OK);
    CU_ASSERT_NOT_EQUAL(o.id, 0);
}

void test_order_get_all(void) {
    Order *orders = NULL;
    int count = 0;
    
    Order o1 = {0, "Клиент1", "Комп1", 1, "2025-03-20", "", "pending"};
    Order o2 = {0, "Клиент2", "Комп2", 2, "2025-03-21", "", "pending"};
    order_add(test_db, &o1);
    order_add(test_db, &o2);
    
    int rc = order_get_all(test_db, &orders, &count);
    CU_ASSERT_EQUAL(rc, SQLITE_OK);
    CU_ASSERT_EQUAL(count, 2);
    
    order_free_list(orders, count);
}

void test_order_complete(void) {
    Order o = {0, "ТестКлиент", "ТестКомпозиция", 1, "2025-03-20", "", "pending"};
    order_add(test_db, &o);
    
    int rc = order_complete(test_db, o.id, "2025-03-21");
    CU_ASSERT_EQUAL(rc, SQLITE_OK);
    
    Order completed;
    order_get_by_id(test_db, o.id, &completed);
    CU_ASSERT_STRING_EQUAL(completed.status, "completed");
}

int main() {
    CU_initialize_registry();
    CU_pSuite suite = CU_add_suite("Order Tests", init_suite, clean_suite);
    CU_add_test(suite, "test_order_add", test_order_add);
    CU_add_test(suite, "test_order_get_all", test_order_get_all);
    CU_add_test(suite, "test_order_complete", test_order_complete);
    
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return 0;
}
