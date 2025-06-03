#include <stdio.h>
#include "pico/stdlib.h"
#include "cam.h"
#include "hardware/pwm.h"

#define ENA 17
#define PHA 16
#define ENB 19
#define PHB 18

#define CENTER 30
#define MAX_COM 10
#define MIN_COM -10
#define MIN_PWM 25
#define MAX_PWM 75

#define MOTOR_PWM_FREQ 20000
#define DIV 100
#define WRAP 150000000/(DIV*MOTOR_PWM_FREQ)

void motor_setup(const unsigned char ena, const unsigned char pha, const unsigned char enb, const unsigned char phb){
    gpio_init(ena);
    gpio_init(pha);
    gpio_init(enb);
    gpio_init(phb);

    gpio_set_dir(ena, GPIO_OUT);
    gpio_set_dir(pha, GPIO_OUT);
    gpio_set_dir(enb, GPIO_OUT);
    gpio_set_dir(phb, GPIO_OUT);

    gpio_set_function(ena, GPIO_FUNC_PWM); 
    uint slice_num_a = pwm_gpio_to_slice_num(ena);
    pwm_set_clkdiv(slice_num_a, DIV);
    pwm_set_wrap(slice_num_a, WRAP);
    pwm_set_enabled(slice_num_a, true); 

    gpio_set_function(enb, GPIO_FUNC_PWM); 
    uint slice_num_b = pwm_gpio_to_slice_num(enb);
    pwm_set_clkdiv(slice_num_b, DIV);
    pwm_set_wrap(slice_num_b, WRAP);
    pwm_set_enabled(slice_num_b, true); 
}


int main()
{
    stdio_init_all();

    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }


    init_camera_pins();
    motor_setup(ENA, PHA, ENB, PHB);
 
    while (true) {
        int duty_cycle_a = 50;
        int duty_cycle_b = 50;

        setSaveImage(1);
        while(getSaveImage()==1){}
        convertImage();
        int com = findLine(IMAGESIZEY/2) - CENTER; // calculate the position of the center of the ine
        
        duty_cycle_a = (MAX_PWM+MIN_PWM)/2 + (MAX_PWM-MIN_PWM)/(2*MAX_COM)*com;
        duty_cycle_b = (MAX_PWM+MIN_PWM)/2 - (MAX_PWM-MIN_PWM)/(2*MAX_COM)*com;

        pwm_set_gpio_level(ENA, (int)(WRAP*duty_cycle_a/100.0)); 
        pwm_set_gpio_level(ENB, (int)(WRAP*duty_cycle_b/100.0)); 
        gpio_put(PHA, 0);
        gpio_put(PHB, 0);

        printf("%d %d %d\r\n",com, duty_cycle_a, duty_cycle_b); // comment this when testing with python
    }
}

