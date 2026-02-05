#ifndef SERVICES_H
#define SERVICES_H

#include "repositories.h"
#include <memory>
#include <sstream>
#include <iomanip>

// --- Strategies ---
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

// --- Services ---

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
public:
    std::unique_ptr<IBonusStrategy> getStrategy(int userLevel) {
        if (userLevel >= 5) return std::make_unique<EcoHeroBonusStrategy>();
        return std::make_unique<StandardBonusStrategy>();
    }

    double processRecycling(const RecycleDto& dto) {
        // Lab 3: Add logic - check if user is active (omitted for brevity, assume middleware checks or simple logic)
        double price = wasteRepo.getPricePerKg(dto.waste_id);
        int userLevel = 1; 
        auto strategy = getStrategy(userLevel);
        double bonus = strategy->calculate(dto.weight, price);
        
        Transaction t;
        t.user_id = dto.user_id;
        t.waste_id = dto.waste_id;
        t.weight = dto.weight;
        t.bonus = bonus;
        
        if (transRepo.log(t)) return bonus;
        return -1.0;
    }
};

// Lab 3: Admin & Stats Services

class AdminService {
    UserRepository userRepo;
    WasteRepository wasteRepo;
    TransactionRepository transRepo;
public:
    bool blockUser(int userId) {
        return userRepo.block(userId);
    }
    
    bool addWasteType(const AddWasteTypeDto& dto) {
        WasteCategory wc;
        wc.name = dto.name;
        wc.price_per_kg = dto.price;
        return wasteRepo.add(wc);
    }
    
    // Generate CSV string
    std::string exportTransactionsToCsv() {
        auto transactions = transRepo.getAll();
        std::stringstream ss;
        ss << "ID,UserID,WasteID,Weight,Bonus,Timestamp\n";
        for (const auto& t : transactions) {
            ss << t.id << "," << t.user_id << "," << t.waste_id << "," 
               << t.weight << "," << t.bonus << "," 
               << (t.timestamp.empty() ? "N/A" : t.timestamp) << "\n";
        }
        return ss.str();
    }
};

class StatsService {
public:
    GlobalStatsDto getGlobalStats() {
        GlobalStatsDto stats;
        stats.total_weight_all_time = Database::getInstance().getTotalWeight();
        stats.top_users = Database::getInstance().getTopUsers(3);
        stats.average_point_load = 0.75; // Emulated math
        return stats;
    }
};

#endif // SERVICES_H
