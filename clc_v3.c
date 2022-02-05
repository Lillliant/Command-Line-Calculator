/* 
Program Title: clc.c
Name: Christine Wong
Date: 2021-Dec-1

This program implements a simple command-line calculator, in which a user can perform math operations.

NOTE: 
*/

//import C Standard Library for input/output functions, etc.
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

/*
---------------------------------
CONSTANTS AND GLOBAL VARIABLES
---------------------------------
*/
#define TRUE 1
#define FALSE 0
#define MEMORY_RANGE 1000 //the number of elements that the memory in calculator stores
//constants that indicates what type of information the memory array stores
#define NUM 0
#define OPERATOR 1
#define NUM2 2
#define RESULT 3
#define CALCULATION_MODE 4
//For testing inputs against valid inputs
#define MAINMENU_OPS "BUVAMRX"
#define ADVANCED_OPS "BUX"
#define BINARY_OPS "+-*/%^"
#define UNARY_OPS	"SLECF"
#define VARIABLE_NAMES "ABCDE"
//for printing out some constant messages in the program.
#define BINARY_OPS_PROMPT "Please enter the operation ( + , - , * , / , % , ^ (for power) ):"
#define UNARY_OPS_PROMPT "Please enter the operation ( S (for square root) , L (for logarithm) , E (for exponential) , C (for ceil) , F (for floor) ):"
#define VARIABLE_PROMPT "Please enter the name of the variable ( A single character between 'a' to 'e' ):"
#define MAINMENU_PROMPT "-----------------------------------------------\nPlease select your option ( B (Binary operation), U (Unary operation) , A (Advanced), M (Memory), R (Reset Memory), V (Variables), X (Exit)):"
#define ADVANCEDMENU_PROMPT "-----------------------------------------------\nPlease select your option ( B (Binary operation), U (Unary operation), X (Exit and back to the main menu))"
//global variables
float variables[5] = {0, 0, 0, 0, 0}; //variables A, B, C, D, E respectively
float memory[5][MEMORY_RANGE] = {0}; //stores the result of operations, along with other information related to the operation (operator mode, numbers involved, etc.) for formatting purposes
int savedElement = 0; //keeps track of number of elements within the array.

/*
---------------------------------
FUNCTION PROTOTYPES
---------------------------------
*/
//message display functions/printing function
void displayWelcomeMessage();
void displayExitMessage();
void displayAdvancedMenu();
void displayOperatorMenu(char Ops[]);
void displayMemory(int range); //determine how many elements in memory to print out
void prettyPrintMemoryOutput(int start, int range);
//input functions
char getCharacter(char msgPrompt[], char validChars[]);
int getValidatedMemoryOption();
float getFloat(char displayMessage[]);
float getFloatOrVar(char msgPrompt[]);
int getMemoryRange();
//validates input from input functions
int validateChar(char toBeValidated, char validChars[]);
float validatedBinaryNum2(char op, char msgPrompt[]);
float validatedUnaryNum(char op, char msgPrompt[]);
float validatedAdvancedUnaryNum(char op, char msgPrompt[]); //used in advanced mode
float validatedAdvancedBinaryNum(char op, char msgPrompt[]); //used in advanced mode
//calculating/main control center function within different modes
float calculateBinaryMode(float n, char op, float n2);
float calculateUnaryMode(float n, char op);
void assignVariable(char var, float val);
void advancedMode();
void memoryMode();
//handles misc operations within calculators
float returnVarVal(char v);
//memory functions
void shiftMemory(); //shifts elements in array one unit to the left if new elements can cause overflow (exceeds array length)
void validatedAddToMemory(float n, char op, float n2, float result, char mode); //handles exceptions when adding to memory
void addToMemory(int pos, float n, char op, float n2, float result, char mode); //raw function that adds to memory array
void resetMemory();

/*
---------------------------------
MAIN FUNCTION
---------------------------------
*/
int main(void){
  char option = ' '; //records what option user selected
  char op = ' '; //records what operator user selected
  float num = 0; //the first (or the only) number in the operation
  float num2 = 0; //the second number in the operation
  float result = 0; //records what result user's operation produces
  
  displayWelcomeMessage();

  do{//gets user option for what mode to receive and responds appropriately
    option = getCharacter(MAINMENU_PROMPT, MAINMENU_OPS);

    switch(option){
      case 'B':
        //gets valid input for operation
        num = getFloat("Please enter the first number:");
        op = getCharacter(BINARY_OPS_PROMPT, BINARY_OPS);
        num2 = validatedBinaryNum2(op, "Please enter the second number:");
        //performs operation accordingly
        result = calculateBinaryMode(num, op, num2);
        validatedAddToMemory(num, op, num2, result, 'B'); //adds to memory after operation completes
        break;
      case 'U':
        //gets valid input for operation
        op = getCharacter(UNARY_OPS_PROMPT, UNARY_OPS);
        num = validatedUnaryNum(op, "Please enter a number:");
        //performs operation accordingly
        result = calculateUnaryMode(num, op);
        validatedAddToMemory(num, op, 0, result, 'U'); //adds to memory after operation completes (note num2 = 0 since it is not used in the operation)
        break;
      case 'A':
        advancedMode(); //enters an inner while loop for advanced mode
        break;
      case 'M':
        memoryMode(); //enters an inner while loop for memory mode
        break;
      case 'R':
        resetMemory(); //resets the memory array
        break;
      case 'V':
        //gets valid input for operation
        op = getCharacter(VARIABLE_PROMPT, VARIABLE_NAMES);
        num = getFloat("Please enter a value to assign to the variable:");
        //performs operation accordingly
        assignVariable(op, num);
        validatedAddToMemory(num, op, 0, 0, 'V'); //adds to memory after operation completes (note num2 and result = 0 since they are not used in the operation)
        break;
      case 'X':
        displayExitMessage(); //displays final message of program
        break;
    }
  } while(option!='X');
} //program terminates when option  = 'X' to Exit

/*
---------------------------------
FUNCTION DEFINITIONS
---------------------------------
*/

//Shows a welcome message and display the main menu to the user when they first enter the program
void displayWelcomeMessage(){
  puts("Welcome to my Command-Line Calculator (CLC)");
  puts("Developer: Christine Wong");
  puts("Version: 3");
  puts("Date: 2021-Dec-01");
  puts("-----------------------------------------------");
  puts("B) - Binary Mathematical Operations such as addition and subtraction.");
  puts("U) - Unary Mathematical Operations, such as square root, and log.");
  puts("A) - Advanced Mathematical Operations, using variables, arrays.");
  puts("M) - View the calculator's memory.");
  puts("R) - Reset the calculator's memory.");
  puts("V) - Define variables and assign them values.");
  puts("X) - Exit.");
}

//show a goodbye message to the user
void displayExitMessage(){
  puts("Thanks for using my Simple Calculator. Hope to see you soon again. Goodbye!");
}

//Shows the advanced mode's menu to the user when they first enter the advanced mode
void displayAdvancedMenu(){
  puts("Select one of the following items:\nB) - Binary Mathematical Operations, such as addition and subtraction.\nU) - Unary Mathematical Operations, such as square root, and log.\nX) - Exit and back to the main menu.");
}

//gets user input for a character that is from a list of valid characters
char getCharacter(char msgPrompt[], char validChars[]){
  char op = ' '; //the option user selects

  do{
    puts(msgPrompt); //prompts user for option/valid input
    scanf(" %1c", &op);
    scanf("%*[^\n]"); //clears input stream so wrong input doesn't overflow to the next scanf call

  } while(!validateChar(toupper(op), validChars)); //if the input is not within the list of valid options, repeat above process until gets a valid input.
 
  return toupper(op);
}

//validates a character by checking if it is in a list of valid characters
int validateChar(char toBeValidated, char validChars[]){
  int length = strlen(validChars);

  for(int i = 0; i < length; i++){
    if(toBeValidated == validChars[i]){
      return TRUE;
    }
  }

  return FALSE; //if reaches here, then no character in the given string matches the char that is checked on
}

//gets user input as a valid float value
float getFloat(char displayMessage[]){
  float n = 0;
  int valid = 0;

  do{
    puts(displayMessage); //prompts user for input
    valid = scanf(" %f", &n);
    scanf("%*[^\n]"); //clears input stream so wrong input doesn't overflow to the next scanf call
  } while(!valid); //if scanf didn't receive a valid float, repeat prompt for valid input

  return n;
}

//calculates operations in binary mode
float calculateBinaryMode(float n, char op, float n2){
  float r = 0;

  switch(op){//performs operations according to the operator received
    case '+': r = n+n2; break;
    case '-': r = n-n2; break;
    case '*': r =  n*n2; break;
    case '/': r =  n/n2; break;
    case '%': r =  fmod(n, n2); break;
    case '^': r = pow(n, n2); break;
  }

  printf("The result of %.2f %c %.2f is\t%.2f\n", n, op, n2, r); //prints out the result for user to see
  return r;
}

//calculates operations in unary mode
float calculateUnaryMode(float n, char op){
  float r = 0;

  switch(op){//performs operations according to the operator received
    case 'S': r = sqrt(n); break;
    case 'L': r = log10(n); break;
    case 'E': r = exp(n); break;
    case 'C': r = ceil(n); break;
    case 'F': r = floor(n); break;
  }
  
  printf("The result of %c(%.2f) is\t%.2f\n", op, n, r); //prints out the result for user to see
  return r;
}

//performs assignment to variables in variable mode
void assignVariable(char var, float val){
  switch(var){//given appropriate variable name, assign to appropriate locations
    case 'A': variables[0] = val; break;
    case 'B': variables[1] = val; break;
    case 'C': variables[2] = val; break;
    case 'D': variables[3] = val; break;
    case 'E': variables[4] = val; break;
  }

  printf("Variable %c is set to:\t%.2f\n", var, val); //prints out the result for user to see
}

//performs the necessary operations in advanced mode (since inner loop)
void advancedMode(){
  char option = ' '; //the option used in advanced mode
  //the variables necessary for calculation
  float num = 0;
  float num2 = 0;
  char op = ' ';
  float result = ' ';

  displayAdvancedMenu(); //displays menu for user to see so they know what to select

  do{
    option = getCharacter(ADVANCEDMENU_PROMPT, ADVANCED_OPS);

    switch(option){//gotten user input for option and calculate accordingly
      case 'B':
        //get valid inputs for operation
        num = getFloatOrVar("Please enter the first number or a variable name ('a' to 'e'):");
        op = getCharacter(BINARY_OPS_PROMPT, BINARY_OPS);
        num2 = validatedAdvancedBinaryNum(op, "Please enter the second number or a variable name ('a' to 'e'):");
        //performs operation accordingly
        result = calculateBinaryMode(num, op, num2);
        validatedAddToMemory(num, op, num2, result, 'B'); //adds result to memory
        break;
      case 'U':
        //get valid inputs for operation
        op = getCharacter(UNARY_OPS_PROMPT, UNARY_OPS);
        num = validatedAdvancedUnaryNum(op, "Please enter a number or a variable name ('a' to 'e'):");
        //performs operation accordingly
        result = calculateUnaryMode(num, op);
        validatedAddToMemory(num, op, 0, result, 'U'); //adds result to memory
        break;
    }
  } while(option != 'X'); //if option = 'X', returns to main menu
}

//unique to the advanced mode: gets either a numerical input or character input to look for appropriate variable
float getFloatOrVar(char msgPrompt[]){
  float num = 0;
  char varName = ' ';
  do{
    puts(msgPrompt);
    scanf("%*[^\n]"); //clears input stream to prevent overflow
    if (scanf(" %f", &num)){//if numerical input
      break;
    }
    else{//if character input
      scanf(" %1c", &varName);
      if(validateChar(toupper(varName), VARIABLE_NAMES)){//checks if it is appropriate variable name (between a-e)
        num = returnVarVal(toupper(varName)); //gets the value at the position of the valid variable name
        break;
      }
    }
  } while(1);

  return num; //if reaches here, then must've gotten valid input to break out the loop
}

//given a variable name, return the value associated with that variable
//if cannot find the right variable, return a default value of 0
float returnVarVal(char v){
  switch(v){
    case 'A': return variables[0]; break;
    case 'B': return variables[1]; break;
    case 'C': return variables[2]; break;
    case 'D': return variables[3]; break;
    case 'E': return variables[4]; break;
    default: return 0; break;
  }
}

//validates a number that is used in binary mode to prevent undefined operation
//(within the context of this program, only checks for num2 since no value of num will result in undefined operation)
float validatedBinaryNum2(char op, char msgPrompt[]){
  float n = getFloat(msgPrompt);
  while((op == '/' || op == '%') && n == 0){//division by zero is undefined
    n = getFloat("Please enter the second number (non-zero):"); //get new value using error message prompt until input is finally valid
  }
  return n;
}

//validates a number that is used in unary mode to prevent undefined operation
float validatedUnaryNum(char op, char msgPrompt[]){
  float n = getFloat(msgPrompt);
  while(op == 'S' && n < 0){//sqrt cannot take negative number in real number system
    n = getFloat("Enter a positive value:");
  }

  while (op == 'L' && n <= 0){//log 10 cannot take negative number and 0 in real number system
    n = getFloat("Enter a positive value:");
  }

  return n;
}

//validates a number that is used in advanced binary mode to prevent undefined operation
float validatedAdvancedBinaryNum(char op, char msgPrompt[]){
  float n = getFloatOrVar(msgPrompt);
  while((op == '/' || op == '%') && n == 0){//division by 0 is undefined
    n = getFloatOrVar("Denominator can't be zero.\nPlease enter the second number or a variable name ('a' to 'e'):"); //get a valid float OR variable value, which is unique to advanced mode
  }
  return n;
}

//validates a number that is used in advanced unary mode to prevent undefined operation
float validatedAdvancedUnaryNum(char op, char msgPrompt[]){
  float n = getFloatOrVar(msgPrompt); 
  while(op == 'S' && n < 0){//sqrt-ing negative number is undefined 
    n = getFloatOrVar("Value can't be negative.\nPlease enter the second number or a variable name ('a' to 'e'):"); //gets valid float OR variable value, which is unique to advanced mode
  }

  while (op == 'L' && n <= 0){//log-ing negative number and 0 is undefined
    n = getFloatOrVar("Value can't be zero or negative.\nPlease enter the second number or a variable name ('a' to 'e'):");
  }

  return n;
}

//adds an operation to the memory array
//there are 5 "rows" in the 2D (5 x n) array, each row accounting for a specific aspect of the operation they collectively store
void addToMemory(int pos, float n, char op, float n2, float result, char mode){
  switch(mode){//stores the appropriate value to the appropriate location as labelled
    case 'B':
    case 'U':
    case 'V':
      memory[NUM][pos] = n;
      memory[OPERATOR][pos] = op;
      memory[NUM2][pos] = n2;
      memory[RESULT][pos] = result;
      memory[CALCULATION_MODE][pos] = mode;
      break;
  }
}

//serves to validate the input before adding something to memory array.
//makes sure that no element is accidentally erased by overwriting
void validatedAddToMemory(float n, char op, float n2, float result, char mode){
  if (savedElement+1 > MEMORY_RANGE){//if adding the element exceeds memory range
    puts("Exceeds memory. The least recent operation is deleted.");
    shiftMemory(); //deletes the oldest element to not overflow
    addToMemory(MEMORY_RANGE-1, n, op, n2, result, mode); 
    //because already exceeding, so deleting the oldest element still leave the memory array at maximum capacity. MEMORY_RANGE-1 as position to store elements because array index starts at 0, and MEMORY_RANGE is based on notion of saved elements which starts at 1.
    savedElement = MEMORY_RANGE; //doesn't -1 because MEMORY_RANGE is based on notion of saved elements which starts at 1.
  }
  else{//if no potential overflow - meaning there are spaces in memory array, so can just ''append'' to the end of existing elements
    addToMemory(savedElement, n, op, n2, result, mode);
    savedElement++; //+1 to account for the new addition to the operation history
  }
}

//handles "overflow" of elements of memory array by deleting the first and oldest element in the array and shifting all other element to the left to make room for the new element
void shiftMemory(){
  for (int i = 0; i<5; i++){//there are 5 "rows" in the 2D array, each accounting for a specific aspect of the operation they collectively store
    for (int j = 0; j < MEMORY_RANGE-1; j++){//the columns represent one operation. -1 so j+1 doesn't exceed index of array since index starts at 0 and MEMORY_RANGE starts at 1.
      memory[i][j]=memory[i][j+1]; //shifts right element to the left. By doing so the left-est element is overwritten
    }
    memory[i][MEMORY_RANGE-1]=0; //the right-est element is reserved for the new element, so clear that specific spot for the row to be 0.
  }
}

//creates an environment for user to operate in for mode M.
//because need user input to exit the mode, use a do-while loop
void memoryMode(){
  int option = 0; //instead of char, use int to determine whether user wants display certain elements, or to exit
  printf("There are currently %d elements in memory\n", savedElement);
  if(savedElement > 0){//only operate memory viewing when there is elements
    do{
      option = getValidatedMemoryOption(); //see what user wants to do
      displayMemory(option); //behave based on user's input
    } while(option != -1); //-1 means to exit the loop.
  }
  else if (savedElement == 0){//if no elements in memory, kick user out as specified in assignment 3 
    puts("Since there is no elements in memory, there's really nothing you can do here. You will be returned to the main menu.");
  }
}

//validates user input for memory option and returns a guaranteed valid value
int getValidatedMemoryOption(){
  int n = 0;
  do {
    n = getMemoryRange(); //gets user input first before validating
  } while (n>savedElement || n<-1); //if n>savedElement, cannot print the values that exceed index of memory array. If n = -1, then exit memory mode. If n = 0, see whole operation history. If n<-1, no assigned value to know what user wants to do.

  return n;
}

//gets user input on what they want to do in memory mode
int getMemoryRange(){
  int range = 0;
  char option = ' ';

  do{
    printf("-----------------------------------------------\nPlease select your option ( 0 (See whole history), a number from 1 to %d to see a specific item, X or -1 (Exit and back to the main menu:\n-----------------------------------------------\n", savedElement); //prompt for input
    if (scanf(" %d", &range)){//if input a number, automatically a "valid" input (needs to return to getValidatedMemoryOption to know the value is in range)
      break;
    }
    else{//if input a character
      scanf(" %1c", &option);
      if(validateChar(toupper(option), "X")){//makes sure user wants to exit the memory mode
        range = -1;
        break;
      }
    }

    scanf("%*[^\n]"); //clears input stream so wrong input doesn't overflow to the next scanf call
  } while(1); //only exits loop if enter a number or 'X'

  return range;
}

//takes in a range and determines how much of the memory array to display
void displayMemory(int range){
  if(range == 0){//0 represents the whole history in assignment 3
    prettyPrintMemoryOutput(0, savedElement);
  }
  else if (range == -1){//since exiting, doesn't display anything but notify user of this info - only available if savedElement > 0 and user wants to exist mode M
    puts("Exiting Memory mode...");
  }
  else{//given a range and pretty print base off of the range (assuming the range is valid: between 1-number of stored elements)
    prettyPrintMemoryOutput(range-1, range); //index for array starts at 0, so need to -1
  }
}

//print out specified range (assume valid because already validated previously when getting user input)
void prettyPrintMemoryOutput(int start, int range){
  puts("-----------------------------------------------");
  for(int i = start; i < range; i++){//formats output based on what mode they were (to ensure best representation)
    if(memory[4][i]=='B'){
      printf("[%d]\t%.2f %c %.2f = %.2f\n", i+1, memory[NUM][i], (int)memory[OPERATOR][i], memory[NUM2][i], memory[RESULT][i]); //typecast operator to int first, then use it as character since the array stores the ASCII value
    }
    else if(memory[4][i]=='U'){
      printf("[%d]\t%c(%.2f) = %.2f\n", i+1, (int)memory[OPERATOR][i], memory[NUM][i], memory[RESULT][i]);//typecast operator to int first, then use it as character since the array stores the ASCII value
    }
    else if(memory[4][i]=='V'){
      printf("[%d]\t%c -> %.2f\n", i+1, (int)memory[OPERATOR][i], memory[NUM][i]); //typecast operator to int first, then use it as character since the array stores the ASCII value
    }
  }
}

//for mode R - automatically clears all elements in memory array
void resetMemory(){
  for(int i = 0; i < savedElement; i++){
    memory[NUM][i] = 0;
    memory[OPERATOR][i] = 0;
    memory[NUM2][i] = 0;
    memory[RESULT][i] = 0;
    memory[CALCULATION_MODE][i] = 0;
  }
  savedElement = 0; //no elements, so savedElement = 0

  puts("All previous operations have been cleared."); //alert user
}

