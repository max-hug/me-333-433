// ascii_generator.c
// Max Hughes
// ME 333 HW 2

#include <stdio.h>

int main (void){

    printf("ABRIDGED ASCII TABLE\n\n");
    printf("Value | Representation\n");

    for(unsigned char i = 33; i <= 127; i++){
        printf("%5d | %5c\n", i, i);
    }
    
}