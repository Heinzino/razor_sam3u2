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
static u32 lcg_state = 0;


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
//State Machine Declarations

//static u32 UserApp1_u32Timeout;                           /*!< @brief Timeout counter used across states */

static StateMachineType UserApp1SM;

static void HomeState(StateMachineEventType Event);
static void SetTimeState(StateMachineEventType Event);
static void SetAlarmState(StateMachineEventType Event);
static void AlarmState(StateMachineEventType Event);



/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

//AUDIO
 int melody[] = {

  // Nokia Ringtone 
  // Score available at https://musescore.com/user/29944637/scores/5266155
  
  NOTE_E5, 8, NOTE_D5, 8, NOTE_F4_SHARP, 4, NOTE_G4_SHARP, 4, 
  NOTE_C5_SHARP, 8, NOTE_B4, 8, NOTE_D4, 4, NOTE_E4, 4, 
  NOTE_B4, 8, NOTE_A4, 8, NOTE_C4_SHARP, 4, NOTE_E4, 4,
  NOTE_A4, 2, 
 };
 u8 numNotes = (sizeof(melody) / sizeof(melody[0])) / 2;
 int divider = 0, noteDuration = 0;
 static bool endOfSong = TRUE;
 static u8 lightCodeIndex = 0;
 int randomPassword[NUM_DIGITS];



 

//TIME
#define OFFSET_TO_CENTER_TIME 8
static u32 timeDisplayed = 27000;
static u32 oldTimerTime = 0, newTimerTime = 0;
static u32 alarmTime = 0;
static u32 u32Alarmtimer = 0; 
static bool alarmSet = FALSE;
static bool cursorOn = FALSE;
static int newTime = -1;



 
/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

void lcg_seed(unsigned int seed) {
    lcg_state = seed;
}

u32 lcg_rand(){
    lcg_state = (LCG_MULTIPLIER * lcg_state + LCG_INCREMENT) % LCG_MODULUS;
    return lcg_state;
}

void getRandomPassword(int randomPassword[NUM_DIGITS]) {
    u8 au8possibleCodes[NUM_DIGITS] = {0,1,2,3};
    for (int i = 0; i < NUM_DIGITS; i++) {
        u32 randIndex = lcg_rand() % NUM_DIGITS; 
        randomPassword[i] = au8possibleCodes[randIndex]; 
    }
}

void alarmOn(int randomPassword[NUM_DIGITS]){
  
  alarmSound();

  if(endOfSong){
     endOfSong = FALSE;
     if(lightCodeIndex == 0){
      PWMAudioSetFrequency(BUZZER1,NOTE_E6);
      PWMAudioOn(BUZZER1);
    }
    
     switch(randomPassword[lightCodeIndex]){
      case 0:
        LedOn(WHITE);
        LedOff(BLUE);
        LedOff(YELLOW);
        LedOff(RED);
        break;
      case 1:
        LedOn(BLUE);
        LedOff(YELLOW);
        LedOff(RED);
        LedOff(WHITE);
        break;
      case 2:
        LedOn(YELLOW);
        LedOff(RED);
        LedOff(WHITE);
        LedOff(BLUE);
        break;
      case 3:
        LedOn(RED);
        LedOff(WHITE);
        LedOff(BLUE);
        LedOff(YELLOW);
        break;
    }
    
    lightCodeIndex++;
    if(lightCodeIndex >= NUM_DIGITS){
      lightCodeIndex = 0;
    }
  }

  
}

void alarmSound(void){
  static int alarmIndex = 0;
  static u8 notesPlayed = 0;
  static u16 u16CurrentDuration = 0;
  static u16 u16NoteSilentDuration = 0;
  static bool bNoteActive = TRUE;
  
  u8 u8currentIndex;

  
  if(IsTimeUp(&u32Alarmtimer, u16CurrentDuration)){
    
    u32Alarmtimer = G_u32SystemTime1ms;
    u8currentIndex = alarmIndex;
    
    if(bNoteActive){
        divider = melody[u8currentIndex + 1];
        noteDuration = MEASURE_TIME / divider;
        u16CurrentDuration = noteDuration - REGULAR_NOTE_ADJUSTMENT;
        u16NoteSilentDuration = REGULAR_NOTE_ADJUSTMENT;
        bNoteActive = FALSE;
        
        PWMAudioSetFrequency(BUZZER2,melody[u8currentIndex]);
        PWMAudioOn(BUZZER2);
    }
    
    else{
        notesPlayed += 1;
        alarmIndex += 2;
        PWMAudioOff(BUZZER2);
        PWMAudioOff(BUZZER1);
        u32Alarmtimer = G_u32SystemTime1ms;
        u16CurrentDuration = u16NoteSilentDuration;
        bNoteActive = TRUE;
        
        if(notesPlayed == numNotes){
           alarmIndex = 0;
           notesPlayed = 0;
           endOfSong = TRUE;
        }
    }
    
  }

}

static bool incrementedTime(void){
  newTimerTime = G_u32SystemTime1s;
  u32 diffInSec = newTimerTime - oldTimerTime; //Loop time again incase you add time over a day
  if(diffInSec > 60){
    timeDisplayed+= 60*(diffInSec%60);
    timeDisplayed%= 86400;
    oldTimerTime= newTimerTime;
    return TRUE;
  }
  else{
    return FALSE;
  }
}

void updateTime(void){
  if(incrementedTime()){
    displayTime(timeDisplayed);
  }
}

static void displayHourAndMinutes(u32 timeInSeconds){
  u8 hrs = timeInSeconds / 3600;
  timeInSeconds %= 3600;
  u8 mins = timeInSeconds / 60;
  
  u8 timeString[6];
  timeString[0] = (hrs / 10) + '0';
  timeString[1] = (hrs % 10) + '0';
  timeString[2] = ':';
    
    // Format minutes
  timeString[3] = (mins / 10) + '0';
  timeString[4] = (mins % 10) + '0';
    
  // Null-terminate the string
  timeString[5] = '\0';
  
  LcdMessage(LINE1_START_ADDR+OFFSET_TO_CENTER_TIME,timeString);
}

void displayTime(u32 timeInSeconds){
  
  displayHourAndMinutes(timeInSeconds);
  
  LcdMessage(LINE2_START_ADDR,"ST");
  LcdMessage(LINE2_END_ADDR-1,"SA");
}

void displayChangeTime(u32 timeInSeconds){
  displayHourAndMinutes(timeInSeconds);
  
  u8 au8BtnConfirmMessage[] = "HOLD B3 TO CONFIRM  ";
  LcdMessage(LINE2_START_ADDR,au8BtnConfirmMessage);
}

void changeTime(void){
   
  if(newTime == -1){
    newTime = timeDisplayed;
  }
  
  u8 hrs = newTime / 3600;
  u8 min = (newTime%3600) / 60;
  
  u8 hrsTenDigit = (hrs/10) % 10;
  u8 hrsOneDigit = hrs%10;
  u8 minTenDigit = (min/10) % 10;
  u8 minOneDigit = min % 10;
  
  static u8 position = 0;
  static bool cursorInPosition = FALSE;
  static bool newTimeValueChange = FALSE;
  
  if(!cursorOn){
    LcdCommand(LCD_DISPLAY_CMD | LCD_DISPLAY_ON | LCD_DISPLAY_CURSOR | LCD_DISPLAY_BLINK);
    cursorOn = TRUE;
  }
  
  
  if(cursorOn && !cursorInPosition){
     LcdCommand(LCD_ADDRESS_CMD | (position+OFFSET_TO_CENTER_TIME));
     cursorInPosition = TRUE;
  }
  
  if(newTimeValueChange){
     displayChangeTime(newTime);
     newTimeValueChange = FALSE;
  }
  
  if(WasButtonPressed(BUTTON0)){
    ButtonAcknowledge(BUTTON0);
    cursorInPosition = FALSE;
    if(position == 3){
       position -= 2; //To jump the colon
    }
    else if(position > 0){
       position -= 1;
    }
  }
  else if(WasButtonPressed(BUTTON3)){
    ButtonAcknowledge(BUTTON3);
    cursorInPosition = FALSE;
    if(position == 1){
       position += 2; //To jump the colon
    }
    else if(position < 4){
      position += 1;
    }
  }
  else if(WasButtonPressed(BUTTON2)){
    ButtonAcknowledge(BUTTON2);
    newTimeValueChange = TRUE;
    switch(position){
      case 0:
        if(newTime <= 50340 ){//Less than 13:59
        newTime += 36000;
        }
        break;
      case 1:
        if(hrsOneDigit < 9){
        newTime += 3600;
        }
        break;
      case 3:
        if(minTenDigit < 5){
        newTime += 600;
        }
        break;
      case 4:
        if(minOneDigit < 9){
        newTime += 60;
        }
        break;
    }
  }
  else if(WasButtonPressed(BUTTON1)){
    ButtonAcknowledge(BUTTON1);
    newTimeValueChange = TRUE;
    switch(position){
      case 0:
        if(hrsTenDigit > 0){
          newTime -= 36000;
        }
        break;
      case 1:
        if(hrsOneDigit > 0){
          newTime -= 3600;
        }
        break;
      case 3:
        if(minTenDigit > 0){
          newTime -= 600;
        }
        break;
      case 4:
        if(minOneDigit > 0){
        newTime -= 60;
        }
        break;
    }
  }
  
  return;
}

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
   lcg_seed(1);
      
  /* If good initialization, set state to Idle */
  LcdCommand(LCD_CLEAR_CMD);
  displayTime(timeDisplayed);
  LcdCommand(LCD_HOME_CMD);
  oldTimerTime = G_u32SystemTime1s;

  InitStateMachine(&UserApp1SM, HomeState);

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
  RunStateMachine(&UserApp1SM);
} /* end UserApp1RunActiveState */


void HomeState(StateMachineEventType Event){
  
  switch(Event){
    
  case SM_ENTER_EVENT:
    LedOn(LCD_RED);
    LedOn(LCD_GREEN);
    LedOn(LCD_BLUE);
    AckAllBtns();
    displayTime(timeDisplayed);
    LedOff(BLUE);
    LedOff(YELLOW);
    LedOff(RED);
    LedOff(WHITE);
    PWMAudioOff(BUZZER1);
    PWMAudioOff(BUZZER2);
    break;
    
  case SM_TICK_EVENT:
    updateTime();
    
    if(alarmTime == timeDisplayed && alarmSet){
      alarmSet = FALSE;
      ChangeState(&UserApp1SM,AlarmState);
    }
    
    if(WasButtonPressed(BUTTON0)){
      ButtonAcknowledge(BUTTON0);
      ChangeState(&UserApp1SM, SetTimeState);
    }
    if(WasButtonPressed(BUTTON3)){
      ButtonAcknowledge(BUTTON3);
      ChangeState(&UserApp1SM,SetAlarmState);
    }
    break;
  case SM_TIMEOUT_EVENT:
    ChangeState(&UserApp1SM, HomeState);
    break;
    
  case SM_EXIT_EVENT:
    break;


  }  
}

void SetTimeState(StateMachineEventType Event){
  
  switch(Event){
    
  case SM_ENTER_EVENT:
    AckAllBtns();
    LedOff(LCD_RED);
    LedOff(LCD_GREEN);
    LedOn(LCD_BLUE);
    displayChangeTime(timeDisplayed);
    break;
  case SM_TICK_EVENT:
    changeTime();
    if(IsButtonHeld(BUTTON3,5000)){
      ButtonAcknowledge(BUTTON3);
      timeDisplayed = newTime;
      oldTimerTime = G_u32SystemTime1s;
      ChangeState(&UserApp1SM, HomeState);
    }
    break;
   
  case SM_TIMEOUT_EVENT:
    if(anyBtnPressed()){
      AckAllBtns();
      ChangeState(&UserApp1SM,SetTimeState);
    }
    else{
      ChangeState(&UserApp1SM, HomeState);
    }
    break;
    
  case SM_EXIT_EVENT:
    LcdCommand(LCD_DISPLAY_BLINK);
    LcdCommand(LCD_DISPLAY_CURSOR);
    cursorOn = FALSE;
    break;
    
  }
}

void SetAlarmState(StateMachineEventType Event){
  
  switch(Event){
    
  case SM_ENTER_EVENT:
    AckAllBtns();
    LedOff(LCD_RED);
    LedOff(LCD_GREEN);
    LedOn(LCD_BLUE);
    displayChangeTime(timeDisplayed);
    break;
  case SM_TICK_EVENT:
    changeTime();
    if(IsButtonHeld(BUTTON3,5000)){
      ButtonAcknowledge(BUTTON3);
      alarmTime = newTime;
      alarmSet = TRUE;
      ChangeState(&UserApp1SM, HomeState);
    }
    break;
   
  case SM_TIMEOUT_EVENT:
    if(anyBtnPressed()){
      AckAllBtns();
      ChangeState(&UserApp1SM,SetAlarmState);
    }
    else{
      ChangeState(&UserApp1SM, HomeState);
    }
    break;
    
  case SM_EXIT_EVENT:
    LcdCommand(LCD_DISPLAY_BLINK);
    LcdCommand(LCD_DISPLAY_CURSOR);
    cursorOn = FALSE;
    break;
    
  }
}

static u8 passwordEntry[NUM_DIGITS];
static u8 u8entryPassLength = 0;
static bool successfulPassword = FALSE;
void AlarmState(StateMachineEventType Event){

  switch(Event){
    
  case SM_ENTER_EVENT:
    LedOn(LCD_RED);
    LedOn(LCD_GREEN);
    LedOn(LCD_BLUE);
    AckAllBtns();
    displayTime(timeDisplayed);
    getRandomPassword(randomPassword);
    break;
   
  case SM_TICK_EVENT:
     alarmOn(randomPassword);
     updateTime();
     
     if(successfulPassword){
       ChangeState(&UserApp1SM,HomeState);
     }
     
     if(u8entryPassLength == 1){
       LedOn(LCD_RED);
       LedOn(LCD_GREEN);
       LedOn(LCD_BLUE);
     }
     
     if(u8entryPassLength < NUM_DIGITS){
       if(WasButtonPressed(BUTTON0)){
         ButtonAcknowledge(BUTTON0);
         passwordEntry[u8entryPassLength++] = 0;
       }
       else if(WasButtonPressed(BUTTON1)){
         ButtonAcknowledge(BUTTON1);
         passwordEntry[u8entryPassLength++] = 1;
       }
       else if(WasButtonPressed(BUTTON2)){
         ButtonAcknowledge(BUTTON2);
         passwordEntry[u8entryPassLength++] = 2;
       }  
       else if(WasButtonPressed(BUTTON3)){
         ButtonAcknowledge(BUTTON3);
         passwordEntry[u8entryPassLength++] = 3;
       }  
     }
     else if(u8entryPassLength == NUM_DIGITS){
        
       for(int i = 0; i < NUM_DIGITS; ++i){
         if(passwordEntry[i] != randomPassword[i]){
              LedOn(LCD_RED);
              LedOff(LCD_GREEN);
              LedOff(LCD_BLUE);
              u8entryPassLength = 0;
              successfulPassword = FALSE;
         }
       }
       
       if(u8entryPassLength == NUM_DIGITS){
         successfulPassword = TRUE;
          LedOff(LCD_RED);
          LedOn(LCD_GREEN);
          LedOff(LCD_BLUE);
       }  
     }
    
  case SM_TIMEOUT_EVENT:
     if(successfulPassword){
       ChangeState(&UserApp1SM,HomeState);
     }
     else{
       ChangeState(&UserApp1SM,AlarmState);
     }
     
  case SM_EXIT_EVENT:
    if(successfulPassword){
      u8entryPassLength=0;
      alarmSet = FALSE;
      successfulPassword = FALSE;
      lightCodeIndex = 0;
      endOfSong = TRUE;
    }
    break;
     
    
  }
}
/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/
/*-------------------------------------------------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------------------------------------------------*/




/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
