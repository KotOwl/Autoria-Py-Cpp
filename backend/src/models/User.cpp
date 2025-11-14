// FILE: backend/src/models/User.cpp
#include "User.h"
#include <sstream>
#include <ctime>
#include <iomanip>

// Функція для екранування JSON рядків (повторюється з Listing.cpp, можна винести в окремий файл)
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

User::User(int id, const std::string& email, const std::string& passwordHash,
           const std::string& firstName, const std::string& lastName,
           const std::string& phone, const std::string& accountType)
    : id_(id), email_(email), passwordHash_(passwordHash),
      firstName_(firstName), lastName_(lastName), phone_(phone),
      accountType_(accountType), isActive_(true) {
    time(&createdAt_);
    updatedAt_ = createdAt_;
}

std::string User::toJson() const {
    std::ostringstream oss;
    oss << "{\"id\":" << id_
        << ",\"email\":\"" << escapeJson(email_) << "\""
        << ",\"firstName\":\"" << escapeJson(firstName_) << "\""
        << ",\"lastName\":\"" << escapeJson(lastName_) << "\""
        << ",\"phone\":\"" << escapeJson(phone_) << "\""
        << ",\"accountType\":\"" << escapeJson(accountType_) << "\""
        << ",\"isActive\":" << (isActive_ ? "true" : "false")
        << "}";
    return oss.str();
}

Seller::Seller(int id, const std::string& email, const std::string& passwordHash,
               const std::string& firstName, const std::string& lastName,
               const std::string& phone, const std::string& accountType)
    : User(id, email, passwordHash, firstName, lastName, phone, accountType) {}

bool Seller::canCreateListing() const {
    if (isPremium()) return true;
    return listingIds_.size() < 1; // Базовий акаунт - лише 1 оголошення
}

void Seller::addListing(int listingId) {
    listingIds_.push_back(listingId);
}

Buyer::Buyer(int id, const std::string& email, const std::string& passwordHash,
             const std::string& firstName, const std::string& lastName,
             const std::string& phone)
    : User(id, email, passwordHash, firstName, lastName, phone, "basic") {}

Manager::Manager(int id, const std::string& email, const std::string& passwordHash,
                 const std::string& firstName, const std::string& lastName,
                 const std::string& phone, int createdByAdminId)
    : User(id, email, passwordHash, firstName, lastName, phone, "basic"),
      createdByAdminId_(createdByAdminId) {}

Administrator::Administrator(int id, const std::string& email, const std::string& passwordHash,
                             const std::string& firstName, const std::string& lastName,
                             const std::string& phone)
    : User(id, email, passwordHash, firstName, lastName, phone, "basic") {}

