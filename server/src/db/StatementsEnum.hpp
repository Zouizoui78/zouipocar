#ifndef STATEMENTSENUM_HPP
#define STATEMENTSENUM_HPP

namespace zouipocar::db {

enum class Statements {
    GET_FIX,
    GET_FIRST_FIX,
    GET_LAST_FIX,
    GET_FIX_RANGE,
    INSERT_FIX,
};

}

#endif
