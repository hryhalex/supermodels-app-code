#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "../include/order.h"

int order_create_table(sqlite3 *db) {
    char *err_msg = 0;
    const char *sql = "CREATE TABLE IF NOT EXISTS orders ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "customer_name TEXT NOT NULL,"
                      "composition_name TEXT NOT NULL,"
                      "quantity INTEGER NOT NULL,"
                      "order_date TEXT NOT NULL,"
                      "completion_date TEXT,"
                      "status TEXT DEFAULT 'pending');";
    
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return rc;
    }
    return SQLITE_OK;
}

int order_add(sqlite3 *db, const Order *order) {
    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO orders (customer_name, composition_name, quantity, order_date, completion_date, status) "
                      "VALUES (?, ?, ?, ?, ?, ?)";
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) return rc;
    
    sqlite3_bind_text(stmt, 1, order->customer_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, order->composition_name, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, order->quantity);
    sqlite3_bind_text(stmt, 4, order->order_date, -1, SQLITE_STATIC);
    
    if (order->completion_date[0] != '\0') {
        sqlite3_bind_text(stmt, 5, order->completion_date, -1, SQLITE_STATIC);
    } else {
        sqlite3_bind_null(stmt, 5);
    }
    
    sqlite3_bind_text(stmt, 6, order->status, -1, SQLITE_STATIC);
    
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        return rc;
    }
    
    *((int*)&order->id) = sqlite3_last_insert_rowid(db);
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int order_get_all(sqlite3 *db, Order **orders, int *count) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, customer_name, composition_name, quantity, order_date, "
                      "completion_date, status FROM orders ORDER BY order_date DESC";
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) return rc;
    
    *count = 0;
    Order *list = NULL;
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        list = realloc(list, (*count + 1) * sizeof(Order));
        if (!list) {
            sqlite3_finalize(stmt);
            return SQLITE_NOMEM;
        }
        
        list[*count].id = sqlite3_column_int(stmt, 0);
        strcpy(list[*count].customer_name, (const char*)sqlite3_column_text(stmt, 1));
        strcpy(list[*count].composition_name, (const char*)sqlite3_column_text(stmt, 2));
        list[*count].quantity = sqlite3_column_int(stmt, 3);
        strcpy(list[*count].order_date, (const char*)sqlite3_column_text(stmt, 4));
        
        const char *comp_date = (const char*)sqlite3_column_text(stmt, 5);
        if (comp_date) {
            strcpy(list[*count].completion_date, comp_date);
        } else {
            list[*count].completion_date[0] = '\0';
        }
        
        strcpy(list[*count].status, (const char*)sqlite3_column_text(stmt, 6));
        (*count)++;
    }
    
    *orders = list;
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int order_get_by_id(sqlite3 *db, int id, Order *order) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, customer_name, composition_name, quantity, order_date, "
                      "completion_date, status FROM orders WHERE id = ?";
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) return rc;
    
    sqlite3_bind_int(stmt, 1, id);
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        order->id = sqlite3_column_int(stmt, 0);
        strcpy(order->customer_name, (const char*)sqlite3_column_text(stmt, 1));
        strcpy(order->composition_name, (const char*)sqlite3_column_text(stmt, 2));
        order->quantity = sqlite3_column_int(stmt, 3);
        strcpy(order->order_date, (const char*)sqlite3_column_text(stmt, 4));
        
        const char *comp_date = (const char*)sqlite3_column_text(stmt, 5);
        if (comp_date) {
            strcpy(order->completion_date, comp_date);
        } else {
            order->completion_date[0] = '\0';
        }
        
        strcpy(order->status, (const char*)sqlite3_column_text(stmt, 6));
        sqlite3_finalize(stmt);
        return SQLITE_OK;
    }
    
    sqlite3_finalize(stmt);
    return SQLITE_DONE;
}

int order_update(sqlite3 *db, const Order *order) {
    sqlite3_stmt *stmt;
    const char *sql = "UPDATE orders SET customer_name = ?, composition_name = ?, quantity = ?, "
                      "order_date = ?, completion_date = ?, status = ? WHERE id = ?";
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) return rc;
    
    sqlite3_bind_text(stmt, 1, order->customer_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, order->composition_name, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, order->quantity);
    sqlite3_bind_text(stmt, 4, order->order_date, -1, SQLITE_STATIC);
    
    if (order->completion_date[0] != '\0') {
        sqlite3_bind_text(stmt, 5, order->completion_date, -1, SQLITE_STATIC);
    } else {
        sqlite3_bind_null(stmt, 5);
    }
    
    sqlite3_bind_text(stmt, 6, order->status, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 7, order->id);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? SQLITE_OK : rc;
}

int order_delete(sqlite3 *db, int id) {
    sqlite3_stmt *stmt;
    const char *sql = "DELETE FROM orders WHERE id = ?";
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) return rc;
    
    sqlite3_bind_int(stmt, 1, id);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? SQLITE_OK : rc;
}

int order_get_by_status(sqlite3 *db, const char *status, Order **orders, int *count) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, customer_name, composition_name, quantity, order_date, "
                      "completion_date, status FROM orders WHERE status = ? ORDER BY order_date DESC";
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) return rc;
    
    sqlite3_bind_text(stmt, 1, status, -1, SQLITE_STATIC);
    
    *count = 0;
    Order *list = NULL;
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        list = realloc(list, (*count + 1) * sizeof(Order));
        if (!list) {
            sqlite3_finalize(stmt);
            return SQLITE_NOMEM;
        }
        
        list[*count].id = sqlite3_column_int(stmt, 0);
        strcpy(list[*count].customer_name, (const char*)sqlite3_column_text(stmt, 1));
        strcpy(list[*count].composition_name, (const char*)sqlite3_column_text(stmt, 2));
        list[*count].quantity = sqlite3_column_int(stmt, 3);
        strcpy(list[*count].order_date, (const char*)sqlite3_column_text(stmt, 4));
        
        const char *comp_date = (const char*)sqlite3_column_text(stmt, 5);
        if (comp_date) {
            strcpy(list[*count].completion_date, comp_date);
        } else {
            list[*count].completion_date[0] = '\0';
        }
        
        strcpy(list[*count].status, (const char*)sqlite3_column_text(stmt, 6));
        (*count)++;
    }
    
    *orders = list;
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int order_get_by_customer(sqlite3 *db, const char *customer, Order **orders, int *count) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, customer_name, composition_name, quantity, order_date, "
                      "completion_date, status FROM orders WHERE customer_name LIKE ? ORDER BY order_date DESC";
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) return rc;
    
    char pattern[120];
    snprintf(pattern, sizeof(pattern), "%%%s%%", customer);
    sqlite3_bind_text(stmt, 1, pattern, -1, SQLITE_STATIC);
    
    *count = 0;
    Order *list = NULL;
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        list = realloc(list, (*count + 1) * sizeof(Order));
        if (!list) {
            sqlite3_finalize(stmt);
            return SQLITE_NOMEM;
        }
        
        list[*count].id = sqlite3_column_int(stmt, 0);
        strcpy(list[*count].customer_name, (const char*)sqlite3_column_text(stmt, 1));
        strcpy(list[*count].composition_name, (const char*)sqlite3_column_text(stmt, 2));
        list[*count].quantity = sqlite3_column_int(stmt, 3);
        strcpy(list[*count].order_date, (const char*)sqlite3_column_text(stmt, 4));
        
        const char *comp_date = (const char*)sqlite3_column_text(stmt, 5);
        if (comp_date) {
            strcpy(list[*count].completion_date, comp_date);
        } else {
            list[*count].completion_date[0] = '\0';
        }
        
        strcpy(list[*count].status, (const char*)sqlite3_column_text(stmt, 6));
        (*count)++;
    }
    
    *orders = list;
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int order_get_by_date_range(sqlite3 *db, const char *start_date, const char *end_date, Order **orders, int *count) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, customer_name, composition_name, quantity, order_date, "
                      "completion_date, status FROM orders "
                      "WHERE order_date BETWEEN ? AND ? ORDER BY order_date DESC";
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) return rc;
    
    sqlite3_bind_text(stmt, 1, start_date, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, end_date, -1, SQLITE_STATIC);
    
    *count = 0;
    Order *list = NULL;
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        list = realloc(list, (*count + 1) * sizeof(Order));
        if (!list) {
            sqlite3_finalize(stmt);
            return SQLITE_NOMEM;
        }
        
        list[*count].id = sqlite3_column_int(stmt, 0);
        strcpy(list[*count].customer_name, (const char*)sqlite3_column_text(stmt, 1));
        strcpy(list[*count].composition_name, (const char*)sqlite3_column_text(stmt, 2));
        list[*count].quantity = sqlite3_column_int(stmt, 3);
        strcpy(list[*count].order_date, (const char*)sqlite3_column_text(stmt, 4));
        
        const char *comp_date = (const char*)sqlite3_column_text(stmt, 5);
        if (comp_date) {
            strcpy(list[*count].completion_date, comp_date);
        } else {
            list[*count].completion_date[0] = '\0';
        }
        
        strcpy(list[*count].status, (const char*)sqlite3_column_text(stmt, 6));
        (*count)++;
    }
    
    *orders = list;
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int order_complete(sqlite3 *db, int id, const char *completion_date) {
    sqlite3_stmt *stmt;
    const char *sql = "UPDATE orders SET completion_date = ?, status = 'completed' WHERE id = ?";
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) return rc;
    
    sqlite3_bind_text(stmt, 1, completion_date, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, id);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? SQLITE_OK : rc;
}

void order_free_list(Order *orders, int count) {
    (void)count;
    free(orders);
}
