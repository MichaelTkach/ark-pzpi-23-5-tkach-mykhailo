#include "controllers.h"
#include <sstream>
#include <iomanip>

crow::response Controllers::registerUser(const crow::request& req) {
    auto x = crow::json::load(req.body);
    if (!x) return crow::response(400);
    std::string email = x["email"].s();
    std::string password = x["password"].s();
    
    if (Database::getInstance().createUser(email, password + "_salt", "user")) {
        return crow::response(201, "Registered");
    }
    return crow::response(500);
}

crow::response Controllers::getPoints(const crow::request& req) {
    auto points = Database::getInstance().getAllPoints();
    crow::json::wvalue res;
    for (size_t i = 0; i < points.size(); ++i) {
        res[i]["name"] = points[i].name;
        res[i]["address"] = points[i].address;
    }
    return crow::response(res);
}

crow::response Controllers::recycle(const crow::request& req) {
    auto x = crow::json::load(req.body);
    if (!x) return crow::response(400);
    
    int userId = x["user_id"].i();
    int wasteId = x["waste_id"].i();
    double weight = x["weight"].d();
    
    double bonus = weight * 5.0; // Simplified calculation
    
    if (Database::getInstance().createTransaction(userId, wasteId, weight, bonus)) {
        crow::json::wvalue res;
        res["bonus_earned"] = bonus;
        return crow::response(res);
    }
    return crow::response(500);
}

// --- Lab 3 Implementations ---

crow::response Controllers::addWasteType(const crow::request& req) {
    auto x = crow::json::load(req.body);
    if (!x) return crow::response(400, "Invalid JSON");
    
    std::string name = x["name"].s();
    double price = x["price"].d();
    
    if (Database::getInstance().addWasteType(name, price)) {
        return crow::response(201, "Waste type added");
    }
    return crow::response(500);
}

crow::response Controllers::blockUser(const crow::request& req) {
    auto x = crow::json::load(req.body);
    if (!x) {
        // Fallback: Check if we can parse query params for simpler PUT
        // But spec suggests body usually. Let's assume body for now or query param `id`.
        return crow::response(400);
    }
    
    int userId = x["user_id"].i();
    bool block = x["block"].b(); // Expecting boolean
    
    if (Database::getInstance().blockUser(userId, block)) {
        return crow::response(200, block ? "User blocked" : "User unblocked");
    }
    return crow::response(500);
}

crow::response Controllers::exportCsv(const crow::request& req) {
    auto transactions = Database::getInstance().getAllTransactions();
    
    std::stringstream ss;
    ss << "ID,UserID,WasteID,Weight,Bonus,Timestamp\n";
    
    for (const auto& t : transactions) {
        ss << t.id << "," 
           << t.user_id << "," 
           << t.waste_id << "," 
           << std::fixed << std::setprecision(2) << t.weight << "," 
           << t.bonus << "," 
           << t.timestamp << "\n";
    }
    
    crow::response res(ss.str());
    res.set_header("Content-Type", "text/csv");
    res.set_header("Content-Disposition", "attachment; filename=export.csv");
    return res;
}

crow::response Controllers::getStats(const crow::request& req) {
    double totalWeight = Database::getInstance().getTotalWeight();
    double totalBonuses = Database::getInstance().getTotalBonuses();
    int count = Database::getInstance().getTransactionCount();
    
    double avgWeight = (count > 0) ? (totalWeight / count) : 0.0;
    
    crow::json::wvalue res;
    res["total_weight"] = totalWeight;
    res["total_bonuses"] = totalBonuses;
    res["transactions_count"] = count;
    res["average_weight_per_transaction"] = avgWeight; // Math calculation
    
    return crow::response(res);
}
