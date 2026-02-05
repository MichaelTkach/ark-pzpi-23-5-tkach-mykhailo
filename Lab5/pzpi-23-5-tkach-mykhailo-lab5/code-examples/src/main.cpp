#include "crow.h"
#include "database.h"
#include "controllers.h"


struct SecurityMiddleware {
    struct context {};

    void before_handle(crow::request& req, crow::response& res, context& ctx) {
        if (req.url.find("/api/admin") == 0) {
            if (req.get_header_value("Authorization").empty()) {
                res.code = 401;
                res.end();
            }
        }
    }

    void after_handle(crow::request& req, crow::response& res, context& ctx) {}
};

int main() {
    if (!Database::getInstance().init("ecorecycle_lab3.db")) {
        return 1;
    }

    crow::App<SecurityMiddleware> app;

    CROW_ROUTE(app, "/api/auth/register").methods(crow::HTTPMethod::POST)(Controllers::registerUser);
    CROW_ROUTE(app, "/api/points")(Controllers::getPoints);
    CROW_ROUTE(app, "/api/recycle").methods(crow::HTTPMethod::POST)(Controllers::recycle);

    CROW_ROUTE(app, "/api/admin/waste-types").methods(crow::HTTPMethod::POST)(Controllers::addWasteType);
    
    CROW_ROUTE(app, "/api/admin/users/block").methods(crow::HTTPMethod::PUT)(Controllers::blockUser);
    
    CROW_ROUTE(app, "/api/admin/export/csv")(Controllers::exportCsv);
    CROW_ROUTE(app, "/api/admin/import/csv").methods(crow::HTTPMethod::POST)(Controllers::importCsv);
    
    CROW_ROUTE(app, "/api/stats")(Controllers::getStats);

    app.port(18081).multithreaded().run();
    
    return 0;
}
