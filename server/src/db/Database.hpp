#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <cstdint>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Fix.hpp"
#include "SQLiteError.hpp"
#include "SQLiteStatement.hpp"
#include "StatementsEnum.hpp"

#include "sqlite3.h"

namespace zouipocar::db {

class Database {
public:
    Database(std::string_view path);

    SQLiteError get_last_error() const;

    // Return false if insertion fails, true otherwise.
    bool insert_fix(const Fix &fix);

    // Return the fix if found, std::nullopt otherwise.
    std::optional<Fix> get_fix(uint32_t date);

    // Return first fix if it exists, std::nullopt otherwise.
    std::optional<Fix> get_first_fix();

    // Return latest fix if it exists, std::nullopt otherwise.
    std::optional<Fix> get_last_fix();

    // Return a vector containing the fixes if the range is valid,
    // and an empty vector otherwise.
    std::vector<Fix> get_fix_range(uint32_t start, uint32_t end);

private:
    struct SQLiteDeleter {
        void operator()(sqlite3 *handle);
    };
    std::unique_ptr<sqlite3, SQLiteDeleter> _handle;

    std::map<Statements, SQLiteStatement> _prepared_statements;

    std::string _errmsg;

    // Create a table with the given table_name.
    // Return the sqlite return code.
    // In case of error, the error is stored in _errmsg.
    void create_table(std::string_view table_name);

    // Prepare the hardcoded statements from _statement_sources.
    // Return SQLITE_OK or the error returned by sqlite in case of error.
    // Prepared statements are stored in the _prepared_statements internal map.
    void prepare_statements();
};

} // namespace zouipocar::db

#endif
