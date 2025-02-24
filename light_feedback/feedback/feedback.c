#include "NU32DIP.h"          // constants, functions for startup and UART

#define NUMSAMPS 1000 // number of points in waveform
#define DUTY_CYCLE 0.75
#define NUMSAMPS 1000 // number of points in waveform
#define PLOTPTS 200 // number of data points to plot
#define DECIMATION 10 // plot every 10th point

static volatile int Waveform[NUMSAMPS]; // waveform
static volatile int ADCarray[PLOTPTS]; // measured values to plot
static volatile int REFarray[PLOTPTS]; // reference values to plot
static volatile int StoringData = 0; // if this flag = 1, currently storing plot data
static volatile float Kp = 0, Ki = 0; // control gains
static volatile int Eint = 0;
static volatile int Emax = 0;

void makeWaveform() {
  int i = 0, center = 500, A = 200; // square wave, fill in center value and amplitude
  for (i = 0; i < NUMSAMPS; ++i) {
    if ( i < (int)(NUMSAMPS/2)) {
      Waveform[i] = center + A;
    }
    else {
      Waveform[i] = center - A;
    }
  }
}

void __ISR(_TIMER_2_VECTOR, IPL5SOFT) Controller(void) {
  static int counter = 0; // initialize counter once
  static int plotind = 0; // index for data arrays; counts up to PLOTPTS
  static int decctr = 0; // counts to store data one every DECIMATION

  int adcval = NU32DIP_adc_sample_convert(0);
  int E = adcval - Waveform[counter];

  Eint += E;
  if(Eint > Emax){
    Eint = Emax;
  }

  float u = Kp*E + Ki*Eint + 50.0;
  if (u > 100.0){u = 100.0;}
  else if (u < 0.0){u = 0.0;}

  OC1RS = (unsigned int) ((u/100.0) * PR3); // control PWM

  if (StoringData) {
    decctr++;
    if (decctr == DECIMATION) { // after DECIMATION control loops,
      decctr = 0; // reset decimation counter
      ADCarray[plotind] = adcval; // store data in global arrays
      REFarray[plotind] = Waveform[counter];
      plotind++; // increment plot data index
    }
    if (plotind == PLOTPTS) { // if max number of plot points plot is reached,
      plotind = 0; // reset the plot index
      StoringData = 0; // tell main data is ready to be sent to MATLAB
    }
  }

  counter++; // add one to counter every time ISR is entered
  if (counter == NUMSAMPS) {
    counter = 0; // rollover counter over when end of waveform reached
  }

  IFS0bits.T2IF = 0; // flag
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

void ADC_Setup(){
  AD1CON3bits.ADCS = 2;                   // ADC clock period is Tad = 2*(ADCS+1)*Tpb = 2*3*12.5ns = 75ns
  AD1CON1bits.ADON = 1;                   // turn on A/D converter
}

int main(void) {
  NU32DIP_Startup();          // cache on, interrupts on, LED/button init, UART init
  makeWaveform();
  OC1_Setup();
  Control_Timer_Setup();
  ADC_Setup();

  char message[100]; // message to and from MATLAB
  float kptemp = 0, kitemp = 0; // temporary local gains
  int Emaxtemp = 0; // temp max
  int i = 0; // plot data loop counter

  while (1) {
    NU32DIP_ReadUART1(message, sizeof(message)); // wait for a message from MATLAB
    sscanf(message, "%f %f %d" , &kptemp, &kitemp, &Emaxtemp);

    __builtin_disable_interrupts(); // keep ISR disabled as briefly as possible
    Kp = kptemp; // copy local variables to globals used by ISR
    Ki = kitemp;
    Eint = 0;
    Emax = Emaxtemp;
    __builtin_enable_interrupts(); // only 2 simple C commands while ISRs disabled

    StoringData = 1; // message to ISR to start storing data
    while (StoringData) { // wait until ISR says data storing is done
      ; // do nothing
    }

    for (i=0; i<PLOTPTS; i++) { // send plot data to python
      // when first number sent = 1, python knows we’re done
      sprintf(message, "%d %d %d\r\n", PLOTPTS-i, ADCarray[i], REFarray[i]);
      NU32DIP_WriteUART1(message);
    }
  }
  return 0;
}

