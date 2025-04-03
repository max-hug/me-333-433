//
// encoder.c
//
// Handles encoder input through UART2 from PICO
//
// ME 333 Code modified by Max Hughes
// ME 333
// Feb 2025
//

#include "encoder.h"
#include <stdio.h>

#define UART2_DESIRED_BAUD 230400
#define MAX_RX_MESSAGE 100
#define COUNT_TO_DEG 0.2635 // 360 deg / 1366 counts, constant to mult bt

volatile int rx_num_bytes = 0;
char rx_message[MAX_RX_MESSAGE];
volatile int pos = 0;
volatile int newPosFlag = 0;

// gets flag related to whether new data has arrived from Pico
int get_encoder_flag(){
    return newPosFlag;
}

// sets flag related to whether new data has arrived from Pico
void set_encoder_flag(int f){
    newPosFlag = f;
}

// returns stored encoder value
int get_encoder_count(){
    return pos;
}

// takes stored encoder value and returns after converting to deg
float get_encoder_degrees(){
    __builtin_disable_interrupts();
    int temp = pos;
    __builtin_enable_interrupts();
    return 1.0*temp*COUNT_TO_DEG;
}

// UART2 ISR to handle communication
void __ISR(_UART_2_VECTOR, IPL7SOFT) U2ISR(void) {
    char data = U2RXREG; // read the data
    if (data == '\n') {
        rx_message[rx_num_bytes] = '\0';
        sscanf(rx_message,"%d",&pos);
        newPosFlag = 1;
        rx_num_bytes = 0;
    }
    else {
        rx_message[rx_num_bytes] = data;
        ++rx_num_bytes;
        // roll over if got more bytes than the size of the array
        if (rx_num_bytes >= MAX_RX_MESSAGE) {
            rx_num_bytes = 0;
        }
    }
    IFS1bits.U2RXIF = 0;
}

// Write a character array using UART2
void WriteUART2(const char * string) {
    while (*string != '\0') {
        while (U2STAbits.UTXBF) {
            ; // wait until tx buffer isn't full
        }
        U2TXREG = *string;
        ++string;
    }
}

// Initialize the serial port - UART2
void encoder_startup() {
    // disable interrupts
    __builtin_disable_interrupts();
    // set UART2 TX and RX pins
    ANSELBCLR = 0b11;
    U2RXRbits.U2RXR = 0b0010; // U2RX on B1, pin 5
    RPB0Rbits.RPB0R = 0b0010; // U2TX on B0, pin 4
    // turn on UART2 with interrupt on RX
    U2MODEbits.BRGH = 0; // set baud to NU32_DESIRED_BAUD
    U2BRG = ((NU32DIP_SYS_FREQ / UART2_DESIRED_BAUD) / 16) - 1;
    // 8 bit, no parity bit, and 1 stop bit (8N1 setup)
    U2MODEbits.PDSEL = 0;
    U2MODEbits.STSEL = 0;
    // configure TX & RX pins as output & input pins
    U2STAbits.UTXEN = 1;
    U2STAbits.URXEN = 1;
    U2STAbits.URXISEL = 0; // interrupt when a char is rcvd
    IPC9bits.U2IP = 7;
    IPC9bits.U2IS = 0;
    IFS1bits.U2RXIF = 0;
    IEC1bits.U2RXIE = 1;
    // enable the uart
    U2MODEbits.ON = 1;
    __builtin_enable_interrupts();
}

// get a new value for the encoder from the Pico
void refresh_encoder(){
    WriteUART2("a");
    while(!get_encoder_flag()){}
    set_encoder_flag(0);
}

// abstracted way to reset encoder, send "b" to Pico
void tare_encoder(){
    WriteUART2("b");
}