#ifndef CONTROLLERS_H
#define CONTROLLERS_H

#include "crow.h"
#include "database.h"

class Controllers {
public:
    static crow::response registerUser(const crow::request& req);
    static crow::response getPoints(const crow::request& req);
    static crow::response recycle(const crow::request& req);
};

#endif
