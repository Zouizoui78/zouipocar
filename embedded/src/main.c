#include "at.h"
#include "constants.h"
#include "gps.h"
#include "interrupt.h"
#include "mc60.h"
#include "uart.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

#define PACKET_SIZE 15
#define WAIT_FAIL 1000

int main(void) {
    sei(); // Enable interrupts.
    setup_pins();
    uart_init(BAUDRATE, FOSC, UART_TIMEOUT_INTERRUPT);

    int res = -1;
    while (res != AT_OK) {
        res = disable_echo();
        if (res != AT_OK)
            _delay_ms(WAIT_FAIL);
    }

    res = -1;
    while (res != AT_OK) {
        res = enable_gps();
        if (res != AT_OK)
            _delay_ms(WAIT_FAIL);
    }

    res = -1;
    while (res != AT_NETSTATE_REGISTERED) {
        res = check_network_status();
        if (res != AT_NETSTATE_REGISTERED)
            _delay_ms(WAIT_FAIL);
    }

    res = -1;
    while (res != AT_OK) {
        res = set_sms_text_mode();
        if (res != AT_OK)
            _delay_ms(WAIT_FAIL);
    }

    send_sms(PHONE, "Starting");

    res = -1;
    while (res != AT_OK) {
        res = set_apn(APN);
        if (res != AT_OK)
            _delay_ms(WAIT_FAIL);
    }

    res = -1;
    while (res != AT_OK) {
        res = udp_use_domain();
        if (res != AT_OK)
            _delay_ms(WAIT_FAIL);
    }

    res = -1;
    while (res != AT_GPS_TIME_SYNCED) {
        res = check_time_sync_status();
        if (res != AT_GPS_TIME_SYNCED)
            _delay_ms(WAIT_FAIL);
    }

    res = -1;
    while (res != AT_OK) {
        res = enable_epo();
        if (res != AT_OK)
            _delay_ms(WAIT_FAIL);
    }

    char v[50] = "EPO : ";
    res = -1;
    while (res != AT_OK) {
        res = get_epo_validity(v + 6);
        if (res != AT_OK)
            _delay_ms(WAIT_FAIL);
    }

    send_sms(PHONE, v);

    res = -1;
    while (res != AT_OK) {
        res = set_ip_multiplexing(0);
        if (res != AT_OK)
            _delay_ms(WAIT_FAIL);
    }

    char rmc[80];
    uint8_t packet[PACKET_SIZE];
    uint8_t connection_lost_sms_sent = 0;
    uint8_t trying_to_connect_sms_sent = 0;
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
                udp_deact();
                _delay_ms(1000);
            }

            // Here we should be in a state from
            // which we can establish connection.

            udp_open(SERVER, SERVER_PORT);
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

        if (process_rmc(rmc, packet) == AT_OK) {
            udp_send(packet, PACKET_SIZE);
        }
        memset(rmc, 0, 80);
        memset(packet, 0, PACKET_SIZE);

        _delay_ms(1000);
    }

    return 0;
}