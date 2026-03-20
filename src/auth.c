#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "../include/auth.h"
#include <openssl/sha.h>  // Для хеширования паролей

// Простое хеширование пароля (SHA-256)
void auth_hash_password(const char *password, char *hash, size_t hash_size) {
    unsigned char sha256_hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, password, strlen(password));
    SHA256_Final(sha256_hash, &sha256);
    
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        snprintf(hash + (i * 2), hash_size - (i * 2), "%02x", sha256_hash[i]);
    }
}

int auth_create_table(sqlite3 *db) {
    char *err_msg = 0;
    const char *sql = "CREATE TABLE IF NOT EXISTS users ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "username TEXT NOT NULL UNIQUE,"
                      "password_hash TEXT NOT NULL,"
                      "role TEXT NOT NULL);";
    
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return rc;
    }
    
    // Проверяем, есть ли admin, если нет — создаём
    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM users WHERE role = 'admin'", -1, &stmt, 0);
    if (rc == SQLITE_OK && sqlite3_step(stmt) == SQLITE_ROW && sqlite3_column_int(stmt, 0) == 0) {
        sqlite3_finalize(stmt);
        char admin_hash[65];
        auth_hash_password("admin", admin_hash, sizeof(admin_hash));
        sql = "INSERT INTO users (username, password_hash, role) VALUES ('admin', ?, 'admin')";
        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
        if (rc == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, admin_hash, -1, SQLITE_STATIC);
            sqlite3_step(stmt);
        }
    }
    sqlite3_finalize(stmt);
    
    return SQLITE_OK;
}

int auth_register(sqlite3 *db, const char *username, const char *password, const char *role) {
    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO users (username, password_hash, role) VALUES (?, ?, ?)";
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) return rc;
    
    char hash[65];
    auth_hash_password(password, hash, sizeof(hash));
    
    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, hash, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, role, -1, SQLITE_STATIC);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? SQLITE_OK : rc;
}

int auth_login(sqlite3 *db, const char *username, const char *password, User *user) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, username, password_hash, role FROM users WHERE username = ?";
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) return rc;
    
    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        char stored_hash[65];
        strcpy(stored_hash, (const char*)sqlite3_column_text(stmt, 2));
        
        char input_hash[65];
        auth_hash_password(password, input_hash, sizeof(input_hash));
        
        if (strcmp(stored_hash, input_hash) == 0) {
            user->id = sqlite3_column_int(stmt, 0);
            strcpy(user->username, (const char*)sqlite3_column_text(stmt, 1));
            strcpy(user->role, (const char*)sqlite3_column_text(stmt, 3));
            sqlite3_finalize(stmt);
            return SQLITE_OK;
        }
    }
    
    sqlite3_finalize(stmt);
    return SQLITE_ERROR;
}

int auth_change_password(sqlite3 *db, int user_id, const char *new_password) {
    sqlite3_stmt *stmt;
    const char *sql = "UPDATE users SET password_hash = ? WHERE id = ?";
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) return rc;
    
    char hash[65];
    auth_hash_password(new_password, hash, sizeof(hash));
    
    sqlite3_bind_text(stmt, 1, hash, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, user_id);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? SQLITE_OK : rc;
}

int auth_get_user_by_id(sqlite3 *db, int id, User *user) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, username, role FROM users WHERE id = ?";
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) return rc;
    
    sqlite3_bind_int(stmt, 1, id);
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        user->id = sqlite3_column_int(stmt, 0);
        strcpy(user->username, (const char*)sqlite3_column_text(stmt, 1));
        strcpy(user->role, (const char*)sqlite3_column_text(stmt, 2));
        sqlite3_finalize(stmt);
        return SQLITE_OK;
    }
    
    sqlite3_finalize(stmt);
    return SQLITE_DONE;
}

int auth_get_user_by_username(sqlite3 *db, const char *username, User *user) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, username, role FROM users WHERE username = ?";
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) return rc;
    
    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        user->id = sqlite3_column_int(stmt, 0);
        strcpy(user->username, (const char*)sqlite3_column_text(stmt, 1));
        strcpy(user->role, (const char*)sqlite3_column_text(stmt, 2));
        sqlite3_finalize(stmt);
        return SQLITE_OK;
    }
    
    sqlite3_finalize(stmt);
    return SQLITE_DONE;
}

int auth_delete_user(sqlite3 *db, int user_id) {
    sqlite3_stmt *stmt;
    const char *sql = "DELETE FROM users WHERE id = ? AND role != 'admin'";  // нельзя удалить админа
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) return rc;
    
    sqlite3_bind_int(stmt, 1, user_id);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? SQLITE_OK : rc;
}

int auth_get_all_users(sqlite3 *db, User **users, int *count) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, username, role FROM users ORDER BY username";
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) return rc;
    
    *count = 0;
    User *list = NULL;
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        list = realloc(list, (*count + 1) * sizeof(User));
        if (!list) {
            sqlite3_finalize(stmt);
            return SQLITE_NOMEM;
        }
        
        list[*count].id = sqlite3_column_int(stmt, 0);
        strcpy(list[*count].username, (const char*)sqlite3_column_text(stmt, 1));
        strcpy(list[*count].role, (const char*)sqlite3_column_text(stmt, 2));
        (*count)++;
    }
    
    *users = list;
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

void auth_free_user_list(User *users, int count) {
    (void)count;
    free(users);
}

bool auth_check_role(const User *user, const char *required_role) {
    return strcmp(user->role, required_role) == 0;
}
