#ifndef HELPER__H__
#define HELPER__H__

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

#endif // HELPER__H__