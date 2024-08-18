#include "Fix.hpp"
#include "../../common/common_constants.h"

#include <cstring>

namespace zouipocar {

Fix deserialize_fix(const std::vector<uint8_t> &packet) {
    Fix fix;
    memcpy(&fix, packet.data(), sizeof(Fix));
    return fix;
}

} // namespace zouipocar
