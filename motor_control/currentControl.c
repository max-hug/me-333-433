//
// currentControl.c
//
// PID current controller and direct motor control through H bridge
//
// Max Hughes
// ME 333
// Feb 2025
//

#include "currentControl.h"
#include "utilities.h"
#include "ina219.h"
#include "NU32DIP.h"

volatile float desired_pwm_duty_cycle = 0.0; // can be set internally or externally
volatile float target_curent = 0.0;
static volatile float gains[3] = {0.0016, 0.0004, 2000}; // P, I, Eint_max

// start PWM (Timer 3, on pin RB7, 8:1 Prescale), freq is in Hz  --> min freq ~ 100Hz
static void OC1_setup(unsigned long freq){
    __builtin_disable_interrupts();

    T3CONbits.TCKPS = 0b011; // Timer3 prescaler N=8
    TMR3 = 0; // initial TMR3 count is 0
    
    RPB7Rbits.RPB7R = 0b0101; // map OC1 to RB7

    PR3 = (NU32DIP_PB_FREQ/(freq*8)) - 1; // period = (PR3+1) * N
    OC1CONbits.OCM = 0b110; // PWM mode without fault pin; other OC1CON bits are defaults
    OC1RS = 0; // start off for safety
    OC1R = OC1RS; // initialize before turning OC1 on; afterward it is read-only

    OC1CONbits.OCTSEL = 1;    // use Timer3
    T3CONbits.ON = 1;        // turn on Timer3
    OC1CONbits.ON = 1;       // turn on OC1

    __builtin_enable_interrupts();
}

// start control timer (Timer 2, 8:1 Prescale, Priority 5), freq is in Hz --> min freq ~ 100Hz
static void current_control_timer_setup(unsigned long freq){
    __builtin_disable_interrupts();
  
    TMR2 = 0; // initial count is 0     
    T2CONbits.TCKPS = 0b011; // prescale       
    
    PR2 = (NU32DIP_PB_FREQ/(freq*8)) - 1;
  
    IPC2bits.T2IP = 5; // priority
    IPC2bits.T2IS = 0; // subpriority
    IFS0bits.T2IF = 0; // clear interrupt flag
    IEC0bits.T2IE = 1; // enable interrupt
  
    T2CONbits.ON = 1;        // turn on Timer2
    __builtin_enable_interrupts();
  }

// ISR where all the control happens. changes duty cycle based on mode switch, then sets 
// that duty cycleto OC1 (+direction to B6)
void __ISR(_TIMER_2_VECTOR, IPL5SOFT) current_controller(void) {
    
    enum Mode curr_mode = get_mode();
    float duty_cycle = 0.0;

    static float E_int = 0.0;

    if(curr_mode == IDLE){
        duty_cycle = 0.0;
    }
    else if (curr_mode == PWM){
        duty_cycle = desired_pwm_duty_cycle;
    }
    else{ // we are doing control

        float measured_current = INA219_read_current();

        if(curr_mode == ITEST){
            target_curent = get_desired_buffer();
            add_measured_buffer(measured_current);
            increment_buffer();
            if(buffer_completed()){
                set_mode(IDLE);
            }
        }

        float E = target_curent - measured_current;
        E_int += E;

        // integrator windup
        if(E_int > gains[2]) {E_int = gains[2];}
        else if(E_int < -gains[2]) {E_int = -gains[2];}

        duty_cycle += gains[0]*E + gains[1]*E_int;
        if (duty_cycle > 100.0){duty_cycle = 100.0;}
        else if (duty_cycle < -100.0){duty_cycle = -100.0;}

    }
    
     // set PWM based on duty cycle --> pos = 1, neg = 0;
    if(duty_cycle >= 0){
        OC1RS =(int)(PR3*duty_cycle);
        LATBbits.LATB6 = 1;
    }
    else{    
        OC1RS =(int)(PR3*(-duty_cycle));
        LATBbits.LATB6 = 0;
    }

    IFS0bits.T2IF = 0; // flag
}


// setup for current control (calls perihperal setup + IO for H bridge)
void current_control_init(){

    TRISBbits.TRISB6 = 0;

    OC1_setup(5000); // 5KHz
    current_control_timer_setup(20000); // 20KHz
}




// sets target current (in mA)
void set_target_current(float new_target){
    __builtin_disable_interrupts();
    target_curent = new_target;
    __builtin_enable_interrupts();
}

float get_target_current(){
    return target_curent;
}

// set pwm value from -1.0 to 1.0 (percentage), for use by internal (current controller) & external (PWM mode)
void set_desired_pwm(float pwm){
    __builtin_disable_interrupts();
    desired_pwm_duty_cycle = pwm;
    __builtin_enable_interrupts();
}



// sets gains for current controller
void set_current_gains(float* new_gains){
    gains[0] = new_gains[0];
    gains[1] = new_gains[1];
    gains[2] = new_gains[2];
}

// gets current controller gains
void get_current_gains(float* return_gains){
    return_gains[0] = gains[0];
    return_gains[1] = gains[1];
    return_gains[2] = gains[2];
}


