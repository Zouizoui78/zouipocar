#ifndef AT
#define AT

#include <stddef.h>
#include <stdint.h>

int disable_echo(void);

int set_apn(char *apn_url);
int set_dns_server(char *addr);
int dns_resolve(char *domain);
int set_pdp_context(int context);

int check_network_status(void);
int check_time_sync_status(void);
int enable_ntp(void);

// If IP multiplexing is disabled,
// only one connection can exist at a given time.
int set_ip_multiplexing(uint8_t state);
int get_ip_status(void);

// Enable domain names resolution.
int udp_use_domain(void);
int udp_open(char *addr, char *port);
int udp_close(void);
int udp_send(uint8_t *data, uint8_t data_size);

// Bring the TCP stack back to its initial state.
int udp_deact(void);

int set_sms_reception_mode(void);
int set_sms_text_mode(void);
int send_sms(char *phone_number, char *text);
int clear_sms_memory(void);

int enable_gps(void);
int enable_epo(void);
int remove_epo_data(void);
int get_epo_validity(char *output);

int gps_get_rmc(char *output);

#endif