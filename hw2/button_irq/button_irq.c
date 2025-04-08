// basic code to handle printing to usb and GPIO through interrupts
// Max H
// April 2025


#include <stdio.h>
#include "pico/stdlib.h"


#define LED_PIN 0
#define BUTTON_PIN 15

// Initialize the GPIO for the LED
void pico_led_init(void) {
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
}

void gpio_callback(uint gpio, uint32_t events) {
    static int num_presses = 0;
    static bool state = false;

    state = !state;
    num_presses++;

    gpio_put(LED_PIN, state);
    printf("Button has Been Pressed %d times\n", num_presses);

}

int main() {
    sleep_ms(1000);

    stdio_init_all();
    pico_led_init();
    gpio_init(BUTTON_PIN);
    gpio_set_irq_enabled_with_callback(BUTTON_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    while (true){
    }
}
