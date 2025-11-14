// FILE: backend/src/main.cpp
#include "database/Database.h"
#include "repositories/UserRepository.h"
#include "repositories/ListingRepository.h"
#include "repositories/BrandRepository.h"
#include "middleware/AuthMiddleware.h"
#include "api/ApiServer.h"
#include <iostream>
#include <memory>

int main() {
    // Створення БД
    auto db = Database::create("/app/build/data/autoria.db");
    if (!db) {
        std::cerr << "Failed to create database" << std::endl;
        return 1;
    }
    
    if (!db->initializeSchema()) {
        std::cerr << "Failed to initialize schema" << std::endl;
        return 1;
    }
    
    // Створення репозиторіїв
    auto sharedDb = std::shared_ptr<Database>(db.release());
    auto userRepo = std::make_shared<UserRepository>(sharedDb);
    auto listingRepo = std::make_shared<ListingRepository>(sharedDb);
    auto brandRepo = std::make_shared<BrandRepository>(sharedDb);
    auto modelRepo = std::make_shared<ModelRepository>(sharedDb);
    
    // Створення middleware
    auto authMiddleware = std::make_shared<AuthMiddleware>(userRepo);
    
    // Створення API сервера
    ApiServer server(userRepo, listingRepo, brandRepo, modelRepo, authMiddleware, 8080);
    
    std::cout << "AutoRia API Server" << std::endl;
    std::cout << "API available at http://localhost:8080/api" << std::endl;
    
    if (!server.start()) {
        std::cerr << "Failed to start server" << std::endl;
        return 1;
    }
    
    return 0;
}

