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
    bool blockUser(int userId, bool block); // Lab 3
    
    // Waste & Point methods
    bool addWasteType(const std::string& name, double price); // Lab 3
    std::vector<RecyclingPoint> getAllPoints();
    std::vector<WasteCategory> getAllWasteTypes();
    
    // Transaction methods
    bool createTransaction(int userId, int wasteId, double weight, double bonus);
    std::vector<Transaction> getAllTransactions(); // Lab 3 Export
    
    // Stats methods
    double getTotalWeight();
    double getTotalBonuses();
    int getTransactionCount();

private:
    Database() : db(nullptr) {}
    ~Database();
    
    sqlite3* db;
};

#endif // DATABASE_H
