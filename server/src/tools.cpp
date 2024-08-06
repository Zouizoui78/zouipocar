#include "tools.hpp"

using json = nlohmann::json;

time_t string_to_time(const std::string &time, const std::string &format) {
    std::tm tm {};
    strptime(time.c_str(), format.c_str(), &tm);
    return timegm(&tm);
}

json parse_packet(uint8_t *data) {
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

    json j = {
        { "timestamp", string_to_time(timestamp_str, "%d%m%y-%H%M%S") },
        { "speed", speed },
        { "latitude", latitude },
        { "longitude", longitude }
    };

    return j;
}