/*!*********************************************************************************************************************
@file user_app1.h                                                                
@brief Header file for user_app1

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app1 as a template:
1. Follow the instructions at the top of user_app1.c
2. Use ctrl-h to find and replace all instances of "user_app1" with "yournewtaskname"
3. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
4. Use ctrl-h to find and replace all instances of "USER_APP1" with "YOUR_NEW_TASK_NAME"
5. Add #include yournewtaskname.h" to configuration.h
6. Add/update any special configurations required in configuration.h (e.g. peripheral assignment and setup values)
7. Delete this text (between the dashed lines)
----------------------------------------------------------------------------------------------------------------------

**********************************************************************************************************************/

#ifndef __USER_APP1_H
#define __USER_APP1_H

/**********************************************************************************************************************
Type Definitions
**********************************************************************************************************************/


/**********************************************************************************************************************
Function Declarations
**********************************************************************************************************************/
#define NUM_DIGITS 4
#define NUM_POSSIBILITIES (NUM_DIGITS* NUM_DIGITS * NUM_DIGITS* NUM_DIGITS)

// Define parameters for the LCG algorithm
#define LCG_MULTIPLIER (u8)13
#define LCG_INCREMENT (u8)7
#define LCG_MODULUS (u8)251  //Prime Number

/*------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

//PROMISES:
//Creates an array with all permutations of possible passwords
u32 lcg_rand(void);
void lcg_seed(unsigned int seed);
void alarmOn(int randomPassword[NUM_DIGITS]);
void getRandomPassword(int randomPassword[NUM_DIGITS]);

void alarmSound(void);

void displayTime(u32 timeInSeconds);
void displayChangeTime(u32 timeInSeconds);
void updateTime(void);
void changeTime(void);

/*------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/
void UserApp1Initialize(void);
void UserApp1RunActiveState(void);


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/***********************************************************************************************************************
State Machine Declarations
***********************************************************************************************************************/
       


/**********************************************************************************************************************
Constants / Definitions
**********************************************************************************************************************/



#endif /* __USER_APP1_H */

/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
