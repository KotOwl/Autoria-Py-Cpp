// FILE: backend/src/api/ApiServer.h
#pragma once
#include "../repositories/UserRepository.h"
#include "../repositories/ListingRepository.h"
#include "../repositories/BrandRepository.h"
#include "../middleware/AuthMiddleware.h"
#include "../services/ModerationService.h"
#include "../services/CurrencyService.h"
#include "../services/StatisticsService.h"
#include "httplib.h"
#include <string>
#include <memory>

// Клас ApiServer - інкапсуляція HTTP сервера та REST API
class ApiServer {
private:
    std::shared_ptr<UserRepository> userRepository_;
    std::shared_ptr<ListingRepository> listingRepository_;
    std::shared_ptr<BrandRepository> brandRepository_;
    std::shared_ptr<ModelRepository> modelRepository_;
    std::shared_ptr<AuthMiddleware> authMiddleware_;
    std::shared_ptr<ModerationService> moderationService_;
    CurrencyService* currencyService_; // Singleton, не shared_ptr
    std::shared_ptr<StatisticsService> statisticsService_;
    int port_;
    void* server_; // httplib::Server*

public:
    ApiServer(std::shared_ptr<UserRepository> userRepo,
              std::shared_ptr<ListingRepository> listingRepo,
              std::shared_ptr<BrandRepository> brandRepo,
              std::shared_ptr<ModelRepository> modelRepo,
              std::shared_ptr<AuthMiddleware> auth,
              int port = 8080);
    ~ApiServer();
    
    bool start();
    void stop();
    
private:
    // Обробники HTTP запитів
    std::string handleGetListings(const std::string& query);
    std::string handleGetMyListings(const std::string& authToken);
    std::string handleGetListing(int id, const std::string& authToken);
    std::string handleCreateListing(const std::string& body, const std::string& authToken);
    std::string handleUpdateListing(int id, const std::string& body, const std::string& authToken);
    std::string handleDeleteListing(int id, const std::string& authToken);
    
    std::string handleGetBrands();
    std::string handleGetModels(int brandId);
    std::string handleRequestBrand(const std::string& body, const std::string& authToken);
    std::string handleRequestModel(const std::string& body, const std::string& authToken);
    
    std::string handleGetUser(int id, const std::string& authToken);
    std::string handleGetCurrentUser(const std::string& authToken);
    std::string handleCreateUser(const std::string& body);
    std::string handleLogin(const std::string& body);
    std::string handleCreateManager(const std::string& body, const std::string& authToken);
    
    std::string handleGetListingStats(int id, const std::string& authToken);
    std::string handleGetSellerStats(const std::string& authToken);
    std::string handleCreatePurchaseRequest(int listingId, const std::string& body, const std::string& authToken);
    std::string handleMarkAsSold(int listingId, const std::string& authToken);
    std::string handleGetListingsFiltered(const std::string& query);
    std::string handleAddToFavorites(int listingId, const std::string& authToken);
    std::string handleRemoveFromFavorites(int listingId, const std::string& authToken);
    std::string handleGetFavorites(const std::string& authToken);
    std::string handleAddComment(int listingId, const std::string& body, const std::string& authToken);
    std::string handleGetComments(int listingId);
    std::string handleGetNotifications(const std::string& authToken);
    std::string handleMarkNotificationRead(int notificationId, const std::string& authToken);
    
    // Повідомлення
    std::string handleSendMessage(const std::string& body, const std::string& authToken);
    std::string handleGetMessages(const std::string& authToken, int otherUserId = 0);
    std::string handleMarkMessageRead(int messageId, const std::string& authToken);
    
    // Адмін панель
    std::string handleGetPendingListings(const std::string& authToken);
    std::string handleModerateListing(int listingId, const std::string& body, const std::string& authToken);
    std::string handleGetAllUsers(const std::string& authToken);
    std::string handleBanUser(int userId, const std::string& body, const std::string& authToken);
    std::string handleGetPlatformStats(const std::string& authToken);
    
    // Порівняння оголошень
    std::string handleCompareListings(const std::string& body, const std::string& authToken);
    std::string handleGetComparison(int comparisonId, const std::string& authToken);
    
    // Історія переглядів
    std::string handleGetViewHistory(const std::string& authToken);
    std::string handleAddViewHistory(int listingId, const std::string& authToken);
    
    // Рекомендації
    std::string handleGetRecommendations(const std::string& authToken, int listingId = 0);
    
    // Завантаження фото
    std::string handleUploadPhoto(int listingId, const httplib::Request& req, const std::string& authToken);
    
    // Валідація
    bool validateListingJson(const std::string& json, std::string& error);
    std::string extractAuthToken(const std::string& header);

    // Maintenance
    void normalizeStoredText();
};

