// FILE: backend/src/services/CurrencyService.h
#pragma once
#include <string>
#include <ctime>

// Структура для курсу валют
struct ExchangeRate {
    double usdToUah;
    double eurToUah;
    double eurToUsd;
    time_t lastUpdate;
};

// Клас CurrencyService - інкапсуляція роботи з валютами
class CurrencyService {
private:
    ExchangeRate currentRate_;
    static CurrencyService* instance_; // Singleton pattern

    CurrencyService();

public:
    static CurrencyService* getInstance();
    
    // Оновлення курсів з ПриватБанку (мокована версія)
    bool updateRates();
    
    // Отримання поточних курсів
    ExchangeRate getCurrentRates() const { return currentRate_; }
    
    // Конвертація між валютами
    double convert(double amount, const std::string& from, const std::string& to) const;
    
private:
    // Мокована функція для отримання курсів (в реальності буде HTTP запит до ПриватБанку)
    ExchangeRate fetchRatesFromPrivatBank();
};



