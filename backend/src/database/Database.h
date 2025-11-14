// FILE: backend/src/database/Database.h
#pragma once
#include <sqlite3.h>
#include <string>
#include <memory>

// Клас Database - інкапсуляція роботи з SQLite
class Database {
private:
    sqlite3* db_;
    std::string dbPath_;

    Database(const std::string& dbPath);

public:
    ~Database();
    
    static std::unique_ptr<Database> create(const std::string& dbPath);
    
    bool execute(const std::string& sql);
    sqlite3* getHandle() { return db_; }
    
    bool initializeSchema();
};



