#ifndef MODELS_H
#define MODELS_H

#include <string>
#include <vector>

struct User {
    int id;
    std::string email;
    std::string password_hash;
    std::string role; // "admin", "user"
    double balance;
    int level;
    bool is_active; // Lab 3: Block user feature
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

// --- DTOs ---

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

// Lab 3: Admin DTOs
struct AddWasteTypeDto {
    std::string name;
    double price;
};

struct UserStatsDto {
    int user_id;
    std::string email;
    double total_weight;
};

struct GlobalStatsDto {
    double total_weight_all_time;
    std::vector<UserStatsDto> top_users; // Top 3
    double average_point_load; // Mocked %
};

#endif // MODELS_H
