#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "../include/plant.h"

sqlite3 *test_db;

int init_suite(void) {
    sqlite3_open(":memory:", &test_db);
    plant_create_table(test_db);
    return 0;
}

int clean_suite(void) {
    sqlite3_close(test_db);
    return 0;
}

void test_plant_add(void) {
    Plant p = {0, "Роза", "Чайная", 7.0};
    int rc = plant_add(test_db, &p);
    CU_ASSERT_EQUAL(rc, SQLITE_OK);
    CU_ASSERT_NOT_EQUAL(p.id, 0);
}

void test_plant_get_all(void) {
    Plant *plants = NULL;
    int count = 0;
    
    Plant p1 = {0, "Роза1", "Сорт1", 10.0};
    Plant p2 = {0, "Роза2", "Сорт2", 15.0};
    plant_add(test_db, &p1);
    plant_add(test_db, &p2);
    
    int rc = plant_get_all(test_db, &plants, &count);
    CU_ASSERT_EQUAL(rc, SQLITE_OK);
    CU_ASSERT_EQUAL(count, 2);
    
    plant_free_list(plants, count);
}

void test_plant_get_by_id(void) {
    Plant p = {0, "ТестРоза", "ТестСорт", 20.0};
    plant_add(test_db, &p);
    
    Plant found;
    int rc = plant_get_by_id(test_db, p.id, &found);
    CU_ASSERT_EQUAL(rc, SQLITE_OK);
    CU_ASSERT_STRING_EQUAL(found.name, "ТестРоза");
}

int main() {
    CU_initialize_registry();
    CU_pSuite suite = CU_add_suite("Plant Tests", init_suite, clean_suite);
    CU_add_test(suite, "test_plant_add", test_plant_add);
    CU_add_test(suite, "test_plant_get_all", test_plant_get_all);
    CU_add_test(suite, "test_plant_get_by_id", test_plant_get_by_id);
    
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return 0;
}
