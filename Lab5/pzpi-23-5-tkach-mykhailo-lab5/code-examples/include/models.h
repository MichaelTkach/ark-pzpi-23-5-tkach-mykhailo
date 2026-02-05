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
    bool is_active;
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
};

struct AddWasteTypeDto {
    std::string name;
    double price;
};

struct BlockUserDto {
    int user_id;
    bool block;
};

struct UserStatDto {
    std::string email;
    double total_weight;
};

struct GlobalStatsDto {
    double total_weight;
    double total_bonuses;
    int transactions_count;
    double average_weight_per_transaction;
    std::vector<UserStatDto> top_users;
};

#endif
