#include "controllers.h"
#include <iostream>

crow::response Controllers::registerUser(const crow::request& req) {
    auto x = crow::json::load(req.body);
    if (!x) return crow::response(400);
    
    std::string email = x["email"].s();
    std::string password = x["password"].s();
    
    std::string hash = password + "_hashed";
    
    if (Database::getInstance().createUser(email, hash, "user")) {
        return crow::response(201, "User registered");
    } else {
        return crow::response(500, "Registration failed");
    }
}

crow::response Controllers::getPoints(const crow::request& req) {
    auto points = Database::getInstance().getAllPoints();
    crow::json::wvalue query_result;
    
    for (size_t i = 0; i < points.size(); ++i) {
        query_result[i]["id"] = points[i].id;
        query_result[i]["name"] = points[i].name;
        query_result[i]["address"] = points[i].address;
        query_result[i]["coords"] = points[i].coords;
    }
    
    return crow::response(query_result);
}

crow::response Controllers::recycle(const crow::request& req) {
    auto x = crow::json::load(req.body);
    if (!x) return crow::response(400);
    
    int userId = x["user_id"].i();
    int wasteId = x["waste_id"].i();
    double weight = x["weight"].d();
    
    double bonus = weight * 5.0; 
    
    if (Database::getInstance().createTransaction(userId, wasteId, weight, bonus)) {
        crow::json::wvalue res;
        res["status"] = "success";
        res["bonus_earned"] = bonus;
        return crow::response(res);
    } else {
        return crow::response(500, "Transaction failed");
    }
}
