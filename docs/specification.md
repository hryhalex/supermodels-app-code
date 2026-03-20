# Спецификация проекта
## Командный проект «Цветочная оранжерея» (вариант 8)

### Диаграммы UML

#### Диаграмма вариантов использования
![Use Case Diagram](https://github.com/user-attachments/assets/254040b7-a423-4a78-b3c9-b3edc85fb190))

#### Диаграмма классов
![Class Diagram](https://github.com/user-attachments/assets/9b39c884-1199-494e-b2ec-97e5583e8dad))

#### Диаграмма компонентов
![Component Diagram](https://github.com/user-attachments/assets/57972301-c29b-4668-9bf4-05fd3fe170cc))

#### Диаграмма последовательности
![Sequence Diagram](https://github.com/user-attachments/assets/34fefc66-c223-4cd0-858c-1962d85c19f6))

#### Диаграмма развертывания
![Deployment Diagram](https://github.com/user-attachments/assets/a0048a9a-305a-47ae-8e26-7beb12cd114f))

---

### Модель КИС

Проект организован по модели КИС (Клиент-Интерфейс-Сервер):

| Компонент | Роль |
|-----------|------|
| `include/*.h` | Интерфейсы (объявления структур и функций) |
| `src/*.c` | Серверы (реализация функций) |
| `src/main.c` | Клиент (точка входа, меню) |

---

### Структура проекта

```text
supermodels-app-code/
├── .github/workflows/   # GitHub Actions
├── bin/                 # Исполняемые файлы
├── data/                # База данных SQLite
├── docs/                # Документация (GitHub Pages)
├── images/              # Фотографии растений
├── include/             # Заголовочные файлы
├── src/                 # Исходный код
├── tests_gtest/         # Unit-тесты
├── Makefile             # Сборка
└── README.md            # Описание проекта
```

---

### Ссылки
- [Репозиторий с кодом](https://github.com/hryhalex/supermodels-app-code)
- [Wiki первого репозитория](https://github.com/fpmi-tpmp2026/tpmp-gr12a-lab4-supermodels/wiki)
