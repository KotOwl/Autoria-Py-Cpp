// FILE: backend/src/repositories/UserRepository.cpp
#include "UserRepository.h"
#include "../models/User.h"
#include <iostream>
#include <cstring>

UserRepository::UserRepository(std::shared_ptr<Database> db) : db_(db) {}

std::unique_ptr<User> UserRepository::findById(int id) {
    const char* sql = "SELECT * FROM users WHERE id = ?";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db_->getHandle(), sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);
        
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            auto user = createUserFromRow(stmt);
            sqlite3_finalize(stmt);
            return user;
        }
    }
    sqlite3_finalize(stmt);
    return nullptr;
}

std::unique_ptr<User> UserRepository::findByEmail(const std::string& email) {
    const char* sql = "SELECT * FROM users WHERE email = ?";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db_->getHandle(), sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, email.c_str(), static_cast<int>(email.length()), SQLITE_TRANSIENT);
        
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            auto user = createUserFromRow(stmt);
            sqlite3_finalize(stmt);
            return user;
        }
    }
    sqlite3_finalize(stmt);
    return nullptr;
}

bool UserRepository::create(std::unique_ptr<User> user) {
    // Визначаємо роль з типу користувача ДО використання
    std::string role = "buyer";
    int createdByAdminId = -1;
    
    if (auto* seller = dynamic_cast<Seller*>(user.get())) {
        role = "seller";
    } else if (auto* manager = dynamic_cast<Manager*>(user.get())) {
        role = "manager";
        createdByAdminId = manager->getCreatedByAdminId();
    } else if (dynamic_cast<Administrator*>(user.get())) {
        role = "administrator";
    }
    
    // Зберігаємо дані до переміщення
    std::string email = user->getEmail();
    std::string firstName = user->getFirstName();
    std::string lastName = user->getLastName();
    std::string accountType = user->getAccountType();
    bool isActive = user->isActive();
    
    const char* sql = R"(INSERT INTO users (email, password_hash, first_name, last_name, 
                phone, account_type, is_active, role, created_by_admin_id, created_at, updated_at) 
                VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?))";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_->getHandle(), sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, email.c_str(), static_cast<int>(email.length()), SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, "hashed_password", 14, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, firstName.c_str(), static_cast<int>(firstName.length()), SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, lastName.c_str(), static_cast<int>(lastName.length()), SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 5, "", 0, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 6, accountType.c_str(), static_cast<int>(accountType.length()), SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 7, isActive ? 1 : 0);
        sqlite3_bind_text(stmt, 8, role.c_str(), static_cast<int>(role.length()), SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 9, createdByAdminId);
        
        time_t now = time(nullptr);
        sqlite3_bind_int(stmt, 10, now);
        sqlite3_bind_int(stmt, 11, now);
        
        int rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        return rc == SQLITE_DONE;
    }
    return false;
}

bool UserRepository::update(std::unique_ptr<User> user) {
    const char* sql = R"(UPDATE users SET email=?, first_name=?, last_name=?, 
                account_type=?, is_active=?, updated_at=? WHERE id=?)";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_->getHandle(), sql, -1, &stmt, nullptr) == SQLITE_OK) {
        {
            auto s = user->getEmail();
            sqlite3_bind_text(stmt, 1, s.c_str(), static_cast<int>(s.length()), SQLITE_TRANSIENT);
        }
        {
            auto s = user->getFirstName();
            sqlite3_bind_text(stmt, 2, s.c_str(), static_cast<int>(s.length()), SQLITE_TRANSIENT);
        }
        {
            auto s = user->getLastName();
            sqlite3_bind_text(stmt, 3, s.c_str(), static_cast<int>(s.length()), SQLITE_TRANSIENT);
        }
        {
            auto s = user->getAccountType();
            sqlite3_bind_text(stmt, 4, s.c_str(), static_cast<int>(s.length()), SQLITE_TRANSIENT);
        }
        sqlite3_bind_int(stmt, 5, user->isActive() ? 1 : 0);
        
        time_t now = time(nullptr);
        sqlite3_bind_int(stmt, 6, now);
        sqlite3_bind_int(stmt, 7, user->getId());
        
        int rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        return rc == SQLITE_DONE;
    }
    return false;
}

bool UserRepository::deleteUser(int id) {
    const char* sql = "DELETE FROM users WHERE id = ?";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db_->getHandle(), sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);
        int rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        return rc == SQLITE_DONE;
    }
    return false;
}

std::vector<std::unique_ptr<User>> UserRepository::getAll() {
    std::vector<std::unique_ptr<User>> users;
    const char* sql = "SELECT * FROM users";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db_->getHandle(), sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            auto user = createUserFromRow(stmt);
            if (user) {
                users.push_back(std::move(user));
            }
        }
    }
    sqlite3_finalize(stmt);
    return users;
}

bool UserRepository::banUser(int id) {
    const char* sql = "UPDATE users SET is_active = 0 WHERE id = ?";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db_->getHandle(), sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);
        int rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        return rc == SQLITE_DONE;
    }
    return false;
}

bool UserRepository::unbanUser(int id) {
    const char* sql = "UPDATE users SET is_active = 1 WHERE id = ?";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db_->getHandle(), sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);
        int rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        return rc == SQLITE_DONE;
    }
    return false;
}

std::unique_ptr<User> UserRepository::createUserFromRow(sqlite3_stmt* stmt) {
    int id = sqlite3_column_int(stmt, 0);
    std::string email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    std::string firstName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
    std::string lastName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
    std::string phone = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
    std::string accountType = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
    std::string role = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));
    int createdByAdminId = sqlite3_column_int(stmt, 9);
    
    std::string passwordHash = "hashed"; // В реальності з БД
    
    if (role == "seller") {
        return std::make_unique<Seller>(id, email, passwordHash, firstName, lastName, phone, accountType);
    } else if (role == "manager") {
        return std::make_unique<Manager>(id, email, passwordHash, firstName, lastName, phone, createdByAdminId);
    } else if (role == "administrator") {
        return std::make_unique<Administrator>(id, email, passwordHash, firstName, lastName, phone);
    } else {
        return std::make_unique<Buyer>(id, email, passwordHash, firstName, lastName, phone);
    }
}

