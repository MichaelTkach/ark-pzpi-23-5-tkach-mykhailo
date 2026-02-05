#include "database.h"
#include <iostream>

Database& Database::getInstance() {
    static Database instance;
    return instance;
}

Database::~Database() {
    if (db) sqlite3_close(db);
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
                      "balance REAL DEFAULT 0.0, "
                      "level INTEGER DEFAULT 1, "
                      "is_active INTEGER DEFAULT 1);"
                      
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
    const char* sql = "INSERT INTO users (email, password_hash, role, is_active) VALUES (?, ?, ?, 1);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, password_hash.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, role.c_str(), -1, SQLITE_STATIC);
        
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            std::cerr << "Execution failed: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_finalize(stmt);
            return false;
        }
    } else {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    sqlite3_finalize(stmt);
    return true;
}

bool Database::blockUser(int userId, bool block) {
    int status = block ? 0 : 1;
    const char* sql = "UPDATE users SET is_active = ? WHERE id = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, status);
        sqlite3_bind_int(stmt, 2, userId);
        
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            sqlite3_finalize(stmt);
            return false;
        }
    } else {
        return false;
    }
    sqlite3_finalize(stmt);
    return true;
}

bool Database::addWasteType(const std::string& name, double price) {
    const char* sql = "INSERT INTO waste_categories (name, price_per_kg) VALUES (?, ?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_double(stmt, 2, price);
        
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            sqlite3_finalize(stmt);
            return false;
        }
    } else {
        return false;
    }
    sqlite3_finalize(stmt);
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

std::vector<WasteCategory> Database::getAllWasteTypes() {
    std::vector<WasteCategory> types;
    const char* sql = "SELECT id, name, price_per_kg FROM waste_categories;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            WasteCategory w;
            w.id = sqlite3_column_int(stmt, 0);
            w.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            w.price_per_kg = sqlite3_column_double(stmt, 2);
            types.push_back(w);
        }
    }
    sqlite3_finalize(stmt);
    return types;
}

bool Database::createTransaction(int userId, int wasteId, double weight, double bonus) {
    const char* sql = "INSERT INTO transactions (user_id, waste_id, weight, bonus) VALUES (?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, userId);
        sqlite3_bind_int(stmt, 2, wasteId);
        sqlite3_bind_double(stmt, 3, weight);
        sqlite3_bind_double(stmt, 4, bonus);
        
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            sqlite3_finalize(stmt);
            return false;
        }
    } else {
        return false;
    }
    sqlite3_finalize(stmt);
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

double Database::getTotalWeight() {
    sqlite3_stmt* stmt;
    double res = 0;
    if (sqlite3_prepare_v2(db, "SELECT SUM(weight) FROM transactions;", -1, &stmt, 0) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) res = sqlite3_column_double(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return res;
}

double Database::getTotalBonuses() {
    sqlite3_stmt* stmt;
    double res = 0;
    if (sqlite3_prepare_v2(db, "SELECT SUM(bonus) FROM transactions;", -1, &stmt, 0) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) res = sqlite3_column_double(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return res;
}

int Database::getTransactionCount() {
    sqlite3_stmt* stmt;
    int res = 0;
    if (sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM transactions;", -1, &stmt, 0) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) res = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return res;
}

std::vector<UserStatDto> Database::getTopUsers(int limit) {
    std::vector<UserStatDto> users;
    std::string sql = "SELECT u.email, SUM(t.weight) as total " 
                      "FROM transactions t "
                      "JOIN users u ON t.user_id = u.id "
                      "GROUP BY u.id "
                      "ORDER BY total DESC LIMIT " + std::to_string(limit) + ";";
                      
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            UserStatDto u;
            u.email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            u.total_weight = sqlite3_column_double(stmt, 1);
            users.push_back(u);
        }
    }
    sqlite3_finalize(stmt);
    return users;
}

User Database::getUserByEmail(const std::string& email) {
    User u = {0, "", "", "", 0.0, 1, true};
    return u;
}
