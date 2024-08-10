#include "gtest/gtest.h"
#include "Fix.cpp"
#include "test_tools.hpp"

using namespace zouipocar;
using namespace zouipocar_test;

TEST(TestFix, test_parse_packet) {
    std::cout << sizeof(uint32_t) << std::endl;
    uint32_t timestamp = 1723276072;
    float lat = 48.931835;
    float lon = 2.054971;
    uint8_t speed = 123;

    std::vector<uint8_t> data;
    data.resize(ZOUIPOCAR_PACKET_SIZE);
    uint8_t* buffer = data.data();
    memcpy(buffer + ZOUIPOCAR_TIMESTAMP_OFFSET, &timestamp, ZOUIPOCAR_TIMESTAMP_SIZE);
    memcpy(buffer + ZOUIPOCAR_LATITUDE_OFFSET, &lat, ZOUIPOCAR_LATITUDE_SIZE);
    memcpy(buffer + ZOUIPOCAR_LONGITUDE_OFFSET, &lon, ZOUIPOCAR_LONGITUDE_SIZE);
    memcpy(buffer + ZOUIPOCAR_SPEED_OFFSET, &speed, ZOUIPOCAR_SPEED_SIZE);

    Fix fix;
    fix.timestamp = timestamp;
    fix.latitude = lat;
    fix.longitude = lon;
    fix.speed = speed;

    Fix from_data(data);
    compare_fixes(fix, from_data);
}