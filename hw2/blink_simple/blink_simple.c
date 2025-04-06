// blink based on example, just changed defines
// Max H 
// April 2025

#include "pico/stdlib.h"

#ifndef LED_DELAY_MS
#define LED_DELAY_MS 1000
#endif

#define LED_PIN 0

// Initialize the GPIO for the LED
void pico_led_init(void) {
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
}

// Turn the LED on or off
void pico_set_led(bool led_on) {
gpio_put(LED_PIN, led_on);
}

int main() {
    pico_led_init();
    while (true) {
        pico_set_led(true);
        sleep_ms(LED_DELAY_MS);
        pico_set_led(false);
        sleep_ms(LED_DELAY_MS);
    }
}
