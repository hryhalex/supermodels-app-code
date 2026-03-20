#ifndef ORDER_H
#define ORDER_H

#include <sqlite3.h>

typedef struct {
    int id;
    char customer_name[100];
    char composition_name[100];
    int quantity;
    char order_date[11];      // YYYY-MM-DD
    char completion_date[11]; // может быть NULL
    char status[20];          // pending, completed, urgent
} Order;

// Прототипы функций
int order_create_table(sqlite3 *db);
int order_add(sqlite3 *db, const Order *order);
int order_get_all(sqlite3 *db, Order **orders, int *count);
int order_get_by_id(sqlite3 *db, int id, Order *order);
int order_update(sqlite3 *db, const Order *order);
int order_delete(sqlite3 *db, int id);
int order_get_by_status(sqlite3 *db, const char *status, Order **orders, int *count);
int order_get_by_customer(sqlite3 *db, const char *customer, Order **orders, int *count);
int order_get_by_date_range(sqlite3 *db, const char *start_date, const char *end_date, Order **orders, int *count);
int order_complete(sqlite3 *db, int id, const char *completion_date);
void order_free_list(Order *orders, int count);

#endif
