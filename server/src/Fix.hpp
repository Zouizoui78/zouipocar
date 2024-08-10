#ifndef FIX_HPP
#define FIX_HPP

#include <stdint.h>
#include <vector>

namespace zouipocar {

struct Fix {
public:
    Fix() = default;
    Fix(const std::vector<uint8_t>& packet);

    uint32_t timestamp = 0;
    int64_t speed = 0;
    float latitude = 0;
    float longitude = 0;
};

}

#endif
