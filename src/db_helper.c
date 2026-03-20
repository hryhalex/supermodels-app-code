#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "../include/db_helper.h"

int db_init(sqlite3 **db) {
    int rc = sqlite3_open(DB_PATH, db);
    if (rc != SQLITE_OK) {
        db_print_error("Opening database", sqlite3_errmsg(*db));
        return rc;
    }
    
    // Включаем foreign keys
    char *err_msg = 0;
    rc = sqlite3_exec(*db, "PRAGMA foreign_keys = ON;", 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        db_print_error("Enabling foreign keys", err_msg);
        sqlite3_free(err_msg);
        return rc;
    }
    
    return SQLITE_OK;
}

void db_close(sqlite3 *db) {
    if (db) {
        sqlite3_close(db);
    }
}

int db_execute(sqlite3 *db, const char *sql) {
    char *err_msg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        db_print_error("Executing SQL", err_msg);
        sqlite3_free(err_msg);
        return rc;
    }
    return SQLITE_OK;
}

int db_table_exists(sqlite3 *db, const char *table_name) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT name FROM sqlite_master WHERE type='table' AND name=?;";
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) return 0;
    
    sqlite3_bind_text(stmt, 1, table_name, -1, SQLITE_STATIC);
    
    int exists = (sqlite3_step(stmt) == SQLITE_ROW);
    sqlite3_finalize(stmt);
    return exists;
}

int db_get_last_insert_id(sqlite3 *db) {
    return sqlite3_last_insert_rowid(db);
}

int db_begin_transaction(sqlite3 *db) {
    return db_execute(db, "BEGIN TRANSACTION;");
}

int db_commit_transaction(sqlite3 *db) {
    return db_execute(db, "COMMIT;");
}

int db_rollback_transaction(sqlite3 *db) {
    return db_execute(db, "ROLLBACK;");
}

void db_print_error(const char *operation, const char *error_msg) {
    fprintf(stderr, "Database error during %s: %s\n", operation, error_msg);
}

int db_check_integrity(sqlite3 *db) {
    sqlite3_stmt *stmt;
    const char *sql = "PRAGMA integrity_check;";
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) return 0;
    
    int ok = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *result = (const char*)sqlite3_column_text(stmt, 0);
        ok = (strcmp(result, "ok") == 0);
        if (!ok) {
            fprintf(stderr, "Integrity check failed: %s\n", result);
        }
    }
    
    sqlite3_finalize(stmt);
    return ok;
}
