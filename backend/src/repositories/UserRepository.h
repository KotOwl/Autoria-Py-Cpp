// FILE: backend/src/repositories/UserRepository.h
#pragma once
#include "../models/User.h"
#include "../database/Database.h"
#include <memory>
#include <vector>
#include <string>

// Інтерфейс для репозиторію користувачів
class IUserRepository {
public:
    virtual ~IUserRepository() = default;
    virtual std::unique_ptr<User> findById(int id) = 0;
    virtual std::unique_ptr<User> findByEmail(const std::string& email) = 0;
    virtual bool create(std::unique_ptr<User> user) = 0;
    virtual bool update(std::unique_ptr<User> user) = 0;
    virtual bool deleteUser(int id) = 0;
};

// Реалізація репозиторію користувачів
class UserRepository : public IUserRepository {
private:
    std::shared_ptr<Database> db_;

public:
    UserRepository(std::shared_ptr<Database> db);
    std::shared_ptr<Database> getDb() const { return db_; }
    
    std::unique_ptr<User> findById(int id) override;
    std::unique_ptr<User> findByEmail(const std::string& email) override;
    bool create(std::unique_ptr<User> user) override;
    bool update(std::unique_ptr<User> user) override;
    bool deleteUser(int id) override;
    
    // Додаткові методи
    std::vector<std::unique_ptr<User>> getAll();
    bool banUser(int id);
    bool unbanUser(int id);
    
private:
    std::unique_ptr<User> createUserFromRow(sqlite3_stmt* stmt);
};

