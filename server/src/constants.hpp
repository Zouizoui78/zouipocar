#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <string>

namespace zouipocar {

constexpr int PORT = 3001;
constexpr int PACKET_SIZE = 15;
constexpr std::string_view DB_PATH = "zouipocar.db";
constexpr std::string_view WEB_UI_PATH = "www";

}

#endif