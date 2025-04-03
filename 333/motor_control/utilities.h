//
// utilities.h
//
// Utilities for client communication and mode switching
//
// Max Hughes
// ME 333
// Feb 2025
//

#ifndef UTIL__H__
#define UTIL__H__
  
#include"nu32dip.h"

#define DATA_BUF_SIZE 4000

enum Mode {
    IDLE, // cut power to motor
    PWM, // constant pwm signal to motor
    ITEST, // current controled
    HOLD, // position held (controlled)
    TRACK // position tracked (controlled)
};

// sets the current mode using the enum
void set_mode(enum Mode newMode);

// gets the current Mode using the enum
enum Mode get_mode();

// adds a single point to the data buffer for the measured data array
void add_measured_buffer(float measured);

// get data from the desired buffer for the current index
float get_desired_buffer();

// increment buffer to the next index
void increment_buffer();

// resets buffer to zero index
void reset_buffer();

// creates desired trajectory for ITEST
void create_buffer_ITEST();

// checks whether the buffer has been filled
short buffer_completed();

// recieves desired data array from client
void recieve_buffer();

// sends both data arrays to the client 
void send_buffer(enum Mode sending_mode);




#endif // UTIL__H__
