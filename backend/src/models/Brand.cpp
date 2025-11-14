// FILE: backend/src/models/Brand.cpp
#include "Brand.h"
#include <sstream>
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

Brand::Brand(int id, const std::string& name, bool isActive)
    : id_(id), name_(name), isActive_(isActive) {}

std::string Brand::toJson() const {
    std::ostringstream oss;
    oss << "{\"id\":" << id_
        << ",\"name\":\"" << name_ << "\""
        << ",\"isActive\":" << (isActive_ ? "true" : "false")
        << "}";
    return oss.str();
}

Model::Model(int id, int brandId, const std::string& name, bool isActive)
    : id_(id), brandId_(brandId), name_(name), isActive_(isActive) {}

std::string Model::toJson() const {
    std::ostringstream oss;
    oss << "{\"id\":" << id_
        << ",\"brandId\":" << brandId_
        << ",\"name\":\"" << name_ << "\""
        << ",\"isActive\":" << (isActive_ ? "true" : "false")
        << "}";
    return oss.str();
}

