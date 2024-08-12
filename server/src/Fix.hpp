#ifndef FIX_HPP
#define FIX_HPP

#include <cstdint>
#include <vector>

namespace zouipocar::fix {

#include "../../common/Fix.h"

Fix from_packet(const std::vector<uint8_t>& packet);

}

#endif
