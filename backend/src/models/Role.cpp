// FILE: backend/src/models/Role.cpp
#include "Role.h"
#include <sstream>

Permission::Permission(const std::string& name, const std::string& resource, const std::string& action)
    : name_(name), resource_(resource), action_(action) {}

std::string Permission::toJson() const {
    std::ostringstream oss;
    oss << "{\"name\":\"" << name_ << "\""
        << ",\"resource\":\"" << resource_ << "\""
        << ",\"action\":\"" << action_ << "\"}";
    return oss.str();
}

bool Permission::operator==(const Permission& other) const {
    return name_ == other.name_ && resource_ == other.resource_ && action_ == other.action_;
}

Role::Role(const std::string& name, const std::string& description)
    : name_(name), description_(description) {}

void Role::addPermission(std::shared_ptr<Permission> permission) {
    permissions_.push_back(permission);
}

bool Role::hasPermission(const std::string& resource, const std::string& action) const {
    for (const auto& perm : permissions_) {
        if (perm->getResource() == resource && perm->getAction() == action) {
            return true;
        }
    }
    return false;
}

std::string Role::toJson() const {
    std::ostringstream oss;
    oss << "{\"name\":\"" << name_ << "\""
        << ",\"description\":\"" << description_ << "\""
        << ",\"permissions\":[";
    for (size_t i = 0; i < permissions_.size(); ++i) {
        if (i > 0) oss << ",";
        oss << permissions_[i]->toJson();
    }
    oss << "]}";
    return oss.str();
}

// Фабрика ролей
std::shared_ptr<Role> RoleFactory::createBuyerRole() {
    auto role = std::make_shared<Role>("buyer", "Покупець - може переглядати оголошення");
    role->addPermission(std::make_shared<Permission>("view_listings", "listings", "read"));
    role->addPermission(std::make_shared<Permission>("contact_seller", "listings", "contact"));
    return role;
}

std::shared_ptr<Role> RoleFactory::createSellerRole() {
    auto role = std::make_shared<Role>("seller", "Продавець - може створювати оголошення");
    role->addPermission(std::make_shared<Permission>("create_listing", "listings", "create"));
    role->addPermission(std::make_shared<Permission>("edit_own_listing", "listings", "update"));
    role->addPermission(std::make_shared<Permission>("view_own_listing", "listings", "read"));
    role->addPermission(std::make_shared<Permission>("delete_own_listing", "listings", "delete"));
    return role;
}

std::shared_ptr<Role> RoleFactory::createManagerRole() {
    auto role = std::make_shared<Role>("manager", "Менеджер - модерація та управління");
    // Наслідуємо всі права продавця
    auto sellerRole = createSellerRole();
    for (const auto& perm : sellerRole->getPermissions()) {
        role->addPermission(perm);
    }
    // Додаткові права менеджера
    role->addPermission(std::make_shared<Permission>("moderate_listings", "listings", "moderate"));
    role->addPermission(std::make_shared<Permission>("ban_users", "users", "ban"));
    role->addPermission(std::make_shared<Permission>("view_all_listings", "listings", "read_all"));
    role->addPermission(std::make_shared<Permission>("delete_any_listing", "listings", "delete_any"));
    return role;
}

std::shared_ptr<Role> RoleFactory::createAdministratorRole() {
    auto role = std::make_shared<Role>("administrator", "Адміністратор - повний доступ");
    // Наслідуємо всі права менеджера
    auto managerRole = createManagerRole();
    for (const auto& perm : managerRole->getPermissions()) {
        role->addPermission(perm);
    }
    // Додаткові права адміністратора
    role->addPermission(std::make_shared<Permission>("create_manager", "users", "create_manager"));
    role->addPermission(std::make_shared<Permission>("manage_brands", "brands", "manage"));
    role->addPermission(std::make_shared<Permission>("manage_models", "models", "manage"));
    role->addPermission(std::make_shared<Permission>("manage_all_users", "users", "manage_all"));
    role->addPermission(std::make_shared<Permission>("view_statistics", "system", "statistics"));
    return role;
}



