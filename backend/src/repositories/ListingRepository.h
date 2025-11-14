// FILE: backend/src/repositories/ListingRepository.h
#pragma once
#include "../models/Listing.h"
#include "../database/Database.h"
#include <memory>
#include <vector>

// Інтерфейс для репозиторію оголошень
class IListingRepository {
public:
    virtual ~IListingRepository() = default;
    virtual std::unique_ptr<Listing> findById(int id) = 0;
    virtual std::vector<std::unique_ptr<Listing>> findBySellerId(int sellerId) = 0;
    virtual std::vector<std::unique_ptr<Listing>> findActive() = 0;
    virtual bool create(std::unique_ptr<Listing> listing) = 0;
    virtual bool update(std::unique_ptr<Listing> listing) = 0;
    virtual bool deleteListing(int id) = 0;
};

// Реалізація репозиторію оголошень
class ListingRepository : public IListingRepository {
private:
    std::shared_ptr<Database> db_;

public:
    ListingRepository(std::shared_ptr<Database> db);
    std::shared_ptr<Database> getDb() const { return db_; }
    
    std::unique_ptr<Listing> findById(int id) override;
    std::vector<std::unique_ptr<Listing>> findBySellerId(int sellerId) override;
    std::vector<std::unique_ptr<Listing>> findActive() override;
    bool create(std::unique_ptr<Listing> listing) override;
    bool update(std::unique_ptr<Listing> listing) override;
    bool deleteListing(int id) override;
    
    // Додаткові методи
    std::vector<std::unique_ptr<Listing>> findByStatus(const std::string& status);
    bool incrementViewCount(int listingId);
    
    // Пошук та сортування
    std::vector<std::unique_ptr<Listing>> searchAndFilter(
        const std::string& searchQuery = "",
        int brandId = 0,
        int modelId = 0,
        double minPrice = 0,
        double maxPrice = 0,
        const std::string& region = "",
        const std::string& fuelType = "",
        const std::string& transmission = "",
        const std::string& sortBy = "created_at",
        const std::string& sortOrder = "DESC",
        int limit = 100,
        int offset = 0
    );
    
private:
    std::unique_ptr<Listing> createListingFromRow(sqlite3_stmt* stmt);
};

