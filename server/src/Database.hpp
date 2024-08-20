#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Fix.hpp"
#include "sqlite3.h"

namespace zouipocar {

class Database {
public:
    Database(std::string_view path);

    // Return false if insertion fails, true otherwise.
    bool insert_fix(Fix fix);

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
    struct Sqlite3Deleter {
        void operator()(sqlite3 *handle);
    };
    std::unique_ptr<sqlite3, Sqlite3Deleter> _handle;

    char *_errmsg = nullptr;

    void create_table();

    // Return the number of affected rows, or -1 in case of error.
    template <typename T> int query(const std::string &statement, T &&callback);
};

} // namespace zouipocar

#endif