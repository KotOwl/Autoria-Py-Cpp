// FILE: backend/src/models/User.h
#pragma once
#include <string>
#include <vector>
#include <memory>
#include <ctime>

// Базовий клас User - інкапсуляція користувача
class User {
protected:
    int id_;
    std::string email_;
    std::string passwordHash_;
    std::string firstName_;
    std::string lastName_;
    std::string phone_;
    std::string accountType_; // "basic" або "premium"
    bool isActive_;
    time_t createdAt_;
    time_t updatedAt_;

public:
    User(int id, const std::string& email, const std::string& passwordHash,
         const std::string& firstName, const std::string& lastName,
         const std::string& phone, const std::string& accountType = "basic");
    virtual ~User() = default;

    // Геттери
    int getId() const { return id_; }
    std::string getEmail() const { return email_; }
    std::string getAccountType() const { return accountType_; }
    std::string getFirstName() const { return firstName_; }
    std::string getLastName() const { return lastName_; }
    bool isPremium() const { return accountType_ == "premium"; }
    bool isActive() const { return isActive_; }

    // Сеттери
    void setAccountType(const std::string& type) { accountType_ = type; }
    void setActive(bool active) { isActive_ = active; }

    // Віртуальний метод для отримання ролей (поліморфізм)
    virtual std::vector<std::string> getRoles() const = 0;
    
    // Серіалізація
    virtual std::string toJson() const;
};

// Клас Seller - наслідування від User
class Seller : public User {
private:
    std::vector<int> listingIds_;

public:
    Seller(int id, const std::string& email, const std::string& passwordHash,
           const std::string& firstName, const std::string& lastName,
           const std::string& phone, const std::string& accountType = "basic");
    
    std::vector<std::string> getRoles() const override { return {"seller"}; }
    
    bool canCreateListing() const;
    void addListing(int listingId);
    int getListingCount() const { return listingIds_.size(); }
};

// Клас Buyer - наслідування від User
class Buyer : public User {
public:
    Buyer(int id, const std::string& email, const std::string& passwordHash,
          const std::string& firstName, const std::string& lastName,
          const std::string& phone);
    
    std::vector<std::string> getRoles() const override { return {"buyer"}; }
};

// Клас Manager - наслідування від User
class Manager : public User {
private:
    int createdByAdminId_;

public:
    Manager(int id, const std::string& email, const std::string& passwordHash,
            const std::string& firstName, const std::string& lastName,
            const std::string& phone, int createdByAdminId);
    
    std::vector<std::string> getRoles() const override { return {"manager"}; }
    int getCreatedByAdminId() const { return createdByAdminId_; }
};

// Клас Administrator - наслідування від User
class Administrator : public User {
public:
    Administrator(int id, const std::string& email, const std::string& passwordHash,
                  const std::string& firstName, const std::string& lastName,
                  const std::string& phone);
    
    std::vector<std::string> getRoles() const override { return {"administrator"}; }
};



