#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

#define LED_PIN 0 // GP0, Pin 2
#define BUTTON_PIN 15 // GP12, Pin 20
#define MAX_NUM_SAMPLES 100
#define WAIT_MS 100
#define SAMP_FREQ 100

int main() {
    stdio_init_all();

    // GPIO
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);

    // ADC
    adc_init(); // init the adc module
    adc_gpio_init(26); // set ADC0 pin to be adc input instead of GPIO
    adc_select_input(0); // select to read from ADC0

    int samp_delay_ms = 1000/SAMP_FREQ;

    gpio_put(LED_PIN, 0);

    while (!stdio_usb_connected()) {
        sleep_ms(WAIT_MS);
    }

    gpio_put(LED_PIN, 1);

    while (gpio_get(BUTTON_PIN) != 0){
        sleep_ms(WAIT_MS);
    }

    gpio_put(LED_PIN, 0);

    int num_samples = 0;
    int samples[MAX_NUM_SAMPLES];

    while (1) {

        printf("input the number of samples: ");
        scanf("%d", &num_samples);
        printf("\nnumber of samples: %d\n", num_samples);
        
        for(int i = 0; i < num_samples; i++){
            samples[i] = adc_read();
            sleep_ms(samp_delay_ms);

        }
        for(int i = 0; i < num_samples; i++){
            printf("Sample %d: %.2f\r\n", i, 3.3*samples[i]/4095.0);
            samples[i] = adc_read();
            sleep_ms(samp_delay_ms);

        }
    }
}