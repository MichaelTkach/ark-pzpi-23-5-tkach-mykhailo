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

#endif // MODELS_H
