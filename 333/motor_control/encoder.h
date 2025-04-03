//
// encoder.h
//
// Handles encoder input through UART2 from PICO
//
// ME 333 Code modified by Max Hughes
// ME 333
// Feb 2025
//

#ifndef ENCODER__H__
#define ENCODER__H__

#include <xc.h> // processor SFR definitions
#include <sys/attribs.h> // __ISR macro
#include "NU32DIP.h"

// Initialize the serial port - UART2
void encoder_startup();

// Write a character array using UART2
void WriteUART2(const char * string);

// gets flag related to whether new data has arrived from Pico
int get_encoder_flag();

// sets flag related to whether new data has arrived from Pico
void set_encoder_flag();

// returns stored encoder value
int get_encoder_count();

// takes stored encoder value and returns after converting to deg
float get_encoder_degrees();

// get a new value for the encoder from the Pico
void refresh_encoder();

// abstracted way to reset encoder, send "b" to Pico
void tare_encoder();

#endif // ENCODER__H__
