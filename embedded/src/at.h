#ifndef AT
#define AT

#include <stdint.h>
#include <stddef.h>

#include "return_codes.h"

#define MAX_PACKET_SIZE 500
#define MAX_STOP_WORD_SIZE 30

typedef struct Response {
    int8_t ret_code;
    char data[MAX_PACKET_SIZE];
    size_t data_length;
} Response;

/**
 * @brief Search in str if it contains a string of stop_words.
 * The function returns the index of the first found occurence.
 * 
 * @param str String to search into.
 * @param stop_words Stop words information.
 * @return int Index of the first found word ; -1 if none.
 */
int search_stop_words(char *str, char stop_words[][MAX_STOP_WORD_SIZE], uint8_t stop_words_size);

void cmd_raw_data(uint8_t *data, uint8_t data_size, Response *response, char stop_words[][MAX_STOP_WORD_SIZE], uint8_t stop_words_size, uint16_t timeout);

void cmd(char *cmd, Response *response, char stop_words[][MAX_STOP_WORD_SIZE], uint8_t stop_words_size, uint16_t timeout);

int simple_cmd(char *cmd);

int disable_echo(void);

int set_apn(char *apn_url);
int set_dns_server(char *addr);
int dns_resolve(char *domain);
int set_pdp_context(int context);

int check_network_status(void);
int check_time_sync_status(void);

int set_ip_multiplexing(uint8_t state);
int get_ip_status(void);
int udp_use_domain(void);
int udp_open(char *addr, char *port);
int udp_close(void);
int udp_send(uint8_t *data, uint8_t data_size);
int udp_deact(void);

int set_sms_reception_mode(void);
int set_sms_text_mode(void);
int send_sms(char *phone_number, char *text);
int clear_sms_memory(void);

int enable_gps(void);
int enable_epo(void);
int remove_epo_data(void);
int get_epo_validity(char *output);
int get_gps_rmc(char *rmc);

#endif // AT