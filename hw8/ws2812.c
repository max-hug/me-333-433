/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"

#define SERVO_PIN 12
#define SERVO_PWM_FREQ 50
#define MIN_SERVO_PULSE_US 350
#define MAX_SERVO_PULSE_US 2800 
#define MIN_SERVO_DUTY_CYCLE MIN_SERVO_PULSE_US*SERVO_PWM_FREQ/1.0e6
#define MAX_SERVO_DUTY_CYCLE MAX_SERVO_PULSE_US*SERVO_PWM_FREQ/1.0e6
#define DIV 100
#define WRAP 150000000/(DIV*SERVO_PWM_FREQ)

#define NUM_PIXELS 4
#define WS2812_PIN 16

void servo_setup(int servo_pin){
    gpio_set_function(servo_pin, GPIO_FUNC_PWM); // Set the LED Pin to be PWM
    uint slice_num = pwm_gpio_to_slice_num(servo_pin); // Get PWM slice number

    pwm_set_clkdiv(slice_num, DIV); // divider
    pwm_set_wrap(slice_num, WRAP);
    pwm_set_enabled(slice_num, true); // turn on the PWM
}
void servo_write(int servo_pin, float angle){
    if(angle < 0) {angle = 0;}
    else if (angle > 180) {angle = 180;}

    float duty_cycle = (angle/180.0)*(MAX_SERVO_DUTY_CYCLE-MIN_SERVO_DUTY_CYCLE) + MIN_SERVO_DUTY_CYCLE;

    printf("%.3f %.3f\n", MAX_SERVO_DUTY_CYCLE-MIN_SERVO_DUTY_CYCLE, duty_cycle);
    pwm_set_gpio_level(servo_pin, (int)(WRAP*duty_cycle)); // set the duty cycle to 50%
}
    

static inline void put_pixel(PIO pio, uint sm, uint32_t pixel_grb) {
    pio_sm_put_blocking(pio, sm, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return
            ((uint32_t) (r) << 16) |
            ((uint32_t) (g) << 8) |
            (uint32_t) (b);
}

static inline uint32_t urgbw_u32(uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
    return
            ((uint32_t) (r) << 16) |
            ((uint32_t) (g) << 8) |
            ((uint32_t) (w) << 24) |
            (uint32_t) (b);
}

// link three 8bit colors together
typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} wsColor; 

// adapted from https://forum.arduino.cc/index.php?topic=8498.0
// hue is a number from 0 to 360 that describes a color on the color wheel
// sat is the saturation level, from 0 to 1, where 1 is full color and 0 is gray
// brightness sets the maximum brightness, from 0 to 1
wsColor HSBtoRGB(float hue, float sat, float brightness) {
    float red = 0.0;
    float green = 0.0;
    float blue = 0.0;

    if (sat == 0.0) {
        red = brightness;
        green = brightness;
        blue = brightness;
    } else {
        if (hue == 360.0) {
            hue = 0;
        }

        int slice = hue / 60.0;
        float hue_frac = (hue / 60.0) - slice;

        float aa = brightness * (1.0 - sat);
        float bb = brightness * (1.0 - sat * hue_frac);
        float cc = brightness * (1.0 - sat * (1.0 - hue_frac));

        switch (slice) {
            case 0:
                red = brightness;
                green = cc;
                blue = aa;
                break;
            case 1:
                red = bb;
                green = brightness;
                blue = aa;
                break;
            case 2:
                red = aa;
                green = brightness;
                blue = cc;
                break;
            case 3:
                red = aa;
                green = bb;
                blue = brightness;
                break;
            case 4:
                red = cc;
                green = aa;
                blue = brightness;
                break;
            case 5:
                red = brightness;
                green = aa;
                blue = bb;
                break;
            default:
                red = 0.0;
                green = 0.0;
                blue = 0.0;
                break;
        }
    }

    unsigned char ired = red * 255.0;
    unsigned char igreen = green * 255.0;
    unsigned char iblue = blue * 255.0;

    wsColor c;
    c.r = ired;
    c.g = igreen;
    c.b = iblue;
    return c;
}

int main() {
    stdio_init_all();

    servo_setup(SERVO_PIN);

    // PIO setup
    PIO pio;
    uint sm;
    uint offset;
    bool success = pio_claim_free_sm_and_add_program_for_gpio_range(&ws2812_program, &pio, &sm, &offset, WS2812_PIN, 1, true);
    hard_assert(success);
    // WS 2812 Setup
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, false);

    while (true) {
        for(int i = 0; i < 360; i++){
            float servo_angle = i/2.0;

            servo_write(SERVO_PIN, servo_angle);

            for(int j = 0; j < 4; j++){
                wsColor currColor = HSBtoRGB((i + j*90) % 360, 1, 1);
                put_pixel(pio, sm, urgb_u32(currColor.r, currColor.g, currColor.b));
            }


            sleep_ms(5000/360); // want to take 5000ms total
        }

    }

    // This will free resources and unload our program
    pio_remove_program_and_unclaim_sm(&ws2812_program, pio, sm, offset);
}
