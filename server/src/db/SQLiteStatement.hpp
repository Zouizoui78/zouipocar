#ifndef SQLITESTATEMENT_HPP
#define SQLITESTATEMENT_HPP

#include <memory>
#include <string>

#include "sqlite3.h"

namespace zouipocar::db {

class SQLiteStatement {
public:
    /////// Statement lifecycle management functions ///////
    SQLiteStatement(sqlite3 *db_handle, std::string_view source);

    template <typename T> bool bind_arg(int index, T arg) const {
        int res = 0;

        if constexpr (std::is_integral_v<T>) {
            res = sqlite3_bind_int(this->get(), index, arg);
        }
        else if constexpr (std::is_floating_point_v<T>) {
            res = sqlite3_bind_double(this->get(), index, arg);
        }
        else {
            throw std::runtime_error("Unsupported type to bind to statement");
        }

        return res == SQLITE_OK;
    }

    int step() const;

    template <typename T> int step_until_done(T &&callback) const {
        int count = 0;
        int res = 0;

        auto stmt = this->get();

        for (res = sqlite3_step(stmt); res == SQLITE_ROW;
             res = sqlite3_step(stmt)) {
            callback(*this);
            ++count;
        }

        if (res != SQLITE_DONE) {
            return -1;
        }

        return count;
    }

    bool reset() const;
    ////////////////////////////////////////////////////////

    std::string_view get_source() const;
    sqlite3_stmt *get() const;

private:
    std::string _source;

    struct SQLiteStatementDeleter {
        void operator()(sqlite3_stmt *stmt);
    };
    std::unique_ptr<sqlite3_stmt, SQLiteStatementDeleter> _prepared;
};

} // namespace zouipocar::db

#endif
