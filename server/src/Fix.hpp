#ifndef FIX_HPP
#define FIX_HPP

#include <stdint.h>
#include <ctime>

namespace zouipocar {

struct Fix {
public:
    Fix() = default;
    Fix(const std::vector<uint8_t>& packet);

    time_t timestamp = 0;
    int64_t speed = 0;
    double latitude = 0;
    double longitude = 0;
};

}

#endif
