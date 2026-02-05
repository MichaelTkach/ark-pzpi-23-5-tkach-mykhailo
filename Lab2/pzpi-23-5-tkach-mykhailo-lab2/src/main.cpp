#include "crow.h"
#include "database.h"
#include "controllers.h"

int main() {
    // Initialize Database
    if (!Database::getInstance().init("ecorecycle.db")) {
        return 1;
    }

    crow::SimpleApp app;

    // Routes
    CROW_ROUTE(app, "/api/auth/register").methods(crow::HTTPMethod::POST)(Controllers::registerUser);
    CROW_ROUTE(app, "/api/points")(Controllers::getPoints);
    CROW_ROUTE(app, "/api/recycle").methods(crow::HTTPMethod::POST)(Controllers::recycle);

    // Run Server
    app.port(18080).multithreaded().run();
    
    return 0;
}
