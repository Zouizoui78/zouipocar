#include "gps.h"
#include "at.h"
#include "constants.h"
#include "return_codes.h"
#include "time_utils.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

int process_rmc(char *src, Fix *output) {
    char *token = NULL;
    char *delim = ",";
    char *end = NULL;
    token = strtok(src, delim); // $GNRMC
    if (token == NULL) {
        return AT_GPS_FAIL_PARSE;
    }

    token = strtok(NULL, delim); // Time
    if (token == NULL) {
        return AT_GPS_FAIL_PARSE;
    }

    uint32_t time = strtol(token, &end, 10);
    if (errno == ERANGE) {
        return AT_GPS_FAIL_TIME;
    }

    token = strtok(NULL, delim); // Validity
    if (token == NULL) {
        return AT_GPS_FAIL_PARSE;
    }

    if (strcmp(token, "V") == 0) {
        return AT_GPS_FAIL_VALID;
    }

    token = strtok(NULL, delim); // Latitude
    if (token == NULL) {
        return AT_GPS_FAIL_PARSE;
    }

    float latitude = strtod(token, &end);
    if (errno == ERANGE) {
        return AT_GPS_FAIL_LATITUDE;
    }

    int deg = latitude / 100;
    latitude = deg + (latitude - deg * 100) / 60;

    token = strtok(NULL, delim); // Latitude direction
    if (token == NULL) {
        return AT_GPS_FAIL_PARSE;
    }

    if (strcmp(token, "S") == 0) {
        latitude *= -1;
    }

    token = strtok(NULL, delim); // Longitude
    if (token == NULL) {
        return AT_GPS_FAIL_PARSE;
    }

    float longitude = strtod(token, &end);
    if (errno == ERANGE) {
        return AT_GPS_FAIL_LONGITUDE;
    }

    deg = longitude / 100;
    longitude = deg + (longitude - deg * 100) / 60;

    token = strtok(NULL, delim); // Longitude direction
    if (token == NULL) {
        return AT_GPS_FAIL_PARSE;
    }

    if (strcmp(token, "W") == 0) {
        longitude *= -1;
    }

    token = strtok(NULL, delim); // Speed (knots)
    if (token == NULL) {
        return AT_GPS_FAIL_PARSE;
    }

    uint8_t speed = strtod(token, &end);
    if (errno == ERANGE) {
        return AT_GPS_FAIL_SPEED;
    }

    // Knots to km/h
    speed *= 1.852;

    token = strtok(NULL, delim); // Course Made Good
    if (token == NULL) {
        return AT_GPS_FAIL_PARSE;
    }

    token = strtok(NULL, delim); // Date
    if (token == NULL) {
        return AT_GPS_FAIL_PARSE;
    }

    uint32_t date = strtol(token, &end, 10);
    if (errno == ERANGE) {
        return AT_GPS_FAIL_DATE;
    }

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
    if (token == NULL) {
        return AT_GPS_FAIL_PARSE;
    }

    token = strtok(NULL, delim); // Time
    if (token == NULL) {
        return AT_GPS_FAIL_PARSE;
    }

    token = strtok(NULL, delim); // Latitude
    if (token == NULL) {
        return AT_GPS_FAIL_PARSE;
    }

    token = strtok(NULL, delim); // Latitude direction
    if (token == NULL) {
        return AT_GPS_FAIL_PARSE;
    }

    token = strtok(NULL, delim); // Longitude
    if (token == NULL) {
        return AT_GPS_FAIL_PARSE;
    }

    token = strtok(NULL, delim); // Longitude direction
    if (token == NULL) {
        return AT_GPS_FAIL_PARSE;
    }

    token = strtok(NULL, delim); // GPS Quality indicator
    if (token == NULL) {
        return AT_GPS_FAIL_PARSE;
    }

    token = strtok(NULL, delim); // Number of satellites
    if (token == NULL) {
        return AT_GPS_FAIL_SATELLITES;
    }

    uint8_t satellites = strtol(token, &end, 10);
    if (errno == ERANGE) {
        return AT_GPS_FAIL_SATELLITES;
    }

    output->satellites = satellites;

    return AT_OK;
}

int get_nmea_data(Fix *output, int (*get_func)(char *),
                  int (*process_func)(char *, Fix *)) {
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