// FILE: backend/src/services/StatisticsService.h
#pragma once
#include <string>
#include <vector>
#include <memory>

// Forward declarations
class Database;
class ListingRepository;

// Структура статистики оголошення
struct ListingStatistics {
    int totalViews;
    int viewsPerDay;
    int viewsPerWeek;
    int viewsPerMonth;
    double averagePriceByRegion;
    double averagePriceByUkraine;
};

// Структура статистики продавця
struct SellerStatistics {
    int totalListings;
    int activeListings;
    int soldListings;
    int totalViews;
    double averagePrice;
    std::vector<std::pair<int, int>> viewsByDay; // (timestamp, views)
    std::vector<std::pair<std::string, double>> popularListings; // (listing_name, views)
};

// Клас StatisticsService - інкапсуляція логіки статистики
class StatisticsService {
private:
    std::shared_ptr<Database> db_;
    std::shared_ptr<ListingRepository> listingRepository_;

public:
    StatisticsService(std::shared_ptr<Database> db, std::shared_ptr<ListingRepository> listingRepo);
    
    // Отримання статистики для оголошення
    ListingStatistics getListingStatistics(int listingId, const std::string& region);
    
    // Отримання статистики для продавця
    SellerStatistics getSellerStatistics(int sellerId);
    
    // Реєстрація перегляду оголошення
    void recordView(int listingId, int userId);
    
    // Розрахунок середньої ціни по регіону
    double calculateAveragePriceByRegion(int brandId, int modelId, const std::string& region);
    
    // Розрахунок середньої ціни по Україні
    double calculateAveragePriceByUkraine(int brandId, int modelId);
    
private:
    // Отримання історії переглядів з БД
    std::vector<int> getViewsHistory(int listingId);
};

