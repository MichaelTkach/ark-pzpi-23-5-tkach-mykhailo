#ifndef MODELS_H
#define MODELS_H

#include <string>
#include <vector>

struct User {
    int id;
    std::string email;
    std::string password_hash;
    std::string role;
    double balance;
    int level;
};

struct WasteCategory {
    int id;
    std::string name;
    double price_per_kg;
};

struct RecyclingPoint {
    int id;
    std::string name;
    std::string address;
    std::string coords;
};

struct Transaction {
    int id;
    int user_id;
    int waste_id;
    double weight;
    double bonus;
    std::string timestamp;
};

struct RegisterDto {
    std::string email;
    std::string password;
};

struct RecycleDto {
    int user_id;
    int waste_id;
    double weight;
    std::string token;
};

struct UserSession {
    std::string token;
    int user_id;
    long long expires_at;
};

struct BonusCoefficient {
    int waste_type_id;
    double coefficient;
};

#endif
