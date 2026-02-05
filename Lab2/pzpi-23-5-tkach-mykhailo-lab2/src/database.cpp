#include "database.h"
#include <iostream>

Database& Database::getInstance() {
    static Database instance;
    return instance;
}

Database::~Database() {
    if (db) {
        sqlite3_close(db);
    }
}

bool Database::init(const std::string& dbPath) {
    if (sqlite3_open(dbPath.c_str(), &db)) {
        std::cerr << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    const char* sql = "CREATE TABLE IF NOT EXISTS users ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "email TEXT NOT NULL UNIQUE, "
                      "password_hash TEXT NOT NULL, "
                      "role TEXT NOT NULL, "
                      "balance REAL DEFAULT 0.0);"
                      
                      "CREATE TABLE IF NOT EXISTS waste_categories ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "name TEXT NOT NULL, "
                      "price_per_kg REAL NOT NULL);"
                      
                      "CREATE TABLE IF NOT EXISTS recycling_points ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "name TEXT NOT NULL, "
                      "address TEXT NOT NULL, "
                      "coords TEXT NOT NULL);"
                      
                      "CREATE TABLE IF NOT EXISTS transactions ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "user_id INTEGER, "
                      "waste_id INTEGER, "
                      "weight REAL, "
                      "bonus REAL, "
                      "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP);"
                      
                      // Seed data for demo
                      "INSERT OR IGNORE INTO recycling_points (name, address, coords) "
                      "VALUES ('Central Point', 'Main St 1', '50.00, 36.23');"
                      "INSERT OR IGNORE INTO waste_categories (name, price_per_kg) "
                      "VALUES ('Plastic', 5.0), ('Glass', 2.0);";

    char* errMsg = 0;
    if (sqlite3_exec(db, sql, 0, 0, &errMsg) != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    
    return true;
}

bool Database::createUser(const std::string& email, const std::string& password_hash, const std::string& role) {
    std::string sql = "INSERT INTO users (email, password_hash, role) VALUES ('" + 
                      email + "', '" + password_hash + "', '" + role + "');";
    char* errMsg = 0;
    if (sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg) != SQLITE_OK) {
        std::cerr << "Register error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

User Database::getUserByEmail(const std::string& email) {
    // Simplified: in real app, use prepared statements to prevent injection
    User user = {0, "", "", "", 0.0};
    // Implementation of query logic would go here
    return user; 
}

std::vector<RecyclingPoint> Database::getAllPoints() {
    std::vector<RecyclingPoint> points;
    const char* sql = "SELECT id, name, address, coords FROM recycling_points;";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            RecyclingPoint p;
            p.id = sqlite3_column_int(stmt, 0);
            p.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            p.address = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            p.coords = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            points.push_back(p);
        }
    }
    sqlite3_finalize(stmt);
    return points;
}

bool Database::createTransaction(int userId, int wasteId, double weight, double bonus) {
    std::string sql = "INSERT INTO transactions (user_id, waste_id, weight, bonus) VALUES (" + 
                      std::to_string(userId) + ", " + std::to_string(wasteId) + ", " + 
                      std::to_string(weight) + ", " + std::to_string(bonus) + ");";
                      
    char* errMsg = 0;
    if (sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg) != SQLITE_OK) {
        std::cerr << "Transaction error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    
    // Update balance
    std::string updateSql = "UPDATE users SET balance = balance + " + std::to_string(bonus) + 
                            " WHERE id = " + std::to_string(userId) + ";";
    sqlite3_exec(db, updateSql.c_str(), 0, 0, 0);
    
    return true;
}
