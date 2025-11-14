// FILE: backend/src/middleware/AuthMiddleware.cpp
#include "AuthMiddleware.h"
#include "../models/Role.h"
#include <iostream>

AuthMiddleware::AuthMiddleware(std::shared_ptr<UserRepository> userRepository)
    : userRepository_(userRepository) {
    initializeRoles();
}

void AuthMiddleware::initializeRoles() {
    roles_["buyer"] = RoleFactory::createBuyerRole();
    roles_["seller"] = RoleFactory::createSellerRole();
    roles_["manager"] = RoleFactory::createManagerRole();
    roles_["administrator"] = RoleFactory::createAdministratorRole();
}

std::unique_ptr<User> AuthMiddleware::authenticate(const std::string& token) {
    // Мокована авторизація (в реальності буде JWT або інший механізм)
    // Токен має формат "user_id:email"
    size_t pos = token.find(':');
    if (pos == std::string::npos) {
        return nullptr;
    }
    
    int userId = std::stoi(token.substr(0, pos));
    return userRepository_->findById(userId);
}

bool AuthMiddleware::hasPermission(User* user, const std::string& resource, const std::string& action) {
    if (!user) return false;
    
    auto role = getRoleForUser(user);
    if (!role) return false;
    
    return role->hasPermission(resource, action);
}

bool AuthMiddleware::hasRole(User* user, const std::string& roleName) {
    if (!user) return false;
    
    auto userRoles = user->getRoles();
    for (const auto& role : userRoles) {
        if (role == roleName) {
            return true;
        }
    }
    return false;
}

std::shared_ptr<Role> AuthMiddleware::getRoleForUser(User* user) {
    auto userRoles = user->getRoles();
    if (userRoles.empty()) return nullptr;
    
    std::string primaryRole = userRoles[0];
    if (roles_.find(primaryRole) != roles_.end()) {
        return roles_[primaryRole];
    }
    return nullptr;
}



