#ifndef REPOSITORIES_H
#define REPOSITORIES_H

#include "database.h"
#include "models.h"
#include <optional>
#include <vector>

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
    
    bool block(int userId) {
        return Database::getInstance().blockUser(userId, true);
    }
    
    std::vector<User> getAll() override { return {}; }
    std::optional<User> getById(int id) override { return std::nullopt; }
};

class WasteRepository : public IRepository<WasteCategory> {
public:
    bool add(const WasteCategory& waste) {
        return Database::getInstance().addWasteType(waste.name, waste.price_per_kg);
    }

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
    
    std::vector<Transaction> getAll() {
        return Database::getInstance().getAllTransactions();
    }
};

#endif
