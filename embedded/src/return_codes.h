#ifndef RETCODES
#define RETCODES

typedef enum ATRetCode {
    AT_OK,
    AT_ERROR,
    AT_UDP_CONNECT_FAIL,
    AT_UDP_SEND_FAIL,
    AT_GPS_FAIL_PARSE,
    AT_GPS_FAIL_TIME,
    AT_GPS_FAIL_VALID,
    AT_GPS_FAIL_LATITUDE,
    AT_GPS_FAIL_LAT_DIRECTION,
    AT_GPS_FAIL_LONGITUDE,
    AT_GPS_FAIL_LONG_DIRECTION,
    AT_GPS_FAIL_SPEED,
    AT_GPS_FAIL_DATE,
    AT_GPS_FAIL_COURSE,
    AT_GPS_TIME_SYNCED,
    AT_GPS_TIME_NOT_SYNCED,
    AT_IPSTATE_INITIAL,
    AT_IPSTATE_START,
    AT_IPSTATE_CONFIG,
    AT_IPSTATE_IND,
    AT_IPSTATE_GPRSACT,
    AT_IPSTATE_STATUS,
    AT_IPSTATE_TCP_CONNECTING,
    AT_IPSTATE_UDP_CONNECTING,
    AT_IPSTATE_CLOSE,
    AT_IPSTATE_CONNECT_OK,
    AT_IPSTATE_PDP_DEACT,
    AT_NETSTATE_NOT_REGISTERED_NOT_SEARCHING,
    AT_NETSTATE_REGISTERED,
    AT_NETSTATE_NOT_REGISTERED_SEARCHING,
    AT_NETSTATE_REGISTRATION_DENIED,
    AT_NETSTATE_UNKNOWN,
    AT_SMS_SEND_FAIL,
    AT_NTP_UNKNOWN_FAIL,
    AT_NTP_NO_RESPONSE,
    AT_NTP_TCP_STACK_BUSY,
    AT_NTP_SERVER_NOT_FOUND,
    AT_NTP_PODP_CONTEXT_FAILED
} ATRetCode;

#endif