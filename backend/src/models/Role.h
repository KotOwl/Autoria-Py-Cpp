// FILE: backend/src/models/Role.h
#pragma once
#include <string>
#include <vector>
#include <memory>

// Клас Permission - інкапсуляція дозволу
class Permission {
private:
    std::string name_;
    std::string resource_;
    std::string action_;

public:
    Permission(const std::string& name, const std::string& resource, const std::string& action);
    
    std::string getName() const { return name_; }
    std::string getResource() const { return resource_; }
    std::string getAction() const { return action_; }
    std::string toJson() const;
    
    bool operator==(const Permission& other) const;
};

// Клас Role - інкапсуляція ролі з пермішнами
class Role {
private:
    std::string name_;
    std::string description_;
    std::vector<std::shared_ptr<Permission>> permissions_;

public:
    Role(const std::string& name, const std::string& description);
    
    void addPermission(std::shared_ptr<Permission> permission);
    bool hasPermission(const std::string& resource, const std::string& action) const;
    std::vector<std::shared_ptr<Permission>> getPermissions() const { return permissions_; }
    std::string getName() const { return name_; }
    
    std::string toJson() const;
};

// Фабрика ролей - створює стандартні ролі з пермішнами
class RoleFactory {
public:
    static std::shared_ptr<Role> createBuyerRole();
    static std::shared_ptr<Role> createSellerRole();
    static std::shared_ptr<Role> createManagerRole();
    static std::shared_ptr<Role> createAdministratorRole();
};



