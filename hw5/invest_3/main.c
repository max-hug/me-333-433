#include <stdio.h>
#include "nu32dip.h"  //added
#include "calculate.h"
#include "io.h"

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