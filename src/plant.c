#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "../include/plant.h"

int plant_create_table(sqlite3 *db) {
    char *err_msg = 0;
    const char *sql = "CREATE TABLE IF NOT EXISTS plants ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "name TEXT NOT NULL,"
                      "variety TEXT NOT NULL,"
                      "price_per_unit REAL NOT NULL,"
                      "image_path TEXT);";
    
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return rc;
    }
    return SQLITE_OK;
}

int plant_add(sqlite3 *db, const Plant *plant) {
    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO plants (name, variety, price_per_unit) VALUES (?, ?, ?)";
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) return rc;
    
    sqlite3_bind_text(stmt, 1, plant->name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, plant->variety, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 3, plant->price_per_unit);
    
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        return rc;
    }
    
    *((int*)&plant->id) = sqlite3_last_insert_rowid(db);
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int plant_get_all(sqlite3 *db, Plant **plants, int *count) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, name, variety, price_per_unit, image_path FROM plants";
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) return rc;
    
    *count = 0;
    Plant *list = NULL;
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        list = realloc(list, (*count + 1) * sizeof(Plant));
        if (!list) {
            sqlite3_finalize(stmt);
            return SQLITE_NOMEM;
        }
        
        list[*count].id = sqlite3_column_int(stmt, 0);
        strcpy(list[*count].name, (const char*)sqlite3_column_text(stmt, 1));
        strcpy(list[*count].variety, (const char*)sqlite3_column_text(stmt, 2));
        list[*count].price_per_unit = sqlite3_column_double(stmt, 3);
        
        const char *img = (const char*)sqlite3_column_text(stmt, 4);
        if (img) {
            strcpy(list[*count].image_path, img);
        } else {
            list[*count].image_path[0] = '\0';
        }
        
        (*count)++;
    }
    
    *plants = list;
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int plant_get_by_id(sqlite3 *db, int id, Plant *plant) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, name, variety, price_per_unit FROM plants WHERE id = ?";
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) return rc;
    
    sqlite3_bind_int(stmt, 1, id);
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        plant->id = sqlite3_column_int(stmt, 0);
        strcpy(plant->name, (const char*)sqlite3_column_text(stmt, 1));
        strcpy(plant->variety, (const char*)sqlite3_column_text(stmt, 2));
        plant->price_per_unit = sqlite3_column_double(stmt, 3);
        sqlite3_finalize(stmt);
        return SQLITE_OK;
    }
    
    sqlite3_finalize(stmt);
    return SQLITE_DONE; // not found
}

int plant_update(sqlite3 *db, const Plant *plant) {
    sqlite3_stmt *stmt;
    const char *sql = "UPDATE plants SET name = ?, variety = ?, price_per_unit = ? WHERE id = ?";
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) return rc;
    
    sqlite3_bind_text(stmt, 1, plant->name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, plant->variety, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 3, plant->price_per_unit);
    sqlite3_bind_int(stmt, 4, plant->id);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? SQLITE_OK : rc;
}

int plant_delete(sqlite3 *db, int id) {
    sqlite3_stmt *stmt;
    const char *sql = "DELETE FROM plants WHERE id = ?";
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) return rc;
    
    sqlite3_bind_int(stmt, 1, id);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? SQLITE_OK : rc;
}

void plant_free_list(Plant *plants, int count) {
    (void)count;
    free(plants);
}
