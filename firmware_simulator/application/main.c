/***********************************************************************************************************************
File: main.c                                                                

Description:
Container for the Cprimer "Server" application firmware.  
***********************************************************************************************************************/

#include "configuration.h"

#ifndef SOLUTION
/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_"
***********************************************************************************************************************/
/* New variables */


/*--------------------------------------------------------------------------------------------------------------------*/
/* External global variables defined in other files (must indicate which file they are defined in) */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "Main_" and be declared as static.
***********************************************************************************************************************/

static u8 Main_u8Servers = 0;

/***********************************************************************************************************************
Main Program
***********************************************************************************************************************/

void main(void)
{
/* Module code
  u32 u32UselessVariableForExample;
  u32 au32BigArray[] = {5,4,3,2,1};
  DrinkType aeDrink[3] = {BEER,SHOOTER};
  
  u8 u8Test = 0xA5;
  u8* pu8Example;
  u32 u32Test = 0x0000ffff;
  u32* pu32Example;
  
  pu8Example = &u8Test;
  pu32Example = &u32Test;
  
  *pu8Example += 1;
  (*pu32Example)++;
  
  pu8Example++;
  *pu32Example++;
  
  u8 u8CurrentServer;
  ServerType sServer1;
  ServerType* psServerParser;
  
  psServerParser = &sServer1;
  sServer1.u8ServerNumber = 18;
  u8CurrentServer = psServerParser -> u8ServerNumber;
 */
  
  u32 u32LoopCounter = 0;
  bool bOrderDrink = FALSE;
  
  ServerType *servers = NULL;
  ServerType *serverParser = NULL;
  
  const char au8MessageDrinkOrdered[]  = "Drink ordered   ";
  const char au8MessageDrinkServed[]   = "Drink served    ";
  const char au8MessageNewServer[]     = "New server added";
  const char au8MessageNoServer[]      = "No free server  ";
  const char au8MessageServerRemoved[] = "Server removed  ";
  char au8MessageCurrent[]             = "                ";
  bool bNewMessage = FALSE;
  
  while(1)
  {
    
    if(bOrderDrink){
        DrinkType drinkToOrder = (DrinkType)((u32LoopCounter % 4) + 1);
        serverParser = servers;
        
        while(serverParser != NULL && bOrderDrink){
          
          for(int i = 0; i < U8_MAX_DRINKS; ++i){
            if(serverParser->asServingTray[i] == EMPTY){
              serverParser->asServingTray[i] = drinkToOrder;
              bOrderDrink = FALSE;
              
              bNewMessage = TRUE;
              strcpy(au8MessageCurrent,au8MessageDrinkOrdered);
              break;
            }
          }
          
          serverParser = serverParser->psNextServer; 
        }
    }
    
    if(bOrderDrink){ //All servers are busy,see if you can make a new one
      
      if(CreateServer(&servers)){
        strcpy(au8MessageCurrent,au8MessageNewServer);
        bNewMessage= TRUE;
      }
      
      else{
         strcpy(au8MessageCurrent,au8MessageNoServer);
         bNewMessage= TRUE;
         
         bOrderDrink= FALSE;
      }
    }
    
    bool timeToRemoveDrink = (u32LoopCounter% 3000 == 0);
    
    if(timeToRemoveDrink){
      
      int serverToRemoveDrink = (u32LoopCounter% Main_u8Servers);
      
      //Move to that server
      serverParser= servers;
      for(int i = 0; i < serverToRemoveDrink; ++i){
        if(serverParser->psNextServer!= NULL){
           serverParser= serverParser->psNextServer;
        }
      }
      
      u8 u8emptyDrinkCounter = 0;
      for(int i = 0; i < U8_MAX_DRINKS; ++i){
        
        if(serverParser->asServingTray[i] != EMPTY){
           bool isFirstDrinkFoundOnTray = u8emptyDrinkCounter== 0;   
           if(isFirstDrinkFoundOnTray){
              serverParser->asServingTray[i] = EMPTY;
              u8emptyDrinkCounter++;
              strcpy(au8MessageCurrent, au8MessageDrinkServed);
              bNewMessage= TRUE;
           }
        }
        
        else{
          u8emptyDrinkCounter++;        
        }
      }
      
      bool needToRemoveServer = u8emptyDrinkCounter== U8_MAX_DRINKS;
      if(needToRemoveServer){
        
         ServerType *serverToRemove = serverParser;
         serverParser = servers;
         
         if(serverToRemove == serverParser){ //First one in the list
            servers = serverToRemove->psNextServer;
         }
         
         else{
           //Node right before the one to delete 
           while(serverParser->psNextServer != serverToRemove){
             serverParser = serverParser->psNextServer;
           }
           
           serverParser->psNextServer = serverToRemove->psNextServer;
         }
         
         free(serverToRemove);
         serverToRemove = NULL;
         bNewMessage = TRUE;
         strcpy(au8MessageCurrent,au8MessageServerRemoved);
         Main_u8Servers--;
        
      }
      
    }
    
    
    if(bNewMessage){
      bNewMessage = FALSE;
    }
    
    for(int i = 0; i < 1700; ++i);//Delay of 1ms
    
    u32LoopCounter++; 

  } /* end while(1) main loop */
    
} /* end main() */


/***********************************************************************************************************************
* Function definitions
***********************************************************************************************************************/
/*---------------------------------------------------------------------------------------------------------------------/
Function InitializeServer

Description:
Initializes a new server.  A new server has an empty tray of drinks and is
assigned the next number available.

Requires:
  - psServer_ points to the server list where a new server is to be initialized; 
    since the server list is a pointer to a linked list, this is a pointer-to-pointer
  - Main_u8Servers holds the current number of active servers

Promises:
  - Returns TRUE if the server is initialized
  - Returns FALSE if the server cannot be initialized
*/
bool InitializeServer(ServerType** psServer_)
{
  if(*psServer_ == NULL)
  {
    return(FALSE);
  }

  for(u8 i = 0; i < U8_MAX_DRINKS; ++i){
    (*psServer_) -> asServingTray[i] = EMPTY;
  }

  return(TRUE);
  
} /* end InitializeServer() */

/*--------------------------------------------------------------------------------------------------------------------
Function CreateServer

Creates a new, uninitialized server

Requires:
psServerList is a valid pointer where server will be added
Enough heap space to allocate

Promises:
Returns TRUE if server can be created and is appended to end of psServerList
Returns FALSE otherwise
*/

bool CreateServer(ServerType **psServerList){
  
  if(Main_u8Servers >= U8_MAX_SERVERS){
     return FALSE;
  }
  
  ServerType *newServer = 0;
  ServerType **serverParser = NULL;
  
  newServer = malloc(sizeof(ServerType));
  if(newServer== NULL){
     return FALSE; //malloc fail
  }
  
  Main_u8Servers++;
  serverParser= psServerList;
  
  if(serverParser== NULL){
    *serverParser= newServer;
  }
  
  else{  
     while((*serverParser)->psNextServer!= NULL){ //Go to end of linked list
        serverParser= (*serverParser)->psNextServer;
     }
     (*serverParser)->psNextServer= newServer;
  }
  
  return TRUE;
  
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File */
/*--------------------------------------------------------------------------------------------------------------------*/
#endif /* SOLUTION */