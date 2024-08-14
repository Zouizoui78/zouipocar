#include "at.h"
#include "../../common/common_constants.h"
#include "constants.h"
#include "gps.h"
#include "mc60.h"
#include "return_codes.h"
#include "uart.h"

#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>

#define WAIT_FAIL 1000

int CMD_RES = -1;
#define LOOP_UNTIL_VALUE(_FUNCTION, _EXPECTED_VALUE, _WAIT) \
    CMD_RES = _FUNCTION; \
    while (CMD_RES != _EXPECTED_VALUE) { \
        _delay_ms(_WAIT); \
        CMD_RES = _FUNCTION; \
    }

int main(void) {
    sei();
    setup_pins();
    uart_init(BAUDRATE, FOSC, UART_TIMEOUT_INTERRUPT);

    LOOP_UNTIL_VALUE(disable_echo(), AT_OK, WAIT_FAIL);
    LOOP_UNTIL_VALUE(set_ip_multiplexing(0), AT_OK, WAIT_FAIL);
    LOOP_UNTIL_VALUE(set_sms_text_mode(), AT_OK, WAIT_FAIL);
    LOOP_UNTIL_VALUE(udp_use_domain(), AT_OK, WAIT_FAIL);

    // We follow the steps from Quectel_MC60_GNSS_AGPS_Application_Note_V1.1
    // Operation Processes of EPO Function (Type B)
    // - Set APN (no need to set the PDP context for some reason)
    // - Wait for network connection
    // - Enable NTP (in case NITZ is not available)
    // - Wait for time synchronization
    // - Enable EPO
    // - Enable GPS
    LOOP_UNTIL_VALUE(set_apn(APN), AT_OK, WAIT_FAIL);
    LOOP_UNTIL_VALUE(check_network_status(), AT_NETSTATE_REGISTERED, WAIT_FAIL);
    LOOP_UNTIL_VALUE(enable_ntp(), AT_OK, WAIT_FAIL);

    LOOP_UNTIL_VALUE(enable_epo(), AT_OK, WAIT_FAIL);
    LOOP_UNTIL_VALUE(enable_gps(), AT_OK, WAIT_FAIL);

    char port[5];
    snprintf(port, 5, "%d", ZOUIPOCAR_PORT);

    char rmc[NMEA_SENTENCE_MAX_SIZE];
    Fix fix;
    uint8_t connection_lost_sms_sent = 1;
    uint8_t trying_to_connect_sms_sent = 1;
    uint8_t connected_sms_sent = 0;
    while (1) {
        int ip_status = get_ip_status();
        while (ip_status != AT_IPSTATE_CONNECT_OK) {
            connected_sms_sent = 0;
            if (connection_lost_sms_sent == 0) {
                send_sms(PHONE, "Not connected to server");
                connection_lost_sms_sent = 1;
            }

            if (ip_status != AT_IPSTATE_INITIAL) {
                if (trying_to_connect_sms_sent == 0) {
                    send_sms(PHONE, "Trying to enter IP INITIAL state");
                    trying_to_connect_sms_sent = 1;
                }
                // This should put the system back in IP INITIAL
                // state from which we can establish connection.
                LOOP_UNTIL_VALUE(udp_deact(), AT_OK, WAIT_FAIL);
                _delay_ms(1000);
            }

            // Here we should be in a state from
            // which we can establish connection.

            udp_open(SERVER, port);
            _delay_ms(1000);
            ip_status = get_ip_status();
        }

        if (connected_sms_sent == 0) {
            send_sms(PHONE, "Connected to server");
            connected_sms_sent = 1;
            connection_lost_sms_sent = 0;
            trying_to_connect_sms_sent = 0;
        }

        if (get_gps_rmc(rmc) != AT_OK) {
            continue;
        }

        if (process_rmc(rmc, &fix) == AT_OK) {
            udp_send((uint8_t*)&fix, sizeof (Fix));
        }
        memset(rmc, 0, NMEA_SENTENCE_MAX_SIZE);
        memset(&fix, 0, sizeof (Fix));

        _delay_ms(1000);
    }

    return 0;
}