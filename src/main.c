#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "../include/db_helper.h"
#include "../include/plant.h"
#include "../include/composition.h"
#include "../include/order.h"
#include "../include/auth.h"

// Глобальная переменная для текущего пользователя
User current_user;

// Проверка прав доступа
int has_permission(const User *user, const char *action) {
    if (strcmp(user->role, "admin") == 0) return 1;  // admin может всё
    
    if (strcmp(user->role, "manager") == 0) {
        // manager может всё, кроме управления пользователями
        if (strcmp(action, "manage_users") == 0) return 0;
        return 1;
    }
    
    if (strcmp(user->role, "customer") == 0) {
        // customer только просмотр
        if (strncmp(action, "view", 4) == 0) return 1;
        return 0;
    }
    
    return 0;
}

void print_main_menu(const User *user) {
    printf("\n========================================\n");
    printf("     ЦВЕТОЧНАЯ ОРАНЖЕРЕЯ\n");
    printf("     Пользователь: %s (роль: %s)\n", user->username, user->role);
    printf("========================================\n");
    
    if (has_permission(user, "view_plants")) {
        printf("1. Управление растениями\n");
    }
    if (has_permission(user, "view_compositions")) {
        printf("2. Управление композициями\n");
    }
    if (has_permission(user, "view_orders")) {
        printf("3. Управление заказами\n");
    }
    if (has_permission(user, "view_stats")) {
        printf("4. Статистика и отчёты\n");
    }
    printf("0. Выход\n");
    printf("========================================\n");
    printf("Выберите опцию: ");
}

void print_plants_menu(const User *user) {
    printf("\n--- Управление растениями ---\n");
    printf("1. Показать все растения\n");
    
    if (has_permission(user, "add_plant")) {
        printf("2. Добавить растение\n");
    }
    if (has_permission(user, "edit_plant")) {
        printf("3. Обновить растение\n");
    }
    if (has_permission(user, "delete_plant")) {
        printf("4. Удалить растение\n");
    }
    printf("0. Назад\n");
    printf("Выберите опцию: ");
}

void print_compositions_menu(const User *user) {
    printf("\n--- Управление композициями ---\n");
    printf("1. Показать все композиции\n");
    
    if (has_permission(user, "add_composition")) {
        printf("2. Добавить композицию\n");
    }
    if (has_permission(user, "edit_composition")) {
        printf("3. Обновить композицию\n");
    }
    if (has_permission(user, "delete_composition")) {
        printf("4. Удалить композицию\n");
    }
    if (has_permission(user, "add_to_composition")) {
        printf("5. Добавить растение в композицию\n");
    }
    printf("6. Показать состав композиции\n");
    if (has_permission(user, "remove_from_composition")) {
        printf("7. Удалить растение из композиции\n");
    }
    printf("8. Рассчитать стоимость композиции\n");
    printf("0. Назад\n");
    printf("Выберите опцию: ");
}

void print_orders_menu(const User *user) {
    printf("\n--- Управление заказами ---\n");
    printf("1. Показать все заказы\n");
    
    if (has_permission(user, "add_order")) {
        printf("2. Добавить заказ\n");
    }
    if (has_permission(user, "edit_order")) {
        printf("3. Обновить заказ\n");
    }
    if (has_permission(user, "delete_order")) {
        printf("4. Удалить заказ\n");
    }
    if (has_permission(user, "complete_order")) {
        printf("5. Завершить заказ\n");
    }
    printf("6. Показать заказы по статусу\n");
    printf("7. Показать заказы по клиенту\n");
    printf("8. Показать заказы за период\n");
    printf("0. Назад\n");
    printf("Выберите опцию: ");
}

void print_reports_menu(const User *user) {
    printf("\n--- Статистика и отчёты ---\n");
    printf("1. Общая выручка\n");
    printf("2. Самые популярные композиции\n");
    printf("3. Заказы по срочности\n");
    printf("0. Назад\n");
    printf("Выберите опцию: ");
}

void show_all_plants(sqlite3 *db) {
    Plant *plants = NULL;
    int count = 0;
    
    int rc = plant_get_all(db, &plants, &count);
    if (rc != SQLITE_OK) {
        printf("Ошибка при получении списка растений\n");
        return;
    }
    
    printf("\n=== Растения ===\n");
    printf("%-4s %-20s %-15s %-10s\n", "ID", "Название", "Сорт", "Цена (BYN)");
    printf("--------------------------------------------------------\n");
    
    for (int i = 0; i < count; i++) {
        printf("%-4d %-20s %-15s %-10.2f\n", 
               plants[i].id, 
               plants[i].name, 
               plants[i].variety, 
               plants[i].price_per_unit);
    }
    
    plant_free_list(plants, count);
}

void show_all_compositions(sqlite3 *db) {
    Composition *comps = NULL;
    int count = 0;
    
    int rc = composition_get_all(db, &comps, &count);
    if (rc != SQLITE_OK) {
        printf("Ошибка при получении списка композиций\n");
        return;
    }
    
    printf("\n=== Композиции ===\n");
    printf("%-4s %-30s\n", "ID", "Название");
    printf("----------------------------------------\n");
    
    for (int i = 0; i < count; i++) {
        printf("%-4d %-30s\n", comps[i].id, comps[i].name);
    }
    
    composition_free_list(comps, count);
}

void show_all_orders(sqlite3 *db) {
    Order *orders = NULL;
    int count = 0;
    
    int rc = order_get_all(db, &orders, &count);
    if (rc != SQLITE_OK) {
        printf("Ошибка при получении списка заказов\n");
        return;
    }
    
    printf("\n=== Заказы ===\n");
    printf("%-4s %-20s %-25s %-6s %-12s %-12s %-10s\n", 
           "ID", "Клиент", "Композиция", "Кол-во", "Дата заказа", "Дата вып.", "Статус");
    printf("--------------------------------------------------------------------------------\n");
    
    for (int i = 0; i < count; i++) {
        printf("%-4d %-20s %-25s %-6d %-12s %-12s %-10s\n", 
               orders[i].id,
               orders[i].customer_name,
               orders[i].composition_name,
               orders[i].quantity,
               orders[i].order_date,
               orders[i].completion_date[0] ? orders[i].completion_date : "-",
               orders[i].status);
    }
    
    order_free_list(orders, count);
}

int main(void) {
    sqlite3 *db;
    char username[50], password[50];
    int login_attempts = 0;
    
    printf("========================================\n");
    printf("   ЦВЕТОЧНАЯ ОРАНЖЕРЕЯ\n");
    printf("   Система управления\n");
    printf("========================================\n");
    
    // Инициализация БД
    if (db_init(&db) != SQLITE_OK) {
        printf("Ошибка подключения к базе данных\n");
        return 1;
    }
    
    // Создание таблиц
    plant_create_table(db);
    composition_create_table(db);
    order_create_table(db);
    auth_create_table(db);
    
    printf("База данных успешно инициализирована\n");
    
    // Аутентификация
    while (login_attempts < 3) {
        printf("\n--- Вход в систему ---\n");
        printf("Логин: ");
        scanf("%s", username);
        printf("Пароль: ");
        scanf("%s", password);
        
        if (auth_login(db, username, password, &current_user) == SQLITE_OK) {
            printf("\nДобро пожаловать, %s! Роль: %s\n", current_user.username, current_user.role);
            break;
        }
        
        login_attempts++;
        printf("Неверный логин или пароль. Осталось попыток: %d\n", 3 - login_attempts);
    }
    
    if (login_attempts >= 3) {
        printf("Превышено количество попыток входа. Доступ закрыт.\n");
        db_close(db);
        return 1;
    }
    
    int choice;
    do {
        print_main_menu(&current_user);
        scanf("%d", &choice);
        
        switch (choice) {
            case 1: { // Растения
                if (!has_permission(&current_user, "view_plants")) {
                    printf("У вас нет прав на управление растениями\n");
                    break;
                }
                
                int subchoice;
                do {
                    print_plants_menu(&current_user);
                    scanf("%d", &subchoice);
                    
                    switch (subchoice) {
                        case 1:
                            show_all_plants(db);
                            break;
                        case 2:
                            if (has_permission(&current_user, "add_plant")) {
                                Plant p;
                                printf("Название: "); scanf("%s", p.name);
                                printf("Сорт: "); scanf("%s", p.variety);
                                printf("Цена за штуку (BYN): "); scanf("%lf", &p.price_per_unit);
                                if (plant_add(db, &p) == SQLITE_OK) {
                                    printf("Растение добавлено (ID: %d)\n", p.id);
                                } else {
                                    printf("Ошибка добавления\n");
                                }
                            } else {
                                printf("У вас нет прав на добавление растений\n");
                            }
                            break;
                        case 3:
                            if (has_permission(&current_user, "edit_plant")) {
                                Plant p;
                                printf("ID растения: "); scanf("%d", &p.id);
                                if (plant_get_by_id(db, p.id, &p) != SQLITE_OK) {
                                    printf("Растение не найдено\n");
                                    break;
                                }
                                printf("Новое название (%s): ", p.name); scanf("%s", p.name);
                                printf("Новый сорт (%s): ", p.variety); scanf("%s", p.variety);
                                printf("Новая цена (%.2f): ", p.price_per_unit); scanf("%lf", &p.price_per_unit);
                                if (plant_update(db, &p) == SQLITE_OK) {
                                    printf("Растение обновлено\n");
                                } else {
                                    printf("Ошибка обновления\n");
                                }
                            } else {
                                printf("У вас нет прав на редактирование растений\n");
                            }
                            break;
                        case 4:
                            if (has_permission(&current_user, "delete_plant")) {
                                int id;
                                printf("ID растения для удаления: "); scanf("%d", &id);
                                if (plant_delete(db, id) == SQLITE_OK) {
                                    printf("Растение удалено\n");
                                } else {
                                    printf("Ошибка удаления\n");
                                }
                            } else {
                                printf("У вас нет прав на удаление растений\n");
                            }
                            break;
                    }
                } while (subchoice != 0);
                break;
            }
            
            case 2: { // Композиции
                if (!has_permission(&current_user, "view_compositions")) {
                    printf("У вас нет прав на управление композициями\n");
                    break;
                }
                
                int subchoice;
                do {
                    print_compositions_menu(&current_user);
                    scanf("%d", &subchoice);
                    
                    switch (subchoice) {
                        case 1:
                            show_all_compositions(db);
                            break;
                        case 2:
                            if (has_permission(&current_user, "add_composition")) {
                                Composition c;
                                printf("Название композиции: "); scanf("%s", c.name);
                                if (composition_add(db, &c) == SQLITE_OK) {
                                    printf("Композиция добавлена (ID: %d)\n", c.id);
                                } else {
                                    printf("Ошибка добавления\n");
                                }
                            } else {
                                printf("У вас нет прав на добавление композиций\n");
                            }
                            break;
                        case 3:
                            if (has_permission(&current_user, "edit_composition")) {
                                Composition c;
                                printf("ID композиции: "); scanf("%d", &c.id);
                                if (composition_get_by_id(db, c.id, &c) != SQLITE_OK) {
                                    printf("Композиция не найдена\n");
                                    break;
                                }
                                printf("Новое название (%s): ", c.name); scanf("%s", c.name);
                                if (composition_update(db, &c) == SQLITE_OK) {
                                    printf("Композиция обновлена\n");
                                } else {
                                    printf("Ошибка обновления\n");
                                }
                            } else {
                                printf("У вас нет прав на редактирование композиций\n");
                            }
                            break;
                        case 4:
                            if (has_permission(&current_user, "delete_composition")) {
                                int id;
                                printf("ID композиции для удаления: "); scanf("%d", &id);
                                if (composition_delete(db, id) == SQLITE_OK) {
                                    printf("Композиция удалена\n");
                                } else {
                                    printf("Ошибка удаления\n");
                                }
                            } else {
                                printf("У вас нет прав на удаление композиций\n");
                            }
                            break;
                        case 5:
                            if (has_permission(&current_user, "add_to_composition")) {
                                int comp_id, plant_id, quantity;
                                printf("ID композиции: "); scanf("%d", &comp_id);
                                printf("ID растения: "); scanf("%d", &plant_id);
                                printf("Количество: "); scanf("%d", &quantity);
                                if (composition_add_plant(db, comp_id, plant_id, quantity) == SQLITE_OK) {
                                    printf("Растение добавлено в композицию\n");
                                } else {
                                    printf("Ошибка добавления\n");
                                }
                            } else {
                                printf("У вас нет прав на добавление растений в композиции\n");
                            }
                            break;
                        case 6: {
                            int comp_id;
                            printf("ID композиции: "); scanf("%d", &comp_id);
                            CompositionPlant *plants = NULL;
                            int count = 0;
                            if (composition_get_plants(db, comp_id, &plants, &count) == SQLITE_OK) {
                                printf("\n=== Состав композиции ===\n");
                                printf("%-4s %-20s %-15s %-8s %-8s\n", 
                                       "ID", "Название", "Сорт", "Кол-во", "Цена");
                                printf("------------------------------------------------\n");
                                for (int i = 0; i < count; i++) {
                                    printf("%-4d %-20s %-15s %-8d %-8.2f\n",
                                           plants[i].plant_id,
                                           plants[i].plant_name,
                                           plants[i].plant_variety,
                                           plants[i].quantity,
                                           plants[i].plant_price);
                                }
                                free(plants);
                            }
                            break;
                        }
                        case 7:
                            if (has_permission(&current_user, "remove_from_composition")) {
                                int comp_id, plant_id;
                                printf("ID композиции: "); scanf("%d", &comp_id);
                                printf("ID растения: "); scanf("%d", &plant_id);
                                if (composition_remove_plant(db, comp_id, plant_id) == SQLITE_OK) {
                                    printf("Растение удалено из композиции\n");
                                } else {
                                    printf("Ошибка удаления\n");
                                }
                            } else {
                                printf("У вас нет прав на удаление растений из композиций\n");
                            }
                            break;
                        case 8: {
                            int comp_id;
                            double total;
                            printf("ID композиции: "); scanf("%d", &comp_id);
                            if (composition_get_total_price(db, comp_id, &total) == SQLITE_OK) {
                                printf("Стоимость композиции: %.2f BYN\n", total);
                            } else {
                                printf("Ошибка расчёта\n");
                            }
                            break;
                        }
                    }
                } while (subchoice != 0);
                break;
            }
            
            case 3: { // Заказы
                if (!has_permission(&current_user, "view_orders")) {
                    printf("У вас нет прав на управление заказами\n");
                    break;
                }
                
                int subchoice;
                do {
                    print_orders_menu(&current_user);
                    scanf("%d", &subchoice);
                    
                    switch (subchoice) {
                        case 1:
                            show_all_orders(db);
                            break;
                        case 2:
                            if (has_permission(&current_user, "add_order")) {
                                Order o;
                                printf("ФИО клиента: "); scanf(" %[^\n]", o.customer_name);
                                printf("Название композиции: "); scanf("%s", o.composition_name);
                                printf("Количество: "); scanf("%d", &o.quantity);
                                printf("Дата заказа (YYYY-MM-DD): "); scanf("%s", o.order_date);
                                printf("Статус (pending/urgent): "); scanf("%s", o.status);
                                o.completion_date[0] = '\0';
                                if (order_add(db, &o) == SQLITE_OK) {
                                    printf("Заказ добавлен (ID: %d)\n", o.id);
                                } else {
                                    printf("Ошибка добавления\n");
                                }
                            } else {
                                printf("У вас нет прав на добавление заказов\n");
                            }
                            break;
                        case 3:
                            if (has_permission(&current_user, "edit_order")) {
                                Order o;
                                printf("ID заказа: "); scanf("%d", &o.id);
                                if (order_get_by_id(db, o.id, &o) != SQLITE_OK) {
                                    printf("Заказ не найден\n");
                                    break;
                                }
                                printf("Новый статус (%s): ", o.status); scanf("%s", o.status);
                                if (order_update(db, &o) == SQLITE_OK) {
                                    printf("Заказ обновлён\n");
                                } else {
                                    printf("Ошибка обновления\n");
                                }
                            } else {
                                printf("У вас нет прав на редактирование заказов\n");
                            }
                            break;
                        case 4:
                            if (has_permission(&current_user, "delete_order")) {
                                int id;
                                printf("ID заказа для удаления: "); scanf("%d", &id);
                                if (order_delete(db, id) == SQLITE_OK) {
                                    printf("Заказ удалён\n");
                                } else {
                                    printf("Ошибка удаления\n");
                                }
                            } else {
                                printf("У вас нет прав на удаление заказов\n");
                            }
                            break;
                        case 5:
                            if (has_permission(&current_user, "complete_order")) {
                                int id;
                                char completion_date[11];
                                printf("ID заказа: "); scanf("%d", &id);
                                printf("Дата выполнения (YYYY-MM-DD): "); scanf("%s", completion_date);
                                if (order_complete(db, id, completion_date) == SQLITE_OK) {
                                    printf("Заказ завершён\n");
                                } else {
                                    printf("Ошибка завершения\n");
                                }
                            } else {
                                printf("У вас нет прав на завершение заказов\n");
                            }
                            break;
                        case 6: {
                            char status[20];
                            printf("Статус (pending/completed/urgent): "); scanf("%s", status);
                            Order *orders = NULL;
                            int count = 0;
                            if (order_get_by_status(db, status, &orders, &count) == SQLITE_OK) {
                                printf("\n=== Заказы со статусом '%s' ===\n", status);
                                for (int i = 0; i < count; i++) {
                                    printf("ID: %d, Клиент: %s, Композиция: %s, Кол-во: %d\n",
                                           orders[i].id, orders[i].customer_name,
                                           orders[i].composition_name, orders[i].quantity);
                                }
                                free(orders);
                            }
                            break;
                        }
                        case 7: {
                            char customer[100];
                            printf("ФИО клиента: "); scanf(" %[^\n]", customer);
                            Order *orders = NULL;
                            int count = 0;
                            if (order_get_by_customer(db, customer, &orders, &count) == SQLITE_OK) {
                                printf("\n=== Заказы клиента '%s' ===\n", customer);
                                for (int i = 0; i < count; i++) {
                                    printf("ID: %d, Композиция: %s, Кол-во: %d, Дата: %s, Статус: %s\n",
                                           orders[i].id, orders[i].composition_name,
                                           orders[i].quantity, orders[i].order_date, orders[i].status);
                                }
                                free(orders);
                            }
                            break;
                        }
                        case 8: {
                            char start[11], end[11];
                            printf("Начальная дата (YYYY-MM-DD): "); scanf("%s", start);
                            printf("Конечная дата (YYYY-MM-DD): "); scanf("%s", end);
                            Order *orders = NULL;
                            int count = 0;
                            if (order_get_by_date_range(db, start, end, &orders, &count) == SQLITE_OK) {
                                printf("\n=== Заказы за период %s - %s ===\n", start, end);
                                for (int i = 0; i < count; i++) {
                                    printf("ID: %d, Клиент: %s, Композиция: %s, Дата: %s\n",
                                           orders[i].id, orders[i].customer_name,
                                           orders[i].composition_name, orders[i].order_date);
                                }
                                free(orders);
                            }
                            break;
                        }
                    }
                } while (subchoice != 0);
                break;
            }
            
            case 4: { // Статистика и отчёты
                if (!has_permission(&current_user, "view_stats")) {
                    printf("У вас нет прав на просмотр статистики\n");
                    break;
                }
                
                int subchoice;
                do {
                    print_reports_menu(&current_user);
                    scanf("%d", &subchoice);
                    
                    switch (subchoice) {
                        case 1: {
                            sqlite3_stmt *stmt;
                            double total = 0;
                            const char *sql = "SELECT SUM(quantity * price) FROM ("
                                              "SELECT o.quantity, c.name, "
                                              "SUM(cp.quantity * p.price_per_unit) as price "
                                              "FROM orders o "
                                              "JOIN compositions c ON o.composition_name = c.name "
                                              "JOIN composition_plants cp ON c.id = cp.composition_id "
                                              "JOIN plants p ON cp.plant_id = p.id "
                                              "WHERE o.status = 'completed' "
                                              "GROUP BY o.id)";
                            if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
                                if (sqlite3_step(stmt) == SQLITE_ROW) {
                                    total = sqlite3_column_double(stmt, 0);
                                }
                                sqlite3_finalize(stmt);
                            }
                            printf("Общая выручка от выполненных заказов: %.2f BYN\n", total);
                            break;
                        }
                        case 2: {
                            sqlite3_stmt *stmt;
                            const char *sql = "SELECT composition_name, COUNT(*) as orders_count "
                                              "FROM orders GROUP BY composition_name "
                                              "ORDER BY orders_count DESC LIMIT 5";
                            printf("\n=== Самые популярные композиции ===\n");
                            if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
                                while (sqlite3_step(stmt) == SQLITE_ROW) {
                                    const char *name = (const char*)sqlite3_column_text(stmt, 0);
                                    int count = sqlite3_column_int(stmt, 1);
                                    printf("%s - %d заказов\n", name, count);
                                }
                                sqlite3_finalize(stmt);
                            }
                            break;
                        }
                        case 3: {
                            sqlite3_stmt *stmt;
                            const char *sql = "SELECT status, COUNT(*) FROM orders GROUP BY status";
                            printf("\n=== Заказы по срочности ===\n");
                            if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
                                while (sqlite3_step(stmt) == SQLITE_ROW) {
                                    const char *status = (const char*)sqlite3_column_text(stmt, 0);
                                    int count = sqlite3_column_int(stmt, 1);
                                    printf("%s: %d заказов\n", status, count);
                                }
                                sqlite3_finalize(stmt);
                            }
                            break;
                        }
                    }
                } while (subchoice != 0);
                break;
            }
            
            case 0:
                printf("До свидания!\n");
                break;
                
            default:
                printf("Неверный выбор. Попробуйте снова.\n");
        }
    } while (choice != 0);
    
    db_close(db);
    return 0;
}
