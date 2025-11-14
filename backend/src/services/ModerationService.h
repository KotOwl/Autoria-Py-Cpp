// FILE: backend/src/services/ModerationService.h
#pragma once
#include <string>
#include <vector>

// Клас ModerationService - інкапсуляція логіки модерації
class ModerationService {
private:
    std::vector<std::string> badWords_; // Список нецензурних слів

public:
    ModerationService();
    
    // Перевірка тексту на нецензурну лексику
    bool checkForBadWords(const std::string& text, std::vector<std::string>& foundWords);
    
    // Модерація оголошення
    std::string moderateListing(const std::string& description);
    
private:
    void initializeBadWords();
    std::string toLower(const std::string& str);
};



