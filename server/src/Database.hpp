#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <functional>
#include <optional>
#include <string>
#include <vector>

#include "Fix.hpp"
#include "sqlite3.h"

namespace zouipocar {

class Database {
public:
    using DBQueryCallback = std::function<void (sqlite3_stmt*)>;

    Database();
    ~Database() noexcept;

    Database(const Database& other) = delete;
    Database(Database&& other) = delete;
    Database& operator=(const Database& other) = delete;
    Database& operator=(Database&& other) = delete;

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
    sqlite3 *_db_handler = nullptr;
    char *_errmsg = nullptr;

    void create_table();
    bool query(const std::string &statement, DBQueryCallback callback);
};

}

#endif // DATABASE_HPP