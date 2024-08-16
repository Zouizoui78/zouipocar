#ifndef GPS
#define GPS

#include "fix.h"

// output must be allocated, this function only sets its values.
int get_gps_data(Fix *output);

#endif