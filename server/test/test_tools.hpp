#ifndef TEST_TOOLS_HPP
#define TEST_TOOLS_HPP

#include "Fix.hpp"

namespace zouipocar_test {

// Compare each of the fixes members.
void compare_fixes(const zouipocar::fix::Fix& first, const zouipocar::fix::Fix& second);

}

#endif