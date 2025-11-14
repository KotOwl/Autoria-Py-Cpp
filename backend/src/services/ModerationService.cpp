// FILE: backend/src/services/ModerationService.cpp
#include "ModerationService.h"
#include <algorithm>
#include <cctype>

ModerationService::ModerationService() {
    initializeBadWords();
}

void ModerationService::initializeBadWords() {
    // Базовий список нецензурних слів (для демонстрації)
    badWords_ = {
        "мат1", "мат2", "мат3" // В реальному проєкті тут буде повний список
    };
}

std::string ModerationService::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

bool ModerationService::checkForBadWords(const std::string& text, std::vector<std::string>& foundWords) {
    std::string lowerText = toLower(text);
    foundWords.clear();
    
    for (const auto& word : badWords_) {
        if (lowerText.find(toLower(word)) != std::string::npos) {
            foundWords.push_back(word);
        }
    }
    
    return !foundWords.empty();
}

std::string ModerationService::moderateListing(const std::string& description) {
    std::vector<std::string> foundWords;
    
    if (checkForBadWords(description, foundWords)) {
        return "rejected"; // Знайдено нецензурну лексику
    }
    
    return "active"; // Оголошення пройшло перевірку
}



