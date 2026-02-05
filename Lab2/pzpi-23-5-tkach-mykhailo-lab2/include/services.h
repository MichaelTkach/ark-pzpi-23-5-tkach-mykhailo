#ifndef SERVICES_H
#define SERVICES_H

#include "repositories.h"
#include <memory>
#include <ctime>

class IBonusStrategy {
public:
    virtual ~IBonusStrategy() = default;
    virtual double calculate(double weight, double pricePerKg) = 0;
};

class StandardBonusStrategy : public IBonusStrategy {
public:
    double calculate(double weight, double pricePerKg) override {
        return weight * pricePerKg;
    }
};

class EcoHeroBonusStrategy : public IBonusStrategy {
public:
    double calculate(double weight, double pricePerKg) override {
        return weight * pricePerKg * 1.5;
    }
};

class AuthService {
    UserRepository userRepo;
public:
    std::string hashPassword(const std::string& password) {
        return password + "_salt_v1";
    }

    bool registerUser(const RegisterDto& dto) {
        User u;
        u.email = dto.email;
        u.password_hash = hashPassword(dto.password);
        u.role = "user";
        return userRepo.create(u);
    }
};

class RecyclingService {
    WasteRepository wasteRepo;
    TransactionRepository transRepo;
    UserRepository userRepo;
    
public:
    std::unique_ptr<IBonusStrategy> getStrategy(int userLevel) {
        if (userLevel >= 5) {
            return std::make_unique<EcoHeroBonusStrategy>();
        }
        return std::make_unique<StandardBonusStrategy>();
    }

    double processRecycling(const RecycleDto& dto) {
        double price = wasteRepo.getPricePerKg(dto.waste_id);
        
        int userLevel = 1; 
        
        auto strategy = getStrategy(userLevel);
        
        double bonus = strategy->calculate(dto.weight, price);
        
        Transaction t;
        t.user_id = dto.user_id;
        t.waste_id = dto.waste_id;
        t.weight = dto.weight;
        t.bonus = bonus;
        
        if (transRepo.log(t)) {
            return bonus;
        }
        return -1.0;
    }
};

#endif
