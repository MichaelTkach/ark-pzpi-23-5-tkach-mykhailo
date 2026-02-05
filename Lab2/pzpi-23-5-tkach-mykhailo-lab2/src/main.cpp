#include "crow.h"
#include "database.h"
#include "services.h"

struct SecurityMiddleware {
    struct context {};

    void before_handle(crow::request& req, crow::response& res, context& ctx) {
        if (req.url == "/api/recycle" && req.method == crow::HTTPMethod::POST) {
            auto authHeader = req.get_header_value("Authorization");
            if (authHeader.empty() || authHeader.find("Bearer ") == std::string::npos) {
                std::cout << "[SECURITY] Warning: Unsecured request to /api/recycle" << std::endl;
            }
        }
    }

    void after_handle(crow::request& req, crow::response& res, context& ctx) {}
};

int main() {
    if (!Database::getInstance().init("ecorecycle.db")) {
        return 1;
    }

    crow::App<SecurityMiddleware> app;

    AuthService authService;
    RecyclingService recycleService;

    CROW_ROUTE(app, "/api/auth/register").methods(crow::HTTPMethod::POST)
    ([&authService](const crow::request& req){
        auto x = crow::json::load(req.body);
        if (!x) return crow::response(400, "Invalid JSON");
        
        RegisterDto dto;
        dto.email = x["email"].s();
        dto.password = x["password"].s();
        
        if (authService.registerUser(dto)) {
            return crow::response(201, "User registered successfully");
        }
        return crow::response(500, "Registration failed");
    });

    CROW_ROUTE(app, "/api/recycle").methods(crow::HTTPMethod::POST)
    ([&recycleService](const crow::request& req){
        auto x = crow::json::load(req.body);
        if (!x) return crow::response(400);
        
        RecycleDto dto;
        dto.user_id = x["user_id"].i();
        dto.waste_id = x["waste_id"].i();
        dto.weight = x["weight"].d();
        
        double earned = recycleService.processRecycling(dto);
        
        if (earned >= 0) {
            crow::json::wvalue res;
            res["status"] = "success";
            res["bonus_earned"] = earned;
            res["message"] = "Transaction processed via Strategy Pattern";
            return crow::response(res);
        }
        return crow::response(500, "Transaction failed");
    });
    
    CROW_ROUTE(app, "/api/points")([](){
        auto points = Database::getInstance().getAllPoints();
        crow::json::wvalue query_result;
        for (size_t i = 0; i < points.size(); ++i) {
            query_result[i]["id"] = points[i].id;
            query_result[i]["name"] = points[i].name;
            query_result[i]["address"] = points[i].address;
            query_result[i]["coords"] = points[i].coords;
        }
        return crow::response(query_result);
    });

    std::cout << "Starting EcoRecycle Server (Advanced Arch)..." << std::endl;
    app.port(18080).multithreaded().run();
    
    return 0;
}
