//
// positionControl.h
//
// PID position controller and direct motor control through H bridge
//
// Max Hughes
// ME 333
// Feb 2025
//

#ifndef POSCONT__H__
#define POSCONT__H__
  
#include"nu32dip.h"

// start control timer (Timer 4, 8:1 Prescale), freq is in Hz --> min freq ~ 100Hz
static void position_control_timer_setup(unsigned long freq);

// setup for position control
void position_control_init();

// sets target angle for PID control
void set_target_pos(float new_target);

// sets gains for position controller
void set_position_gains(float* new_gains);

// gets position controller gains
void get_position_gains(float* return_gains);

#endif // POSCONT__H__
