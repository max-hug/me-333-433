#include "sr04.h"
#include <xc.h>
#include "nu32dip.h"

// macros for the pins
#define TRIG LATBbits.LATB15
#define ECHO PORTBbits.RB14

// initialize the pins used by the SR04
void SR04_Startup(){
    ANSELA = 0; // turn off the analog input functionality that overrides everything else
    ANSELB = 0;
    TRISBbits.TRISB15 = 0; // B15 is TRIG, output from the PIC
    TRISBbits.TRISB14 = 1; // B14 is ECHO, input to the PIC
    TRIG = 0; // initialize TRIG to LOW
}

// trigger the SR04 and return the value in core timer ticks
unsigned int SR04_read_raw(unsigned int timeout){
    // turn on TRIG
    TRIG = 1; 
    // wait at least 10us
    NU32DIP_DelayMicroseconds(10);
    // turn off TRIG
    TRIG = 0;
    // wait until ECHO is on
    while(ECHO != 1){};
    // note the value of the core timer
    _CP0_SET_COUNT(0);
    // wait until ECHO is off or timeout core ticks has passed
    while(ECHO == 1 && _CP0_GET_COUNT() < timeout){};
    // note the core timer
    // return the difference in core times
    return _CP0_GET_COUNT();
}

float SR04_read_meters(){
    // read the raw rs04 value
    int ticks = SR04_read_raw(NU32DIP_SYS_FREQ);
    // convert the time to meters, the velocity of sound in air is 346 m/s
    float dist = ((346.0*ticks)/NU32DIP_SYS_FREQ);
    // return the distance in meters
    return dist;
}