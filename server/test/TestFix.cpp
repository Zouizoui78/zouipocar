#include "gtest/gtest.h"
#include "Fix.cpp"
#include "test_tools.hpp"

using namespace zouipocar;
using namespace zouipocar_test;

TEST(TestFix, test_parse_packet) {
    uint8_t speed = 123;
    uint32_t time = 180210;
    uint32_t date = 150422;
    float lat = 48.931835;
    float lon = 2.054971;

    std::vector<uint8_t> data;
    data.resize(15);
    data[0] = speed;
    memcpy(data.data() + 1, &time, 3);
    memcpy(data.data() + 4, &date, 3);
    memcpy(data.data() + 7, &lat, 4);
    memcpy(data.data() + 11, &lon, 4);

    Fix fix;
    fix.timestamp = 1650045730;
    fix.speed = speed;
    fix.latitude = lat;
    fix.longitude = lon;

    Fix from_data(data);
    compare_fixes(fix, from_data);
}