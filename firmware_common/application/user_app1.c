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

extern u8 G_au8DebugScanfBuffer[];
extern u8 G_u8DebugScanfCharCount;

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
typedef enum{OCTAVE3=0, OCTAVE4=1, OCTAVE5=2, OCTAVE6=3} Octave;
static Octave previousOctave = OCTAVE3;

static u16 getNote(u8 keyboardInput){
  
  switch(keyboardInput){
    
  case 'Z':
    return C3;
  case 'X':
    return D3;
  case 'C':
    return E3;
  case 'V':
    return F3;
  case 'B':
    return G3;
  case 'N':
    return A3;
  case 'M':
    return B3;
  case 'S':
    return C3S;
  case 'D':
    return D3S;
  case 'G':
     return F3S;
  case 'H':
     return G3S;
  case 'J':
     return A3S;
  default:
    return NONE;
  }
}

static void playBuzzerFor500ms(void){
  PWMAudioOn(BUZZER1);
  for(u16 i = 0; i < 500; ++i);
  PWMAudioOff(BUZZER1);
}

static Octave getCurrentOctave(void){
  if(WasButtonPressed(BUTTON0)){
    ButtonAcknowledge(BUTTON0);
    previousOctave = OCTAVE3;
    return OCTAVE3;
  }
  else if(WasButtonPressed(BUTTON1)){
    ButtonAcknowledge(BUTTON1);
    previousOctave = OCTAVE4;
    return OCTAVE4;
  }
  else if(WasButtonPressed(BUTTON2)){
    ButtonAcknowledge(BUTTON2);
    previousOctave = OCTAVE5;
    return OCTAVE5;
  }
  else if(WasButtonPressed(BUTTON3)){
    ButtonAcknowledge(BUTTON3);
    previousOctave = OCTAVE6;
    return OCTAVE6;
  }
  else{
    return previousOctave;
  }
}

static void turnOnOctaveLED(Octave currentOctave){
   LedOff(CYAN);
   LedOff(GREEN);
   LedOff(YELLOW);
   LedOff(ORANGE);
   
   switch(currentOctave){
     
     case OCTAVE3:
       LedOn(CYAN);
       return;
   
    case OCTAVE4:
       LedOn(GREEN);
       return;
   case OCTAVE5:
       LedOn(YELLOW);
       return;
   case OCTAVE6:
       LedOn(ORANGE);
       return;
   }
}

/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/
/*-------------------------------------------------------------------------------------------------------------------*/
/* What does this state do? */

static void UserApp1SM_Idle(void)
{
    /*
  
  int octave = 1;
  int octaveMultiplier = pow(2,octave);
  */
  Octave currentOctave = getCurrentOctave();
  turnOnOctaveLED(currentOctave);
  u8 octaveMultiplier = pow(2,(double) currentOctave);
  
  G_u8DebugScanfCharCount = DebugScanf(au8UserInputBuffer);
  for(u8 i = 0; i < G_u8DebugScanfCharCount; ++i){
    u16 noteToPlay = getNote(au8UserInputBuffer[i]) * octaveMultiplier;
    PWMAudioSetFrequency(BUZZER1, noteToPlay);
    if(noteToPlay != NONE)
      playBuzzerFor500ms();
  }
  
} /* end UserApp1SM_Idle() */
     

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{

} /* end UserApp1SM_Error() */




/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
