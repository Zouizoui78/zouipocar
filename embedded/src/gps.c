#include "../../common/common_constants.h"
#include "gps.h"
#include "time_utils.h"
#include "return_codes.h"

#include <stdlib.h>
#include <string.h>

int process_rmc(char *rmc, uint8_t *output) {
    char *token = NULL;
    char *delim = ",";
    char *end = NULL;
    token = strtok(rmc, delim); // $GNRMC
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

    uint32_t timestamp = time_from_gps(time, date);

    memcpy(output + ZOUIPOCAR_TIMESTAMP_OFFSET, &timestamp, ZOUIPOCAR_TIMESTAMP_SIZE);
    memcpy(output + ZOUIPOCAR_LATITUDE_OFFSET, &lat, ZOUIPOCAR_LATITUDE_SIZE);
    memcpy(output + ZOUIPOCAR_LONGITUDE_OFFSET, &longitude, ZOUIPOCAR_LONGITUDE_SIZE);
    memcpy(output + ZOUIPOCAR_SPEED_OFFSET, &speed, ZOUIPOCAR_SPEED_SIZE);

    return AT_OK;
}