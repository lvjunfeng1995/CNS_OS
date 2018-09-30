/* ************************************************************************** */
/** Descriptive File Name

  @Company
     NewDoon

  @File Name
    nd_os_msg.c

  @Description
   Set up a message pool;
 */
/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

#include"nd_os.h"





/* ************************************************************************** */
/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
/* ************************************************************************** */

static ND_OS_MSG msg_addr[ND_CFG_MSG_POOL_SIZE] = {0};                          /*Point to message pool*/

int add_num = 0;
int sub_num = 0;
int create_num = 0;


/* ************************************************************************** */
/* ************************************************************************** */
// Section: Local Functions                                                   */
/* ************************************************************************** */
/* ************************************************************************** */



/* ************************************************************************** */ 
/** 
  @Function
    void ND_OSMsgPoolCreate(          ND_OS_MSG_POOL const *pool,
                                      ND_OS_ERR            *err );

  @Description
    Initialize a new message pool table;

  @Parameters
    @ *pool   Point to the message pool table;
    
    @ *err  Initialization error type;

  @Returns
     void

  @Remarks
     Create the table before the system runs;
     Can only be created once; 

 */

void ND_OSMsgPoolCreate(            ND_OS_MSG_POOL    * const pool,
                                    ND_OS_ERR                 *err )
{
    if(NULL == pool)
    {
        *err = POOL_NULL;
        
        return;
    }
    
    INT32U i=1;
    
    for(;i < ND_CFG_MSG_POOL_SIZE;i++)                                          /*Initialize the message pool */
    {
        ND_OS_MSG *msg =(ND_OS_MSG *)&msg_addr[i-1];
        ND_OS_MSG *msg_next = (ND_OS_MSG *)&msg_addr[i];
        
        msg -> NextPtr = msg_next;
        msg -> MsgSize = 0;
        msg -> MsgPtr = NULL;
        
        if( ND_CFG_MSG_POOL_SIZE - 1 == i)
        {
            msg_next -> NextPtr = NULL;
            msg_next -> MsgSize = 0;
            msg_next -> MsgPtr = NULL;
        }
    }
    
    pool -> NbrFree = ND_CFG_MSG_POOL_SIZE;
    pool -> NbrUsed = 0;
    pool -> NextPtr = msg_addr;
		
		create_num++;
    
    *err = NONE;
}

/* ************************************************************************** */ 
/** 
  @Function
    ND_OS_MSG *ND_OSMsgGet(          ND_OS_MSG_POOL const    *pool,
                                     ND_OS_ERR               *err);

  @Description
    Get idle message block from message pool;

  @Parameters
    @ *mem   Point to the message pool table; 
    
    @ *err  Get error type;

  @Returns
     Returns an idle message block;

  @Remarks
     The requested table must exist;

 */

ND_OS_MSG *ND_OSMsgGet(           ND_OS_MSG_POOL    * const pool,
                                  ND_OS_ERR                *err)
{
	   add_num++;
	
    if(NULL == pool)
    {
        *err = POOL_NULL;
        
        return NULL;
    }
    
    INTERRUPT_DISABLE;
    
    ND_OS_MSG *msg = pool-> NextPtr;
    NBRMSG free_msg = pool -> NbrFree;
    NBRMSG used_msg = pool -> NbrUsed;
    
    if(0 == free_msg)
    {
        *err = POOL_NONE;
        
        INTERRUPT_ENABLE;
        
        return NULL;
    }
    
		if(msg -> NextPtr == NULL)
		{
		  while(1);
		}
		
    pool -> NextPtr = msg -> NextPtr;
    pool -> NbrFree--;
    pool -> NbrUsed++;
       		
		
		if(pool -> NbrUsed == 0)
		{
		  while(1);
		}
    
    msg -> NextPtr = NULL;
		
		if(pool -> NextPtr == NULL)
		{
		   while(1);
		}
    
    *err = NONE;
		
		if(NULL == msg)
		{
		  while(1);
		}
    
    INTERRUPT_ENABLE;
            
    return msg;
}

/* ************************************************************************** */ 
/** 
  @Function
    void ND_OSMsgPut(          ND_OS_MSG_POOL const    *pool,
                               ND_OS_MSG               *msg,
                               ND_OS_ERR               *err);

  @Description
    Return message block to message pool;

  @Parameters
    @ *pool   Point to the message pool; 
    
    @ *msg   Point to returned message block;
  
    @ *err  Put error type;

  @Returns
     void;

  @Remarks
     The requested table must exist;

 */

void ND_OSMsgPut(           ND_OS_MSG_POOL    * const pool,
                            ND_OS_MSG                *msg,
                            ND_OS_ERR                *err)
{
	sub_num++;
	
    if(NULL == pool)
    {
        *err = POOL_NULL;
        
        return;
    }
    
    if(NULL == msg)
    {
        *err = MSG_ADDR_NULL;
        
        return;
    }
    
    INTERRUPT_DISABLE;
            
    NBRMSG used_msg = pool -> NbrUsed;
    
    if(ND_CFG_MSG_POOL_SIZE < used_msg)                                         /*Prevent message pool overflow*/
    {
        pool -> NbrUsed = ND_CFG_MSG_POOL_SIZE;
        pool -> NbrFree = 0;
        pool -> NextPtr = NULL;
        
    }
    
    if(0 == used_msg)
    {
        *err = POOL_FULL;
        
        INTERRUPT_ENABLE;
        
        return;
    }

   MEM_ADDR *mem_addr = msg -> MsgPtr; 
   
   if(NULL != mem_addr)
	 {
	    ND_OSMemPut(Mem_Table_One,mem_addr,err);
	 }
   
		
    msg -> MsgPtr = NULL;
    msg -> MsgSize = 0;
    msg -> NextPtr = pool -> NextPtr;
	 
	 if(pool -> NextPtr == NULL)
	 {
	   while(1);
	 }
    
    pool -> NbrFree++;
    pool -> NbrUsed--;
    pool -> NextPtr = msg;                                                      /*Put a message block into message pool*/
		    
    
    *err = NONE;
    
    INTERRUPT_ENABLE;
}


/* *****************************************************************************
 End of File
 */
