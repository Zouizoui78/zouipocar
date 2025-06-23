#ifndef SQLITESTATEMENT_HPP
#define SQLITESTATEMENT_HPP

#include <memory>
#include <string>

#include "sqlite3.h"

namespace zouipocar::db {

class SQLiteStatement {
public:
    SQLiteStatement(sqlite3 *db_handle, std::string_view source);

    std::string_view get_source() const;
    sqlite3_stmt *get() const;

private:
    std::string _source;

    struct SQLiteStatementDeleter {
        void operator()(sqlite3_stmt *stmt);
    };
    using SQLitePreparedStatement =
        std::unique_ptr<sqlite3_stmt, SQLiteStatementDeleter>;
    SQLitePreparedStatement _prepared;
};

} // namespace zouipocar::db

#endif
