#include <xc.h>          // Load the proper header for the processor


#define MAXCYCLES 1000000
#define DELTACYCLES 100000

void delay(int cycles);

int main(void) {
  TRISBCLR = 0x30; // Make TRIS for B4 and B5 0, 
  LATBbits.LATB4 = 0;    // Turn GREEN on and YELLOW off.  These pins sink current
  LATBbits.LATB5 = 1;    // on the NU32DIP, so "high" (1) = "off" and "low" (0) = "on"

  int numCycles = MAXCYCLES;

  while(1) {
    delay(numCycles);
    LATBINV = 0b110000;    // toggle GREEN and YELLOW; 
    numCycles-= DELTACYCLES;
    if(numCycles < 0){
      numCycles = MAXCYCLES;
    }
  }
  return 0;
}

void delay(int cycles) {
  int j;
  for (j = 0; j < cycles; j++) { // number is 1 million
    while(!PORTAbits.RA4) {
        ;   // Pin A4 is the USER switch, low (FALSE) if pressed.
    }
  }
}
