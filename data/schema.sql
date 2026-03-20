-- База данных "Цветочная оранжерея"
-- Вариант 8
-- Файл: greenhouse.db

-- Таблица растений
CREATE TABLE IF NOT EXISTS plants (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    variety TEXT NOT NULL,
    price_per_unit REAL NOT NULL
);

-- Таблица композиций
CREATE TABLE IF NOT EXISTS compositions (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL UNIQUE
);

-- Таблица связи композиций с растениями
CREATE TABLE IF NOT EXISTS composition_plants (
    composition_id INTEGER,
    plant_id INTEGER,
    quantity INTEGER NOT NULL,
    FOREIGN KEY (composition_id) REFERENCES compositions(id) ON DELETE CASCADE,
    FOREIGN KEY (plant_id) REFERENCES plants(id) ON DELETE CASCADE,
    PRIMARY KEY (composition_id, plant_id)
);

-- Таблица заказов
CREATE TABLE IF NOT EXISTS orders (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    customer_name TEXT NOT NULL,
    composition_name TEXT NOT NULL,
    quantity INTEGER NOT NULL,
    order_date TEXT NOT NULL,
    completion_date TEXT,
    status TEXT DEFAULT 'pending'
);

-- Таблица пользователей (для аутентификации)
CREATE TABLE IF NOT EXISTS users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    username TEXT NOT NULL UNIQUE,
    password_hash TEXT NOT NULL,
    role TEXT NOT NULL
);

-- Добавление тестовых данных (с проверкой на существование)

-- Растения
INSERT OR IGNORE INTO plants (name, variety, price_per_unit) VALUES 
('Роза', 'Чайная', 7.0),
('Роза', 'Плетистая', 8.5),
('Тюльпан', 'Красный', 3.5),
('Тюльпан', 'Жёлтый', 3.5),
('Лилия', 'Белая', 5.0),
('Хризантема', 'Кустовая', 4.0);

-- Композиции
INSERT OR IGNORE INTO compositions (name) VALUES 
('Букет невесты'),
('Классический букет'),
('Весеннее настроение');

-- Состав композиций
-- Букет невесты (id=1)
INSERT OR IGNORE INTO composition_plants (composition_id, plant_id, quantity) 
SELECT 1, id, 5 FROM plants WHERE name = 'Роза' AND variety = 'Чайная';
INSERT OR IGNORE INTO composition_plants (composition_id, plant_id, quantity) 
SELECT 1, id, 2 FROM plants WHERE name = 'Лилия' AND variety = 'Белая';

-- Классический букет (id=2)
INSERT OR IGNORE INTO composition_plants (composition_id, plant_id, quantity) 
SELECT 2, id, 3 FROM plants WHERE name = 'Роза' AND variety = 'Чайная';
INSERT OR IGNORE INTO composition_plants (composition_id, plant_id, quantity) 
SELECT 2, id, 2 FROM plants WHERE name = 'Роза' AND variety = 'Плетистая';

-- Весеннее настроение (id=3)
INSERT OR IGNORE INTO composition_plants (composition_id, plant_id, quantity) 
SELECT 3, id, 5 FROM plants WHERE name = 'Тюльпан' AND variety = 'Красный';
INSERT OR IGNORE INTO composition_plants (composition_id, plant_id, quantity) 
SELECT 3, id, 5 FROM plants WHERE name = 'Тюльпан' AND variety = 'Жёлтый';

-- Пользователи
INSERT OR IGNORE INTO users (username, password_hash, role) VALUES 
('admin', '8c6976e5b5410415bde908bd4dee15dfb167a9c873fc4bb8a81f6f2ab448a918', 'admin'),
('manager', '8d969eef6ecad3c29a3a629280e686cf0c3f5d5a86aff3ca12020c923adc6c92', 'manager'),
('customer', '8d969eef6ecad3c29a3a629280e686cf0c3f5d5a86aff3ca12020c923adc6c92', 'customer');

-- Заказы
INSERT OR IGNORE INTO orders (customer_name, composition_name, quantity, order_date, completion_date, status) VALUES 
('Иванова А.А.', 'Букет невесты', 1, '2025-03-15', '2025-03-16', 'completed'),
('Петров Б.Б.', 'Классический букет', 2, '2025-03-17', NULL, 'pending'),
('Сидорова В.В.', 'Весеннее настроение', 1, '2025-03-18', NULL, 'urgent');