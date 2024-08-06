#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <iostream>
#include <string>
#include <sqlite3.h>

#include "json.hpp"

using json = nlohmann::json;

class Database {
    public:
    Database(const std::string &path);
    ~Database();
    bool is_open();

    bool insert_fix(json fix);
    json query_fix(time_t date);
    json query_fix_range(time_t start, time_t end);
    json query_first_fix();
    json query_last_fix();

    private:
    std::string _path = "";
    bool _is_open = false;
    sqlite3 *_db_handler = nullptr;
    char *_errmsg = nullptr;

    bool create_table();
    json initialize_fix_json();

    template <typename T>
    bool query(const std::string &statement, T callback) {
        sqlite3_stmt *stmt = nullptr;

        int res = sqlite3_prepare_v2(_db_handler, statement.c_str(), -1, &stmt, nullptr);
        if (res != SQLITE_OK) {
            std::cout << "Failed to prepare SQL statement : " << sqlite3_errmsg(_db_handler);
            return false;
        }

        res = sqlite3_step(stmt);
        while (res == SQLITE_ROW) {
            callback(stmt);
            res = sqlite3_step(stmt);
        }

        sqlite3_finalize(stmt);
        return true;
    }
};

#endif // DATABASE_HPP