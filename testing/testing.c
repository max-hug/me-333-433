#include <stdio.h>
#include "pico/stdlib.h"


int main()
{
    stdio_init_all();

    while (true) {
        volatile float f1, f2;
        uint64_t t1, t2;

        printf("Enter two floats to use:");
        scanf("%f %f", &f1, &f2);

        volatile float f_res;

        t1 = time_us_64();
        for(int i = 0; i < 1000; i++){
            f_res = f1+f2;   
        }
        t2 = time_us_64();
        printf("Add Time: %.2f cycles\n", (t2-t1)/6.667);

        t1 = time_us_64();
        for(int i = 0; i < 1000; i++){
            f_res = f1-f2;   
        }
        t2 = time_us_64();
        printf("Subtract Time: %.2f cycles\n", (t2-t1)/6.667);

        t1 = time_us_64();
        for(int i = 0; i < 1000; i++){
            f_res = f1*f2;   
        }
        t2 = time_us_64();
        printf("Mult Time: %.2f cycles\n", (t2-t1)/6.667);

        t1 = time_us_64();
        for(int i = 0; i < 1000; i++){
            f_res = f1/f2;   
        }
        t2 = time_us_64();
        printf("Divide Time: %.2f cycles\n\n", (t2-t1)/6.667);

        sleep_ms(1000);


    }
}
