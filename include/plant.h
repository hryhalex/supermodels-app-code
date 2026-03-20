#ifndef PLANT_H
#define PLANT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sqlite3.h>

typedef struct {
    int id;
    char name[50];
    char variety[50];
    double price_per_unit;
} Plant;

// Прототипы функций
int plant_create_table(sqlite3 *db);
int plant_add(sqlite3 *db, const Plant *plant);
int plant_get_all(sqlite3 *db, Plant **plants, int *count);
int plant_get_by_id(sqlite3 *db, int id, Plant *plant);
int plant_update(sqlite3 *db, const Plant *plant);
int plant_delete(sqlite3 *db, int id);
void plant_free_list(Plant *plants, int count);

#ifdef __cplusplus
}
#endif

#endif
