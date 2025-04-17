#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define LED_PIN 0

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 4
#define I2C_SCL 5

// Regs
#define REG_IODIR 0x00
#define REG_GPIO 0x09
#define REG_OLAT 0x0A

// other
#define ADDR 0b0100000
#define I2C_PIN_LED 7
#define I2C_PIN_BUTTON 0

void set_reg(unsigned char addr, unsigned char reg, unsigned char value){

    unsigned char buf[2];
    buf[0] = reg;
    buf[1] = value;

    i2c_write_blocking(I2C_PORT, addr, buf, 2, false);
}

unsigned char read_reg(unsigned char addr, unsigned char reg){

    unsigned char buf;

    i2c_write_blocking(I2C_PORT, ADDR, &reg, 1, true);  // true to keep master control of bus
    i2c_read_blocking(I2C_PORT, ADDR, &buf, 1, false);  // false - finished with bus  

    return buf;
}

// I2C Initialisation. Using it at 400Khz.
void i2c_io_startup(){
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    set_reg(ADDR, REG_IODIR, 0b01111111);
}

int main()
{
    stdio_init_all();

    i2c_io_startup();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    while (true) {
        gpio_put(LED_PIN, (time_us_64()/1000000)%2);

        unsigned char input = read_reg(ADDR, REG_GPIO);
        unsigned char button_val = input & 0b1;

        set_reg(ADDR, REG_OLAT, button_val << 7);
    }
}
