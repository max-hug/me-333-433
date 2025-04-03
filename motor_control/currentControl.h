//
// currentControl.h
//
// PID current controller and direct motor control through H bridge
//
// Max Hughes
// ME 333
// Feb 2025
//

#ifndef CURRCONT__H__
#define CURRCONT__H__
  
#include"nu32dip.h"

// start PWM (Timer 3, on pin RB7, 8:1 Prescale), freq is in Hz  --> min freq ~ 100Hz
static void OC1_setup(unsigned long freq);

// start control timer (Timer 2, 8:1 Prescale), freq is in Hz --> min freq ~ 100Hz
static void current_control_timer_setup(unsigned long freq);




// setup for current control (calls perihperal setup + IO for H bridge)
void current_control_init();

// sets target current (in mA)
void set_target_current(float new_target);

// returns target current (in mA)
float get_target_current();

// set pwm value from -1.0 to 1.0, for use by internal (current controller) & external (PWM mode)
void set_desired_pwm(float pwm);



// sets gains for current controller
void set_current_gains(float* new_gains);

// gets current controller gains
void get_current_gains(float* return_gains);

#endif // CURRCONT__H__
