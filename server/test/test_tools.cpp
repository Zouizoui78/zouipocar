#include "gtest/gtest.h"
#include "test_tools.hpp"

namespace zouipocar_test {

void compare_fixes(const zouipocar::Fix& first, const zouipocar::Fix& second) {
    ASSERT_EQ(first.timestamp, second.timestamp);
    ASSERT_EQ(first.speed, second.speed);
    ASSERT_NEAR(first.latitude, second.latitude, 1e-5);
    ASSERT_NEAR(first.longitude, second.longitude, 1e-5);
}

}