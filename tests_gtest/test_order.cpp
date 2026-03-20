#include <gtest/gtest.h>
#include <sqlite3.h>
#include "../include/order.h"

class OrderTest : public ::testing::Test {
protected:
    sqlite3* db;
    
    void SetUp() override {
        sqlite3_open(":memory:", &db);
        order_create_table(db);
    }
    
    void TearDown() override {
        sqlite3_close(db);
    }
};

TEST_F(OrderTest, AddOrder) {
    Order o = {0, "Иванов Иван", "Букет невесты", 2, "2025-03-20", "", "pending"};
    int rc = order_add(db, &o);
    EXPECT_EQ(rc, SQLITE_OK);
    EXPECT_NE(o.id, 0);
}

TEST_F(OrderTest, GetAllOrders) {
    Order o1 = {0, "Клиент1", "Комп1", 1, "2025-03-20", "", "pending"};
    Order o2 = {0, "Клиент2", "Комп2", 2, "2025-03-21", "", "pending"};
    order_add(db, &o1);
    order_add(db, &o2);
    
    Order *orders = NULL;
    int count = 0;
    int rc = order_get_all(db, &orders, &count);
    
    EXPECT_EQ(rc, SQLITE_OK);
    EXPECT_EQ(count, 2);
    
    order_free_list(orders, count);
}

TEST_F(OrderTest, CompleteOrder) {
    Order o = {0, "ТестКлиент", "ТестКомпозиция", 1, "2025-03-20", "", "pending"};
    order_add(db, &o);
    
    int rc = order_complete(db, o.id, "2025-03-21");
    EXPECT_EQ(rc, SQLITE_OK);
    
    Order completed;
    order_get_by_id(db, o.id, &completed);
    EXPECT_STREQ(completed.status, "completed");
}
