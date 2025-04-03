//
// main.c
//
// main file for motor control project, handles menu and calling other files
//
// ME 333 Code modified by Max Hughes
// ME 333
// Feb 2025
//

#include "NU32dip.h"          // config bits, constants, funcs for startup and UART

#include "utilities.h" 
#include "ina219.h" 
#include "currentControl.h"
#include "positionControl.h"
#include "encoder.h"


#define BUF_SIZE 200

int main() 
{
  char buffer[BUF_SIZE];
  NU32DIP_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init
  NU32DIP_GREEN = 1;  // turn off the LEDs       
  NU32DIP_YELLOW = 1;  // turn off the LEDs    
  
  set_mode(IDLE);

  // startups
  __builtin_disable_interrupts();
  encoder_startup();
  INA219_Startup();
  current_control_init();
  position_control_init();
  __builtin_enable_interrupts();

  while(1)
  {
    NU32DIP_ReadUART1(buffer,BUF_SIZE); // we expect the next character to be a menu command
    NU32DIP_GREEN = 1;                   // clear the error LED
    switch (buffer[0]) {
      case 'a': // read current (counts)
      {
        sprintf(buffer,"%d\r\n", 0);
        NU32DIP_WriteUART1(buffer);
        break;
      }
      case 'b': // read current (mA)
      {
        sprintf(buffer,"%.2f\r\n", INA219_read_current());
        NU32DIP_WriteUART1(buffer);
        break;
      }
      case 'c': // read encoder (counts)
      {
        refresh_encoder();
        int p = get_encoder_count();
        sprintf(buffer, "%d\r\n", p);
        NU32DIP_WriteUART1(buffer);
        break;
      }
      case 'd': // read encoder (deg)
      {
        refresh_encoder();
        float p = get_encoder_degrees();
        sprintf(buffer, "%.3f\r\n", p);
        NU32DIP_WriteUART1(buffer);
        break;
      }
      case 'e': // reset encoder
      {
        tare_encoder();
        break;
      }
      case 'f': // set PWM (-100 to 100)
      {
        int input_pwm = 0;
        NU32DIP_ReadUART1(buffer,BUF_SIZE);
        sscanf(buffer,"%d\r\n", &input_pwm);
        set_desired_pwm(input_pwm/100.0);
        set_mode(PWM);

        break;
      }
      case 'g': // set current gains
      {
        float gains[3];
        NU32DIP_ReadUART1(buffer,BUF_SIZE);
        sscanf(buffer,"%f %f %f\r\n", gains, gains+1, gains+2);

        set_current_gains(gains);

        break;
      }
      case 'h': // get current gains
      {
        float gains[3];
        get_current_gains(gains);
        
        sprintf(buffer, "%.4f %.4f %.4f\r\n", gains[0], gains[1], gains[2]);
        NU32DIP_WriteUART1(buffer);
        
        break;
      }
      case 'i': // set position gains
      {
        float gains[4];
        NU32DIP_ReadUART1(buffer,BUF_SIZE);
        sscanf(buffer,"%f %f %f %f\r\n", gains, gains+1, gains+2, gains+3);

        set_position_gains(gains);
        
        break;
      }
      case 'j': // get position gains
      {
        float gains[4];
        get_position_gains(gains);
        
        sprintf(buffer, "%.4f %.4f %.4f %.4f\r\n", gains[0], gains[1], gains[2], gains[3]);
        NU32DIP_WriteUART1(buffer);
        
        break;
      }
      case 'k': // test current control
      {
        create_buffer_ITEST();
        set_mode(ITEST); 
        while(get_mode() != IDLE){}
        send_buffer(ITEST);
        reset_buffer();
        break;
      }
      case 'l': // go to angle (deg)
      {
        float angle = 0.0;
        NU32DIP_ReadUART1(buffer,BUF_SIZE);
        sscanf(buffer,"%f\r\n", &angle);
        set_target_pos(angle);
        set_mode(HOLD);
        break;
      }
      case 'm': // load traj
      {
        recieve_buffer();
        break;
      }
      case 'n': // load traj 180 0.1 6000 500
      {
        recieve_buffer();
        break;
      }
      case 'o': // execute traj
      {
        set_mode(TRACK);
        while(get_mode() != IDLE){}
        send_buffer(TRACK);
        reset_buffer();
        break;
      }
      case 'p': // unpower motor
      {
        set_mode(IDLE);
        break;
      }
      case 'r': // get mode
      {
        sprintf(buffer,"%d\r\n", get_mode());
        NU32DIP_WriteUART1(buffer);
        break;
      }
      case 'q':
      {
        // handle q for quit. Later you may want to return to IDLE mode here.
        set_mode(IDLE);
        break;
      }
      default:
      {
        NU32DIP_GREEN = 0;  // turn on LED2 to indicate an error
        break;
      }
    }
  }
  return 0;
}
