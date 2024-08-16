#include "at.h"
#include "constants.h"
#include "gps.h"
#include "time_utils.h"
#include "return_codes.h"

#include <stdlib.h>
#include <string.h>

int process_rmc(char *src, Fix *output) {
    char *token = NULL;
    char *delim = ",";
    char *end = NULL;
    token = strtok(src, delim); // $GNRMC
    if (token == NULL)
        return AT_GPS_FAIL_PARSE;

    token = strtok(NULL, delim); // Time
    uint32_t time = strtol(token, &end, 10);
    if (token == NULL || time == 0)
        return AT_GPS_FAIL_TIME;

    token = strtok(NULL, delim); // Validity
    if (token == NULL || strcmp(token, "V") == 0)
        return AT_GPS_FAIL_VALID;

    token = strtok(NULL, delim); // Latitude
    if (token == NULL)
        return AT_GPS_FAIL_LATITUDE;
    float latitude = strtod(token, &end);
    int deg = latitude / 100;
    latitude = deg + (latitude - deg * 100) / 60;

    token = strtok(NULL, delim); // Latitude direction
    if (token == NULL)
        return AT_GPS_FAIL_LAT_DIRECTION;
    if (strcmp(token, "S") == 0)
        latitude *= -1;

    token = strtok(NULL, delim); // Longitude
    if (token == NULL)
        return AT_GPS_FAIL_LONGITUDE;
    float longitude = strtod(token, &end);
    deg = longitude / 100;
    longitude = deg + (longitude - deg * 100) / 60;

    token = strtok(NULL, delim); // Longitude direction
    if (token == NULL)
        return AT_GPS_FAIL_LONG_DIRECTION;
    if (strcmp(token, "W") == 0)
        longitude *= -1;

    token = strtok(NULL, delim); // Speed (knots)
    if (token == NULL)
        return AT_GPS_FAIL_SPEED;
    uint8_t speed = strtod(token, &end) * 1.852;

    token = strtok(NULL, delim); // Course Made Good
    if (token == NULL)
        return AT_GPS_FAIL_COURSE;

    token = strtok(NULL, delim); // Date
    if (token == NULL)
        return AT_GPS_FAIL_DATE;
    uint32_t date = strtol(token, &end, 10);
    if (date == 0)
        return AT_GPS_FAIL_DATE;

    uint32_t timestamp = time_from_gps(time, date);

    output->timestamp = timestamp;
    output->latitude = latitude;
    output->longitude = longitude;
    output->speed = speed;

    return AT_OK;
}

int process_gga(char *src, Fix *output) {
    char *token = NULL;
    char *delim = ",";
    char *end = NULL;
    token = strtok(src, delim); // $GNGGA
    if (token == NULL)
        return AT_GPS_FAIL_PARSE;

    token = strtok(NULL, delim); // Time
    token = strtok(NULL, delim); // Latitude
    token = strtok(NULL, delim); // Latitude direction
    token = strtok(NULL, delim); // Longitude
    token = strtok(NULL, delim); // Longitude direction
    token = strtok(NULL, delim); // GPS Quality indicator

    token = strtok(NULL, delim); // Number of satellites
    if (token == NULL)
        return AT_GPS_FAIL_SATELLITES;
    uint8_t satellites = strtol(token, &end, 10);
    if (satellites == 0) {
        return AT_GPS_FAIL_SATELLITES;
    }

    token = strtok(NULL, delim); // Horizontal dilution of precision

    token = strtok(NULL, delim); // Altitude
    if (token == NULL)
        return AT_GPS_FAIL_ALTITUDE;
    float altitude = strtod(token, &end);

    output->satellites = satellites;
    output->altitude = altitude;

    return AT_OK;
}

int get_nmea_data(Fix *output, int (*get_func)(char *), int (*process_func)(char *, Fix *)) {
    char nmea[NMEA_SENTENCE_MAX_SIZE];
    memset(nmea, 0, NMEA_SENTENCE_MAX_SIZE);

    int res = get_func(nmea);
    if (res != AT_OK) {
        return res;
    }

    return process_func(nmea, output);
}

int get_gps_data(Fix *output) {
    int res = get_nmea_data(output, gps_get_rmc, process_rmc);
    if (res != AT_OK) {
        return res;
    }
    return get_nmea_data(output, gps_get_gga, process_gga);
}