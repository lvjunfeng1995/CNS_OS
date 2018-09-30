/* ************************************************************************** */
/** Descriptive File Name

  @Company
     NewDoon

  @File Name
    nd_os_queue.c

  @Description
    function of operating message queue
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
// Section: Local Functions                                                   */
/* ************************************************************************** */
/* ************************************************************************** */

/* ************************************************************************** */ 
/** 
  @Function
    void ND_OSQMsgCreate( ND_OS_MSG_Q *msg_q,
                          NBRMSG entries_size,
                          ND_OS_ERR *err);

  @Description
    Initialize a new  message queue table;

  @Parameters
    @ *msg_q Point to the  message queue table; 
    
    @ entries_size Message queue capacity
    
    @ *err Initialization error type;

  @Returns
     void

  @Remarks
     Create the table before the task message queue table;

 */

static void ND_OSQMsgCreate( ND_OS_MSG_Q  *msg_q,
                               NBRMSG        entries_size,
                               ND_OS_ERR    *err)
{
    if(entries_size == 0)
    {
        *err = NUM_CANNOT_ZERO;
        
        return;
    }
    
    msg_q -> InPtr = NULL;   
    msg_q -> OutPtr = NULL;   
    msg_q -> NbrEntriesSize = entries_size;   
    msg_q-> NbrEntries = 0;
    
    *err = NONE;
}

/* ************************************************************************** */ 
/** 
  @Function
    void ND_OSQCreate(       ND_OS_Q  const     *queue,
                                ND_OS_QUEUE_TYPE  type,
                                CHAR             *name,
                                ND_OS_MSG_Q      *msg_q,
                                NBRMSG            entries_size,
                                ND_OS_ERR        *err);

  @Description
    Initialize a new task message queue table;

  @Parameters
    @ *queue Point to the task message queue table; 
    
    @ type Initialize type of the task message queue table;

    @ *name Initialize name of the task message queue table;
 
    @ *msg_q Point to the message queue table of the task message queue table
    
    @ *err Initialization error type;

  @Returns
     void

  @Remarks
     Create the table before the system runs;

 */

void ND_OSQCreate(           ND_OS_Q   * const     queue,
                             ND_OS_QUEUE_TYPE      type,
                             CHAR                 *name,
                             ND_OS_MSG_Q          *msg_q,
                             NBRMSG                entries_size,
                             ND_OS_ERR            *err)
{
    if(NULL ==  queue)
    {
        *err = QUEUE_NULL;
        
        return;
    }
    
    if(NULL == name)
    {
        *err = NAME_NULL;
        
        return;
    }
    
    if(NULL == msg_q)
    {
        *err = MSGQ_NULL;
        
        return;
    }
    
    ND_OSQMsgCreate(msg_q,entries_size,err);
    
    if( NONE != *err)
    {
        return;
    }
    
    queue -> NamePtr = name;    
    queue -> Type = type;  
    queue -> MsgQ = msg_q;
    queue -> TCB = NULL;
    
    *err = NONE;
}

/* ************************************************************************** */ 
/** 
  @Function
     ND_OS_MSG const *ND_OSTaskQPend(          ND_OS_Q   * const        queue,
                                               ND_OS_QUEUE_TYPE         type,
                                               ND_OS_ERR               *err); 

  @Description
    Get the table of the task message queue to determine if you need to run
    the event

  @Parameters
    @ *queue Point to the task message queue table; 
    
    @ type Used to determine whether the table type of the requested queue
           is correct;
    
    @ *err Requested error type;

  @Returns
     Returns an address immutable message block

  @Remarks
     The requested table must exist

 */

 ND_OS_MSG const *ND_OSTaskQPend(            ND_OS_Q   * const        queue,
                                             ND_OS_QUEUE_TYPE         type,
                                             ND_OS_ERR               *err)
{
     ND_OS_MSG  *msg = NULL;
    
    if(queue == NULL)
    {
        *err = TABLE_NULL;
        
        return (ND_OS_MSG const *)msg;
    }
    
    ND_OS_QUEUE_TYPE queue_type = queue -> Type;
    
    if(queue_type != type)
    {
        *err=TYPE_ERR;
        
        return (ND_OS_MSG const *)msg;
    }
    
    ND_OS_MSG_Q *msg_q = queue -> MsgQ;
    
    if(NULL == msg_q)
    {
        *err = MSGQ_NULL;
        
        return (ND_OS_MSG const *)msg;
    }
    
    NBRMSG entry_num = msg_q -> NbrEntries;
    
    if(entry_num == 0)
    {        
        *err = NONE;
        
        return (ND_OS_MSG const *)msg;
    }
    
    msg =msg_q -> OutPtr;
    msg_q -> OutPtr = msg -> NextPtr;
    msg -> NextPtr = NULL;
    msg_q -> NbrEntries--;
    
    *err = EXIST;
    
    return (ND_OS_MSG const *)msg;
}

/* ************************************************************************** */ 
/** 
  @Function
    void ND_OSTaskQPost(          ND_OS_Q const        *queue,
                                  ND_OS_QUEUE_TYPE       type,
                                  ND_OS_MSG             *msg,
                                  ND_OS_ERR             *err);

  @Description
     Put a message into a message queue,call the event

  @Parameters
    @ *queue Point to the task message queue table; 
    
    @ type Used to determine whether the table in which the message is placed
           is correct;
    
    @ *err Insertion error type;

  @Returns
     void

  @Remarks
     The table of you put in must exist

 */

void ND_OSTaskQPost(           ND_OS_Q   * const     queue,
                               ND_OS_QUEUE_TYPE       type,
                               ND_OS_MSG             *msg,
                               ND_OS_ERR             *err)
{
	
	 if(NULL == msg)
    {
        *err = MSG_NULL;
        
        return;
    }
		
		MEM_ADDR *mem = msg -> MsgPtr;
		
		if(NULL == mem)
		{
		    *err = MEM_ADDR_NULL;
						
			ND_OSMsgPut(Msg_Pool_Table,
                                   (ND_OS_MSG *)msg,
                                   (ND_OS_ERR *)err);
			
			  return;
		}
		
    if(NULL ==queue)
    {
        *err = TABLE_NULL;	

      // ND_OSMemPut(Mem_Table_One,(MEM_ADDR *)mem,err);
			
			  ND_OSMsgPut(Msg_Pool_Table,
                                   (ND_OS_MSG *)msg,
                                   (ND_OS_ERR *)err);			
        
        return;
    }
    
    ND_OS_QUEUE_TYPE queue_type = queue -> Type;
    
    if(queue_type != type)
    {
        *err = TYPE_ERR;
			
			 // ND_OSMemPut(Mem_Table_One,(MEM_ADDR *)mem,err);
			
			  ND_OSMsgPut(Msg_Pool_Table,
                                   (ND_OS_MSG *)msg,
                                   (ND_OS_ERR *)err);	
        
        return;
    }
    
    ND_OS_MSG_Q *msg_q = queue -> MsgQ;
    NBRMSG entry_num = msg_q -> NbrEntries;
    NBRMSG entry_max = msg_q -> NbrEntriesSize;
    
    if(entry_num == entry_max)
    {
        *err = FULL;
			
			//  ND_OSMemPut(Mem_Table_One,(MEM_ADDR *)mem,err);
			
			  ND_OSMsgPut(Msg_Pool_Table,
                                   (ND_OS_MSG *)msg,
                                   (ND_OS_ERR *)err);	
        
        return;
    }
    
    if(entry_num == 0)
    {       
        msg_q-> InPtr = msg;
        msg_q-> OutPtr = msg;
        msg -> NextPtr = NULL;
    }
    else
    {
        msg_q -> InPtr -> NextPtr = msg;
        msg_q -> InPtr = msg;
        msg -> NextPtr = NULL;
    }
    
    msg_q -> NbrEntries++;
    
    ND_OS_TCB *queue_tcb = queue -> TCB;
    
    ND_OSSched(queue_tcb,err);
    
    *err = NONE;
    
}

/* *****************************************************************************
 End of File
 */
