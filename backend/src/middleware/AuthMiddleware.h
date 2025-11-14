// FILE: backend/src/middleware/AuthMiddleware.h
#pragma once
#include "../models/User.h"
#include "../models/Role.h"
#include "../repositories/UserRepository.h"
#include <string>
#include <memory>
#include <map>

// Клас AuthMiddleware - інкапсуляція авторизації та перевірки пермішнів
class AuthMiddleware {
private:
    std::shared_ptr<UserRepository> userRepository_;
    std::map<std::string, std::shared_ptr<Role>> roles_; // Кеш ролей

public:
    AuthMiddleware(std::shared_ptr<UserRepository> userRepository);
    
    // Перевірка токену (мокована версія)
    std::unique_ptr<User> authenticate(const std::string& token);
    
    // Перевірка пермішнів
    bool hasPermission(User* user, const std::string& resource, const std::string& action);
    
    // Перевірка ролі
    bool hasRole(User* user, const std::string& roleName);
    
private:
    void initializeRoles();
    std::shared_ptr<Role> getRoleForUser(User* user);
};



