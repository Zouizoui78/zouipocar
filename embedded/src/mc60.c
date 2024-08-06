#include "mc60.h"

void setup_pins(void) {
    // Output pins.
    // DDRB = _BV(PWRKEY) | _BV(GSMON) | _BV(LED);
    pin_configure(PWRKEY, OUTPUT);
    pin_configure(GSMON, OUTPUT);
    pin_configure(LED, OUTPUT);
}

void set_pwrkey(void) {
    pin_set(PWRKEY);
}

void clear_pwrkey(void) {
    pin_clear(PWRKEY);
}

uint8_t get_pwrkey(void) {
    return pin_read(PWRKEY);
}

uint8_t toggle_pwrey(void){
    return pin_toggle(PWRKEY);
}

void set_gsmon(void) {
    pin_set(GSMON);
}

void clear_gsmon(void) {
    pin_clear(GSMON);
}

uint8_t get_gsmon(void) {
    return pin_read(GSMON);
}

uint8_t toggle_gsmon(void){
    return pin_toggle(GSMON);
}

void set_led(void) {
    pin_set(LED);
}

void clear_led(void) {
    pin_clear(LED);
}

uint8_t get_led(void) {
    return pin_read(LED);
}

uint8_t toggle_led(void){
    return pin_toggle(LED);
}