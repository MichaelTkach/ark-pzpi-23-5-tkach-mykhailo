#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <string>
#include <vector>
#include "models.h"

class Database {
public:
    static Database& getInstance();
    
    // Delete copy constructors
    Database(const Database&) = delete;
    void operator=(const Database&) = delete;

    bool init(const std::string& dbPath);
    
    // User methods
    bool createUser(const std::string& email, const std::string& password_hash, const std::string& role);
    User getUserByEmail(const std::string& email);
    
    // Recycling Point methods
    std::vector<RecyclingPoint> getAllPoints();
    
    // Transaction methods
    bool createTransaction(int userId, int wasteId, double weight, double bonus);

private:
    Database() : db(nullptr) {}
    ~Database();
    
    sqlite3* db;
};

#endif // DATABASE_H
