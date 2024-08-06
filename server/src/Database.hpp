#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Fix.hpp"
#include "sqlite3.h"

namespace zouipocar {

class Database {
public:
    Database();

    // Return false if insertion fails, true otherwise.
    bool insert_fix(Fix fix);

    // Return the fix if found, std::nullopt otherwise.
    std::optional<Fix> get_fix(time_t date);

    // Return a vector containing the fixes if the range is valid,
    // and an empty vector otherwise.
    std::vector<Fix> get_fix_range(time_t start, time_t end);

    // Return first fix if it exists, std::nullopt otherwise.
    std::optional<Fix> get_first_fix();

    // Return latest fix if it exists, std::nullopt otherwise.
    std::optional<Fix> get_last_fix();

private:
    struct Sqlite3Deleter {
        void operator()(sqlite3* handle);
    };
    std::unique_ptr<sqlite3, Sqlite3Deleter> _handle;

    char *_errmsg = nullptr;

    void create_table();

    using DBQueryCallback = std::function<void (sqlite3_stmt*)>;
    bool query(const std::string &statement, DBQueryCallback callback);
};

}

#endif // DATABASE_HPP