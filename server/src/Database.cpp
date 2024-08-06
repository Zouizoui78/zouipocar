#include "constants.hpp"
#include "Database.hpp"

#include <format>
#include <iostream>
#include <stdexcept>

namespace zouipocar {

Database::Database() {
    int res = sqlite3_open(DB_PATH.data(), &_db_handler);
    if (res != SQLITE_OK || _db_handler == nullptr) {
        throw std::runtime_error(std::format("Failed to open database file {} : {}", DB_PATH, sqlite3_errmsg(_db_handler)));
    }

    res = sqlite3_exec(_db_handler, "PRAGMA synchronous = OFF", nullptr, nullptr, &_errmsg);
    if (res != SQLITE_OK) {
        std::cout << "Failed to disable fs sync for database.\n";
    }

    create_table();
}

Database::~Database() noexcept {
    if (_db_handler != nullptr) {
        int res = sqlite3_close(_db_handler);
        if (res != SQLITE_OK) {
            std::cout << "Failed to close database " << DB_PATH << std::endl;
        }
    }
}

void Database::create_table() {
    std::string statement = "CREATE TABLE IF NOT EXISTS fixes (timestamp INTEGER, speed INTEGER, latitude REAL, longitude REAL);";
    int res = sqlite3_exec(_db_handler, statement.c_str(), nullptr, nullptr, &_errmsg);
    if (res != SQLITE_OK) {
        throw std::runtime_error(std::format("Failed to create table zoui : {}", _errmsg));
    }
}

bool Database::insert_fix(Fix fix) {
    std::string statement = "INSERT INTO zoui VALUES(";
    statement += std::to_string(fix.timestamp) + ",";
    statement += std::to_string(fix.speed) + ",";
    statement += std::to_string(fix.latitude) + ",";
    statement += std::to_string(fix.longitude) + ");";
    int res = sqlite3_exec(_db_handler, statement.c_str(), nullptr, nullptr, &_errmsg);

    if (res != SQLITE_OK) {
        std::cout << "Failed to insert fix into zoui : " << _errmsg << std::endl;
        return false;
    }

    return true;
}

std::optional<Fix> Database::get_fix(time_t date) {
    Fix fix;

    bool ok = query("SELECT * FROM zoui WHERE timestamp=" + std::to_string(date), [this, &fix](sqlite3_stmt *stmt) {
        fix.timestamp = sqlite3_column_int64(stmt, 0);
        fix.speed = sqlite3_column_int64(stmt, 1);
        fix.latitude = sqlite3_column_double(stmt, 2);
        fix.longitude = sqlite3_column_double(stmt, 3);
    });

    if (!ok) {
        return std::nullopt;
    }

    return fix;
}

std::vector<Fix> Database::get_fix_range(time_t start, time_t end) {
    std::vector<Fix> ret;

    bool ok = query("SELECT * FROM zoui WHERE timestamp BETWEEN " + std::to_string(start) + " AND " + std::to_string(end), [this, &ret](sqlite3_stmt *stmt) {
        ret.emplace_back(
            sqlite3_column_int64(stmt, 0),
            sqlite3_column_int64(stmt, 1),
            sqlite3_column_double(stmt, 2),
            sqlite3_column_double(stmt, 3)
        );
    });

    return ret;
}

std::optional<Fix> Database::get_first_fix() {
    Fix fix;
    bool ok = query("SELECT * FROM zoui LIMIT 1;", [this, &fix](sqlite3_stmt *stmt) {
        fix.timestamp = sqlite3_column_int64(stmt, 0);
        fix.speed = sqlite3_column_int64(stmt, 1);
        fix.latitude = sqlite3_column_double(stmt, 2);
        fix.longitude = sqlite3_column_double(stmt, 3);
    });

    if (!ok) {
        return std::nullopt;
    }

    return fix;
}

std::optional<Fix> Database::get_last_fix() {
    Fix fix;
    bool ok = query("SELECT * FROM zoui ORDER BY timestamp DESC LIMIT 1;", [this, &fix](sqlite3_stmt *stmt) {
        fix.timestamp = sqlite3_column_int64(stmt, 0);
        fix.speed = sqlite3_column_int64(stmt, 1);
        fix.latitude = sqlite3_column_double(stmt, 2);
        fix.longitude = sqlite3_column_double(stmt, 3);
    });

    if (!ok) {
        return std::nullopt;
    }

    return fix;
}

bool Database::query(const std::string &statement, DBQueryCallback callback) {
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

}