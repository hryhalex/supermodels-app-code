#include <gtest/gtest.h>
#include <sqlite3.h>
#include "../include/composition.h"
#include "../include/plant.h"

class CompositionTest : public ::testing::Test {
protected:
    sqlite3* db;
    
    void SetUp() override {
        sqlite3_open(":memory:", &db);
        plant_create_table(db);
        composition_create_table(db);
        
        // Добавляем тестовое растение
        Plant p = {0, "ТестРоза", "ТестСорт", 10.0};
        plant_add(db, &p);
    }
    
    void TearDown() override {
        sqlite3_close(db);
    }
};

TEST_F(CompositionTest, AddComposition) {
    Composition c = {0, "ТестКомпозиция"};
    int rc = composition_add(db, &c);
    EXPECT_EQ(rc, SQLITE_OK);
    EXPECT_NE(c.id, 0);
}

TEST_F(CompositionTest, GetAllCompositions) {
    Composition c1 = {0, "Комп1"};
    Composition c2 = {0, "Комп2"};
    composition_add(db, &c1);
    composition_add(db, &c2);
    
    Composition *comps = NULL;
    int count = 0;
    int rc = composition_get_all(db, &comps, &count);
    
    EXPECT_EQ(rc, SQLITE_OK);
    EXPECT_EQ(count, 2);
    
    composition_free_list(comps, count);
}

TEST_F(CompositionTest, AddPlantToComposition) {
    Plant *plants = NULL;
    int count = 0;
    plant_get_all(db, &plants, &count);
    ASSERT_GT(count, 0);
    
    Composition c = {0, "КомпДляТеста"};
    composition_add(db, &c);
    
    int rc = composition_add_plant(db, c.id, plants[0].id, 5);
    EXPECT_EQ(rc, SQLITE_OK);
    
    plant_free_list(plants, count);
}
