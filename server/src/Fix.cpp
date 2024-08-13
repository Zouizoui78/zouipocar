#include "../../common/common_constants.h"
#include "Fix.hpp"

#include <cstring>

namespace zouipocar {

Fix make_fix(const std::vector<uint8_t>& packet) {
    Fix fix;
    memcpy(&fix, packet.data(), sizeof (Fix));
    return fix;
}

}
