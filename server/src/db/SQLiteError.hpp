#ifndef SQLITEERROR_HPP
#define SQLITEERROR_HPP

#include <string>

namespace zouipocar::db {

struct SQLiteError {
    int code;
    std::string message;
};

} // namespace zouipocar::db

#endif
