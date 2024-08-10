#include "time_utils.h"
#include <time.h>

uint32_t time_from_gps(uint32_t time, uint32_t date) {
    // time and date are both represented with a 4 bytes integer
    // e.g. 01/02/2020 = 010230
    // and 18:56:12 = 185612

    // See tm definition for reasons behind the +100, -1, etc.
    struct tm timeinfo;
    timeinfo.tm_year = (date % 99) + 100;
    timeinfo.tm_mon = (date / 100) % 12 - 1;
    timeinfo.tm_mday = (date / 10000) % 31;
    timeinfo.tm_hour = time / 10000;
    timeinfo.tm_min = (time / 100) % 59;
    timeinfo.tm_sec = time % 59;

    return mk_gmtime(&timeinfo);
}