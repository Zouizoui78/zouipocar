#include "Fix.hpp"

#include <cstring>
#include <string>

namespace zouipocar {

Fix::Fix(const std::vector<uint8_t>& packet) {
    const uint8_t* data = packet.data();
    uint8_t speed = data[0];

    uint32_t time = 0;
    memcpy(&time, data + 1, 3);

    uint32_t date = 0;
    memcpy(&date, data + 4, 3);

    std::string date_str = std::to_string(date);
    if (date_str.length() == 5)
        date_str = "0" + date_str;

    std::string time_str = std::to_string(time);
    if (time_str.length() == 5)
        time_str = "0" + time_str;

    std::string timestamp_str = date_str + "-" + time_str;

    float latitude = 0;
    memcpy(&latitude, data + 7, 4);

    float longitude = 0;
    memcpy(&longitude, data + 11, 4);
}

}
