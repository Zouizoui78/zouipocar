#ifndef FIX_H
#define FIX_H

#include <stdint.h>

#pragma pack(push, 1)
struct Fix {
    uint32_t timestamp;
    float latitude;
    float longitude;
    uint8_t speed;
};
#pragma pack(pop)

#endif