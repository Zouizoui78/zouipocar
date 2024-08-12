#include "../../common/common_constants.h"
#include "Fix.hpp"

#include <cstring>

namespace zouipocar::fix {

Fix from_packet(const std::vector<uint8_t>& packet) {
    Fix fix;
    memcpy(&fix, packet.data(), sizeof (Fix));
    return fix;
}

}
