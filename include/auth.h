#ifndef AUTH_H
#define AUTH_H

#include <sqlite3.h>
#include <stdbool.h>

typedef struct {
    int id;
    char username[50];
    char role[20];  // admin, manager, customer
} User;

// Прототипы функций
int auth_create_table(sqlite3 *db);
int auth_register(sqlite3 *db, const char *username, const char *password, const char *role);
int auth_login(sqlite3 *db, const char *username, const char *password, User *user);
int auth_change_password(sqlite3 *db, int user_id, const char *new_password);
int auth_get_user_by_id(sqlite3 *db, int id, User *user);
int auth_get_user_by_username(sqlite3 *db, const char *username, User *user);
int auth_delete_user(sqlite3 *db, int user_id);
int auth_get_all_users(sqlite3 *db, User **users, int *count);
void auth_free_user_list(User *users, int count);
bool auth_check_role(const User *user, const char *required_role);

// Вспомогательные функции для хеширования
void auth_hash_password(const char *password, char *hash, size_t hash_size);

#endif
