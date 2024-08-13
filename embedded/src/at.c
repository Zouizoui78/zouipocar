#include "at.h"
#include "return_codes.h"
#include "uart.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_TIMEOUT 5

// Global because there is only one thread
// and it's easier that way
static Response res;

int index_of(char *str, char *search) {
    char *addr = strstr(str, search);
    if (addr == NULL) return -1;
    else return addr - str;
}

int search_stop_words(char *str, char stop_words[][MAX_STOP_WORD_SIZE], uint8_t stop_words_size) {
    for (int i = 0 ; i < stop_words_size ; ++i) {
        if (index_of(str, stop_words[i]) != -1)
            return i;
    }
    return -1;
}

void reset_response(Response *res) {
    res->ret_code = -1;
    memset(res->data, '\0', MAX_PACKET_SIZE);
    res->data_length = 0;
}

void get_response(Response *response, char stop_words[][MAX_STOP_WORD_SIZE], uint8_t stop_words_size, uint16_t timeout) {
    reset_response(&res);
    while (response->ret_code == -1) {
        uint8_t ret = uart_read_string(response->data, &(response->data_length), 10, MAX_PACKET_SIZE, timeout);
        if (ret == UART_TIMEOUT) {
            response->ret_code = -1;
            return;
        }
        response->ret_code = search_stop_words(response->data, stop_words, stop_words_size);
    }
}

void cmd_raw_data(uint8_t *data, uint8_t data_size, Response *response, char stop_words[][MAX_STOP_WORD_SIZE], uint8_t stop_words_size, uint16_t timeout) {
    uart_flush();
    uart_write_array(data, data_size);
    get_response(response, stop_words, stop_words_size, timeout);
}

void cmd(char *cmd, Response *response, char stop_words[][MAX_STOP_WORD_SIZE], uint8_t stop_words_size, uint16_t timeout) {
    cmd_raw_data((uint8_t *)cmd, strlen(cmd), response, stop_words, stop_words_size, timeout);
}

int simple_cmd(char *cmdstr) {
    char stop_words[2][MAX_STOP_WORD_SIZE] = {
        "OK", "ERROR"
    };

    cmd(cmdstr, &res, stop_words, 2, DEFAULT_TIMEOUT);
    return res.ret_code;
}

int disable_echo(void) {
    return simple_cmd("ATE0\r");
}

int set_apn(char *apn_url) {
    char str[50] = "AT+QICSGP=1,\"";
    strcat(str, apn_url);
    strcat(str, "\",,\r");
    return simple_cmd(str);
}

int set_dns_server(char *addr) {
    char str[50] = "AT+QIDNSCFG=\"";
    strcat(str, addr);
    strcat(str, "\"\r");
    return simple_cmd(str);
}

int dns_resolve(char *domain) {
    char str[50] = "AT+QIDNSGIP=\"";
    strcat(str, domain);
    strcat(str, "\"\r");
    return simple_cmd(str);
}

int set_pdp_context(int context) {
    char str[20] = "AT+QIFGCNT=\0";
    sprintf(str + strlen(str), "%d", context);
    strcat(str, "\r");
    return simple_cmd(str);
}

int check_network_status(void) {
    char *str = "AT+CGREG?\r";
    if (simple_cmd(str) != AT_OK) return AT_ERROR;

    int pos = index_of(res.data, ",");
    if (pos == -1) return AT_ERROR;

    if (res.data[pos + 1] == '0')
        return AT_NETSTATE_NOT_REGISTERED_NOT_SEARCHING;

    else if (res.data[pos + 1] == '1')
        return AT_NETSTATE_REGISTERED;

    else if (res.data[pos + 1] == '2')
        return AT_NETSTATE_NOT_REGISTERED_SEARCHING;

    else if (res.data[pos + 1] == '3')
        return AT_NETSTATE_REGISTRATION_DENIED;

    else if (res.data[pos + 1] == '4')
        return AT_NETSTATE_UNKNOWN;

    else if (res.data[pos + 1] == '5')
        return AT_NETSTATE_REGISTERED;

    else
        return AT_ERROR;
}

int check_time_sync_status(void) {
    char *str = "AT+QGNSSTS?\r";
    if (simple_cmd(str) != AT_OK) return AT_ERROR;

    int pos = index_of(res.data, ": ");
    if (pos == -1) return AT_ERROR;

    if (res.data[pos + 2] == '0')
        return AT_GPS_TIME_NOT_SYNCED;
    else if (res.data[pos + 2] == '1')
        return AT_GPS_TIME_SYNCED;
    else
        return AT_ERROR;
}

int enable_ntp(void) {
    char *cmdstr = "AT+QNTP=\"time.nist.gov\",123\r";
    char stop_words[2][MAX_STOP_WORD_SIZE] = {
        "OK", "ERROR"
    };
    cmd(cmdstr, &res, stop_words, 2, 120);

    int pos = index_of(res.data, ": ");
    if (pos == -1) return AT_ERROR;

    switch (res.data[pos + 2]) {
        case '0': return AT_OK;
        case '1': return AT_NTP_UNKNOWN_FAIL;
        case '2': return AT_NTP_NO_RESPONSE;
        case '3': return AT_NTP_TCP_STACK_BUSY;
        case '4': return AT_NTP_SERVER_NOT_FOUND;
        case '5': return AT_NTP_PODP_CONTEXT_FAILED;
        default: return AT_ERROR;
    }
}

int set_ip_multiplexing(uint8_t state) {
    if (state > 1) {
        state = 1;
    }
    char str[30] = "AT+QIMUX=\0";
    sprintf(str + strlen(str), "%d", state);
    strcat(str, "\r");
    return simple_cmd(str);
}

int get_ip_status(void) {
    char stop_words[2][MAX_STOP_WORD_SIZE] = {
        "STATE", "ERROR"
    };

    cmd("AT+QISTATE\r", &res, stop_words, 2, DEFAULT_TIMEOUT);
    if (res.ret_code == 1)
        return AT_ERROR;

    int start_pos = index_of(res.data, ": ");
    if (start_pos == -1)
        return AT_ERROR;
    start_pos += 2;

    int end_pos = index_of(res.data + start_pos, "\r") + start_pos;
    if (end_pos == -1)
        return AT_ERROR;

    int status_length = end_pos - start_pos;
    char status[50];
    strncpy(status, res.data + start_pos, status_length);
    status[status_length] = '\0';

    if (strcmp(status, "IP INITIAL") == 0)
        return AT_IPSTATE_INITIAL;

    else if (strcmp(status, "IP START") == 0)
        return AT_IPSTATE_START;

    else if (strcmp(status, "IP CONFIG") == 0)
        return AT_IPSTATE_CONFIG;

    else if (strcmp(status, "IP IND") == 0)
        return AT_IPSTATE_IND;

    else if (strcmp(status, "IP GPRSACT") == 0)
        return AT_IPSTATE_GPRSACT;

    else if (strcmp(status, "IP STATUS") == 0)
        return AT_IPSTATE_STATUS;

    else if (strcmp(status, "TCP CONNECTING") == 0)
        return AT_IPSTATE_TCP_CONNECTING;

    else if (strcmp(status, "UDP CONNECTING") == 0)
        return AT_IPSTATE_UDP_CONNECTING;

    else if (strcmp(status, "IP CLOSE") == 0)
        return AT_IPSTATE_CLOSE;

    else if (strcmp(status, "CONNECT OK") == 0)
        return AT_IPSTATE_CONNECT_OK;

    else if (strcmp(status, "PDP DEACT") == 0)
        return AT_IPSTATE_PDP_DEACT;

    else
        return AT_ERROR;
}

int udp_use_domain(void) {
    return simple_cmd("AT+QIDNSIP=1\r");
}

int udp_open(char *addr, char *port) {
    char str[50] = "AT+QIOPEN=\"UDP\",\"";
    strcat(str, addr);
    strcat(str, "\",");
    strcat(str, port);
    strcat(str, "\r");

    char stop_words[4][MAX_STOP_WORD_SIZE] = {
        "ALREADY CONNECT", "CONNECT OK", "CONNECT FAIL", "ERROR"
    };

    cmd(str, &res, stop_words, 4, 75);
    int8_t ret = res.ret_code;
    if (ret == 0 || ret == 1)
        return AT_OK;
    else if (ret == 2) return AT_UDP_CONNECT_FAIL;
    else return AT_ERROR;
}

int udp_close(void) {
    return simple_cmd("AT+QICLOSE\r");
}

int udp_deact(void) {
    char stop_words[2][MAX_STOP_WORD_SIZE] = {
        "OK", "ERROR"
    };

    cmd("AT+QIDEACT\r", &res, stop_words, 2, 40);
    return res.ret_code;
}

int udp_send(uint8_t *data, uint8_t data_size) {
    char stop_words[3][MAX_STOP_WORD_SIZE] = {
        "OK", "ERROR", "FAIL"
    };

    uart_flush();
    char str[50] = "AT+QISEND=";
    sprintf(str + strlen(str), "%d", data_size);
    strcat(str, "\r");
    uart_write_string(str);

    // Discard "\r\n" sent by MC60.
    if (uart_read_string(res.data, &(res.data_length), 10, MAX_PACKET_SIZE, DEFAULT_TIMEOUT) == UART_TIMEOUT)
        return AT_ERROR;

    char c[2];
    c[0] = uart_read_byte(DEFAULT_TIMEOUT);
    c[1] = uart_read_byte(DEFAULT_TIMEOUT);
    if (c[0] == 'E' && c[1] == 'R') {
        return AT_ERROR;
    }
    else if (c[0] != '>' || c[1] != ' ')
        return AT_UDP_SEND_FAIL;

    // We got "> ", ok to continue.
    uart_flush();
    cmd_raw_data(data, data_size, &res, stop_words, 3, 10);
    int ret = res.ret_code;

    if (ret == 0) return AT_OK;
    else if (ret == 1) return AT_ERROR;
    else if (ret == 2) return AT_UDP_SEND_FAIL;
    else return AT_ERROR;
}

int enable_gps(void) {
    char stop_words[3][MAX_STOP_WORD_SIZE] = {
        "OK", "7103", "ERROR"
    };

    cmd("AT+QGNSSC=1\r", &res, stop_words, 3, DEFAULT_TIMEOUT);
    if (res.ret_code == 2) {
        return AT_ERROR;
    }
    return AT_OK;
}

int set_sms_reception_mode(void) {
    return simple_cmd("AT+CNMI=2,2\r");
}

int set_sms_text_mode(void) {
    return simple_cmd("AT+CMGF=1\r");
}

int send_sms(char *phone_number, char *text) {
    uart_flush();
    char str[50] = "AT+CMGS=\"";
    strcat(str, phone_number);
    strcat(str, "\"\r");
    uart_write_string(str);

    // Discard "\r\n" sent by MC60.
    if (uart_read_string(res.data, &(res.data_length), 10, MAX_PACKET_SIZE, DEFAULT_TIMEOUT) == UART_TIMEOUT)
        return AT_ERROR;

    char c[2];
    c[0] = uart_read_byte(DEFAULT_TIMEOUT);
    c[1] = uart_read_byte(DEFAULT_TIMEOUT);
    if (c[0] == 'E' && c[1] == 'R') {
        return AT_ERROR;
    }
    else if (c[0] != '>' || c[1] != ' ')
        return AT_SMS_SEND_FAIL;

    // We got "> ", ok to continue.
    uart_flush();
    uart_write_string(text);
    char termination[2];
    termination[0] = 26;

    char stop_words[2][MAX_STOP_WORD_SIZE] = {
        "OK", "ERROR"
    };

    cmd(termination, &res, stop_words, 2, 120);

    if (res.ret_code == 0) return AT_OK;
    else return AT_SMS_SEND_FAIL;
}

int clear_sms_memory(void) {
    return simple_cmd("AT+QMGDA=\"DEL ALL\"");
}

int enable_epo(void) {
    int epo_enable = simple_cmd("AT+QGNSSEPO=1\r");
    int epo_trigger = simple_cmd("AT+QGEPOAID\r");

    if (epo_enable == AT_OK && epo_trigger == AT_OK)
        return AT_OK;
    else
        return AT_ERROR;
}

int remove_epo_data(void) {
    return simple_cmd("AT+QGEPOF=1,255\r");
}

int get_epo_validity(char *output) {
    if (simple_cmd("AT+QGEPOF=2\r") != AT_OK) return AT_ERROR;

    int pos = index_of(res.data, ": ") + 2;
    if (pos == -1) return AT_ERROR;
    memcpy(output, res.data + pos, 19);
    output[19] = '\0';
    return AT_OK;
}

int get_gps_rmc(char *rmc) {
    if (simple_cmd("AT+QGNSSRD=\"NMEA/RMC\"\r") != AT_OK)
        return AT_ERROR;
    int start = index_of(res.data, "$");
    int length = index_of(res.data, "*") - start + 3;
    memcpy(rmc, res.data + start, length);
    rmc[length] = '\0';
    return AT_OK;
}