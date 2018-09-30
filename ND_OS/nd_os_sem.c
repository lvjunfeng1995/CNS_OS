/* ************************************************************************** */
/** Descriptive File Name

  @Company
     NewDoon

  @File Name
    nd_os_sem.c

  @Description
    function of operating semaphore
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
    void ND_OSSemCreate (          ND_OS_SEM * const      sem,
                                   ND_OS_SEM_TYPE          type,
                                   CHAR                   *name,
                                   NBRCNT                  cnt,
                                   ND_OS_ERR              *err ); 

  @Description
    Initialize a new semaphore table;

  @Parameters
    @ *sem Point to the table; 
    
    @ type Initialize type of the table;

    @ *name Initialize name of the table;
 
    @ cnt Initialize counter value of the table;
    
    @ *err Initialization error type;

  @Returns
     void

  @Remarks
     Create the table before the system runs;

 */

void ND_OSSemCreate (           ND_OS_SEM * const      sem,
                                ND_OS_SEM_TYPE          type,
                                CHAR                   *name,
                                NBRCNT                  cnt,
                                ND_OS_ERR              *err )
{   
    if(NULL == sem)
    {
        *err = SEM_NULL;
        
        return;
    }
    
    if(NULL == name)
    {
        *err = NAME_NULL;
        
        return;
    }
    
    sem -> NamePtr = name;   
    sem -> Type = type;   
    sem -> Ctr = cnt;
    sem -> TCB = NULL;
    
    *err = NONE;
}

/* ************************************************************************** */ 
/** 
  @Function
    void ND_OSTaskSemPend (          ND_OS_SEM const         *sem,
                                     ND_OS_SEM_TYPE           type,
                                     ND_OS_ERR               *err ); 

  @Description
    Get the table of the semaphore to determine if you need to run the event

  @Parameters
    @ *sem Point to the table; 
    
    @ type Used to determine whether the table type of the requested semaphore
           is correct;
    
    @ *err Requested error type;

  @Returns
     void

  @Remarks
     The requested table must exist

 */

void ND_OSTaskSemPend (            ND_OS_SEM * const        sem,
                                   ND_OS_SEM_TYPE           type,
                                   ND_OS_ERR               *err )
{
    if(sem == NULL)
    {
        *err = TABLE_NULL;
        
        return;
    }
    
    ND_OS_SEM_TYPE sem_type = sem -> Type;
    NBRCNT sem_cnt = sem -> Ctr;
    
    if(sem_type != type)
    {
        *err = TYPE_ERR;
        
        return;
    }
    
    if(sem_cnt == 0)
    {
        *err = NONE;
        
        return;
    }
    
    sem -> Ctr--;
    
    *err = EXIST;
}

/* ************************************************************************** */ 
/** 
  @Function
    void ND_OSTaskSemPost(          ND_OS_SEM const      *sem,
                                    ND_OS_SEM_TYPE        type,
                                    ND_OS_ERR            *err );

  @Description
     Issue the semaphore,call the event

  @Parameters
    @ *sem Point to the table; 
    
    @ type Used to determine whether the table type of the published semaphore
           is correct;
    
    @ *err Published error type;

  @Returns
     void

  @Remarks
     The published table must exist

 */

void ND_OSTaskSemPost(            ND_OS_SEM * const     sem,
                                  ND_OS_SEM_TYPE        type,
                                  ND_OS_ERR            *err )
{
    if(NULL == sem)
    {
        *err = TABLE_NULL;
        
        return;
    }
    
    ND_OS_SEM_TYPE sem_type = sem -> Type;
    
    if(sem_type != type)
    {
        *err = TYPE_ERR;
        
        return;
    }
    
    sem -> Ctr++;
    
    ND_OS_TCB *sem_tcb = sem -> TCB;
    
    ND_OSSched(sem_tcb,err);
    
    *err = NONE;
    
}

/* ************************************************************************** */ 
/** 
  @Function
    void ND_OSTaskSemSet (          ND_OS_SEM const        *sem,
                                    ND_OS_SEM_TYPE          type,
                                    NBRCNT                  cnt,
                                    ND_OS_ERR              *err );

  @Description
    Set the value of the semaphore

  @Parameters
    @ *sem Point to the table; 
    
    @ type Used to determine whether the table type of the requested semaphore
           is correct;
    
    @ *err Set error type;

  @Returns
     void

  @Remarks
     The requested table must exist

 */

void ND_OSTaskSemSet (            ND_OS_SEM * const       sem,
                                  ND_OS_SEM_TYPE          type,
                                  NBRCNT                  cnt,
                                  ND_OS_ERR              *err )
{
    if(sem == NULL)
    {
        *err = TABLE_NULL;
        
        return;
    }
    
    ND_OS_SEM_TYPE sem_type = sem -> Type;
    
    if(sem_type != type)
    {
        *err = TYPE_ERR;
        
        return;
    }
    
    sem -> Ctr = cnt;
    
    *err=NONE;
}


/* *****************************************************************************
 End of File
 */
