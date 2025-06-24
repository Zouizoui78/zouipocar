#include "SQLiteStatement.hpp"

#include <format>
#include <print>
#include <stdexcept>

namespace zouipocar::db {

SQLiteStatement::SQLiteStatement(sqlite3 *db_handle, std::string_view source)
    : _source(source) {
    sqlite3_stmt *stmt = nullptr;
    int res = sqlite3_prepare_v2(db_handle, source.cbegin(), source.size(),
                                 &stmt, nullptr);
    _prepared = SQLitePreparedStatement(stmt);

    if (res != SQLITE_OK) {
        throw std::runtime_error(
            std::format("Failed to prepare SQL statement '{}' : {}", _source,
                        sqlite3_errmsg(db_handle)));
    }
}

std::string_view SQLiteStatement::get_source() const {
    return _source;
}

sqlite3_stmt *SQLiteStatement::get() const {
    return _prepared.get();
}

int SQLiteStatement::step() const {
    return sqlite3_step(this->get());
}

bool SQLiteStatement::reset() const {
    return sqlite3_reset(_prepared.get()) == SQLITE_OK;
}

void SQLiteStatement::SQLiteStatementDeleter::operator()(sqlite3_stmt *stmt) {
    sqlite3_finalize(stmt);
}

} // namespace zouipocar::db
