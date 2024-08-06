#include "gps.h"

#include <stdlib.h>
#include <string.h>

int process_rmc(char *rmc, uint8_t *output) {
    char *token = NULL;
    char *delim = ",";
    char *end = NULL;
    token = strtok(rmc, delim); // $GNRMC
    if (token == NULL)
        return AT_GPS_FAIL_PARSE;

    token = strtok(NULL, delim); // Timestamp
    uint32_t timestamp = strtol(token, &end, 10);
    if (token == NULL || timestamp == 0)
        return AT_GPS_FAIL_TIMESTAMP;

    token = strtok(NULL, delim); // Validity
    if (token == NULL || strcmp(token, "V") == 0)
        return AT_GPS_FAIL_VALID;

    token = strtok(NULL, delim); // Latitude
    if (token == NULL)
        return AT_GPS_FAIL_LATITUDE;
    float lat = strtod(token, &end);
    int deg = lat / 100;
    lat = deg + (lat - deg * 100) / 60;

    token = strtok(NULL, delim); // Latitude direction
    if (token == NULL)
        return AT_GPS_FAIL_LAT_DIRECTION;
    if (strcmp(token, "S") == 0)
        lat *= -1;

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

    output[0] = speed;
    memcpy(output + 1, &timestamp, 3);
    memcpy(output + 4, &date, 3);
    memcpy(output + 7, &lat, 4);
    memcpy(output + 11, &longitude, 4);

    return AT_OK;
}