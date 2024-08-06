#include "gtest/gtest.h"
#include "tools.hpp"

TEST(TestTools, test_str_to_time) {
    // Testing some formats.
    EXPECT_EQ(string_to_time("2022/04/04 12:12:54", "%Y/%m/%d %H:%M:%S"), 1649074374);
    EXPECT_EQ(string_to_time("010322 010544", "%d%m%y %H%M%S"), 1646096744);
    EXPECT_EQ(string_to_time("020222 020544", "%d%m%y %H%M%S"), 1643767544);
    EXPECT_EQ(string_to_time("020222020544", "%d%m%y%H%M%S"), 1643767544);

    // Testing every days in a month.
    std::string date = "0322 010544";
    std::string date_format = "%d%m%y %H%M%S";
    time_t date_start = 1646096744;

    for (int i = 1 ; i <= 31 ; ++i) {
        std::string to_test = std::to_string(i) + date;
        if (i < 10)
            to_test = "0" + to_test;
        EXPECT_EQ(string_to_time(to_test, date_format), date_start + 86400 * (i - 1));
    }
}

TEST(TestTools, test_parse_packet) {
    uint8_t speed = 123;
    uint32_t time = 180210;
    uint32_t date = 150422;
    float lat = 48.931835;
    float lon = 2.054971;

    uint8_t output[15];
    output[0] = speed;
    memcpy(output + 1, &time, 3);
    memcpy(output + 4, &date, 3);
    memcpy(output + 7, &lat, 4);
    memcpy(output + 11, &lon, 4);

    nlohmann::json j = {
        { "timestamp", 1650045730 },
        { "speed", speed },
        { "latitude", lat },
        { "longitude", lon }
    };

    ASSERT_EQ(j, parse_packet(output));
}