#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "../include/composition.h"

int composition_create_table(sqlite3 *db) {
    char *err_msg = 0;
    const char *sql = "CREATE TABLE IF NOT EXISTS compositions ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "name TEXT NOT NULL UNIQUE);"
                      "CREATE TABLE IF NOT EXISTS composition_plants ("
                      "composition_id INTEGER,"
                      "plant_id INTEGER,"
                      "quantity INTEGER NOT NULL,"
                      "FOREIGN KEY (composition_id) REFERENCES compositions(id) ON DELETE CASCADE,"
                      "FOREIGN KEY (plant_id) REFERENCES plants(id) ON DELETE CASCADE,"
                      "PRIMARY KEY (composition_id, plant_id));";
    
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return rc;
    }
    return SQLITE_OK;
}

int composition_add(sqlite3 *db, const Composition *comp) {
    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO compositions (name) VALUES (?)";
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) return rc;
    
    sqlite3_bind_text(stmt, 1, comp->name, -1, SQLITE_STATIC);
    
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        return rc;
    }
    
    *((int*)&comp->id) = sqlite3_last_insert_rowid(db);
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int composition_get_all(sqlite3 *db, Composition **comps, int *count) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, name FROM compositions ORDER BY name";
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) return rc;
    
    *count = 0;
    Composition *list = NULL;
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        list = realloc(list, (*count + 1) * sizeof(Composition));
        if (!list) {
            sqlite3_finalize(stmt);
            return SQLITE_NOMEM;
        }
        
        list[*count].id = sqlite3_column_int(stmt, 0);
        strcpy(list[*count].name, (const char*)sqlite3_column_text(stmt, 1));
        (*count)++;
    }
    
    *comps = list;
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int composition_get_by_id(sqlite3 *db, int id, Composition *comp) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, name FROM compositions WHERE id = ?";
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) return rc;
    
    sqlite3_bind_int(stmt, 1, id);
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        comp->id = sqlite3_column_int(stmt, 0);
        strcpy(comp->name, (const char*)sqlite3_column_text(stmt, 1));
        sqlite3_finalize(stmt);
        return SQLITE_OK;
    }
    
    sqlite3_finalize(stmt);
    return SQLITE_DONE;
}

int composition_update(sqlite3 *db, const Composition *comp) {
    sqlite3_stmt *stmt;
    const char *sql = "UPDATE compositions SET name = ? WHERE id = ?";
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) return rc;
    
    sqlite3_bind_text(stmt, 1, comp->name, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, comp->id);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? SQLITE_OK : rc;
}

int composition_delete(sqlite3 *db, int id) {
    sqlite3_stmt *stmt;
    const char *sql = "DELETE FROM compositions WHERE id = ?";
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) return rc;
    
    sqlite3_bind_int(stmt, 1, id);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? SQLITE_OK : rc;
}

void composition_free_list(Composition *comps, int count) {
    (void)count;
    free(comps);
}

int composition_add_plant(sqlite3 *db, int comp_id, int plant_id, int quantity) {
    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO composition_plants (composition_id, plant_id, quantity) VALUES (?, ?, ?)";
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) return rc;
    
    sqlite3_bind_int(stmt, 1, comp_id);
    sqlite3_bind_int(stmt, 2, plant_id);
    sqlite3_bind_int(stmt, 3, quantity);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? SQLITE_OK : rc;
}

int composition_get_plants(sqlite3 *db, int comp_id, CompositionPlant **plants, int *count) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT cp.composition_id, cp.plant_id, cp.quantity, "
                      "p.name, p.variety, p.price_per_unit "
                      "FROM composition_plants cp "
                      "JOIN plants p ON cp.plant_id = p.id "
                      "WHERE cp.composition_id = ?";
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) return rc;
    
    sqlite3_bind_int(stmt, 1, comp_id);
    
    *count = 0;
    CompositionPlant *list = NULL;
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        list = realloc(list, (*count + 1) * sizeof(CompositionPlant));
        if (!list) {
            sqlite3_finalize(stmt);
            return SQLITE_NOMEM;
        }
        
        list[*count].composition_id = sqlite3_column_int(stmt, 0);
        list[*count].plant_id = sqlite3_column_int(stmt, 1);
        list[*count].quantity = sqlite3_column_int(stmt, 2);
        strcpy(list[*count].plant_name, (const char*)sqlite3_column_text(stmt, 3));
        strcpy(list[*count].plant_variety, (const char*)sqlite3_column_text(stmt, 4));
        list[*count].plant_price = sqlite3_column_double(stmt, 5);
        (*count)++;
    }
    
    *plants = list;
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int composition_remove_plant(sqlite3 *db, int comp_id, int plant_id) {
    sqlite3_stmt *stmt;
    const char *sql = "DELETE FROM composition_plants WHERE composition_id = ? AND plant_id = ?";
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) return rc;
    
    sqlite3_bind_int(stmt, 1, comp_id);
    sqlite3_bind_int(stmt, 2, plant_id);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? SQLITE_OK : rc;
}

int composition_get_total_price(sqlite3 *db, int comp_id, double *total_price) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT SUM(cp.quantity * p.price_per_unit) "
                      "FROM composition_plants cp "
                      "JOIN plants p ON cp.plant_id = p.id "
                      "WHERE cp.composition_id = ?";
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) return rc;
    
    sqlite3_bind_int(stmt, 1, comp_id);
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        *total_price = sqlite3_column_double(stmt, 0);
        sqlite3_finalize(stmt);
        return SQLITE_OK;
    }
    
    sqlite3_finalize(stmt);
    return SQLITE_DONE;
}
