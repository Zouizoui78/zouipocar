#include "Database.hpp"

#include <format>
#include <iostream>
#include <print>
#include <stdexcept>

namespace zouipocar::db {

Fix fix_from_statement(const SQLiteStatement &stmt) {
    auto raw_stmt = stmt.get();
    return Fix{
        .timestamp = static_cast<uint32_t>(sqlite3_column_int(raw_stmt, 0)),
        .latitude = static_cast<float>(sqlite3_column_double(raw_stmt, 2)),
        .longitude = static_cast<float>(sqlite3_column_double(raw_stmt, 3)),
        .speed = static_cast<uint8_t>(sqlite3_column_int(raw_stmt, 1))};
}

Database::Database(std::string_view path) {
    sqlite3 *handle;
    int res = sqlite3_open(path.data(), &handle);
    if (res != SQLITE_OK || handle == nullptr) {
        throw std::runtime_error(
            std::format("Failed to open database file {} : {}", path,
                        sqlite3_errmsg(handle)));
    }

    _handle = std::unique_ptr<sqlite3, SQLiteDeleter>(handle);

    res = sqlite3_exec(_handle.get(), "PRAGMA synchronous = OFF", nullptr,
                       nullptr, nullptr);
    if (res != SQLITE_OK) {
        const auto error = get_last_error();
        std::print("Failed to disable fs sync for database (error {}): {}",
                   error.code, error.message);
    }

    this->create_table("zoui");
    this->prepare_statements();
}

SQLiteError Database::get_last_error() const {
    return SQLiteError{.code = sqlite3_errcode(_handle.get()),
                       .message = sqlite3_errmsg(_handle.get())};
}

void Database::create_table(std::string_view table_name) {
    std::string query =
        std::format("CREATE TABLE IF NOT EXISTS {} ("
                    "timestamp INTEGER PRIMARY KEY NOT NULL UNIQUE,"
                    "speed INTEGER,"
                    "latitude REAL,"
                    "longitude REAL"
                    ");",
                    table_name);
    int res =
        sqlite3_exec(_handle.get(), query.c_str(), nullptr, nullptr, nullptr);

    if (res != SQLITE_OK) {
        const auto error = get_last_error();
        throw std::runtime_error(
            std::format("Failed to create table zoui (error {}): {}",
                        error.code, error.message));
    }
}

void Database::prepare_statements() {
    std::map<Statements, std::string> statements_sources{
        {Statements::GET_FIX, "SELECT * FROM zoui WHERE timestamp=?;"},
        {Statements::GET_FIRST_FIX, "SELECT * FROM zoui LIMIT 1;"},
        {Statements::GET_LAST_FIX,
         "SELECT * FROM zoui ORDER BY timestamp DESC LIMIT 1;"},
        {Statements::GET_FIX_RANGE,
         "SELECT * FROM zoui WHERE timestamp BETWEEN ? AND ?;"},
        {Statements::INSERT_FIX, "INSERT INTO zoui VALUES(?, ?, ?, ?);"},
    };

    for (const auto &[key, value] : statements_sources) {
        _prepared_statements.emplace(key,
                                     SQLiteStatement(_handle.get(), value));
    }
}

bool Database::insert_fix(const Fix &fix) {
    auto &stmt = _prepared_statements.at(Statements::INSERT_FIX);

    if (!stmt.bind_arg(1, fix.timestamp)) {
        return false;
    }
    if (!stmt.bind_arg(2, fix.speed)) {
        return false;
    }
    if (!stmt.bind_arg(3, fix.latitude)) {
        return false;
    }
    if (!stmt.bind_arg(4, fix.longitude)) {
        return false;
    }

    if (stmt.step() != SQLITE_DONE) {
        stmt.reset();
        return false;
    }

    return true;
}

std::optional<Fix> Database::get_fix(uint32_t date) {
    auto &stmt = _prepared_statements.at(Statements::GET_FIX);

    if (!stmt.bind_arg(1, date)) {
        return std::nullopt;
    }

    if (stmt.step() != SQLITE_ROW) {
        stmt.reset();
        return std::nullopt;
    }

    Fix fix{fix_from_statement(stmt)};
    stmt.reset();
    return fix;
}

std::optional<Fix> Database::get_first_fix() {
    auto &stmt = _prepared_statements.at(Statements::GET_FIRST_FIX);

    if (stmt.step() != SQLITE_ROW) {
        stmt.reset();
        return std::nullopt;
    }

    Fix fix{fix_from_statement(stmt)};
    stmt.reset();
    return fix;
}

std::optional<Fix> Database::get_last_fix() {
    auto &stmt = _prepared_statements.at(Statements::GET_LAST_FIX);

    if (stmt.step() != SQLITE_ROW) {
        stmt.reset();
        return std::nullopt;
    }

    Fix fix{fix_from_statement(stmt)};
    stmt.reset();
    return fix;
}

std::vector<Fix> Database::get_fix_range(uint32_t start, uint32_t end) {
    std::vector<Fix> ret;
    auto &stmt = _prepared_statements.at(Statements::GET_FIX_RANGE);

    if (!stmt.bind_arg(1, start)) {
        return ret;
    }

    if (!stmt.bind_arg(2, end)) {
        return ret;
    }

    ret.reserve(end - start);

    stmt.step_until_done([&ret](const SQLiteStatement &stmt) {
        ret.push_back(fix_from_statement(stmt));
    });
    ret.shrink_to_fit();

    return ret;
}

void Database::SQLiteDeleter::operator()(sqlite3 *handle) {
    int res = sqlite3_close(handle);
    if (res != SQLITE_OK) {
        std::cout << "Failed to close database\n";
    }
}

} // namespace zouipocar::db
