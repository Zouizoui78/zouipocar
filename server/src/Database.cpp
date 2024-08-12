#include "Database.hpp"

#include <format>
#include <iostream>
#include <stdexcept>

namespace zouipocar {

Database::Database(std::string_view path) {
    sqlite3* handle;
    int res = sqlite3_open(path.data(), &handle);
    if (res != SQLITE_OK || handle == nullptr) {
        throw std::runtime_error(std::format("Failed to open database file {} : {}", path, sqlite3_errmsg(handle)));
    }

    _handle = std::unique_ptr<sqlite3, Sqlite3Deleter>(handle);

    res = sqlite3_exec(_handle.get(), "PRAGMA synchronous = OFF", nullptr, nullptr, nullptr);
    if (res != SQLITE_OK) {
        std::cout << "Failed to disable fs sync for database.\n";
    }

    create_table();
}

void Database::create_table() {
    std::string statement = "CREATE TABLE IF NOT EXISTS zoui (timestamp INTEGER, speed INTEGER, latitude REAL, longitude REAL);";
    int res = sqlite3_exec(_handle.get(), statement.c_str(), nullptr, nullptr, &_errmsg);
    if (res != SQLITE_OK) {
        throw std::runtime_error(std::format("Failed to create table zoui : {}", _errmsg));
    }
}

bool Database::insert_fix(fix::Fix fix) {
    std::string statement = std::format("INSERT INTO zoui VALUES({}, {}, {}, {});", fix.timestamp, fix.speed, fix.latitude, fix.longitude);
    int res = sqlite3_exec(_handle.get(), statement.c_str(), nullptr, nullptr, &_errmsg);

    if (res != SQLITE_OK) {
        std::cout << "Failed to insert fix into zoui : " << _errmsg << std::endl;
        return false;
    }

    return true;
}

std::optional<fix::Fix> Database::get_fix(uint32_t date) {
    fix::Fix fix;
    int n = query(std::format("SELECT * FROM zoui WHERE timestamp={};", date), [this, &fix](sqlite3_stmt *stmt) {
        fix.timestamp = sqlite3_column_int64(stmt, 0);
        fix.speed = sqlite3_column_int64(stmt, 1);
        fix.latitude = sqlite3_column_double(stmt, 2);
        fix.longitude = sqlite3_column_double(stmt, 3);
    });

    if (n == 0) {
        return std::nullopt;
    }

    return fix;
}

std::optional<fix::Fix> Database::get_first_fix() {
    fix::Fix fix;
    int n = query("SELECT * FROM zoui LIMIT 1;", [this, &fix](sqlite3_stmt *stmt) {
        fix.timestamp = sqlite3_column_int64(stmt, 0);
        fix.speed = sqlite3_column_int64(stmt, 1);
        fix.latitude = sqlite3_column_double(stmt, 2);
        fix.longitude = sqlite3_column_double(stmt, 3);
    });

    if (n == 0) {
        return std::nullopt;
    }

    return fix;
}

std::optional<fix::Fix> Database::get_last_fix() {
    fix::Fix fix;
    int n = query("SELECT * FROM zoui ORDER BY timestamp DESC LIMIT 1;", [this, &fix](sqlite3_stmt *stmt) {
        fix.timestamp = sqlite3_column_int64(stmt, 0);
        fix.speed = sqlite3_column_int64(stmt, 1);
        fix.latitude = sqlite3_column_double(stmt, 2);
        fix.longitude = sqlite3_column_double(stmt, 3);
    });

    if (n == 0) {
        return std::nullopt;
    }

    return fix;
}

std::vector<fix::Fix> Database::get_fix_range(uint32_t start, uint32_t end) {
    std::vector<fix::Fix> ret;

    query(std::format("SELECT * FROM zoui WHERE timestamp BETWEEN {} AND {};", start, end), [this, &ret](sqlite3_stmt *stmt) {
        fix::Fix f;
        f.timestamp = sqlite3_column_int64(stmt, 0);
        f.speed = sqlite3_column_int64(stmt, 1);
        f.latitude = sqlite3_column_double(stmt, 2);
        f.longitude = sqlite3_column_double(stmt, 3);
        ret.push_back(f);
    });

    return ret;
}

template <typename T>
int Database::query(const std::string &statement, T&& callback) {
    sqlite3_stmt *stmt = nullptr;

    int res = sqlite3_prepare_v2(_handle.get(), statement.c_str(), -1, &stmt, nullptr);
    if (res != SQLITE_OK) {
        std::cout << "Failed to prepare SQL statement : " << sqlite3_errmsg(_handle.get()) << std::endl;
        return false;
    }

    res = sqlite3_step(stmt);
    int count = 0;
    while (res == SQLITE_ROW) {
        callback(stmt);
        res = sqlite3_step(stmt);
        ++count;
    }

    sqlite3_finalize(stmt);
    return count;
}

void Database::Sqlite3Deleter::operator()(sqlite3* handle) {
    int res = sqlite3_close(handle);
    if (res != SQLITE_OK) {
        std::cout << "Failed to close database\n";
    }
}

}
