#include <gtest/gtest.h>
#include <sqlite3.h>
#include "../include/plant.h"

class PlantTest : public ::testing::Test {
protected:
    sqlite3* db;
    
    void SetUp() override {
        sqlite3_open(":memory:", &db);
        plant_create_table(db);
    }
    
    void TearDown() override {
        sqlite3_close(db);
    }
};

TEST_F(PlantTest, AddPlant) {
    Plant p = {0, "Роза", "Чайная", 7.0, ""};
    int rc = plant_add(db, &p);
    EXPECT_EQ(rc, SQLITE_OK);
    EXPECT_NE(p.id, 0);
}

TEST_F(PlantTest, GetAllPlants) {
    Plant p1 = {0, "Роза1", "Сорт1", 10.0, ""};
    Plant p2 = {0, "Роза2", "Сорт2", 15.0, ""};
    int rc1 = plant_add(db, &p1);
    int rc2 = plant_add(db, &p2);
    ASSERT_EQ(rc1, SQLITE_OK);
    ASSERT_EQ(rc2, SQLITE_OK);
    
    Plant *plants = NULL;
    int count = 0;
    int rc = plant_get_all(db, &plants, &count);
    
    EXPECT_EQ(rc, SQLITE_OK);
    EXPECT_EQ(count, 2);
    
    plant_free_list(plants, count);
}

TEST_F(PlantTest, GetPlantById) {
    Plant p = {0, "ТестРоза", "ТестСорт", 20.0, ""};
    plant_add(db, &p);
    
    Plant found;
    int rc = plant_get_by_id(db, p.id, &found);
    
    EXPECT_EQ(rc, SQLITE_OK);
    EXPECT_STREQ(found.name, "ТестРоза");
}
