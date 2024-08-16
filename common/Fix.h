#ifndef FIX_H
#define FIX_H

#include <stdint.h>

#pragma pack(push, 1)
struct Fix {
    uint32_t timestamp;
    float latitude;
    float longitude;
    uint8_t speed;
    uint8_t satellites;
    float altitude;
};
#pragma pack(pop)

#endif