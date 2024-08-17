#ifndef FIX_HPP
#define FIX_HPP

#include <cstdint>
#include <vector>

#include "../../common/Fix.h"

namespace zouipocar {

Fix make_fix(const std::vector<uint8_t> &packet);

}

#endif
