#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <math.h>

#define CS_PIN 17
#define SINE_FREQ 2
#define TRIGANGLE_FREQ 1

static void spi_setup(uint cs_pin){
    spi_init(spi_default, 12 * 1000); // the baud, or bits per second

    gpio_set_function(PICO_DEFAULT_SPI_RX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);

    gpio_init(cs_pin);
    gpio_set_dir(cs_pin, GPIO_OUT);
}

static inline void cs_select(uint cs_pin) {
    asm volatile("nop \n nop \n nop"); // FIXME
    gpio_put(cs_pin, 0);
    asm volatile("nop \n nop \n nop"); // FIXME
}

static inline void cs_deselect(uint cs_pin) {
    asm volatile("nop \n nop \n nop"); // FIXME
    gpio_put(cs_pin, 1);
    asm volatile("nop \n nop \n nop"); // FIXME
}

static void write(uint8_t* buf, uint len){
    cs_select(CS_PIN);
    spi_write_blocking(spi_default, buf, len); // where data is a uint8_t array with length len
    cs_deselect(CS_PIN);
}

static void generate_buffer(uint8_t* buf, uint8_t channel, uint16_t data){
    buf[0] = 0b00110000 | (data >> 6) | channel << 7;
    buf[1] = data << 2;
}

int main()
{
    stdio_init_all();
    spi_setup(CS_PIN);

    while (true){

        uint64_t time = time_us_64();

        uint16_t sine = (uint16_t)(((sin((2.0*M_PI*SINE_FREQ/1e6)*time)+1)/2)*1024);
        uint16_t trig = abs(2*((TRIGANGLE_FREQ/1.0e6)*time - floor(TRIGANGLE_FREQ*time/1.0e6 + 0.5))*1024);

        uint8_t buf[2];

        generate_buffer(buf, 0, sine);
        write(buf, 2);

        generate_buffer(buf, 1, trig);
        write(buf, 2);

        sleep_us(1000);

    }


}
