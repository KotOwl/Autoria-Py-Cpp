// FILE: backend/src/models/Listing.h
#pragma once
#include <string>
#include <ctime>

// Клас Listing - інкапсуляція оголошення про продаж авто
class Listing {
private:
    int id_;
    int sellerId_;
    int brandId_;
    int modelId_;
    int year_;
    double price_;
    std::string currency_; // "USD", "EUR", "UAH"
    double exchangeRate_; // Курс на момент створення
    std::string description_;
    std::string region_; // Регіон продажу
    int mileage_;
    std::string status_; // "draft", "pending", "active", "rejected", "inactive"
    int editCount_; // Кількість редагувань
    time_t createdAt_;
    time_t updatedAt_;
    int viewCount_;
    time_t lastModerationDate_;
    std::string photos_; // JSON array of photo URLs/paths
    std::string fuelType_; // "petrol", "diesel", "electric", "hybrid"
    std::string transmission_; // "manual", "automatic", "robot"
    std::string color_;
    double engineVolume_; // в літрах
    std::string bodyType_; // "sedan", "hatchback", "suv", "coupe", etc.
    int doorsCount_;
    int enginePower_; // в к.с.

public:
    Listing(int id, int sellerId, int brandId, int modelId, int year,
            double price, const std::string& currency, double exchangeRate,
            const std::string& description, const std::string& region, int mileage);
    
    // Геттери
    int getId() const { return id_; }
    int getSellerId() const { return sellerId_; }
    int getBrandId() const { return brandId_; }
    int getModelId() const { return modelId_; }
    int getYear() const { return year_; }
    double getPrice() const { return price_; }
    std::string getCurrency() const { return currency_; }
    double getExchangeRate() const { return exchangeRate_; }
    std::string getStatus() const { return status_; }
    int getEditCount() const { return editCount_; }
    int getViewCount() const { return viewCount_; }
    std::string getRegion() const { return region_; }
    std::string getDescription() const { return description_; }
    int getMileage() const { return mileage_; }
    std::string getPhotos() const { return photos_; }
    std::string getFuelType() const { return fuelType_; }
    std::string getTransmission() const { return transmission_; }
    std::string getColor() const { return color_; }
    double getEngineVolume() const { return engineVolume_; }
    std::string getBodyType() const { return bodyType_; }
    int getDoorsCount() const { return doorsCount_; }
    int getEnginePower() const { return enginePower_; }
    
    // Сеттери
    void setStatus(const std::string& status) { status_ = status; }
    void setPhotos(const std::string& photos) { photos_ = photos; }
    void setFuelType(const std::string& fuelType) { fuelType_ = fuelType; }
    void setTransmission(const std::string& transmission) { transmission_ = transmission; }
    void setColor(const std::string& color) { color_ = color; }
    void setEngineVolume(double volume) { engineVolume_ = volume; }
    void setBodyType(const std::string& bodyType) { bodyType_ = bodyType; }
    void setDoorsCount(int count) { doorsCount_ = count; }
    void setEnginePower(int power) { enginePower_ = power; }
    void incrementEditCount() { editCount_++; }
    void incrementViewCount() { viewCount_++; }
    void setLastModerationDate(time_t date) { lastModerationDate_ = date; }
    
    // Перевірка можливості редагування
    bool canEdit() const { return editCount_ < 3; }
    
    // Конвертація ціни в інші валюти
    double getPriceInUSD() const;
    double getPriceInEUR() const;
    double getPriceInUAH() const;
    
    // Серіалізація
    std::string toJson() const;
    std::string toJsonWithStats() const; // Зі статистикою для преміум
};

