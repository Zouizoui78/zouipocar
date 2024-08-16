#ifndef GPS
#define GPS

#include "fix.h"

// output must be allocated, these functions only sets its values.

int process_rmc(char *src, Fix *output);
int process_gga(char *src, Fix *output);

#endif