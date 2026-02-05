#ifndef REPOSITORIES_H
#define REPOSITORIES_H

#include "database.h"
#include "models.h"
#include <optional>
#include <iostream>

template <typename T>
class IRepository {
public:
    virtual ~IRepository() = default;
    virtual std::vector<T> getAll() = 0;
    virtual std::optional<T> getById(int id) = 0;
};

class UserRepository : public IRepository<User> {
public:
    bool create(const User& user) {
        return Database::getInstance().createUser(user.email, user.password_hash, user.role);
    }
    
    std::optional<User> getByEmail(const std::string& email) {
        User u = Database::getInstance().getUserByEmail(email);
        if (u.id == 0 && u.email.empty()) return std::nullopt;
        return u;
    }

    std::vector<User> getAll() override { return {}; }
    std::optional<User> getById(int id) override { return std::nullopt; }
    
    bool updateStats(int userId, double balanceDelta, int levelDelta) {
        return true; 
    }
};

class WasteRepository : public IRepository<WasteCategory> {
public:
    std::vector<WasteCategory> getAll() override {
        return { {1, "Plastic", 5.0}, {2, "Glass", 2.0} };
    }
    std::optional<WasteCategory> getById(int id) override {
        if(id == 1) return WasteCategory{1, "Plastic", 5.0};
        if(id == 2) return WasteCategory{2, "Glass", 2.0};
        return std::nullopt;
    }
    
    double getPricePerKg(int id) {
        auto cat = getById(id);
        return cat.has_value() ? cat->price_per_kg : 0.0;
    }
};

class TransactionRepository {
public:
    bool log(const Transaction& trans) {
        return Database::getInstance().createTransaction(trans.user_id, trans.waste_id, trans.weight, trans.bonus);
    }
};

#endif
