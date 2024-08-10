#include "time_utils.h"
#include <time.h>

uint32_t time_from_gps(uint32_t time, uint32_t date) {
    // time and date are both represented with a 4 bytes integer
    // e.g. 01/02/2020 = 010220
    // and 18:56:12 = 185612

    // See tm definition for reasons behind the +100, -1, etc.
    struct tm timeinfo;
    timeinfo.tm_year = (date % 100) + 100;
    timeinfo.tm_mon = (date / 100) % 100 - 1;
    timeinfo.tm_mday = (date / 10000) % 100;
    timeinfo.tm_hour = time / 10000;
    timeinfo.tm_min = (time / 100) % 100;
    timeinfo.tm_sec = time % 100;

    // mk_gmtime convert to AVR timestamp (from 01/01/2000 00:00:00)
    // so we need to add an offset to convert to UNIX timestamp.
    return mk_gmtime(&timeinfo) + UNIX_OFFSET;
}