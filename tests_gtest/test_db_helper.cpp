#include <gtest/gtest.h>
#include <sqlite3.h>
#include "../include/db_helper.h"

class DBHelperTest : public ::testing::Test {
protected:
    sqlite3* db;
    
    void SetUp() override {
        sqlite3_open(":memory:", &db);
    }
    
    void TearDown() override {
        sqlite3_close(db);
    }
};

TEST_F(DBHelperTest, ExecuteSQL) {
    int rc = db_execute(db, "CREATE TABLE test (id INTEGER);");
    EXPECT_EQ(rc, SQLITE_OK);
}

TEST_F(DBHelperTest, TableExists) {
    db_execute(db, "CREATE TABLE test_table (id INTEGER);");
    
    int exists = db_table_exists(db, "test_table");
    EXPECT_EQ(exists, 1);
    
    int not_exists = db_table_exists(db, "nonexistent");
    EXPECT_EQ(not_exists, 0);
}

TEST_F(DBHelperTest, Transaction) {
    int rc = db_begin_transaction(db);
    EXPECT_EQ(rc, SQLITE_OK);
    
    rc = db_commit_transaction(db);
    EXPECT_EQ(rc, SQLITE_OK);
    
    rc = db_begin_transaction(db);
    EXPECT_EQ(rc, SQLITE_OK);
    
    rc = db_rollback_transaction(db);
    EXPECT_EQ(rc, SQLITE_OK);
}
