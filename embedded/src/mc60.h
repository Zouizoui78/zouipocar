#ifndef MC60
#define MC60

#include "pin.h"

/////////////////////
// Pins definition //
/////////////////////

// ???
#define PWRKEY D7

// Must be HIGH to enable communication with MC60
#define GSMON B2

// Built-in Uno led.
#define LED B5

void setup_pins(void);

void set_pwrkey(void);
void clear_pwrkey(void);
uint8_t get_pwrkey(void);
uint8_t toggle_pwrey(void);

void set_gsmon(void);
void clear_gsmon(void);
uint8_t get_gsmon(void);
uint8_t toggle_gsmon(void);

void set_led(void);
void clear_led(void);
uint8_t get_led(void);
uint8_t toggle_led(void);

#endif // MC60