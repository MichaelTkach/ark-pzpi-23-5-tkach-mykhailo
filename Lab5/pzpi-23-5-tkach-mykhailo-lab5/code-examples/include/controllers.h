#ifndef CONTROLLERS_H
#define CONTROLLERS_H

#include "crow.h"
#include "database.h"

class Controllers {
public:
    static crow::response registerUser(const crow::request& req);
    static crow::response getPoints(const crow::request& req);
    static crow::response recycle(const crow::request& req);

    static crow::response addWasteType(const crow::request& req);
    static crow::response blockUser(const crow::request& req);
    static crow::response exportCsv(const crow::request& req);
    static crow::response importCsv(const crow::request& req);

    static crow::response getStats(const crow::request& req);
};

#endif
