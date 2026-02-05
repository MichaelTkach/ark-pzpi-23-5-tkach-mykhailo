#include "crow.h"
#include "database.h"
#include "controllers.h"

int main() {
    // Init DB with Lab 3 name to distinguish
    if (!Database::getInstance().init("ecorecycle_lab3.db")) {
        return 1;
    }

    crow::SimpleApp app;

    // --- Lab 2 Routes ---
    CROW_ROUTE(app, "/api/auth/register").methods(crow::HTTPMethod::POST)(Controllers::registerUser);
    CROW_ROUTE(app, "/api/points")(Controllers::getPoints);
    CROW_ROUTE(app, "/api/recycle").methods(crow::HTTPMethod::POST)(Controllers::recycle);

    // --- Lab 3 Admin & Stats Routes ---
    
    // POST /api/admin/waste-types
    CROW_ROUTE(app, "/api/admin/waste-types").methods(crow::HTTPMethod::POST)(Controllers::addWasteType);
    
    // PUT /api/admin/users/block
    CROW_ROUTE(app, "/api/admin/users/block").methods(crow::HTTPMethod::PUT)(Controllers::blockUser);
    
    // GET /api/admin/export/csv
    CROW_ROUTE(app, "/api/admin/export/csv")(Controllers::exportCsv);
    
    // GET /api/stats
    CROW_ROUTE(app, "/api/stats")(Controllers::getStats);

    // Run
    app.port(18081).multithreaded().run(); // Using port 18081 for Lab 3 to avoid conflict if both run
    
    return 0;
}
