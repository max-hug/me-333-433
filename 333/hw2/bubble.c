// bubble.c
// from ME 333 textbook, modified by Max Hughes
// ME 333 HW 2

#include <stdio.h>
#include <string.h>
#define MAXLENGTH 100       // max length of string input

void getString(char *str);  // helper prototypes
void printResult(char *str);
int greaterThan(char ch1, char ch2);
void swap(char *str, int index1, int index2);

int main(void) {
  int len;                  // length of the entered string
  char str[MAXLENGTH];      // input should be no longer than MAXLENGTH

  getString(str);
  len = strlen(str);        // get length of the string, from string.h

  for(int i = 0; i <= len-1; i++){
    for(int j = 0; j <= len-2-i; j++){
        if(greaterThan(str[j], str[j+1])){
            swap(str, j, j+1);
        }
    }
  }

  printResult(str);
  return(0);
}

//
// getString
// Handles user input
//
void getString(char *str){
    printf("Input a string to be used: ");
    scanf("%s", str);
}

//
// printResult
// Prints the resultant string
//
void printResult(char *str){
    printf("The processed string is \"%s\".\n", str);
}

//
// greaterThan
// returns if ch1 is greather than ch2, based on ASCII value
//
int greaterThan(char ch1, char ch2){
    return ch1 > ch2;
}

//
// swap
// swaps the value at indicies index1 and index2 in string str
//
void swap(char *str, int index1, int index2){
    char temp = str[index2];
    str[index2] = str[index1];
    str[index1] = temp;
}

