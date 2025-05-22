#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "ssd1306.h"
#include "font.h"


#define LED_PIN 0

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 4
#define I2C_SCL 5

void draw_char(unsigned char x, unsigned char y, unsigned char letter){
    for(int i = 0; i < 5; i++){
        int col = ASCII[letter-32][i];
        for(int j = 0; j < 8; j++){
            int val = (col >> j) & 0b1;
            ssd1306_drawPixel(x + i, y + j, val);    
        }
    }
}

void draw_str(unsigned char x, unsigned char y, unsigned char* str){
    char* curr_char = str;
    unsigned char curr_x = x;
    unsigned char curr_y = y;

    while(*curr_char != 0){
        draw_char(curr_x, curr_y, *curr_char);
        curr_char += 1;
        curr_x += 6;
        if(curr_x > 128-5){
           curr_x = x;
           curr_y += 8;

        }
    }
}

int main()
{
    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // ADC
    adc_init(); // init the adc module
    adc_gpio_init(26); // set ADC0 pin to be adc input instead of GPIO
    adc_select_input(0); // select to read from ADC0

    // OLED
    ssd1306_setup();
    ssd1306_clear();
    ssd1306_update();
    

    while (true) {
        printf("test\n");
        char output_str[200];
        char freq_str[20];
        uint64_t time_elapsed_us, last_time;

        time_elapsed_us = time_us_64() - last_time;
        last_time = time_us_64();

        unsigned char blink = (last_time/1000000)%2;
        gpio_put(LED_PIN, blink);
        ssd1306_drawPixel(127, 0, blink);

        int adc_val = adc_read();
        sprintf(output_str, "ADC 0: %.3fV", 3.3*adc_val/4095);
        draw_str(0, 0, output_str);

        sprintf(freq_str, "%.1fHz", 1000000.0/time_elapsed_us);
        printf("%.1fHz", 1000000.0/time_elapsed_us);
        draw_str(90, 24, freq_str);

        ssd1306_update();
    }
}
