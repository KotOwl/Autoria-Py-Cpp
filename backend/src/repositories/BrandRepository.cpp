// FILE: backend/src/repositories/BrandRepository.cpp
#include "BrandRepository.h"
#include <iostream>

BrandRepository::BrandRepository(std::shared_ptr<Database> db) : db_(db) {}

std::vector<std::unique_ptr<Brand>> BrandRepository::getAll() {
    std::vector<std::unique_ptr<Brand>> brands;
    const char* sql = "SELECT * FROM brands WHERE is_active = 1 ORDER BY name";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db_->getHandle(), sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            brands.push_back(std::make_unique<Brand>(id, name));
        }
    }
    sqlite3_finalize(stmt);
    return brands;
}

std::unique_ptr<Brand> BrandRepository::findById(int id) {
    const char* sql = "SELECT * FROM brands WHERE id = ?";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db_->getHandle(), sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);
        
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            bool isActive = sqlite3_column_int(stmt, 2) == 1;
            sqlite3_finalize(stmt);
            return std::make_unique<Brand>(id, name, isActive);
        }
    }
    sqlite3_finalize(stmt);
    return nullptr;
}

bool BrandRepository::create(const std::string& name) {
    const char* sql = "INSERT INTO brands (name, is_active) VALUES (?, 1)";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db_->getHandle(), sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, name.c_str(), static_cast<int>(name.length()), SQLITE_TRANSIENT);
        int rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        return rc == SQLITE_DONE;
    }
    return false;
}

bool BrandRepository::update(int id, const std::string& name) {
    const char* sql = "UPDATE brands SET name = ? WHERE id = ?";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db_->getHandle(), sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, name.c_str(), static_cast<int>(name.length()), SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 2, id);
        int rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        return rc == SQLITE_DONE;
    }
    return false;
}

bool BrandRepository::deleteBrand(int id) {
    const char* sql = "UPDATE brands SET is_active = 0 WHERE id = ?";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db_->getHandle(), sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);
        int rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        return rc == SQLITE_DONE;
    }
    return false;
}

ModelRepository::ModelRepository(std::shared_ptr<Database> db) : db_(db) {}

std::vector<std::unique_ptr<Model>> ModelRepository::getByBrandId(int brandId) {
    std::vector<std::unique_ptr<Model>> models;
    const char* sql = "SELECT * FROM models WHERE brand_id = ? AND is_active = 1 ORDER BY name";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db_->getHandle(), sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, brandId);
        
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            int bid = sqlite3_column_int(stmt, 1);
            std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            models.push_back(std::make_unique<Model>(id, bid, name));
        }
    }
    sqlite3_finalize(stmt);
    return models;
}

std::unique_ptr<Model> ModelRepository::findById(int id) {
    const char* sql = "SELECT * FROM models WHERE id = ?";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db_->getHandle(), sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);
        
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            int bid = sqlite3_column_int(stmt, 1);
            std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            bool isActive = sqlite3_column_int(stmt, 3) == 1;
            sqlite3_finalize(stmt);
            return std::make_unique<Model>(id, bid, name, isActive);
        }
    }
    sqlite3_finalize(stmt);
    return nullptr;
}

bool ModelRepository::create(int brandId, const std::string& name) {
    const char* sql = "INSERT INTO models (brand_id, name, is_active) VALUES (?, ?, 1)";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db_->getHandle(), sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, brandId);
        sqlite3_bind_text(stmt, 2, name.c_str(), static_cast<int>(name.length()), SQLITE_TRANSIENT);
        int rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        return rc == SQLITE_DONE;
    }
    return false;
}

bool ModelRepository::update(int id, const std::string& name) {
    const char* sql = "UPDATE models SET name = ? WHERE id = ?";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db_->getHandle(), sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, name.c_str(), static_cast<int>(name.length()), SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 2, id);
        int rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        return rc == SQLITE_DONE;
    }
    return false;
}

bool ModelRepository::deleteModel(int id) {
    const char* sql = "UPDATE models SET is_active = 0 WHERE id = ?";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db_->getHandle(), sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);
        int rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        return rc == SQLITE_DONE;
    }
    return false;
}


