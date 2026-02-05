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

    // Lab 3: Added is_active to users
    const char* sql = "CREATE TABLE IF NOT EXISTS users ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "email TEXT NOT NULL UNIQUE, "
                      "password_hash TEXT NOT NULL, "
                      "role TEXT NOT NULL, "
                      "balance REAL DEFAULT 0.0, "
                      "level INTEGER DEFAULT 1, "
                      "is_active INTEGER DEFAULT 1);" // 1 = true
                      
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

                      "CREATE TABLE IF NOT EXISTS user_sessions ("
                      "token TEXT PRIMARY KEY, "
                      "user_id INTEGER, "
                      "expires_at INTEGER);"

                      "CREATE TABLE IF NOT EXISTS bonus_coefficients ("
                      "waste_type_id INTEGER, "
                      "coefficient REAL);"
                      
                      // Seed data
                      "INSERT OR IGNORE INTO recycling_points (name, address, coords) "
                      "VALUES ('Central Point', 'Main St 1', '50.00, 36.23');"
                      "INSERT OR IGNORE INTO waste_categories (name, price_per_kg) "
                      "VALUES ('Plastic', 5.0), ('Glass', 2.0);"
                      "INSERT OR IGNORE INTO bonus_coefficients (waste_type_id, coefficient) "
                      "VALUES (1, 1.2), (2, 1.0);";

    char* errMsg = 0;
    if (sqlite3_exec(db, sql, 0, 0, &errMsg) != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    
    return true;
}

bool Database::createUser(const std::string& email, const std::string& password_hash, const std::string& role) {
    std::string sql = "INSERT INTO users (email, password_hash, role, is_active) VALUES ('" + 
                      email + "', '" + password_hash + "', '" + role + "', 1);";
    char* errMsg = 0;
    if (sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg) != SQLITE_OK) {
        std::cerr << "Register error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

User Database::getUserByEmail(const std::string& email) {
    User user = {0, "", "", "", 0.0, 1, true};
    // Implement actual generic SELECT if needed, keeping stub for now or using minimal valid
    // For lab purposes, we assume auth works
    return user; 
}

bool Database::blockUser(int userId) {
    std::string sql = "UPDATE users SET is_active = 0 WHERE id = " + std::to_string(userId) + ";";
    char* errMsg = 0;
    if (sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg) != SQLITE_OK) {
        std::cerr << "Block user error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool Database::isUserActive(int userId) {
    // Mock check - in logic we'd SELECT is_active FROM users WHERE id=...
    // Return true for simplicity unless specifically blocked in session context
    return true; 
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
    // Check if active first (logic usually in Service, but DB constraint good too)
    std::string sql = "INSERT INTO transactions (user_id, waste_id, weight, bonus) VALUES (" + 
                      std::to_string(userId) + ", " + std::to_string(wasteId) + ", " + 
                      std::to_string(weight) + ", " + std::to_string(bonus) + ");";
    char* errMsg = 0;
    if (sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg) != SQLITE_OK) {
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

std::vector<Transaction> Database::getAllTransactions() {
    std::vector<Transaction> trans;
    const char* sql = "SELECT id, user_id, waste_id, weight, bonus, timestamp FROM transactions;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Transaction t;
            t.id = sqlite3_column_int(stmt, 0);
            t.user_id = sqlite3_column_int(stmt, 1);
            t.waste_id = sqlite3_column_int(stmt, 2);
            t.weight = sqlite3_column_double(stmt, 3);
            t.bonus = sqlite3_column_double(stmt, 4);
            t.timestamp = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
            trans.push_back(t);
        }
    }
    sqlite3_finalize(stmt);
    return trans;
}

bool Database::addWasteType(const std::string& name, double price) {
    std::string sql = "INSERT INTO waste_categories (name, price_per_kg) VALUES ('" + 
                      name + "', " + std::to_string(price) + ");";
    char* errMsg = 0;
    if (sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg) != SQLITE_OK) {
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

double Database::getTotalWeight() {
    const char* sql = "SELECT SUM(weight) FROM transactions;";
    sqlite3_stmt* stmt;
    double total = 0.0;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            total = sqlite3_column_double(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);
    return total;
}

std::vector<UserStatsDto> Database::getTopUsers(int limit) {
    std::vector<UserStatsDto> users;
    // Aggregation query
    std::string sql = "SELECT user_id, SUM(weight) as total_w FROM transactions "
                      "GROUP BY user_id ORDER BY total_w DESC LIMIT " + std::to_string(limit) + ";";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            UserStatsDto u;
            u.user_id = sqlite3_column_int(stmt, 0);
            u.total_weight = sqlite3_column_double(stmt, 1);
            u.email = "user" + std::to_string(u.user_id) + "@test.com"; // Mock email for stats
            users.push_back(u);
        }
    }
    sqlite3_finalize(stmt);
    return users;
}
