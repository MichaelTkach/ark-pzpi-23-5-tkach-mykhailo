#include "controllers.h"
#include <sstream>
#include <iomanip>
#include <map>


std::string getMessage(const std::string& key, const std::string& lang) {
    static std::map<std::string, std::map<std::string, std::string>> messages = {
        {"REGISTERED", {{"en", "Registered successfully"}, {"uk", "Успішна реєстрація"}}},
        {"ERROR", {{"en", "Internal Error"}, {"uk", "Внутрішня помилка"}}},
        {"WASTE_ADDED", {{"en", "Waste type added"}, {"uk", "Тип відходів додано"}}},
        {"BLOCKED", {{"en", "User blocked"}, {"uk", "Користувача заблоковано"}}},
        {"UNBLOCKED", {{"en", "User unblocked"}, {"uk", "Користувача розблоковано"}}},
        {"IMPORTED", {{"en", "Data imported"}, {"uk", "Дані імпортовано"}}}
    };
    std::string l = (lang == "uk") ? "uk" : "en";
    return messages[key][l];
}


crow::response Controllers::registerUser(const crow::request& req) {
    auto x = crow::json::load(req.body);
    if (!x) return crow::response(400);
    std::string email = x["email"].s();
    std::string password = x["password"].s();
    
    std::string lang = req.get_header_value("Accept-Language");
    if (Database::getInstance().createUser(email, password + "_salt", "user")) {
        return crow::response(201, getMessage("REGISTERED", lang));
    }
    return crow::response(500, getMessage("ERROR", lang));
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
    
    double bonus = weight * 5.0; 
    
    if (Database::getInstance().createTransaction(userId, wasteId, weight, bonus)) {
        crow::json::wvalue res;
        res["bonus_earned"] = bonus;
        return crow::response(res);
    }
    return crow::response(500);
}

crow::response Controllers::addWasteType(const crow::request& req) {
    auto x = crow::json::load(req.body);
    if (!x) return crow::response(400, "Invalid JSON");
    
    std::string name = x["name"].s();
    double price = x["price"].d();
    
    std::string lang = req.get_header_value("Accept-Language");
    if (Database::getInstance().addWasteType(name, price)) {
        return crow::response(201, getMessage("WASTE_ADDED", lang));
    }
    return crow::response(500, getMessage("ERROR", lang));
}

crow::response Controllers::blockUser(const crow::request& req) {
    auto x = crow::json::load(req.body);
    if (!x) {
        return crow::response(400);
    }
    
    int userId = x["user_id"].i();
    bool block = x["block"].b();
    
    std::string lang = req.get_header_value("Accept-Language");
    if (Database::getInstance().blockUser(userId, block)) {
        return crow::response(200, block ? getMessage("BLOCKED", lang) : getMessage("UNBLOCKED", lang));
    }
    return crow::response(500, getMessage("ERROR", lang));
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

crow::response Controllers::importCsv(const crow::request& req) {
    std::string lang = req.get_header_value("Accept-Language");
    std::stringstream ss(req.body);
    std::string line;
    
    std::getline(ss, line);
    
    while(std::getline(ss, line)) {
    }
    
    return crow::response(200, getMessage("IMPORTED", lang));
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
    res["average_weight_per_transaction"] = avgWeight;
    
    return crow::response(res);
}
