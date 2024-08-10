#include "../../common/common_constants.h"
#include "Fix.hpp"

#include <cstring>

namespace zouipocar {

Fix::Fix(const std::vector<uint8_t>& packet) {
    const uint8_t* data = packet.data();
    memcpy(&timestamp, data + ZOUIPOCAR_TIMESTAMP_OFFSET, ZOUIPOCAR_TIMESTAMP_SIZE);
    memcpy(&latitude, data + ZOUIPOCAR_LATITUDE_OFFSET, ZOUIPOCAR_LATITUDE_SIZE);
    memcpy(&longitude, data + ZOUIPOCAR_LONGITUDE_OFFSET, ZOUIPOCAR_LONGITUDE_SIZE);
    memcpy(&speed, data + ZOUIPOCAR_SPEED_OFFSET, ZOUIPOCAR_SPEED_SIZE);
}

}
