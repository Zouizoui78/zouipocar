#ifndef TEST_TOOLS_HPP
#define TEST_TOOLS_HPP

#include "Fix.hpp"

namespace zouipocar_test {

// Compare each of the fixes members.
void expect_fix_eq(const Fix &first, const Fix &second);

} // namespace zouipocar_test

#endif