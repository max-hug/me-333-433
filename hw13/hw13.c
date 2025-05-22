#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "ssd1306.h"
#include "math.h"


#define LED_PIN 0

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 4
#define I2C_SCL 5

#define ADDR 0b1101000

// config registers
#define CONFIG 0x1A
#define GYRO_CONFIG 0x1B
#define ACCEL_CONFIG 0x1C
#define PWR_MGMT_1 0x6B
#define PWR_MGMT_2 0x6C
// sensor data registers:
#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40
#define TEMP_OUT_H   0x41
#define TEMP_OUT_L   0x42
#define GYRO_XOUT_H  0x43
#define GYRO_XOUT_L  0x44
#define GYRO_YOUT_H  0x45
#define GYRO_YOUT_L  0x46
#define GYRO_ZOUT_H  0x47
#define GYRO_ZOUT_L  0x48
#define WHO_AM_I     0x75

void set_reg(unsigned char addr, unsigned char reg, unsigned char value){

    unsigned char buf[2];
    buf[0] = reg;
    buf[1] = value;

    i2c_write_blocking(I2C_PORT, addr, buf, 2, false);
}

void read_reg(unsigned char addr, unsigned char reg, unsigned char* output_buff, int num_regs){
    i2c_write_blocking(I2C_PORT, addr, &reg, 1, true);  // true to keep master control of bus
    i2c_read_blocking(I2C_PORT, addr, output_buff, num_regs, false);  // false - finished with bus  
}

void draw_line(unsigned char axis, unsigned char x, unsigned char y, unsigned char dir, unsigned char len){
    if(axis == 0){
        for(int i = 0; i < len; i++){
            ssd1306_drawPixel(x+i*dir, y, 1);
        }
    }
    else if(axis == 1){
        for(int i = 0; i < len; i++){
            ssd1306_drawPixel(x, y+i*dir, 1);
        }
    }
}

void clear_screen(){
    for(int i = 0; i < 128; i++){
        for(int j = 0; j < 32; j++){
            ssd1306_drawPixel(i, j, 0);
        }
    }
}

void process_imu_data(int* output, unsigned char* data, int num_vals){
    for(int i = 0; i < num_vals; i++){
        output[i] = (int16_t)((data[2*i] << 8) | data[2*i+1]);
    }
}

void read_imu_data(float* output, unsigned char addr){
    unsigned char raw_data[14];
    read_reg(addr, ACCEL_XOUT_H, raw_data, 14);

    int processed_data[7];
    process_imu_data(processed_data, raw_data, 7);

    output[0] = processed_data[0]*0.000061;
    output[1] = processed_data[1]*0.000061;
    output[2] = processed_data[2]*0.000061;

    printf("%.3f, %.3f, %.3f\n", output[0], output[1], output[2]);
}

void imu_setup(unsigned char addr){
    set_reg(addr, PWR_MGMT_1, 0x00);
    set_reg(addr, GYRO_CONFIG, 0b00011000);
    set_reg(addr, ACCEL_CONFIG, 0b0);

    unsigned char check;
    read_reg(addr, WHO_AM_I, &check, 1);
    printf("%p\n", check);
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

    while(!stdio_usb_connected()){}
    sleep_ms(1000);
    printf("starting...\n");
    imu_setup(ADDR);

    // OLED
    ssd1306_setup();
    ssd1306_clear();
    ssd1306_update(); 

    while (true) {
        unsigned long last_read = 0;
        float imu_data[3];

        unsigned char blink = (time_us_64()/1000000)%2;
        gpio_put(LED_PIN, blink);
        ssd1306_drawPixel(127, 0, blink);

        if(time_us_64() - last_read > 10000){
            read_imu_data(imu_data, ADDR);
            last_read = time_us_64();
        }

        draw_line(0, 64, 16, 2*(imu_data[0] > 0)-1, (int)fabs(imu_data[0]*64/0.8));
        draw_line(1, 64, 16, -(2*(imu_data[1] > 0)-1), (int)fabs(imu_data[1]*16/0.8));
        ssd1306_update();
        clear_screen();
    }
}
