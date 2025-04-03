//
// positionControl.c
//
// PID position controller and direct motor control through H bridge
//
// Max Hughes
// ME 333
// Feb 2025
//

#include "positionControl.h"
#include "utilities.h"
#include "currentControl.h"
#include "encoder.h"

static volatile float gains[4] = {180, 0.1, 7000, 500};
static volatile float target_angle = 0.0;

// ISR where all the control happens. changes current target based on mode switch
void __ISR(_TIMER_4_VECTOR, IPL6SOFT) position_controller(void) {
    
    enum Mode curr_mode = get_mode();

    static float E_int = 0.0;
    static float E_prev = 0.0;

    if(curr_mode == HOLD || curr_mode == TRACK){

        refresh_encoder();
        float measured_angle = get_encoder_degrees();

        if(curr_mode == TRACK){
            target_angle = get_desired_buffer();
            add_measured_buffer(measured_angle);
            increment_buffer();
            if(buffer_completed()){
                set_mode(IDLE);
            }
        }

        float E = target_angle - measured_angle;
        E_int += E;

        // integrator windup
        if(E_int > gains[3]) {E_int = gains[3];}
        else if(E_int < -gains[3]) {E_int = -gains[3];}

        float current = gains[0]*E + gains[1]*E_int + gains[2]*(E-E_prev);
        if (current > 700.0){current = 700.0;} // stall current
        else if (current < -700.0){current = -700.0;}

        set_target_current(current);

        E_prev = E;
    }

    IFS0bits.T4IF = 0; // flag
}


// start control timer (Timer 4, 8:1 Prescale, Priority 5), freq is in Hz --> min freq ~ 100Hz
static void position_control_timer_setup(unsigned long freq){
    __builtin_disable_interrupts();
  
    TMR4 = 0; // initial count is 0     
    T4CONbits.TCKPS = 0b011; // prescale       
    
    PR4 = (NU32DIP_PB_FREQ/(freq*8)) - 1;
  
    IPC4bits.T4IP = 6; // priority
    IPC4bits.T4IS = 0; // subpriority
    IFS0bits.T4IF = 0; // clear interrupt flag
    IEC0bits.T4IE = 1; // enable interrupt
  
    T4CONbits.ON = 1;        // turn on Timer2
    __builtin_enable_interrupts();
  }

// setup for position control
void position_control_init(){
    position_control_timer_setup(200); // 200Hz
}

// sets target angle for PID control
void set_target_pos(float new_target){
    __builtin_disable_interrupts();
    target_angle = new_target;
    __builtin_enable_interrupts();
}




// sets gains for position controller
void set_position_gains(float* new_gains){
    gains[0] = new_gains[0];
    gains[1] = new_gains[1];
    gains[2] = new_gains[2];
    gains[3] = new_gains[3];
}

// gets position controller gains
void get_position_gains(float* return_gains){
    return_gains[0] = gains[0];
    return_gains[1] = gains[1];
    return_gains[2] = gains[2];
    return_gains[3] = gains[3];
}