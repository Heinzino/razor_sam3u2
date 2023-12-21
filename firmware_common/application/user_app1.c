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


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_<type>" and be declared as static.
***********************************************************************************************************************/

/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/
//State Machine Declarations

static StateMachineType UserApp1SM;

static void StartUpState(StateMachineEventType Event);
static void LockedState(StateMachineEventType Event);
static void ChangePasswordState(StateMachineEventType Event);
static void CheckPasswordState(StateMachineEventType Event);

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
  LedOff(WHITE);
  LedOff(PURPLE);
  LedOff(BLUE);
  LedOff(CYAN);
  LedOff(GREEN);
  LedOff(YELLOW);
  LedOff(ORANGE);
  LedOff(RED);
  /* If good initialization, set state to Idle */
  
  InitStateMachine(&UserApp1SM, StartUpState);

} /* end UserApp1Initialize() */

/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

static void AckAllBtns(void){
   ButtonAcknowledge(BUTTON0);
   ButtonAcknowledge(BUTTON1);
   ButtonAcknowledge(BUTTON2);
   ButtonAcknowledge(BUTTON3);
}

static bool anyBtnPressed(void){
  return (WasButtonPressed(BUTTON0) || WasButtonPressed(BUTTON1) 
          || WasButtonPressed(BUTTON2) || WasButtonPressed(BUTTON3));
}

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
  //Combo to change password
  if(IsButtonHeld(BUTTON1,3000) && IsButtonHeld(BUTTON2,3000)){ 
    AckAllBtns();
    ChangeState(&UserApp1SM,ChangePasswordState);
  }
  RunStateMachine(&UserApp1SM);
} /* end UserApp1RunActiveState */



/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/
/*-------------------------------------------------------------------------------------------------------------------*/
/* What does this state do? */

#define MAX_PASS_LENGTH 10
 static ButtonNameType password[MAX_PASS_LENGTH] = {BUTTON0, BUTTON1,BUTTON2};
 static u8 u8passLength = 3;
 
 static ButtonNameType passwordEntry[MAX_PASS_LENGTH];
 static u8 u8entryPassLength = 0;

 
 void StartUpState(StateMachineEventType Event){
   
   switch(Event){
     
   case SM_ENTER_EVENT:
     LedPWM(YELLOW,LED_PWM_70);
     SetTimeout(&UserApp1SM,3000); //3 seconds to enter setting state
     AckAllBtns();
     break;
   
   case SM_TICK_EVENT:
     if(WasButtonPressed(BUTTON3)){
       ButtonAcknowledge(BUTTON3);
       ChangeState(&UserApp1SM, ChangePasswordState);
     }
     break;
   
   
    case SM_TIMEOUT_EVENT:
       ChangeState(&UserApp1SM,LockedState);
       break;
     
   case SM_EXIT_EVENT:
       LedOff(YELLOW);
       break;

    }
 }
 
 void ChangePasswordState(StateMachineEventType Event){
   //Change password; input taken up to 10 letters or when button3 is pressed.
   switch(Event){
     
   case SM_ENTER_EVENT:
     LedBlink(RED,LED_1HZ);
     LedBlink(GREEN,LED_1HZ);
     AckAllBtns();
     u8passLength = 0;
     break;
   
   case SM_TIMEOUT_EVENT:
     break;
     
     
   case SM_TICK_EVENT:
     if(u8passLength < MAX_PASS_LENGTH){
       if(WasButtonPressed(BUTTON0)){
         ButtonAcknowledge(BUTTON0);
         password[u8passLength++] = BUTTON0;
       }
       else if(WasButtonPressed(BUTTON1)){
         ButtonAcknowledge(BUTTON1);
         password[u8passLength++] = BUTTON1;
       }
       else if(WasButtonPressed(BUTTON2)){
         ButtonAcknowledge(BUTTON2);
         password[u8passLength++] = BUTTON2;
       }  
     }
     if(u8passLength >= MAX_PASS_LENGTH || WasButtonPressed(BUTTON3)){
       AckAllBtns();
       ChangeState(&UserApp1SM,LockedState);
     }
     break;
     
   case SM_EXIT_EVENT:
     LedOff(RED);
     LedOff(GREEN);
     break;
   
   }//End Switch(Event)
 }//End of Function
 
 
 void LockedState(StateMachineEventType Event){
   
   switch(Event){
     
     
   case SM_ENTER_EVENT:
     LedOn(RED);
     AckAllBtns();
     u8entryPassLength = 0;
     break;
     
   case SM_EXIT_EVENT:
     LedOff(RED);
     break;
     
   case SM_TIMEOUT_EVENT:
     break;
     
   case SM_TICK_EVENT:
     if(u8entryPassLength < MAX_PASS_LENGTH){
       if(WasButtonPressed(BUTTON0)){
         ButtonAcknowledge(BUTTON0);
         passwordEntry[u8entryPassLength++] = BUTTON0;
       }
       else if(WasButtonPressed(BUTTON1)){
         ButtonAcknowledge(BUTTON1);
         passwordEntry[u8entryPassLength++] = BUTTON1;
       }
       else if(WasButtonPressed(BUTTON2)){
         ButtonAcknowledge(BUTTON2);
         passwordEntry[u8entryPassLength++] = BUTTON2;
       }  
     }
     
     if(WasButtonPressed(BUTTON3)){
         ButtonAcknowledge(BUTTON3);
         ChangeState(&UserApp1SM, CheckPasswordState);
      }
      break;
               
   }
 }
 
               
 void CheckPasswordState(StateMachineEventType Event){
   
   switch(Event){
     
   case SM_TIMEOUT_EVENT:
       break;
       
   case SM_ENTER_EVENT:
     bool passMatch = FALSE;
     
     if(u8entryPassLength != u8passLength){
       passMatch = FALSE;
     }
     else{
       for(int i = 0; i < u8passLength; ++i){
         if(passwordEntry[i] != password[i]){
           passMatch = FALSE;
           break;
         }
         passMatch = TRUE;
       }
     }
     (passMatch) ? LedBlink(GREEN,LED_4HZ) : LedBlink(RED, LED_2HZ);  
     break;
   
   case SM_EXIT_EVENT:
     LedOff(GREEN);
     LedOff(RED);
     break;
     
   case SM_TICK_EVENT:
     if(anyBtnPressed()){
       AckAllBtns();
       ChangeState(&UserApp1SM,LockedState);
     }
     break;
   }
 }


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
