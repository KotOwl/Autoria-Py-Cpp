// FILE: backend/src/models/Listing.cpp
#include "Listing.h"
#include "../services/CurrencyService.h"
#include <sstream>
#include <cmath>
#include <iomanip>

// Функція для екранування JSON рядків
static std::string escapeJson(const std::string& str) {
    std::ostringstream oss;
    for (char c : str) {
        switch (c) {
            case '"': oss << "\\\""; break;
            case '\\': oss << "\\\\"; break;
            case '\b': oss << "\\b"; break;
            case '\f': oss << "\\f"; break;
            case '\n': oss << "\\n"; break;
            case '\r': oss << "\\r"; break;
            case '\t': oss << "\\t"; break;
            default:
                if (static_cast<unsigned char>(c) < 0x20) {
                    oss << "\\u" << std::hex << std::setw(4) << std::setfill('0') 
                        << static_cast<int>(static_cast<unsigned char>(c));
                } else {
                    oss << c;
                }
                break;
        }
    }
    return oss.str();
}

Listing::Listing(int id, int sellerId, int brandId, int modelId, int year,
                 double price, const std::string& currency, double exchangeRate,
                 const std::string& description, const std::string& region, int mileage)
    : id_(id), sellerId_(sellerId), brandId_(brandId), modelId_(modelId),
      year_(year), price_(price), currency_(currency), exchangeRate_(exchangeRate),
      description_(description), region_(region), mileage_(mileage),
      status_("draft"), editCount_(0), viewCount_(0), photos_("[]"),
      fuelType_(""), transmission_(""), color_(""), engineVolume_(0.0),
      bodyType_(""), doorsCount_(0), enginePower_(0) {
    time(&createdAt_);
    updatedAt_ = createdAt_;
    lastModerationDate_ = 0;
}

double Listing::getPriceInUSD() const {
    if (currency_ == "USD") return price_;
    
    // Спочатку конвертуємо в UAH, потім в USD
    double priceInUAH = getPriceInUAH();
    
    // Отримуємо актуальний курс USD/UAH
    CurrencyService* currencyService = CurrencyService::getInstance();
    currencyService->updateRates();
    auto rates = currencyService->getCurrentRates();
    
    // UAH -> USD
    return priceInUAH / rates.usdToUah;
}

double Listing::getPriceInEUR() const {
    if (currency_ == "EUR") return price_;
    
    // Спочатку конвертуємо в UAH, потім в EUR
    double priceInUAH = getPriceInUAH();
    
    // Використовуємо збережений курс EUR/UAH, якщо він є
    // Інакше використовуємо актуальний курс
    CurrencyService* currencyService = CurrencyService::getInstance();
    currencyService->updateRates();
    auto rates = currencyService->getCurrentRates();
    
    // Якщо валюта була EUR, exchangeRate_ містить EUR/UAH
    // Якщо валюта була USD, exchangeRate_ містить USD/UAH, потрібно конвертувати через UAH
    double eurToUahRate = (currency_ == "EUR" && exchangeRate_ > 0) ? 
                          exchangeRate_ : rates.eurToUah;
    
    // UAH -> EUR
    return priceInUAH / eurToUahRate;
}

double Listing::getPriceInUAH() const {
    if (currency_ == "UAH") return price_;
    
    // Завжди використовуємо збережений exchangeRate_, якщо він встановлений
    // exchangeRate_ зберігає курс основної валюти до UAH на момент створення оголошення
    // Перевіряємо, чи exchangeRate_ встановлено (більше 0) та чи валюта не UAH
    if (exchangeRate_ > 0 && currency_ != "UAH") {
        return price_ * exchangeRate_;
    }
    
    // Якщо exchangeRate_ не встановлено, використовуємо актуальний курс
    CurrencyService* currencyService = CurrencyService::getInstance();
    currencyService->updateRates();
    auto rates = currencyService->getCurrentRates();
    
    if (currency_ == "USD") {
        return price_ * rates.usdToUah;
    }
    
    if (currency_ == "EUR") {
        return price_ * rates.eurToUah;
    }
    
    return price_;
}

std::string Listing::toJson() const {
    std::ostringstream oss;
    // Додаємо photos до JSON
    oss << std::fixed;
    oss.precision(2);
    oss << "{\"id\":" << id_
        << ",\"sellerId\":" << sellerId_
        << ",\"brandId\":" << brandId_
        << ",\"modelId\":" << modelId_
        << ",\"year\":" << year_
        << ",\"price\":" << price_
        << ",\"currency\":\"" << escapeJson(currency_) << "\""
        << ",\"exchangeRate\":" << exchangeRate_
        << ",\"priceUSD\":" << std::fixed << std::setprecision(2) << getPriceInUSD()
        << ",\"priceEUR\":" << std::fixed << std::setprecision(2) << getPriceInEUR()
        << ",\"priceUAH\":" << std::fixed << std::setprecision(2) << getPriceInUAH()
        << ",\"debug_price\":" << price_
        << ",\"debug_currency\":\"" << currency_ << "\""
        << ",\"debug_exchangeRate\":" << exchangeRate_
        << ",\"debug_calc\":" << (exchangeRate_ > 0 ? price_ * exchangeRate_ : 0.0)
        << ",\"description\":\"" << escapeJson(description_) << "\""
        << ",\"region\":\"" << escapeJson(region_) << "\""
        << ",\"mileage\":" << mileage_
        << ",\"status\":\"" << escapeJson(status_) << "\""
        << ",\"editCount\":" << editCount_
        << ",\"viewCount\":" << viewCount_;
    
    // Обробка photos - перевіряємо, чи це валідний JSON
    if (photos_.empty() || photos_ == "[]") {
        oss << ",\"photos\":[]";
    } else {
        // Перевіряємо, чи photos_ є валідним JSON масивом
        bool isValidJsonArray = false;
        if (photos_.length() >= 2 && photos_[0] == '[' && photos_[photos_.length() - 1] == ']') {
            // Спроба перевірити, чи це валідний JSON (проста перевірка)
            isValidJsonArray = true;
            // Перевіряємо, чи немає невалідних символів
            for (size_t i = 1; i < photos_.length() - 1; ++i) {
                unsigned char c = static_cast<unsigned char>(photos_[i]);
                if (c < 0x20 && c != '\n' && c != '\r' && c != '\t') {
                    isValidJsonArray = false;
                    break;
                }
            }
        }
        
        if (isValidJsonArray) {
            oss << ",\"photos\":" << photos_;
        } else {
            // Якщо невалідний JSON, повертаємо порожній масив
            oss << ",\"photos\":[]";
        }
    }
    
    // Додаткові характеристики
    if (!fuelType_.empty()) oss << ",\"fuelType\":\"" << escapeJson(fuelType_) << "\"";
    if (!transmission_.empty()) oss << ",\"transmission\":\"" << escapeJson(transmission_) << "\"";
    if (!color_.empty()) oss << ",\"color\":\"" << escapeJson(color_) << "\"";
    if (engineVolume_ > 0) oss << ",\"engineVolume\":" << engineVolume_;
    if (!bodyType_.empty()) oss << ",\"bodyType\":\"" << escapeJson(bodyType_) << "\"";
    if (doorsCount_ > 0) oss << ",\"doorsCount\":" << doorsCount_;
    if (enginePower_ > 0) oss << ",\"enginePower\":" << enginePower_;
    
    oss << "}";
    return oss.str();
}

std::string Listing::toJsonWithStats() const {
    std::ostringstream oss;
    oss << toJson();
    // Видаляємо закриваючу дужку і додаємо статистику
    std::string base = oss.str();
    base.pop_back(); // Видаляємо }
    base += ",\"statistics\":{"
           "\"totalViews\":" + std::to_string(viewCount_) +
           ",\"viewsPerDay\":0" // Буде розраховано в сервісі
           ",\"viewsPerWeek\":0"
           ",\"viewsPerMonth\":0"
           ",\"averagePriceByRegion\":0"
           ",\"averagePriceByUkraine\":0"
           "}}";
    return base;
}

