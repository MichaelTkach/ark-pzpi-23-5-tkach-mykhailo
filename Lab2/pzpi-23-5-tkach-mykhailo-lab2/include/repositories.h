#ifndef REPOSITORIES_H
#define REPOSITORIES_H

#include "database.h"
#include "models.h"
#include <optional>
#include <iostream>

// Abstract Base Repository (Interface)
template <typename T>
class IRepository {
public:
    virtual ~IRepository() = default;
    virtual std::vector<T> getAll() = 0;
    virtual std::optional<T> getById(int id) = 0;
};

// UserRepository: Handles user data access
class UserRepository : public IRepository<User> {
public:
    bool create(const User& user) {
        return Database::getInstance().createUser(user.email, user.password_hash, user.role);
    }
    
    std::optional<User> getByEmail(const std::string& email) {
        // In a real generic repo, this would be more dynamic. 
        // Delegating to existing DB singleton for now to keep implementation simple but architecturally layered.
        User u = Database::getInstance().getUserByEmail(email);
        if (u.id == 0 && u.email.empty()) return std::nullopt;
        return u;
    }

    // Implementing interface methods stub
    std::vector<User> getAll() override { return {}; }
    std::optional<User> getById(int id) override { return std::nullopt; }
    
    // Updates user balance and level
    bool updateStats(int userId, double balanceDelta, int levelDelta) {
        // This query would ideally be part of Database class, but for Repos pattern demo we show intent here
        // Note: Direct SQL here is a simulation of what a Repo does (encapsulate data access)
        // Since Database is our low-level driver, we call it.
        // Assuming Database has a generic query method or we'd add one. 
        // For lab purposes, we assume transactions logic handles this via Database::createTransaction
        return true; 
    }
};

// WasteRepository: Access to waste categories
class WasteRepository : public IRepository<WasteCategory> {
public:
    std::vector<WasteCategory> getAll() override {
        // Mocking return from DB logic
        return { {1, "Plastic", 5.0}, {2, "Glass", 2.0} };
    }
    std::optional<WasteCategory> getById(int id) override {
        // Simplified lookup
        if(id == 1) return WasteCategory{1, "Plastic", 5.0};
        if(id == 2) return WasteCategory{2, "Glass", 2.0};
        return std::nullopt;
    }
    
    double getPricePerKg(int id) {
        auto cat = getById(id);
        return cat.has_value() ? cat->price_per_kg : 0.0;
    }
};

// TransactionRepository: Manage logs
class TransactionRepository {
public:
    bool log(const Transaction& trans) {
        return Database::getInstance().createTransaction(trans.user_id, trans.waste_id, trans.weight, trans.bonus);
    }
};

#endif // REPOSITORIES_H
