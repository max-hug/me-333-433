#include <stdio.h>
#include "nu32dip.h"  //added
#define MAX_YEARS 100 
#define MAX_MESSAGE_LENGTH 200

typedef struct {
  double inv0;                   
  double growth;                  
  int years;                     
  double invarray[MAX_YEARS+1];   
} Investment;                    

int getUserInput(Investment *invp);     
void calculateGrowth(Investment *invp); 
void sendOutput(double *arr, int years);

int main(void) {

  Investment inv;                

  while(getUserInput(&inv)) {    
    inv.invarray[0] = inv.inv0;  
    calculateGrowth(&inv);       
    sendOutput(inv.invarray,     
               inv.years);       
  }
  return 0;                      
}

void calculateGrowth(Investment *invp) {

  int i;

  for (i = 1; i <= invp->years; i= i + 1) {   
                                              
    invp->invarray[i] = invp->growth * invp->invarray[i-1]; 
  }
}

int getUserInput(Investment *invp) {

  int valid;
  char buffer[MAX_MESSAGE_LENGTH];

  sprintf(buffer, "Enter investment, growth rate, number of yrs (up to %d): ",MAX_YEARS); //modified to sprintf
  NU32DIP_WriteUART1(buffer); // added
  NU32DIP_ReadUART1(buffer, MAX_MESSAGE_LENGTH); //added
  sscanf(buffer, "%lf %lf %d", &(invp->inv0), &(invp->growth), &(invp->years)); // modified to sscanf

  valid = (invp->inv0 > 0) && (invp->growth > 0) &&
    (invp->years > 0) && (invp->years <= MAX_YEARS);
  sprintf(buffer, "\r\nValid input?  %d\r\n",valid); //modified
  NU32DIP_WriteUART1(buffer); // added

  if (!valid) { 
    NU32DIP_WriteUART1("Invalid input; exiting.\r\n"); //modified
  }
  return(valid);
}

void sendOutput(double *arr, int yrs) {

  int i;
  char outstring[100];

  NU32DIP_WriteUART1("\r\nRESULTS:\r\n\n"); // modified
  for (i=0; i<=yrs; i++) {
    sprintf(outstring,"Year %3d:  %10.2f\r\n",i,arr[i]); 
    NU32DIP_WriteUART1(outstring); // modified
  }
  NU32DIP_WriteUART1("\r\n"); //modified
}
