/* ************************************************************************** */
/** Descriptive File Name

  @Company
    NewDoon

  @File Name
    nd_os_prio.c

  @Description
    Task priority related operations.
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
    ND_OS_TCB *GetHighRdyTcbPtr();

  @Description
    Get the higher priority task from the task ready list   ;

  @Parameters
   

  @Returns
    Return the task control block.

  @Remarks
     Calling procedure must shut off interrupt. 

 */

ND_OS_TCB *GetHighRdyTcbPtr()
{
    ND_OS_TCB *HighTcb =NULL;
    ND_OS_PRIO HighPrio = ND_OS_PRIO_NONE;
    ND_OS_PRIO CurPrio;
    
    if(NULL == ND_TCBCurPtr)
    {
        CurPrio = (ND_OS_PRIO)(ND_CFG_PRIO_MAX+1);
    }
    else
    {
        CurPrio = ND_TCBCurPtr -> Prio;
    }
    
    INT32U i = 0;
    
    for(;i < CurPrio;i++)
    {
        if(NDPrioTbl[i] > 0)
        {
            HighPrio = (ND_OS_PRIO)i;
            
            break;
        }
    }
    
    if(ND_OS_PRIO_NONE == HighPrio)
    {
        return HighTcb;
    }
    else
    {
			  NDPrioTbl[HighPrio]--;
			
        ND_OS_RDY_LIST *RdyList = NDRdyList[HighPrio];
        
        NBRRDY num_task = RdyList -> NbrEntries;
        
        if(0 == num_task)
        {
            return HighTcb;
        }
        
        HighTcb = RdyList->HeadPtr;
        RdyList -> HeadPtr = HighTcb -> NextPtr;
        
        if(RdyList -> HeadPtr != NULL)
        {
            RdyList -> HeadPtr -> PrevPtr = NULL;
        }
        
        HighTcb -> NextPtr = NULL;
				
				HighTcb ->Task_state = ND_OS_TASK_RUNNING;
        
        RdyList -> NbrEntries--;
        
        return HighTcb;      
    }
}

void PendAndRunNextTask()
{
	  INTERRUPT_DISABLE;
	
	  ND_OS_PRIO HighPrio = ND_OS_PRIO_NONE;
	  ND_OS_TCB *HighTcb =NULL;
	
	  if(NULL == ND_TCBCurPtr)
      {
          return;
      }
		
	  ND_OS_PRIO CurPrio = (ND_OS_PRIO)(ND_CFG_PRIO_MAX+1);

		INT32U i = 0;
    
    for(;i < CurPrio;i++)
    {
        if(NDPrioTbl[i] > 0)
        {
            HighPrio = (ND_OS_PRIO)i;
            
            break;
        }
    }
    
    if(ND_OS_PRIO_NONE == HighPrio)
    {
        return ;
    }
    else
    {
			  NDPrioTbl[HighPrio]--;
			
        ND_OS_RDY_LIST *RdyList = NDRdyList[HighPrio];
        
        NBRRDY num_task = RdyList -> NbrEntries;
        
        if(0 == num_task)
        {
            return ;
        }
        
        HighTcb = RdyList->HeadPtr;
        RdyList -> HeadPtr = HighTcb -> NextPtr;
        
        if(RdyList -> HeadPtr != NULL)
        {
            RdyList -> HeadPtr -> PrevPtr = NULL;
        }
        
        HighTcb -> NextPtr = NULL;
        
        RdyList -> NbrEntries--;
				
				HighTcb -> Task_state = ND_OS_TASK_RUNNING;
              
			  ND_TCBHighRdyPtr = HighTcb;
				
				ND_OSCtxSw();
    }
		
		INTERRUPT_ENABLE;
}

ND_OS_TCB *GetSamePrioTask()
{
	 ND_OS_TCB *NextTcb =NULL;
    ND_OS_PRIO CurPrio;
    
    if(NULL == ND_TCBCurPtr)
    {
        return NextTcb;
    }
    else
    {
        CurPrio = ND_TCBCurPtr -> Prio;
    }
		
		if(NDPrioTbl[CurPrio]>0)
		{
			NDPrioTbl[CurPrio]--;
			
			ND_OS_RDY_LIST *RdyList = NDRdyList[CurPrio];
			
			NBRRDY num_task = RdyList -> NbrEntries;
        
        if(0 == num_task)
        {
            return NextTcb;
        }
			
			NextTcb = RdyList->HeadPtr;
			
			RdyList->HeadPtr = NextTcb-> NextPtr;
				
				 if(RdyList -> HeadPtr != NULL)
        {
            RdyList -> HeadPtr -> PrevPtr = NULL;
        }
        
        NextTcb -> NextPtr = NULL;
				
				NextTcb ->Task_state = ND_OS_TASK_RUNNING;
				
				RdyList -> NbrEntries--;
				
		}
		
		return NextTcb;    
}



/* *****************************************************************************
 End of File
 */
