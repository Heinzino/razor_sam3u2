/*!*********************************************************************************************************************
@file user_app1.c                                                                
@brief User's tasks / applications are written here.  This description
should be replaced by something specific to the task.

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app1 as a template:
 1. Copy both user_app1.c and user_app1.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app1" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP1" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
GLOBALS
- NONE

CONSTANTS
- NONE

TYPES
- NONE

PUBLIC FUNCTIONS
- NONE

PROTECTED FUNCTIONS
- void UserApp1Initialize(void)
- void UserApp1RunActiveState(void)


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>UserApp1"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp1Flags;                          /*!< @brief Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;                   /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;                    /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;                     /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;                /*!< @brief From main.c */

extern u8 G_au8DebugScanBuffer[];                       /*From debug.c*/
extern u8 G_u8DebugScanfCharCount;                     /*From debug.c*/
/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_pfStateMachine;               /*!< @brief The state machine function pointer */
//static u32 UserApp1_u32Timeout;                           /*!< @brief Timeout counter used across states */
static u8 au8UserInputBuffer[USER1_INPUT_BUFFER_SIZE]; 

/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!--------------------------------------------------------------------------------------------------------------------
@fn void UserApp1Initialize(void)

@brief
Initializes the State Machine and its variables.

Should only be called once in main init section.

Requires:
- NONE

Promises:
- NONE

*/
void UserApp1Initialize(void)
{
  
/* Module start 
  u8 u8String[] = "A string to print that returns cursor to the start of next line.\n\r";
  u8 u8String2[] = "Here's a number: ";
  u8 u8String3[] = " The 'cursor' was here.";
  u32 u32Number = 1234567;
  
  DebugPrintf(u8String);
  DebugPrintf(u8String2);
  DebugPrintNumber(u32Number);
  DebugPrintf(u8String3);
  DebugLineFeed();
  DebugPrintf(u8String3);
  DebugLineFeed();
*/
  
  
  DebugSetPassthrough();

  for(u32 i = 0; i < USER1_INPUT_BUFFER_SIZE; ++i){
    au8UserInputBuffer[i] = 0;
  }
  
  /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp1_pfStateMachine = UserApp1SM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp1_pfStateMachine = UserApp1SM_Error;
  }

} /* end UserApp1Initialize() */

  
/*!----------------------------------------------------------------------------------------------------------------------
@fn void UserApp1RunActiveState(void)

@brief Selects and runs one iteration of the current state in the state machine.

All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
- State machine function pointer points at current state

Promises:
- Calls the function to pointed by the state machine function pointer

*/
void UserApp1RunActiveState(void)
{
  UserApp1_pfStateMachine();

} /* end UserApp1RunActiveState */


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/
/*-------------------------------------------------------------------------------------------------------------------*/
/* What does this state do? */

#define MAX_NUM_COLS   30

static u8 u8NameCountMessage[] = "Heinz: ";
char *strNameCountMessage = "Heinz: ";

static int getNumDigits(u32 count){
  int result = 1;
  while(count/10 > 0){
    count /= 10;
    result++;
  }
  return result;
}

static void printHalfRow(int numDigits, int numCols){
   DebugPrintf("\n\r");
   for(int i = 0; i < numDigits; ++i){
       u8 rowToPrint[MAX_NUM_COLS];
       for(int j = 0; j < numCols; ++j){
            rowToPrint[j] = '*';
        }
          rowToPrint[numCols] = '\0';
          DebugPrintf(rowToPrint);
          DebugPrintf("\n\r");
    }
}

static void UserApp1SM_Idle(void)
{

  u8 u8charCount;
  const char *sName = "Heinz";
  u8 nameLength = strlen(sName);
  static u8 matchIndex = 0;
  static u32 u32nameOccuranceCount = 0;
  static bool firstInsert = TRUE;
        
  u8charCount = DebugScanf(au8UserInputBuffer);
  
  if(WasButtonPressed(BUTTON3)){
    ButtonAcknowledge(BUTTON3);
    u32nameOccuranceCount *= 10;
  }
  
  for(u8 i = 0; i < u8charCount && matchIndex < nameLength-1; ++i){
    
    char cInput = au8UserInputBuffer[i];
    
    char currentLetter = sName[matchIndex];
    //Condition in for loop -> no out of bounds
    char nextLetter = sName[matchIndex+1]; 
    
                          
    if(cInput == currentLetter){
      if(matchIndex == 0){ 
         firstInsert = FALSE;
      }
       continue;
    }
    else if(cInput == nextLetter && !firstInsert){
      matchIndex++;
    }
    else{
      matchIndex = 0;
      firstInsert = TRUE;
      break;
    }
 }
  
  if(matchIndex == nameLength - 1){
        matchIndex = 0;
        firstInsert = TRUE;
        u32nameOccuranceCount++;
        int numDigits = getNumDigits(u32nameOccuranceCount);
        
        int numCols = strlen(strNameCountMessage) + 3*numDigits;
        
        
        printHalfRow(numDigits,numCols);
        
        //Print Middle Row
        u8 middleRowStars[MAX_NUM_COLS];
        int i = 0;
        for(; i < numDigits; ++i){
          middleRowStars[i] = '*';
        }
        middleRowStars[i] = '\0';
        DebugPrintf(middleRowStars);
        DebugPrintf(u8NameCountMessage);
        DebugPrintNumber(u32nameOccuranceCount);
        DebugPrintf(middleRowStars);
        
        printHalfRow(numDigits,numCols);
    }
 
}
  
/* Module topic
  if(WasButtonPressed(BUTTON0)){
    ButtonAcknowledge(BUTTON0);
    
    DebugPrintf(u8NumCharsMessage);
    DebugPrintNumber(G_u8DebugScanfCharCount);
    DebugLineFeed();
  }
  
  if(WasButtonPressed(BUTTON1)){
    ButtonAcknowledge(BUTTON1);
    
    u8charCount = DebugScanf(au8UserInputBuffer);
    au8UserInputBuffer[u8charCount] = '\0';
    
    DebugPrintf(u8BufferMessage);
    DebugPrintf(au8UserInputBuffer);
    DebugLineFeed();
  }
*/
  
 /* end UserApp1SM_Idle() */
     

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */




/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
