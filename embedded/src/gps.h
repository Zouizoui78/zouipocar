#ifndef GPS
#define GPS

#include <stdint.h>
#include <stddef.h>

#include "return_codes.h"

#define GPS_UDP_PACKET_SIZE

int process_rmc(char *rmc, uint8_t *output);

#endif // GPS