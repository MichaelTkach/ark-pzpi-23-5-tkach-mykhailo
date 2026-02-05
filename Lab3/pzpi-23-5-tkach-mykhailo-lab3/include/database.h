#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <string>
#include <vector>
#include "models.h"

class Database {
public:
    static Database& getInstance();
    
    Database(const Database&) = delete;
    void operator=(const Database&) = delete;

    bool init(const std::string& dbPath);
    
    // User methods
    bool createUser(const std::string& email, const std::string& password_hash, const std::string& role);
    User getUserByEmail(const std::string& email);
    
    // Lab 3: User Management
    bool blockUser(int userId);
    bool isUserActive(int userId);
    
    // Recycling Point methods
    std::vector<RecyclingPoint> getAllPoints();
    
    // Transaction methods
    bool createTransaction(int userId, int wasteId, double weight, double bonus);
    std::vector<Transaction> getAllTransactions(); // For CSV export
    
    // Lab 3: Waste Management
    bool addWasteType(const std::string& name, double price);
    
    // Lab 3: Stats Aggregation
    double getTotalWeight();
    std::vector<UserStatsDto> getTopUsers(int limit);

private:
    Database() : db(nullptr) {}
    ~Database();
    
    sqlite3* db;
};

#endif // DATABASE_H
