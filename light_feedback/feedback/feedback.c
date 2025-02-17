#include "NU32DIP.h"          // constants, functions for startup and UART

#define NUMSAMPS 1000 // number of points in waveform
#define DUTY_CYCLE 0.75

static volatile int Waveform[NUMSAMPS]; // waveform

void makeWaveform() {
  int i = 0, center = 1200, A = 1200; // square wave, fill in center value and amplitude
  for (i = 0; i < NUMSAMPS; ++i) {
    if ( i < (int)(NUMSAMPS/2)) {
      Waveform[i] = center + A;
    }
    else {
      Waveform[i] = center - A;
    }
  }
}

void __ISR(_TIMER_2_VECTOR, IPL5SOFT) Controller(void) { // _TIMER_2_VECTOR = 8
  static int counter = 0; // initialize counter once
  OC1RS = Waveform[counter];
  counter++; // add one to counter every time ISR is entered
  if (counter == NUMSAMPS) {
  counter = 0; // roll the counter over when needed
  }
  IFS0bits.T2IF = 0;  
}

void OC1_Setup(){
  __builtin_disable_interrupts();
  T3CONbits.TCKPS = 0;     // Timer3 prescaler N=4 (1:4)
  TMR3 = 0;                // initial TMR3 count is 0
  
  RPB7Rbits.RPB7R = 0b0101;          // map OC1 to RB7
  PR3 = 2399;              // period = (PR2+1) * N
  OC1CONbits.OCM = 0b110;  // PWM mode without fault pin; other OC1CON bits are defaults
  OC1RS = (int)(DUTY_CYCLE*(PR3+1));             // duty cycle = OC1RS/(PR2+1) = 25%
  OC1R = 500;              // initialize before turning OC1 on; afterward it is read-only
  OC1CONbits.OCTSEL = 1;    // use Timer3

  T3CONbits.ON = 1;        // turn on Timer3
  OC1CONbits.ON = 1;       // turn on OC1
  __builtin_enable_interrupts();
}

void Control_Timer_Setup(){
  __builtin_disable_interrupts();

  TMR2 = 0; // initial count is 0
  PR2 = 5999; // reset count        
  T2CONbits.TCKPS = 3; // prescale          

  IPC2bits.T2IP = 5; // priority
  IPC2bits.T2IS = 0; // subpriority
  IFS0bits.T2IF = 0; // clear interrupt flag
  IEC0bits.T2IE = 1; // enable interrupt

  T2CONbits.ON = 1;        // turn on Timer3
  __builtin_enable_interrupts();
}

int main(void) {
  NU32DIP_Startup();          // cache on, interrupts on, LED/button init, UART init
  makeWaveform();
  OC1_Setup();
  Control_Timer_Setup();
  while(1) {
    ;                      // infinite loop
  }
  return 0;
}

