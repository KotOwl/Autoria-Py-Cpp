// FILE: backend/src/models/Brand.h
#pragma once
#include <string>
#include <vector>

// Клас Brand - інкапсуляція марки автомобіля
class Brand {
private:
    int id_;
    std::string name_;
    bool isActive_;

public:
    Brand(int id, const std::string& name, bool isActive = true);
    
    int getId() const { return id_; }
    std::string getName() const { return name_; }
    bool isActive() const { return isActive_; }
    
    void setActive(bool active) { isActive_ = active; }
    
    std::string toJson() const;
};

// Клас Model - інкапсуляція моделі автомобіля
class Model {
private:
    int id_;
    int brandId_;
    std::string name_;
    bool isActive_;

public:
    Model(int id, int brandId, const std::string& name, bool isActive = true);
    
    int getId() const { return id_; }
    int getBrandId() const { return brandId_; }
    std::string getName() const { return name_; }
    bool isActive() const { return isActive_; }
    
    void setActive(bool active) { isActive_ = active; }
    
    std::string toJson() const;
};



