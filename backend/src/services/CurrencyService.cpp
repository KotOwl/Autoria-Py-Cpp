// FILE: backend/src/services/CurrencyService.cpp
#include "CurrencyService.h"
#include <ctime>
#include <cmath>

CurrencyService* CurrencyService::instance_ = nullptr;

CurrencyService::CurrencyService() {
    updateRates();
}

CurrencyService* CurrencyService::getInstance() {
    if (instance_ == nullptr) {
        instance_ = new CurrencyService();
    }
    return instance_;
}

ExchangeRate CurrencyService::fetchRatesFromPrivatBank() {
    // Моковані курси (в реальності тут буде HTTP запит до API ПриватБанку)
    ExchangeRate rate;
    rate.usdToUah = 37.5; // Приклад курсу
    rate.eurToUah = 40.2;
    rate.eurToUsd = rate.eurToUah / rate.usdToUah;
    time(&rate.lastUpdate);
    return rate;
}

bool CurrencyService::updateRates() {
    // Оновлюємо курси раз на день
    time_t now;
    time(&now);
    
    if (currentRate_.lastUpdate == 0 || 
        (now - currentRate_.lastUpdate) > 86400) { // 24 години
        currentRate_ = fetchRatesFromPrivatBank();
        return true;
    }
    return false;
}

double CurrencyService::convert(double amount, const std::string& from, const std::string& to) const {
    if (from == to) return amount;
    
    // Конвертуємо через UAH як базову валюту
    double amountInUah;
    
    if (from == "UAH") {
        amountInUah = amount;
    } else if (from == "USD") {
        amountInUah = amount * currentRate_.usdToUah;
    } else if (from == "EUR") {
        amountInUah = amount * currentRate_.eurToUah;
    } else {
        return amount; // Невідома валюта
    }
    
    if (to == "UAH") {
        return amountInUah;
    } else if (to == "USD") {
        return amountInUah / currentRate_.usdToUah;
    } else if (to == "EUR") {
        return amountInUah / currentRate_.eurToUah;
    }
    
    return amount;
}



