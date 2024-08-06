#ifndef FIX_HPP
#define FIX_HPP

#include <stdint.h>
#include <time.h>

namespace zouipocar {

struct Fix {
public:
    time_t timestamp = 0;
    int64_t speed = 0;
    double latitude = 0;
    double longitude = 0;
};

}

#endif