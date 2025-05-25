#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define ENA 17
#define PHA 16
#define ENB 19
#define PHB 18

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

    motor_setup(ENA, PHA, ENB, PHB);



    int duty_cycle = 75;
    char input[10];

    while (true) {
        printf("\nCurrent Duty Cycle: %d%%. Input + or - to change: ", duty_cycle);
        scanf("%s", input);
        if(input[0] == '+' && duty_cycle < 100){ duty_cycle++;}
        else if(input[0] == '-' && duty_cycle > -100){ duty_cycle--;}

        if(duty_cycle > 0){
        pwm_set_gpio_level(ENA, (int)(WRAP*duty_cycle/100.0)); 
        pwm_set_gpio_level(ENB, (int)(WRAP*duty_cycle/100.0)); 
        gpio_put(PHA, 0);
        gpio_put(PHB, 1);
        }
        else{
        pwm_set_gpio_level(ENA, (int)(-WRAP*duty_cycle/100.0)); 
        pwm_set_gpio_level(ENB, (int)(-WRAP*duty_cycle/100.0)); 
        gpio_put(PHA, 1);
        gpio_put(PHB, 0);
        }

    }
}
