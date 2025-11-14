// FILE: backend/src/database/Database.cpp
#include "Database.h"
#include <iostream>

Database::Database(const std::string& dbPath) : db_(nullptr), dbPath_(dbPath) {
    int rc = sqlite3_open(dbPath.c_str(), &db_);
    if (rc != SQLITE_OK) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db_) << std::endl;
        db_ = nullptr;
    } else {
        // Встановлюємо UTF-8 кодування для SQLite
        sqlite3_exec(db_, "PRAGMA encoding = 'UTF-8';", nullptr, nullptr, nullptr);
    }
}

Database::~Database() {
    if (db_) {
        sqlite3_close(db_);
    }
}

std::unique_ptr<Database> Database::create(const std::string& dbPath) {
    auto db = std::unique_ptr<Database>(new Database(dbPath));
    if (db->db_) {
        return db;
    }
    return nullptr;
}

bool Database::execute(const std::string& sql) {
    if (!db_) return false;
    
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &errMsg);
    
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool Database::initializeSchema() {
    std::string sql = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            email TEXT UNIQUE NOT NULL,
            password_hash TEXT NOT NULL,
            first_name TEXT NOT NULL,
            last_name TEXT NOT NULL,
            phone TEXT,
            account_type TEXT DEFAULT 'basic',
            is_active INTEGER DEFAULT 1,
            role TEXT NOT NULL,
            created_by_admin_id INTEGER,
            created_at INTEGER,
            updated_at INTEGER
        );
        
        CREATE TABLE IF NOT EXISTS brands (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT UNIQUE NOT NULL,
            is_active INTEGER DEFAULT 1
        );
        
        CREATE TABLE IF NOT EXISTS models (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            brand_id INTEGER NOT NULL,
            name TEXT NOT NULL,
            is_active INTEGER DEFAULT 1,
            FOREIGN KEY (brand_id) REFERENCES brands(id)
        );
        
        CREATE TABLE IF NOT EXISTS listings (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            seller_id INTEGER NOT NULL,
            brand_id INTEGER NOT NULL,
            model_id INTEGER NOT NULL,
            year INTEGER NOT NULL,
            price REAL NOT NULL,
            currency TEXT NOT NULL,
            exchange_rate REAL NOT NULL,
            description TEXT,
            region TEXT,
            mileage INTEGER,
            status TEXT DEFAULT 'draft',
            edit_count INTEGER DEFAULT 0,
            view_count INTEGER DEFAULT 0,
            created_at INTEGER,
            updated_at INTEGER,
            last_moderation_date INTEGER,
            photos TEXT,
            fuel_type TEXT,
            transmission TEXT,
            color TEXT,
            engine_volume REAL,
            body_type TEXT,
            doors_count INTEGER,
            engine_power INTEGER,
            FOREIGN KEY (seller_id) REFERENCES users(id),
            FOREIGN KEY (brand_id) REFERENCES brands(id),
            FOREIGN KEY (model_id) REFERENCES models(id)
        );
        
        CREATE TABLE IF NOT EXISTS favorites (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER NOT NULL,
            listing_id INTEGER NOT NULL,
            created_at INTEGER,
            FOREIGN KEY (user_id) REFERENCES users(id),
            FOREIGN KEY (listing_id) REFERENCES listings(id),
            UNIQUE(user_id, listing_id)
        );
        
        CREATE TABLE IF NOT EXISTS comments (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            listing_id INTEGER NOT NULL,
            user_id INTEGER NOT NULL,
            comment_text TEXT NOT NULL,
            created_at INTEGER,
            is_approved INTEGER DEFAULT 0,
            FOREIGN KEY (listing_id) REFERENCES listings(id),
            FOREIGN KEY (user_id) REFERENCES users(id)
        );
        
        CREATE TABLE IF NOT EXISTS notifications (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER NOT NULL,
            type TEXT NOT NULL,
            message TEXT NOT NULL,
            is_read INTEGER DEFAULT 0,
            created_at INTEGER,
            FOREIGN KEY (user_id) REFERENCES users(id)
        );
        
        CREATE TABLE IF NOT EXISTS listing_comparisons (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER NOT NULL,
            listing_ids TEXT NOT NULL,
            created_at INTEGER,
            FOREIGN KEY (user_id) REFERENCES users(id)
        );
        
        CREATE TABLE IF NOT EXISTS price_history (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            listing_id INTEGER NOT NULL,
            price REAL NOT NULL,
            currency TEXT NOT NULL,
            changed_at INTEGER,
            FOREIGN KEY (listing_id) REFERENCES listings(id)
        );
        
        CREATE TABLE IF NOT EXISTS purchase_requests (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            listing_id INTEGER NOT NULL,
            buyer_id INTEGER NOT NULL,
            seller_id INTEGER NOT NULL,
            status TEXT DEFAULT 'pending',
            message TEXT,
            created_at INTEGER,
            FOREIGN KEY (listing_id) REFERENCES listings(id),
            FOREIGN KEY (buyer_id) REFERENCES users(id),
            FOREIGN KEY (seller_id) REFERENCES users(id)
        );
        
        CREATE TABLE IF NOT EXISTS listing_views (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            listing_id INTEGER NOT NULL,
            user_id INTEGER,
            viewed_at INTEGER,
            FOREIGN KEY (listing_id) REFERENCES listings(id)
        );
        
        CREATE TABLE IF NOT EXISTS brand_requests (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER NOT NULL,
            brand_name TEXT NOT NULL,
            status TEXT DEFAULT 'pending',
            created_at INTEGER,
            FOREIGN KEY (user_id) REFERENCES users(id)
        );
        
        CREATE TABLE IF NOT EXISTS model_requests (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER NOT NULL,
            brand_id INTEGER NOT NULL,
            model_name TEXT NOT NULL,
            status TEXT DEFAULT 'pending',
            created_at INTEGER,
            FOREIGN KEY (user_id) REFERENCES users(id),
            FOREIGN KEY (brand_id) REFERENCES brands(id)
        );
        
        CREATE TABLE IF NOT EXISTS messages (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            sender_id INTEGER NOT NULL,
            receiver_id INTEGER NOT NULL,
            listing_id INTEGER,
            message_text TEXT NOT NULL,
            is_read INTEGER DEFAULT 0,
            created_at INTEGER,
            FOREIGN KEY (sender_id) REFERENCES users(id),
            FOREIGN KEY (receiver_id) REFERENCES users(id),
            FOREIGN KEY (listing_id) REFERENCES listings(id)
        );
        
        CREATE TABLE IF NOT EXISTS seller_reviews (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            seller_id INTEGER NOT NULL,
            reviewer_id INTEGER NOT NULL,
            listing_id INTEGER,
            rating INTEGER NOT NULL CHECK(rating >= 1 AND rating <= 5),
            review_text TEXT,
            created_at INTEGER,
            FOREIGN KEY (seller_id) REFERENCES users(id),
            FOREIGN KEY (reviewer_id) REFERENCES users(id),
            FOREIGN KEY (listing_id) REFERENCES listings(id),
            UNIQUE(seller_id, reviewer_id, listing_id)
        );
    )";
    return execute(sql);
}

