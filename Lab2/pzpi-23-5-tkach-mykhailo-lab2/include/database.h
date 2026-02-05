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
    
    bool createUser(const std::string& email, const std::string& password_hash, const std::string& role);
    User getUserByEmail(const std::string& email);
    
    std::vector<RecyclingPoint> getAllPoints();
    
    bool createTransaction(int userId, int wasteId, double weight, double bonus);

private:
    Database() : db(nullptr) {}
    ~Database();
    
    sqlite3* db;
};

#endif
