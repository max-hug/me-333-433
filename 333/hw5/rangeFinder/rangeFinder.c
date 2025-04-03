#include "nu32dip.h"          // Load the proper header for the processor
#include "sr04.h"

void delay(int cycles);

int main(void) {
  NU32DIP_Startup();
  SR04_Startup();

  char output[10]; 

  while(1) {
    sprintf(output, "%.2f\r\n", SR04_read_meters());
    NU32DIP_WriteUART1(output);
    NU32DIP_Delay(250); //added to NU32DIP, in ms
  }
  return 0;
}
