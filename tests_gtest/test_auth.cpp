#include <gtest/gtest.h>
#include <sqlite3.h>
#include "../include/auth.h"

class AuthTest : public ::testing::Test {
protected:
    sqlite3* db;
    
    void SetUp() override {
        sqlite3_open(":memory:", &db);
        auth_create_table(db);
    }
    
    void TearDown() override {
        sqlite3_close(db);
    }
};

TEST_F(AuthTest, RegisterUser) {
    int rc = auth_register(db, "testuser", "testpass", "customer");
    EXPECT_EQ(rc, SQLITE_OK);
}

TEST_F(AuthTest, LoginSuccess) {
    User user;
    auth_register(db, "logintest", "loginpass", "customer");
    int rc = auth_login(db, "logintest", "loginpass", &user);
    EXPECT_EQ(rc, SQLITE_OK);
    EXPECT_STREQ(user.username, "logintest");
}

TEST_F(AuthTest, LoginFail) {
    User user;
    auth_register(db, "failtest", "correctpass", "customer");
    int rc = auth_login(db, "failtest", "wrongpass", &user);
    EXPECT_NE(rc, SQLITE_OK);
}
