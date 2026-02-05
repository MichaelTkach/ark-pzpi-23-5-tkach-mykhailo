#ifndef CONTROLLERS_H
#define CONTROLLERS_H

#include "crow.h" // Assuming crow is header-only and available
#include "database.h"

class Controllers {
public:
    static crow::response registerUser(const crow::request& req);
    static crow::response getPoints(const crow::request& req);
    static crow::response recycle(const crow::request& req);
};

#endif // CONTROLLERS_H
