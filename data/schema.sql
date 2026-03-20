-- База данных "Цветочная оранжерея"
-- Вариант 8

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

-- Добавление тестовых данных

-- Растения
INSERT INTO plants (name, variety, price_per_unit) VALUES 
('Роза', 'Чайная', 7.0),
('Роза', 'Плетистая', 8.5),
('Тюльпан', 'Красный', 3.5),
('Тюльпан', 'Жёлтый', 3.5),
('Лилия', 'Белая', 5.0),
('Хризантема', 'Кустовая', 4.0);

-- Композиции
INSERT INTO compositions (name) VALUES 
('Букет невесты'),
('Классический букет'),
('Весеннее настроение');

-- Состав композиций
-- Букет невесты
INSERT INTO composition_plants (composition_id, plant_id, quantity) VALUES 
(1, 1, 5),  -- 5 чайных роз
(1, 5, 2);  -- 2 белые лилии

-- Классический букет
INSERT INTO composition_plants (composition_id, plant_id, quantity) VALUES 
(2, 1, 3),  -- 3 чайные розы
(2, 2, 2);  -- 2 плетистые розы

-- Весеннее настроение
INSERT INTO composition_plants (composition_id, plant_id, quantity) VALUES 
(3, 3, 5),  -- 5 красных тюльпанов
(3, 4, 5);  -- 5 жёлтых тюльпанов

-- Пользователи
INSERT INTO users (username, password_hash, role) VALUES 
('admin', '8c6976e5b5410415bde908bd4dee15dfb167a9c873fc4bb8a81f6f2ab448a918', 'admin'),  -- admin
('manager', '8d969eef6ecad3c29a3a629280e686cf0c3f5d5a86aff3ca12020c923adc6c92', 'manager'); -- 123456

-- Заказы
INSERT INTO orders (customer_name, composition_name, quantity, order_date, completion_date, status) VALUES 
('Иванова А.А.', 'Букет невесты', 1, '2025-03-15', '2025-03-16', 'completed'),
('Петров Б.Б.', 'Классический букет', 2, '2025-03-17', NULL, 'pending'),
('Сидорова В.В.', 'Весеннее настроение', 1, '2025-03-18', NULL, 'urgent');
