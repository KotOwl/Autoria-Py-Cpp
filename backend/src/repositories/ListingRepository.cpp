// FILE: backend/src/repositories/ListingRepository.cpp
#include "ListingRepository.h"
#include <iostream>
#include <sstream>
#include <algorithm>

ListingRepository::ListingRepository(std::shared_ptr<Database> db) : db_(db) {}

std::unique_ptr<Listing> ListingRepository::findById(int id) {
    const char* sql = "SELECT * FROM listings WHERE id = ?";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db_->getHandle(), sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);
        
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            auto listing = createListingFromRow(stmt);
            sqlite3_finalize(stmt);
            return listing;
        }
    }
    sqlite3_finalize(stmt);
    return nullptr;
}

std::vector<std::unique_ptr<Listing>> ListingRepository::findBySellerId(int sellerId) {
    std::vector<std::unique_ptr<Listing>> listings;
    const char* sql = "SELECT * FROM listings WHERE seller_id = ?";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db_->getHandle(), sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, sellerId);
        
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            auto listing = createListingFromRow(stmt);
            if (listing) {
                listings.push_back(std::move(listing));
            }
        }
    }
    sqlite3_finalize(stmt);
    return listings;
}

std::vector<std::unique_ptr<Listing>> ListingRepository::findActive() {
    std::vector<std::unique_ptr<Listing>> listings;
    const char* sql = "SELECT * FROM listings WHERE status = 'active' ORDER BY created_at DESC";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db_->getHandle(), sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            auto listing = createListingFromRow(stmt);
            if (listing) {
                listings.push_back(std::move(listing));
            }
        }
    }
    sqlite3_finalize(stmt);
    return listings;
}

bool ListingRepository::create(std::unique_ptr<Listing> listing) {
    const char* sql = R"(INSERT INTO listings (seller_id, brand_id, model_id, year, price, 
                currency, exchange_rate, description, region, mileage, status, edit_count, 
                view_count, photos, fuel_type, transmission, color, engine_volume, body_type, 
                doors_count, engine_power, created_at, updated_at) 
                VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?))";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_->getHandle(), sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, listing->getSellerId());
        sqlite3_bind_int(stmt, 2, listing->getBrandId());
        sqlite3_bind_int(stmt, 3, listing->getModelId());
        sqlite3_bind_int(stmt, 4, listing->getYear());
        sqlite3_bind_double(stmt, 5, listing->getPrice());
        // Використовуємо явну довжину для правильного збереження UTF-8 та пробілів
        std::string currency = listing->getCurrency();
        sqlite3_bind_text(stmt, 6, currency.c_str(), static_cast<int>(currency.length()), SQLITE_TRANSIENT);
        sqlite3_bind_double(stmt, 7, listing->getExchangeRate());
        std::string description = listing->getDescription();
        sqlite3_bind_text(stmt, 8, description.c_str(), static_cast<int>(description.length()), SQLITE_TRANSIENT);
        std::string region = listing->getRegion();
        sqlite3_bind_text(stmt, 9, region.c_str(), static_cast<int>(region.length()), SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 10, listing->getMileage());
        std::string status = listing->getStatus();
        sqlite3_bind_text(stmt, 11, status.c_str(), static_cast<int>(status.length()), SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 12, listing->getEditCount());
        sqlite3_bind_int(stmt, 13, listing->getViewCount());
        std::string photos = listing->getPhotos();
        sqlite3_bind_text(stmt, 14, photos.c_str(), static_cast<int>(photos.length()), SQLITE_TRANSIENT);
        std::string fuelType = listing->getFuelType();
        sqlite3_bind_text(stmt, 15, fuelType.c_str(), static_cast<int>(fuelType.length()), SQLITE_TRANSIENT);
        std::string transmission = listing->getTransmission();
        sqlite3_bind_text(stmt, 16, transmission.c_str(), static_cast<int>(transmission.length()), SQLITE_TRANSIENT);
        std::string color = listing->getColor();
        sqlite3_bind_text(stmt, 17, color.c_str(), static_cast<int>(color.length()), SQLITE_TRANSIENT);
        sqlite3_bind_double(stmt, 18, listing->getEngineVolume());
        std::string bodyType = listing->getBodyType();
        sqlite3_bind_text(stmt, 19, bodyType.c_str(), static_cast<int>(bodyType.length()), SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 20, listing->getDoorsCount());
        sqlite3_bind_int(stmt, 21, listing->getEnginePower());
        
        time_t now = time(nullptr);
        sqlite3_bind_int(stmt, 22, now);
        sqlite3_bind_int(stmt, 23, now);
        
        int rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        return rc == SQLITE_DONE;
    }
    return false;
}

bool ListingRepository::update(std::unique_ptr<Listing> listing) {
    const char* sql = R"(UPDATE listings SET brand_id=?, model_id=?, year=?, price=?, 
                currency=?, exchange_rate=?, description=?, region=?, mileage=?, 
                status=?, edit_count=?, photos=?, fuel_type=?, transmission=?, color=?, 
                engine_volume=?, body_type=?, doors_count=?, engine_power=?, updated_at=? WHERE id=?)";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_->getHandle(), sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, listing->getBrandId());
        sqlite3_bind_int(stmt, 2, listing->getModelId());
        sqlite3_bind_int(stmt, 3, listing->getYear());
        sqlite3_bind_double(stmt, 4, listing->getPrice());
        // Використовуємо явну довжину для правильного збереження UTF-8 та пробілів
        std::string currency = listing->getCurrency();
        sqlite3_bind_text(stmt, 5, currency.c_str(), static_cast<int>(currency.length()), SQLITE_TRANSIENT);
        sqlite3_bind_double(stmt, 6, listing->getExchangeRate());
        std::string description = listing->getDescription();
        sqlite3_bind_text(stmt, 7, description.c_str(), static_cast<int>(description.length()), SQLITE_TRANSIENT);
        std::string region = listing->getRegion();
        sqlite3_bind_text(stmt, 8, region.c_str(), static_cast<int>(region.length()), SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 9, listing->getMileage());
        std::string status = listing->getStatus();
        sqlite3_bind_text(stmt, 10, status.c_str(), static_cast<int>(status.length()), SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 11, listing->getEditCount());
        std::string photos = listing->getPhotos();
        sqlite3_bind_text(stmt, 12, photos.c_str(), static_cast<int>(photos.length()), SQLITE_TRANSIENT);
        std::string fuelType = listing->getFuelType();
        sqlite3_bind_text(stmt, 13, fuelType.c_str(), static_cast<int>(fuelType.length()), SQLITE_TRANSIENT);
        std::string transmission = listing->getTransmission();
        sqlite3_bind_text(stmt, 14, transmission.c_str(), static_cast<int>(transmission.length()), SQLITE_TRANSIENT);
        std::string color = listing->getColor();
        sqlite3_bind_text(stmt, 15, color.c_str(), static_cast<int>(color.length()), SQLITE_TRANSIENT);
        sqlite3_bind_double(stmt, 16, listing->getEngineVolume());
        std::string bodyType = listing->getBodyType();
        sqlite3_bind_text(stmt, 17, bodyType.c_str(), static_cast<int>(bodyType.length()), SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 18, listing->getDoorsCount());
        sqlite3_bind_int(stmt, 19, listing->getEnginePower());
        
        time_t now = time(nullptr);
        sqlite3_bind_int(stmt, 20, now);
        sqlite3_bind_int(stmt, 21, listing->getId());
        
        int rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        return rc == SQLITE_DONE;
    }
    return false;
}

bool ListingRepository::deleteListing(int id) {
    const char* sql = "DELETE FROM listings WHERE id = ?";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db_->getHandle(), sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);
        int rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        return rc == SQLITE_DONE;
    }
    return false;
}

std::vector<std::unique_ptr<Listing>> ListingRepository::findByStatus(const std::string& status) {
    std::vector<std::unique_ptr<Listing>> listings;
    const char* sql = "SELECT * FROM listings WHERE status = ?";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db_->getHandle(), sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, status.c_str(), -1, SQLITE_TRANSIENT);
        
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            auto listing = createListingFromRow(stmt);
            if (listing) {
                listings.push_back(std::move(listing));
            }
        }
    }
    sqlite3_finalize(stmt);
    return listings;
}

bool ListingRepository::incrementViewCount(int listingId) {
    const char* sql = "UPDATE listings SET view_count = view_count + 1 WHERE id = ?";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db_->getHandle(), sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, listingId);
        int rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        return rc == SQLITE_DONE;
    }
    return false;
}

std::unique_ptr<Listing> ListingRepository::createListingFromRow(sqlite3_stmt* stmt) {
    int id = sqlite3_column_int(stmt, 0);
    int sellerId = sqlite3_column_int(stmt, 1);
    int brandId = sqlite3_column_int(stmt, 2);
    int modelId = sqlite3_column_int(stmt, 3);
    int year = sqlite3_column_int(stmt, 4);
    double price = sqlite3_column_double(stmt, 5);
    // Читаємо currency з правильним обробкою UTF-8
    const unsigned char* currencyText = sqlite3_column_text(stmt, 6);
    std::string currency;
    if (currencyText) {
        currency = reinterpret_cast<const char*>(currencyText);
    } else {
        currency = "";
    }
    double exchangeRate = sqlite3_column_double(stmt, 7);
    // Безпечне читання description з перевіркою на NULL
    const unsigned char* descText = sqlite3_column_text(stmt, 8);
    std::string description;
    if (descText) {
        // SQLite повертає UTF-8 рядок, який правильно закінчується на \0
        description = reinterpret_cast<const char*>(descText);
        // Видаляємо тільки нульові байти всередині рядка (якщо такі є)
        description.erase(std::remove(description.begin(), description.end(), '\0'), description.end());
    } else {
        description = "";
    }
    
    // Читаємо region з бази даних
    const unsigned char* regionText = sqlite3_column_text(stmt, 9);
    std::string region;
    if (regionText) {
        // SQLite повертає UTF-8 рядок, який правильно закінчується на \0
        region = reinterpret_cast<const char*>(regionText);
        // Видаляємо тільки нульові байти всередині рядка (якщо такі є)
        region.erase(std::remove(region.begin(), region.end(), '\0'), region.end());
    } else {
        region = "";
    }
    
    int mileage = sqlite3_column_int(stmt, 10);
    
    auto listing = std::make_unique<Listing>(id, sellerId, brandId, modelId, year,
                                             price, currency, exchangeRate,
                                             description, region, mileage);
    
    // Читаємо status з правильним обробкою UTF-8
    const unsigned char* statusText = sqlite3_column_text(stmt, 11);
    std::string status;
    if (statusText) {
        status = reinterpret_cast<const char*>(statusText);
    } else {
        status = "";
    }
    listing->setStatus(status);
    
    int editCount = sqlite3_column_int(stmt, 12);
    for (int i = 0; i < editCount; i++) {
        listing->incrementEditCount();
    }
    
    int viewCount = sqlite3_column_int(stmt, 13);
    for (int i = 0; i < viewCount; i++) {
        listing->incrementViewCount();
    }
    
    // Читаємо photos та додаткові поля
    int colCount = sqlite3_column_count(stmt);
    if (colCount > 17) {
        const unsigned char* photosText = sqlite3_column_text(stmt, 17);
        if (photosText) {
            std::string photosStr = reinterpret_cast<const char*>(photosText);
            // Перевіряємо, чи це валідний JSON масив
            if (photosStr.length() >= 2 && photosStr[0] == '[' && photosStr[photosStr.length() - 1] == ']') {
                listing->setPhotos(photosStr);
            } else {
                // Якщо невалідний JSON, встановлюємо порожній масив
                listing->setPhotos("[]");
            }
        } else {
            listing->setPhotos("[]");
        }
    } else {
        listing->setPhotos("[]");
    }
    
    // Додаткові характеристики (якщо є)
    if (colCount > 18) {
        const unsigned char* fuelTypeText = sqlite3_column_text(stmt, 18);
        if (fuelTypeText) {
            std::string fuelType = reinterpret_cast<const char*>(fuelTypeText);
            if (!fuelType.empty()) {
                listing->setFuelType(fuelType);
            }
        }
    }
    if (colCount > 19) {
        const unsigned char* transmissionText = sqlite3_column_text(stmt, 19);
        if (transmissionText) {
            std::string transmission = reinterpret_cast<const char*>(transmissionText);
            if (!transmission.empty()) {
                listing->setTransmission(transmission);
            }
        }
    }
    if (colCount > 20) {
        const unsigned char* colorText = sqlite3_column_text(stmt, 20);
        if (colorText) {
            std::string color = reinterpret_cast<const char*>(colorText);
            if (!color.empty()) {
                listing->setColor(color);
            }
        }
    }
    if (colCount > 21) {
        double engineVolume = sqlite3_column_double(stmt, 21);
        if (engineVolume > 0) listing->setEngineVolume(engineVolume);
    }
    if (colCount > 22) {
        const unsigned char* bodyTypeText = sqlite3_column_text(stmt, 22);
        if (bodyTypeText) {
            std::string bodyType = reinterpret_cast<const char*>(bodyTypeText);
            if (!bodyType.empty()) {
                listing->setBodyType(bodyType);
            }
        }
    }
    if (colCount > 23) {
        int doorsCount = sqlite3_column_int(stmt, 23);
        if (doorsCount > 0) listing->setDoorsCount(doorsCount);
    }
    if (colCount > 24) {
        int enginePower = sqlite3_column_int(stmt, 24);
        if (enginePower > 0) listing->setEnginePower(enginePower);
    }
    
    return listing;
}

std::vector<std::unique_ptr<Listing>> ListingRepository::searchAndFilter(
    const std::string& searchQuery,
    int brandId,
    int modelId,
    double minPrice,
    double maxPrice,
    const std::string& region,
    const std::string& fuelType,
    const std::string& transmission,
    const std::string& sortBy,
    const std::string& sortOrder,
    int limit,
    int offset
) {
    std::vector<std::unique_ptr<Listing>> listings;
    
    // Побудова динамічного SQL запиту
    std::ostringstream sql;
    sql << "SELECT l.* FROM listings l WHERE l.status = 'active'";
    
    std::vector<std::string> conditions;
    std::vector<std::string> bindValues;
    
    if (!searchQuery.empty()) {
        conditions.push_back("(l.description LIKE ? OR l.region LIKE ?)");
        bindValues.push_back("%" + searchQuery + "%");
        bindValues.push_back("%" + searchQuery + "%");
    }
    
    if (brandId > 0) {
        conditions.push_back("l.brand_id = ?");
        bindValues.push_back(std::to_string(brandId));
    }
    
    if (modelId > 0) {
        conditions.push_back("l.model_id = ?");
        bindValues.push_back(std::to_string(modelId));
    }
    
    if (minPrice > 0) {
        conditions.push_back("l.price >= ?");
        bindValues.push_back(std::to_string(minPrice));
    }
    
    if (maxPrice > 0) {
        conditions.push_back("l.price <= ?");
        bindValues.push_back(std::to_string(maxPrice));
    }
    
    if (!region.empty()) {
        conditions.push_back("l.region LIKE ?");
        bindValues.push_back("%" + region + "%");
    }
    
    if (!fuelType.empty()) {
        conditions.push_back("l.fuel_type = ?");
        bindValues.push_back(fuelType);
    }
    
    if (!transmission.empty()) {
        conditions.push_back("l.transmission = ?");
        bindValues.push_back(transmission);
    }
    
    // Додаємо умови до SQL
    for (const auto& condition : conditions) {
        sql << " AND " << condition;
    }
    
    // Валідація sortBy для безпеки
    std::string validSortBy = sortBy;
    if (sortBy != "price" && sortBy != "created_at" && sortBy != "mileage" && sortBy != "view_count" && sortBy != "year") {
        validSortBy = "created_at";
    }
    
    // Валідація sortOrder
    std::string validSortOrder = (sortOrder == "ASC" || sortOrder == "asc") ? "ASC" : "DESC";
    
    sql << " ORDER BY l." << validSortBy << " " << validSortOrder;
    sql << " LIMIT ? OFFSET ?";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_->getHandle(), sql.str().c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        int bindIndex = 1;
        
        // Біндимо значення для пошуку
        if (!searchQuery.empty()) {
            std::string searchPattern1 = "%" + searchQuery + "%";
            std::string searchPattern2 = "%" + searchQuery + "%";
            sqlite3_bind_text(stmt, bindIndex++, searchPattern1.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, bindIndex++, searchPattern2.c_str(), -1, SQLITE_TRANSIENT);
        }
        
        // Біндимо інші параметри
        if (brandId > 0) {
            sqlite3_bind_int(stmt, bindIndex++, brandId);
        }
        if (modelId > 0) {
            sqlite3_bind_int(stmt, bindIndex++, modelId);
        }
        if (minPrice > 0) {
            sqlite3_bind_double(stmt, bindIndex++, minPrice);
        }
        if (maxPrice > 0) {
            sqlite3_bind_double(stmt, bindIndex++, maxPrice);
        }
        if (!region.empty()) {
            std::string regionPattern = "%" + region + "%";
            sqlite3_bind_text(stmt, bindIndex++, regionPattern.c_str(), -1, SQLITE_TRANSIENT);
        }
        if (!fuelType.empty()) {
            sqlite3_bind_text(stmt, bindIndex++, fuelType.c_str(), -1, SQLITE_TRANSIENT);
        }
        if (!transmission.empty()) {
            sqlite3_bind_text(stmt, bindIndex++, transmission.c_str(), -1, SQLITE_TRANSIENT);
        }
        
        // Біндимо limit та offset
        sqlite3_bind_int(stmt, bindIndex++, limit);
        sqlite3_bind_int(stmt, bindIndex++, offset);
        
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            auto listing = createListingFromRow(stmt);
            if (listing) {
                listings.push_back(std::move(listing));
            }
        }
    }
    sqlite3_finalize(stmt);
    return listings;
}

