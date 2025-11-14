// FILE: backend/src/services/StatisticsService.cpp
#include "StatisticsService.h"
#include "../database/Database.h"
#include "../repositories/ListingRepository.h"
#include <algorithm>
#include <ctime>
#include <cmath>
#include <sqlite3.h>
#include <sstream>

StatisticsService::StatisticsService(std::shared_ptr<Database> db, std::shared_ptr<ListingRepository> listingRepo)
    : db_(db), listingRepository_(listingRepo) {
}

void StatisticsService::recordView(int listingId, int userId) {
    // Запис перегляду в БД
    std::ostringstream sql;
    sql << "INSERT INTO listing_views (listing_id, user_id, viewed_at) VALUES ("
        << listingId << ", " << (userId > 0 ? userId : 0) << ", " << time(nullptr) << ")";
    db_->execute(sql.str());
    
    // Оновлюємо view_count в таблиці listings
    listingRepository_->incrementViewCount(listingId);
}

std::vector<int> StatisticsService::getViewsHistory(int listingId) {
    std::vector<int> views;
    std::ostringstream sql;
    sql << "SELECT viewed_at FROM listing_views WHERE listing_id = " << listingId << " ORDER BY viewed_at DESC";
    
    sqlite3_stmt* stmt;
    const char* sqlStr = sql.str().c_str();
    if (sqlite3_prepare_v2(db_->getHandle(), sqlStr, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int timestamp = sqlite3_column_int(stmt, 0);
            views.push_back(timestamp);
        }
        sqlite3_finalize(stmt);
    }
    return views;
}

ListingStatistics StatisticsService::getListingStatistics(int listingId, const std::string& region) {
    ListingStatistics stats;
    
    auto views = getViewsHistory(listingId);
    stats.totalViews = views.size();
    
    time_t now = time(nullptr);
    int dayAgo = static_cast<int>(now) - 86400;
    int weekAgo = static_cast<int>(now) - 604800;
    int monthAgo = static_cast<int>(now) - 2592000;
    
    stats.viewsPerDay = 0;
    stats.viewsPerWeek = 0;
    stats.viewsPerMonth = 0;
    
    for (int timestamp : views) {
        if (timestamp > dayAgo) stats.viewsPerDay++;
        if (timestamp > weekAgo) stats.viewsPerWeek++;
        if (timestamp > monthAgo) stats.viewsPerMonth++;
    }
    
    // Отримуємо оголошення для розрахунку середніх цін
    auto listing = listingRepository_->findById(listingId);
    if (listing) {
        stats.averagePriceByRegion = calculateAveragePriceByRegion(
            listing->getBrandId(), listing->getModelId(), region);
        stats.averagePriceByUkraine = calculateAveragePriceByUkraine(
            listing->getBrandId(), listing->getModelId());
    } else {
        stats.averagePriceByRegion = 0.0;
        stats.averagePriceByUkraine = 0.0;
    }
    
    return stats;
}

SellerStatistics StatisticsService::getSellerStatistics(int sellerId) {
    SellerStatistics stats;
    
    // Отримуємо всі оголошення продавця
    auto listings = listingRepository_->findBySellerId(sellerId);
    stats.totalListings = listings.size();
    stats.activeListings = 0;
    stats.soldListings = 0;
    stats.totalViews = 0;
    double totalPrice = 0.0;
    int priceCount = 0;
    
    std::vector<std::pair<std::string, int>> popular; // (listing_name, views)
    
    for (const auto& listing : listings) {
        if (listing->getStatus() == "active") {
            stats.activeListings++;
        } else if (listing->getStatus() == "sold") {
            stats.soldListings++;
        }
        
        stats.totalViews += listing->getViewCount();
        totalPrice += listing->getPrice();
        priceCount++;
        
        // Формуємо назву оголошення для популярних
        std::ostringstream name;
        name << "Listing #" << listing->getId();
        popular.push_back({name.str(), listing->getViewCount()});
    }
    
    stats.averagePrice = priceCount > 0 ? totalPrice / priceCount : 0.0;
    
    // Сортуємо популярні оголошення за кількістю переглядів
    std::sort(popular.begin(), popular.end(), 
              [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
                  return a.second > b.second;
              });
    
    // Беремо топ-5
    stats.popularListings.clear();
    for (size_t i = 0; i < std::min(size_t(5), popular.size()); ++i) {
        stats.popularListings.push_back({popular[i].first, static_cast<double>(popular[i].second)});
    }
    
    // Отримуємо перегляди по днях за останні 30 днів
    time_t now = time(nullptr);
    stats.viewsByDay.clear();
    for (int i = 29; i >= 0; --i) {
        int dayStart = static_cast<int>(now) - (i * 86400);
        int dayEnd = dayStart + 86400;
        
        std::ostringstream sql;
        sql << "SELECT COUNT(*) FROM listing_views WHERE listing_id IN ("
            << "SELECT id FROM listings WHERE seller_id = " << sellerId << ")"
            << " AND viewed_at >= " << dayStart << " AND viewed_at < " << dayEnd;
        
        sqlite3_stmt* stmt;
        const char* sqlStr = sql.str().c_str();
        int views = 0;
        if (sqlite3_prepare_v2(db_->getHandle(), sqlStr, -1, &stmt, nullptr) == SQLITE_OK) {
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                views = sqlite3_column_int(stmt, 0);
            }
            sqlite3_finalize(stmt);
        }
        stats.viewsByDay.push_back({dayStart, views});
    }
    
    return stats;
}

double StatisticsService::calculateAveragePriceByRegion(int brandId, int modelId, const std::string& region) {
    std::ostringstream sql;
    sql << "SELECT AVG(price) FROM listings WHERE brand_id = " << brandId
        << " AND model_id = " << modelId
        << " AND status = 'active'";
    if (!region.empty()) {
        sql << " AND region = '" << region << "'";
    }
    
    sqlite3_stmt* stmt;
    const char* sqlStr = sql.str().c_str();
    double avgPrice = 0.0;
    if (sqlite3_prepare_v2(db_->getHandle(), sqlStr, -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            avgPrice = sqlite3_column_double(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }
    return avgPrice;
}

double StatisticsService::calculateAveragePriceByUkraine(int brandId, int modelId) {
    std::ostringstream sql;
    sql << "SELECT AVG(price) FROM listings WHERE brand_id = " << brandId
        << " AND model_id = " << modelId
        << " AND status = 'active'";
    
    sqlite3_stmt* stmt;
    const char* sqlStr = sql.str().c_str();
    double avgPrice = 0.0;
    if (sqlite3_prepare_v2(db_->getHandle(), sqlStr, -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            avgPrice = sqlite3_column_double(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }
    return avgPrice;
}

