#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "../include/composition.h"
#include "../include/plant.h"

sqlite3 *test_db;

int init_suite(void) {
    sqlite3_open(":memory:", &test_db);
    plant_create_table(test_db);
    composition_create_table(test_db);
    
    Plant p = {0, "ТестРоза", "ТестСорт", 10.0};
    plant_add(test_db, &p);
    
    return 0;
}

int clean_suite(void) {
    sqlite3_close(test_db);
    return 0;
}

void test_composition_add(void) {
    Composition c = {0, "ТестКомпозиция"};
    int rc = composition_add(test_db, &c);
    CU_ASSERT_EQUAL(rc, SQLITE_OK);
    CU_ASSERT_NOT_EQUAL(c.id, 0);
}

void test_composition_get_all(void) {
    Composition *comps = NULL;
    int count = 0;
    
    Composition c1 = {0, "Комп1"};
    Composition c2 = {0, "Комп2"};
    composition_add(test_db, &c1);
    composition_add(test_db, &c2);
    
    int rc = composition_get_all(test_db, &comps, &count);
    CU_ASSERT_EQUAL(rc, SQLITE_OK);
    CU_ASSERT_EQUAL(count, 2);
    
    composition_free_list(comps, count);
}

void test_composition_add_plant(void) {
    Plant *plants = NULL;
    int count = 0;
    plant_get_all(test_db, &plants, &count);
    CU_ASSERT(count > 0);
    
    Composition c = {0, "КомпДляТеста"};
    composition_add(test_db, &c);
    
    int rc = composition_add_plant(test_db, c.id, plants[0].id, 5);
    CU_ASSERT_EQUAL(rc, SQLITE_OK);
    
    plant_free_list(plants, count);
}

int main() {
    CU_initialize_registry();
    CU_pSuite suite = CU_add_suite("Composition Tests", init_suite, clean_suite);
    CU_add_test(suite, "test_composition_add", test_composition_add);
    CU_add_test(suite, "test_composition_get_all", test_composition_get_all);
    CU_add_test(suite, "test_composition_add_plant", test_composition_add_plant);
    
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return 0;
}
