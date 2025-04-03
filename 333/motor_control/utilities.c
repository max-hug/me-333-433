//
// utilities.c
//
// Utilities for client communication and mode switching
//
// Max Hughes
// ME 333
// Feb 2025
//

#include "utilities.h"


static volatile enum Mode currMode = IDLE; // make our life easier

static volatile int buffer_index = 0;
static volatile int buffer_end = 0; // index of the last piece of data
static volatile float target_pos_data_buffer[DATA_BUF_SIZE];
static volatile float target_current_data_buffer[DATA_BUF_SIZE];
static volatile float measured_data_buffer[DATA_BUF_SIZE];

// sets current mode through enum
void set_mode(enum Mode newMode){
    currMode = newMode;
}

// returns current mode through enum
enum Mode get_mode(){
    return currMode;
}

// adds a single point to the data buffer for the measured data array
void add_measured_buffer(float measured){
    if(buffer_index <= buffer_end){
        measured_data_buffer[buffer_index] = measured;
    }
}

// get data from the desired buffer for the current index
float get_desired_buffer(){
    if(currMode==ITEST){
        return target_current_data_buffer[buffer_index];
    }
    else{
        return target_pos_data_buffer[buffer_index];
    }
}

// increment buffer to the next index
void increment_buffer(){
    if(buffer_index < buffer_end){
        buffer_index++;
    }
}

// resets buffer to zero index
void reset_buffer(){
    buffer_index = 0;
}

// creates desired trajectory for ITEST
void create_buffer_ITEST(){

    target_current_data_buffer[0] = 200;

    for(int i = 1; i < 100; i++){
        if(i % 25 == 0){
            target_current_data_buffer[i] = -1*target_current_data_buffer[i-1];
        }
        else{
            target_current_data_buffer[i] = target_current_data_buffer[i-1];
        }
    }
    buffer_index = 0;
    buffer_end = 99;
}

// checks whether the buffer has been filled
short buffer_completed(){
    return buffer_end == buffer_index;
}

// recieves desired data array from client
void recieve_buffer(){
    char input_str[20];
    NU32DIP_ReadUART1(input_str, 20);
    int index = 0;

    if(input_str[0] == 'b'){
        NU32DIP_ReadUART1(input_str, 20);
        while(input_str[0] != 's' && index < DATA_BUF_SIZE){
            sscanf(input_str,"%f\r\n", &target_pos_data_buffer[index]);
            NU32DIP_ReadUART1(input_str, 20);
            index++;
        }
        buffer_end = index-1;
        buffer_index = 0;
    }

}

// sends both data arrays to the client 
void send_buffer(enum Mode sending_mode){
    // start sending buffer
    char output_str[20];
    sprintf(output_str,"b"); 
    NU32DIP_WriteUART1(output_str);

    // send buffer contents
    for(int i = 0; i < buffer_index; i++){
        if(sending_mode == ITEST){
            sprintf(output_str,"%.2f %.2f,", target_current_data_buffer[i], measured_data_buffer[i]);
        }
        else{
            sprintf(output_str,"%.2f %.2f,", target_pos_data_buffer[i], measured_data_buffer[i]);
        }
        NU32DIP_WriteUART1(output_str);
    }
    
    // end sending buffer
    sprintf(output_str,"\r\n"); 
    NU32DIP_WriteUART1(output_str); 
}
