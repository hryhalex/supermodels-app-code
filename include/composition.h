#ifndef COMPOSITION_H
#define COMPOSITION_H

#include <sqlite3.h>
#include "plant.h"

typedef struct {
    int id;
    char name[100];
} Composition;

typedef struct {
    int composition_id;
    int plant_id;
    int quantity;
    char plant_name[50];
    char plant_variety[50];
    double plant_price;
} CompositionPlant;

// Прототипы функций
int composition_create_table(sqlite3 *db);
int composition_add(sqlite3 *db, const Composition *comp);
int composition_get_all(sqlite3 *db, Composition **comps, int *count);
int composition_get_by_id(sqlite3 *db, int id, Composition *comp);
int composition_update(sqlite3 *db, const Composition *comp);
int composition_delete(sqlite3 *db, int id);
void composition_free_list(Composition *comps, int count);

// Связь композиций с растениями
int composition_add_plant(sqlite3 *db, int comp_id, int plant_id, int quantity);
int composition_get_plants(sqlite3 *db, int comp_id, CompositionPlant **plants, int *count);
int composition_remove_plant(sqlite3 *db, int comp_id, int plant_id);
int composition_get_total_price(sqlite3 *db, int comp_id, double *total_price);

#endif
