/* ************************************************************************** */
/** Descriptive File Name

  @Company
     NewDoon

  @File Name
    nd_os_core.c

  @Description
    Task scheduling core program
 */
/* ************************************************************************** */




/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

#include "nd_os.h"







/* ************************************************************************** */
/* ************************************************************************** */
// Section: Local Functions                                                   */
/* ************************************************************************** */
/* ************************************************************************** */

/* ************************************************************************** */ 
/** 
  @Function
    static void MountToRdyList(ND_OS_TCB *tcb,
                               ND_OS_ERR *err)

  @Description
    To mount a task to the task ready table;

  @Parameters
    @ *tcb Point to the task; 
    @ *err Mount error type

  @Returns
     void

  @Remarks
     Function cannot be called externally;
 
     Calling procedure must shut off interrupt;

 */

static void MountToRdyList(ND_OS_TCB *tcb,
                             ND_OS_ERR *err)
{
    if(NULL == tcb)
    {
        *err = TCB_NULL;
        
        return;
    }
		
		if((tcb -> Task_state) == ND_OS_TASK_READY)
		{
			*err = MOUNTED;
		  return;
		}
    
    ND_OS_PRIO TcbPrio = tcb -> Prio;
    
    ND_OS_RDY_LIST *RdyList = NDRdyList[TcbPrio];
    
    NBRRDY TaskNum = RdyList -> NbrEntries;
    
    if(TaskNum == 0)
    {   
        RdyList -> NbrEntries = 1;       
        RdyList -> HeadPtr = tcb;
        RdyList -> TailPtr = tcb;
        tcb -> PrevPtr = NULL;
        tcb -> NextPtr = NULL;
    }
    else
    {
        RdyList -> NbrEntries++;
        tcb -> PrevPtr = RdyList -> TailPtr;
        RdyList -> TailPtr -> NextPtr = tcb;
        RdyList -> TailPtr = tcb;
        tcb -> NextPtr = NULL;
    }
    
    NDPrioTbl[TcbPrio]++;
    
    tcb -> Task_state = ND_OS_TASK_READY;
		tcb -> TimeQuantaCtr = tcb -> TimeQuanta;
    
    *err = NONE;
}

/* ************************************************************************** */ 
/** 
  @Function
    void ND_OSSched     ( ND_OS_TCB        *tcb,
                          ND_OS_ERR        *err )

  @Description
    The task to be run is compared to the priority of the current running task,
    if the current task has a high priority, mount the task to be run to the 
    task ready table,if the task to be run is high in priority, stop running 
    the current task and run the task.

  @Parameters
    @ *tcb Point to the task to be run; 
   
    @ *err Task scheduling error type;

  @Returns
     void

  @Remarks
     Calling procedure must shut off interrupt;

 */

void ND_OSSched     ( ND_OS_TCB        *tcb,
                       ND_OS_ERR        *err )
{
    if(NULL == tcb)
    {
        *err = TCB_NULL;
        
        return;
    }
    
    INTERRUPT_DISABLE;
            
    if(NULL == ND_TCBCurPtr)
    {
        ND_TCBHighRdyPtr = tcb;
        
        tcb -> Task_state = ND_OS_TASK_RUNNING;
        
        ND_OSStartHighRdy();
        
        ND_OS_TCB *Err_Tcb = (ND_OS_TCB *)ND_TCBHighRdyPtr;
        ND_TCBHighRdyPtr = NULL;
        
        MountToRdyList(Err_Tcb,err);                                            /*Pending error task*/
        
        *err = SWITCH_ERR;                                                       /*Return error of context switch*/
        
        INTERRUPT_ENABLE;
                
        return;
    }

    ND_OS_PRIO CurPrio = ND_TCBCurPtr -> Prio;
    ND_OS_PRIO NextPrio = tcb -> Prio;
    
    if(NextPrio < CurPrio && CUR_INT_NESTING_CNT_ONE == ND_OSIntNestingCtr)      /*The priority of the current task is lower than that of the next task and in the ISR*/
    {
        if(NULL != ND_TCBHighRdyPtr)
        {
            ND_OS_PRIO HighRdyPtr = ND_TCBHighRdyPtr -> Prio;
            
            if(NextPrio < HighRdyPtr)                                            /*The priority of the interrupt task is higher than that of the nested interrupt*/
            {
                ND_OS_TCB *Pending_Tcb = (ND_OS_TCB *)ND_TCBHighRdyPtr;
                ND_TCBHighRdyPtr = tcb;
                tcb -> Task_state = ND_OS_TASK_RUNNING;
                
                MountToRdyList(Pending_Tcb,err);               
                               
                ND_OS_TCB *Pending_CurTcb = (ND_OS_TCB *)ND_TCBCurPtr;
                
                MountToRdyList(Pending_CurTcb,err);							
               
                ND_OSIntCtxSw();                          
        
                INTERRUPT_ENABLE;
                
                return;
                
            }
            else
            {
                MountToRdyList(tcb,err);
            }
        }
        else
        {
            ND_TCBHighRdyPtr = tcb;
					
					 ND_OS_TCB *Pending_CurTcb = (ND_OS_TCB *)ND_TCBCurPtr;
                
                MountToRdyList(Pending_CurTcb,err);	

               tcb -> Task_state = ND_OS_TASK_RUNNING;			
                
                ND_OSIntCtxSw();                              
        
                INTERRUPT_ENABLE;
                
                return;
        }
    }
    else if(NextPrio < CurPrio && CUR_INT_NESTING_CNT_NONE == ND_OSIntNestingCtr)
    {
        ND_TCBHighRdyPtr = tcb;
        
        tcb -> Task_state = ND_OS_TASK_RUNNING;
        
        ND_OS_TCB *Pending_Tcb = (ND_OS_TCB *)ND_TCBCurPtr;
        
        MountToRdyList(Pending_Tcb,err);
        
        ND_OSCtxSw();
        
        INTERRUPT_ENABLE;
             
        return;
    }
    else if(NextPrio < CurPrio && ND_OSIntNestingCtr > CUR_INT_NESTING_CNT_ONE)
    {
        if(NULL != ND_TCBHighRdyPtr)
        {
            ND_OS_PRIO HighRdyPtr = ND_TCBHighRdyPtr -> Prio;
            
             if(NextPrio < HighRdyPtr)                                            /*The priority of the interrupt task is higher than that of the nested interrupt*/
            {
                ND_OS_TCB *Pending_Tcb = (ND_OS_TCB *)ND_TCBHighRdyPtr;
                ND_TCBHighRdyPtr = tcb;
                
                MountToRdyList(Pending_Tcb,err);
                
            }
             else
             {
                MountToRdyList(tcb,err);
             }
        }
        else
        {
            ND_TCBHighRdyPtr = tcb;
        }      
    }
    else 
    {
        MountToRdyList(tcb,err);
    }
    
    *err = NONE;
    
    INTERRUPT_ENABLE;
}

/* ************************************************************************** */ 
/** 
  @Function
    static STACK_PTR *ND_OSTaskStkInit (TASK_PTR         p_task,
                                        ARG             *p_arg,
                                        STACK_PTR       *p_stk_base,
                                        STACK_SIZE       stk_size)

  @Description
     Initialize the content of the task stack.Save the cpu's general register
     into the task stack.

  @Parameters
    @ p_task       Point to the task program; 
   
    @ *p_arg       Point to the task program parameter;
 
    @ *p_stk_base  Point to the task stack;
 
    @ stk_size     Size of the task stack;

  @Returns
     return address of task stack.

  @Remarks
     Can only be called by ND_OSTaskCreate();

 */

static STACK_PTR *ND_OSTaskStkInit (TASK_PTR         p_task,
                                      ARG             *p_arg,
                                      STACK_PTR       *p_stk_base,
                                      STACK_SIZE       stk_size)
{
    STACK_PTR  *p_stk;
            

    p_stk     = p_stk_base;
    p_stk     = p_stk+(stk_size-1);
    p_stk     = (STACK_PTR  *)((INT32U)p_stk & ALLGN8MASK);                       /*8 byte alignment*/
 //   pstk     = pstk & ALLGN4MASK;                                             /*4 byte alignment*/

                                                             	/* Registers stacked as if auto-saved on exception        */
    *--p_stk = (STACK_PTR)0x01000000u;                        /* xPSR                                                   */
    *--p_stk = (STACK_PTR)p_task;                             /* Entry Point                                            */
    *--p_stk = (STACK_PTR)p_task;                             /* R14 (LR)                                               */
    *--p_stk = (STACK_PTR)0x12121212u;                        /* R12                                                    */
    *--p_stk = (STACK_PTR)0x03030303u;                        /* R3                                                     */
    *--p_stk = (STACK_PTR)0x02020202u;                        /* R2                                                     */
    *--p_stk = (STACK_PTR)0x01010101u;                        /* R1                                                     */
    *--p_stk = (STACK_PTR)p_arg;                              /* R0 : argument                                          */
                                                            /* Remaining registers saved on process stack             */
    *--p_stk = (STACK_PTR)0x11111111u;                        /* R11                                                    */
    *--p_stk = (STACK_PTR)0x10101010u;                        /* R10                                                    */
    *--p_stk = (STACK_PTR)0x09090909u;                        /* R9                                                     */
    *--p_stk = (STACK_PTR)0x08080808u;                        /* R8                                                     */
    *--p_stk = (STACK_PTR)0x07070707u;                        /* R7                                                     */
    *--p_stk = (STACK_PTR)0x06060606u;                        /* R6                                                     */
    *--p_stk = (STACK_PTR)0x05050505u;                        /* R5                                                     */
    *--p_stk = (STACK_PTR)0x04040404u;                        /* R4                                                     */

    return ((STACK_PTR *)p_stk);                                                 /* Return new top of stack                              */
}

/* ************************************************************************** */ 
/** 
  @Function
    void ND_OSTaskCreate( ND_OS_TCB       *p_tcb,
                          CHAR            *p_name,
                          TASK_PTR         p_task,
                          ARG             *p_arg,
                          ND_OS_PRIO       prio,
                          STACK_PTR       *p_stk_base,
                          STACK_SIZE       stk_size,
                          ND_OS_SEM* const sem,
                          ND_OS_Q  * const queue,
                          TIME_CNT         time_quanta,
                          ND_OS_ERR       *err)

  @Description
    Create a new task.

  @Parameters
    @ *p_tcb       Point to the task control block; 
 
    @ *p_name      Point to the task name;

    @ p_task       Point to the task program; 
   
    @ *p_arg       Point to the task program parameter;
 
    @ prio         The priority of the task;
 
    @ *p_stk_base  Point to the task stack;
 
    @ stk_size     Size of the task stack;
 
    @ sem          The semaphore table of the task;
 
    @ queue        The queue table of the task;
 
    @ time_quanta  The time slice of the task;
 
    @ *err         Create task error type

  @Returns
    void

  @Remarks
     

 */

void ND_OSTaskCreate( ND_OS_TCB       *p_tcb,
                        CHAR            *p_name,
                        TASK_PTR         p_task,
                        ARG             *p_arg,
                        ND_OS_PRIO       prio,
                        STACK_PTR       *p_stk_base,
                        STACK_SIZE       stk_size,
                        ND_OS_SEM* const sem,
                        ND_OS_Q  * const queue,
                        TIME_CNT         time_quanta,
                        ND_OS_ERR       *err)
{ 
    if(NULL == p_name)
    {
        *err = NAME_NULL;
        
        return;
    }
    
    if(NULL == p_task)
    {
        *err = TASK_NULL;
        
        return;
    }
    
    if(NULL == p_stk_base)
    {
        *err = STACK_NULL;
        
        return;
    }
    
    if(stk_size < 128)
    {
        *err = STACK_SIZE_FEW;
        
        return;
    }
    
    p_tcb -> StkPtr = ND_OSTaskStkInit(p_task,p_arg,p_stk_base,stk_size);
    p_tcb -> NamePtr = p_name;
    p_tcb -> Prio = prio;
    p_tcb -> StkBasePtr = p_stk_base;
    p_tcb -> TaskEntryAddr = p_task;
    p_tcb -> TaskEntryArg = p_arg;
    p_tcb -> Task_state = ND_OS_TASK_SLEEP;
    p_tcb -> TimeQuanta = time_quanta;
    p_tcb -> TimeQuantaCtr = time_quanta;
    p_tcb -> StkSize = stk_size;
    
    if(sem != NULL)
    {
        p_tcb -> PendDataTblPtr = (ND_OS_SEM *)sem;
        p_tcb -> Pendon = ND_OS_TYPE_SEM;
        
        sem -> TCB = p_tcb;
    }
    else if(queue != NULL)
    {
        p_tcb -> PendDataTblPtr = (ND_OS_Q *)queue;
        p_tcb -> Pendon = ND_OS_TYPE_QUEUE;
        
        queue -> TCB = p_tcb;
    }
    
    INTERRUPT_DISABLE;
            
    MountToRdyList(p_tcb,err);
    
    if(TCB_NULL == *err)
    {
        INTERRUPT_ENABLE;
                
        return;
    }
    else
    {
       RunNextHighPrioTask(err);
    }
    
    INTERRUPT_ENABLE;
    
}

/* ************************************************************************** */ 
/** 
  @Function
    void RunNextHighPrioTask(ND_OS_ERR *err)

  @Description
    Get the next higher priority task and run.

  @Parameters
    @ *err Task scheduling error type;

  @Returns
     void

  @Remarks
     Calling procedure must shut off interrupt;

 */

void RunNextHighPrioTask(ND_OS_ERR *err)
{  
    INTERRUPT_DISABLE;
            
    ND_OS_TCB *HighRdy = GetHighRdyTcbPtr();
    
    if(HighRdy != NULL)
    {
         ND_OSSched(HighRdy,err);
    }
    
    INTERRUPT_ENABLE;
}

void ND_OSExitInt(ND_OS_ERR *err)
{
	if(NULL == ND_TCBCurPtr)
	{
	  return;
	}
	
	if(NULL == ND_TCBHighRdyPtr)
	{
	  return;
	}
	
	if(ND_OSIntNestingCtr > CUR_INT_NESTING_CNT_ONE)
	{
	  return;
	}
	
	if(ND_OSIntNestingCtr == CUR_INT_NESTING_CNT_ONE)
	{
		ND_OS_TCB *NextTcb = (ND_OS_TCB *)ND_TCBHighRdyPtr;
		
		NextTcb -> Task_state = ND_OS_TASK_RUNNING;
		
		ND_OS_TCB *Pending_Tcb = (ND_OS_TCB *)ND_TCBCurPtr;
		
		MountToRdyList(Pending_Tcb,err);	
		
		ND_OSIntCtxSw();
	}
}

void ND_OSSysTick()
{
	INTERRUPT_DISABLE;
	
	if(NULL == ND_TCBCurPtr)
	{
		INTERRUPT_ENABLE;
		
	  return;
	}
	
	ND_OSIntNestingCtr++;
	
	ND_OS_ERR err = NONE;
	
	ND_OS_TCB *current = (ND_OS_TCB *)ND_TCBCurPtr;
	
	current -> TimeQuantaCtr--;
	
	if(0 == current -> TimeQuantaCtr)
	{
		ND_OS_TCB* NextTcb = GetSamePrioTask();
		
		if(NULL == NextTcb)
		{
		  current -> TimeQuantaCtr = current -> TimeQuanta;
			
			ND_OSIntNestingCtr--;
			
			INTERRUPT_ENABLE;
		
	    return;
		}
		else
		{
		  if(NULL == ND_TCBHighRdyPtr)
			{
			  ND_TCBHighRdyPtr = NextTcb;				
			}
			else 
			{
				ND_OS_PRIO HighRdyPtr = ND_TCBHighRdyPtr -> Prio;
            
             if(NextTcb ->Prio < HighRdyPtr)                                            
            {
                ND_OS_TCB *Pending_Tcb = (ND_OS_TCB *)ND_TCBHighRdyPtr;
                ND_TCBHighRdyPtr = NextTcb;
                
                MountToRdyList(Pending_Tcb,&err);    
            }
             else
             {
                MountToRdyList(NextTcb,&err);
             }
			}
			
		}
		
	}
	
	ND_OSExitInt(&err);
	
	ND_OSIntNestingCtr--;
	
	INTERRUPT_ENABLE;
}



/* *****************************************************************************
 End of File
 */
