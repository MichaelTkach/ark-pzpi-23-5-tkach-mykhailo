#include "crow.h"
#include "database.h"
#include "services.h"

struct SecurityMiddleware {
    struct context {};
    void before_handle(crow::request& req, crow::response& res, context& ctx) {
        // Simplified auth check
    }
    void after_handle(crow::request& req, crow::response& res, context& ctx) {}
};

int main() {
    if (!Database::getInstance().init("ecorecycle_lab3.db")) {
        return 1;
    }

    crow::App<SecurityMiddleware> app;

    AuthService authService;
    RecyclingService recycleService;
    AdminService adminService;
    StatsService statsService;

    // --- Standard Routes ---
    
    CROW_ROUTE(app, "/api/auth/register").methods(crow::HTTPMethod::POST)
    ([&](const crow::request& req){
        auto x = crow::json::load(req.body);
        if (!x) return crow::response(400);
        RegisterDto dto{x["email"].s(), x["password"].s()};
        if (authService.registerUser(dto)) return crow::response(201, "Registered");
        return crow::response(500);
    });

    CROW_ROUTE(app, "/api/recycle").methods(crow::HTTPMethod::POST)
    ([&](const crow::request& req){
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
            res["bonus"] = earned;
            return crow::response(res);
        }
        return crow::response(500);
    });

    // --- Lab 3: Admin Routes ---

    // POST /api/admin/waste-types
    CROW_ROUTE(app, "/api/admin/waste-types").methods(crow::HTTPMethod::POST)
    ([&](const crow::request& req){
        auto x = crow::json::load(req.body);
        if (!x) return crow::response(400);
        AddWasteTypeDto dto{x["name"].s(), x["price"].d()};
        if (adminService.addWasteType(dto)) return crow::response(201, "Waste Type Added");
        return crow::response(500);
    });

    // PUT /api/admin/users/<id>/block
    CROW_ROUTE(app, "/api/admin/users/<int>/block").methods(crow::HTTPMethod::PUT)
    ([&](int id){
        if (adminService.blockUser(id)) return crow::response(200, "User Blocked");
        return crow::response(500);
    });

    // GET /api/admin/export/csv
    CROW_ROUTE(app, "/api/admin/export/csv")
    ([&](){
        std::string csv = adminService.exportTransactionsToCsv();
        crow::response res(csv);
        res.set_header("Content-Type", "text/csv");
        res.set_header("Content-Disposition", "attachment; filename=transactions.csv");
        return res;
    });

    // --- Lab 3: Stats Route ---

    // GET /api/stats/global
    CROW_ROUTE(app, "/api/stats/global")
    ([&](){
        auto stats = statsService.getGlobalStats();
        crow::json::wvalue res;
        res["total_weight"] = stats.total_weight_all_time;
        res["average_point_load"] = stats.average_point_load;
        
        crow::json::wvalue topUsers;
        for (size_t i = 0; i < stats.top_users.size(); ++i) {
            topUsers[i]["user_id"] = stats.top_users[i].user_id;
            topUsers[i]["email"] = stats.top_users[i].email;
            topUsers[i]["weight"] = stats.top_users[i].total_weight;
        }
        res["top_users"] = std::move(topUsers);
        
        return crow::response(res);
    });

    std::cout << "Starting EcoRecycle Lab 3 Server..." << std::endl;
    app.port(18080).multithreaded().run();
    return 0;
}
