/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/adc.h"

#define LED_PIN 0

#define ADC_FLAG 101
#define LED_ON_FLAG 102
#define LED_OFF_FLAG 103
#define FLAG_COMPLETE 104

volatile int adc_val = -1;

// Initialize the GPIO for the LED
void pico_led_init(void) {
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
}

void adc_setup(void){
    // ADC
    adc_init(); // init the adc module
    adc_gpio_init(26); // set ADC0 pin to be adc input instead of GPIO
    adc_select_input(0); // select to read from ADC0
}

// Turn the LED on or off
void pico_set_led(bool led_on) {
gpio_put(LED_PIN, led_on);
}

void core1_entry() {

    pico_led_init();
    adc_setup();


    while(1){

        uint32_t g = multicore_fifo_pop_blocking();

        if (g == LED_ON_FLAG){
            pico_set_led(1);
        }
        else if (g == LED_OFF_FLAG){
            pico_set_led(0);
        }
        else if (g == ADC_FLAG){
            adc_val = adc_read();
        }

        multicore_fifo_push_blocking(FLAG_COMPLETE);
    }
}

int main() {
    stdio_init_all();

    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }

    printf("Hello, multicore!\n");

    /// \tag::setup_multicore[]

    multicore_launch_core1(core1_entry);

    // Wait for it to start up

    while (1){
        int input;
        printf("Input 0 for voltage, 1 for LED on, 2 for LED off: ");
        scanf("%d", &input);

        if(input == 0){
            printf("\nReading Voltage...\n");
            multicore_fifo_push_blocking(ADC_FLAG);
            uint32_t g = multicore_fifo_pop_blocking();
            printf("Operation Complete! Voltage is %.3f\n", (3.3/4095)*(adc_val));
        }
        else if (input == 1){
            printf("\nTurning LED On...\n");
            multicore_fifo_push_blocking(LED_ON_FLAG);
            uint32_t g = multicore_fifo_pop_blocking();
            printf("Operation Complete!\n");
            
        }
        else if (input == 2){
            printf("\nTurning LED Off...\n");
            multicore_fifo_push_blocking(LED_OFF_FLAG);
            uint32_t g = multicore_fifo_pop_blocking();
            printf("Operation Complete!\n");
            
        }
        else{
            printf("\nInvalid input.\n");
        }


    }


    /// \end::setup_multicore[]
}
