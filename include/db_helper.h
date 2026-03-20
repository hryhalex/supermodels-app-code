#ifndef DB_HELPER_H
#define DB_HELPER_H

#include <sqlite3.h>
#include <stdbool.h>

// Константы
#define DB_PATH "data/greenhouse.db"

// Инициализация и закрытие
int db_init(sqlite3 **db);
void db_close(sqlite3 *db);

// Вспомогательные функции
int db_execute(sqlite3 *db, const char *sql);
int db_table_exists(sqlite3 *db, const char *table_name);
int db_get_last_insert_id(sqlite3 *db);

// Функции для транзакций
int db_begin_transaction(sqlite3 *db);
int db_commit_transaction(sqlite3 *db);
int db_rollback_transaction(sqlite3 *db);

// Функции для ошибок
void db_print_error(const char *operation, const char *error_msg);

// Проверка целостности БД
int db_check_integrity(sqlite3 *db);

#endif
