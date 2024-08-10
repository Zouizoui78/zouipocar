#include "gtest/gtest.h"
#include "test_tools.hpp"

namespace zouipocar_test {

void compare_fixes(const zouipocar::Fix& first, const zouipocar::Fix& second) {
    EXPECT_EQ(first.timestamp, second.timestamp);
    EXPECT_EQ(first.speed, second.speed);
    EXPECT_NEAR(first.latitude, second.latitude, 1e-5);
    EXPECT_NEAR(first.longitude, second.longitude, 1e-5);
}

}