#ifndef SERVICES_H
#define SERVICES_H

#include "repositories.h"
#include <memory>
#include <ctime>

// --- Strategy Pattern for Bonus Calculation ---

class IBonusStrategy {
public:
    virtual ~IBonusStrategy() = default;
    virtual double calculate(double weight, double pricePerKg) = 0;
};

class StandardBonusStrategy : public IBonusStrategy {
public:
    double calculate(double weight, double pricePerKg) override {
        return weight * pricePerKg; // Standard calculation
    }
};

class EcoHeroBonusStrategy : public IBonusStrategy {
public:
    double calculate(double weight, double pricePerKg) override {
        return weight * pricePerKg * 1.5; // 50% extra bonus for Heroes
    }
};

// --- Services (Business Logic Layer) ---

class AuthService {
    UserRepository userRepo;
public:
    std::string hashPassword(const std::string& password) {
        // "Salting" simulation
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
    // Factory method for strategy (simplified)
    std::unique_ptr<IBonusStrategy> getStrategy(int userLevel) {
        if (userLevel >= 5) {
            return std::make_unique<EcoHeroBonusStrategy>();
        }
        return std::make_unique<StandardBonusStrategy>();
    }

    double processRecycling(const RecycleDto& dto) {
        // 1. Get Waste Price
        double price = wasteRepo.getPricePerKg(dto.waste_id);
        
        // 2. Get User Level (Mocked to 1 for now, or fetch from repo)
        int userLevel = 1; 
        
        // 3. Select Strategy
        auto strategy = getStrategy(userLevel);
        
        // 4. Calculate Bonus
        double bonus = strategy->calculate(dto.weight, price);
        
        // 5. Create Transaction
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

#endif // SERVICES_H
