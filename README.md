# AutoRia Clone - Платформа для продажу автомобілів

## Опис
Веб-платформа для продажу автомобілів з системою ролей, модерацією, валютною конвертацією та статистикою для преміум акаунтів.

## Архітектура

### Ролі та пермішні
- **Покупець (Buyer)** - перегляд оголошень, зв'язок з продавцями
- **Продавець (Seller)** - створення та управління власними оголошеннями
- **Менеджер (Manager)** - модерація, бан користувачів, видалення оголошень
- **Адміністратор (Administrator)** - повний доступ, створення менеджерів

### Типи акаунтів
- **Базовий** - 1 оголошення, без статистики
- **Преміум** - необмежена кількість оголошень, статистика переглядів та середніх цін

## Запуск через Docker

```bash
docker-compose up --build
```

- API: http://localhost:8080
- Frontend: http://localhost:5000

## Запуск без Docker

### Бекенд
```bash
cd backend
mkdir build && cd build
cmake ..
make
./AutoRiaBackend
```

### Фронтенд
```bash
cd frontend
python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt
python app.py
```

## API Ендпойнти

### Оголошення
- `GET /api/listings` - список активних оголошень
- `GET /api/listings/{id}` - деталі оголошення
- `POST /api/listings` - створити оголошення (потрібна авторизація)
- `PUT /api/listings/{id}` - оновити оголошення
- `DELETE /api/listings/{id}` - видалити оголошення
- `GET /api/listings/{id}/stats` - статистика (тільки преміум)

### Марки та моделі
- `GET /api/brands` - список марок
- `GET /api/brands/{id}/models` - моделі марки
- `POST /api/brands/request` - запитати додавання марки
- `POST /api/models/request` - запитати додавання моделі

### Користувачі
- `POST /api/users` - реєстрація
- `POST /api/users/managers` - створити менеджера (тільки адмін)

## Авторизація

Використовуйте заголовок:
```
Authorization: Bearer {user_id}:{email}
```

Приклад: `Authorization: Bearer 1:test@example.com`

## Приклади запитів

### Створити оголошення
```bash
curl -X POST http://localhost:8080/api/listings \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer 1:seller@example.com" \
  -d '{
    "brand_id": 1,
    "model_id": 1,
    "year": 2020,
    "price": 25000,
    "currency": "USD",
    "description": "Відмінний стан",
    "region": "Київ",
    "mileage": 50000
  }'
```

### Отримати статистику (преміум)
```bash
curl http://localhost:8080/api/listings/1/stats \
  -H "Authorization: Bearer 1:premium@example.com"
```

## Особливості

1. **Модерація** - автоматична перевірка на нецензурну лексику
2. **Валюти** - підтримка USD, EUR, UAH з автоматичною конвертацією
3. **Ліміт редагувань** - максимум 3 редагування на оголошення
4. **Статистика** - перегляди, середні ціни по регіону та Україні

## Структура проєкту

```
autoria/
├── backend/          # C++ бекенд
│   ├── src/
│   │   ├── models/   # User, Listing, Brand, Model, Role
│   │   ├── repositories/ # Репозиторії для БД
│   │   ├── services/  # Moderation, Currency, Statistics
│   │   ├── middleware/ # AuthMiddleware
│   │   └── api/       # ApiServer
│   └── CMakeLists.txt
├── frontend/         # Flask фронтенд
└── docker-compose.yml
```

## ООП концепції

- **Інкапсуляція**: приватні поля, публічні методи
- **Наслідування**: User → Seller/Buyer/Manager/Administrator
- **Поліморфізм**: віртуальні методи getRoles()
- **Абстракція**: інтерфейси IUserRepository, IListingRepository
- **Singleton**: CurrencyService
- **Factory**: RoleFactory для створення ролей



