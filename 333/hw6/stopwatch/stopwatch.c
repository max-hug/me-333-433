#include "nu32dip.h"          // constants, funcs for startup and UART
#include <stdbool.h>

volatile bool timing = false;

void __ISR(_EXTERNAL_0_VECTOR, IPL2SOFT) Ext0ISR(void) { // step 1: the ISR

  NU32DIP_Delay(10); // wait 10ms
  if(NU32DIP_USER == 0){

    if(timing){
      char buffer[30];
      sprintf(buffer, "%.3f seconds elapsed\r\n", _CP0_GET_COUNT()*2.0/NU32DIP_SYS_FREQ);
      NU32DIP_WriteUART1(buffer);
      NU32DIP_WriteUART1("Press the USER button again to start the timer\r\n");
    }
    else{
      NU32DIP_WriteUART1("Press the USER button again to stop the timer\r\n");
      _CP0_SET_COUNT(0);
    }
    timing = !timing;
  }
  IFS0bits.INT0IF = 0;            // clear interrupt flag IFS0<3>
}

int main(void) {
  NU32DIP_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init
  __builtin_disable_interrupts(); // step 2: disable interrupts
  INTCONbits.INT0EP = 0;          // step 3: INT0 triggers on falling edge
  IPC0bits.INT0IP = 2;            // step 4: interrupt priority 2
  IPC0bits.INT0IS = 1;            // step 4: interrupt [sub]priority 1
  IFS0bits.INT0IF = 0;            // step 5: clear the int flag
  IEC0bits.INT0IE = 1;            // step 6: enable INT0 by setting IEC0<3>
  __builtin_enable_interrupts();  // step 7: enable interrupts
                                  // Connect RD7 (USER button) to INT0 (RD0)

  NU32DIP_WriteUART1("Press the USER button again to start the timer\r\n");

  while(1) {
      ; // do nothing, loop forever
  }

  return 0;
}
