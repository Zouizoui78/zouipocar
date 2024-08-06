#ifndef TOOLS_HPP
#define TOOLS_HPP

#include <ctime>
#include <string>

#include "json.hpp"

time_t string_to_time(const std::string &time, const std::string &format);

nlohmann::json parse_packet(uint8_t *data);

#endif // TOOLS_HPP