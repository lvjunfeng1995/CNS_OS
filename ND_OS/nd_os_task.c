/* ************************************************************************** */
/** Descriptive File Name

  @Company
   NewDoon

  @File Name
    nd_os_task.c

  @Description
    Create all tasks.
 */
/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

#include "nd_os.h"
#include "nd_os_task.h"
#include "nd_os_event.h"

/* ************************************************************************** */
/* ************************************************************************** */
/* Section:  Global Data                                                      */
/* ************************************************************************** */
/* ************************************************************************** */

static ND_OS_TCB tcb_addr[TASK_NUM]   ={0};

static ND_OS_TCB *DataCheck_Tcb       = &tcb_addr[ND_OS_TASK_DATACHECK];
static ND_OS_TCB *WriteFlash_Tcb      = &tcb_addr[ND_OS_TASK_WRITEFLASH];
static ND_OS_TCB *Stimulate_Tcb       = &tcb_addr[ND_OS_TASK_STIMULATE];
static ND_OS_TCB *ReadFlash_Tcb       = &tcb_addr[ND_OS_TASK_READFLASH];
static ND_OS_TCB *EleAndImp_Tcb       = &tcb_addr[ND_OS_TASK_ELEANDIMP];
static ND_OS_TCB *EleLack_Tcb         = &tcb_addr[ND_OS_TASK_ELELACK];
static ND_OS_TCB *Charge_Tcb          = &tcb_addr[ND_OS_TASK_CHARGE];
static ND_OS_TCB *SampleAndErrChk_Tcb = &tcb_addr[ND_OS_TASK_SAMPLEANDERRCHK];
static ND_OS_TCB *ErrCheck_Tcb        = &tcb_addr[ND_OS_TASK_ERRCHECK];
static ND_OS_TCB *SleepMode_Tcb       = &tcb_addr[ND_OS_TASK_SLEEPMODE];

static STACK_PTR DataCheck_Stk      [TASK_STACK_SIZE]={0};
static STACK_PTR WriteFlash_Stk     [TASK_STACK_SIZE]={0};
static STACK_PTR Stimulate_Stk      [TASK_STACK_SIZE]={0};
static STACK_PTR ReadFlash_Stk      [TASK_STACK_SIZE]={0};
static STACK_PTR EleAndImp_Stk      [TASK_STACK_SIZE]={0};
static STACK_PTR EleLack_Stk        [TASK_STACK_SIZE]={0};
static STACK_PTR Charge_Stk         [TASK_STACK_SIZE]={0};
static STACK_PTR SampleAndErrChk_Stk[TASK_STACK_SIZE]={0};
static STACK_PTR ErrCheck_Stk       [TASK_STACK_SIZE]={0};
static STACK_PTR SleepMode_Stk      [TASK_STACK_SIZE]={0};

EEG_ADDR *Mem_tail = NULL;                                      
EEG_ADDR *Head_buf = NULL;                                    
EEG_ADDR *End_buf = NULL;
EEG_ADDR *Mem_head = NULL;


/* ************************************************************************** */
/* ************************************************************************** */
// Section: Local Functions                                                   */
/* ************************************************************************** */
/* ************************************************************************** */

void DataCheckTask()
{
    ND_OS_ERR err = NONE;
    ND_DATA_ERR data_err = DATA_NONE;    

    while(1)
    {    
       ND_OS_MSG const *msg = ND_OSTaskQPend(DataCheck_Q,
                                             ND_OS_QUEUE_TYPE_DATACHECK,
                                             (ND_OS_ERR *)&err);
    
       switch(err)
       {
           case EXIST:
               
               if(NULL == msg)
               {                    
                   PendAndRunNextTask();
                   
                   break;
               }
            
               ND_DataErrCheck(msg,&data_err);
            
               switch(data_err)
               {
                   case DATA_ERR:
										 
									     ND_OSMsgPut(Msg_Pool_Table,
                                   (ND_OS_MSG *)msg,
                                   (ND_OS_ERR *)&err);									 
										 
									     UsartSend(Reply_buf,Reply_length);
									 
									     break;
                
                   case DATA_IMPCHK:
                       
                       ND_OSMsgPut(Msg_Pool_Table,
                                   (ND_OS_MSG *)msg,
                                   (ND_OS_ERR *)&err);									 
                        
                       ND_OSTaskSemPost(EleImp_Sem,
                                        ND_OS_SEM_TYPE_ELEANDIMP,
                                        (ND_OS_ERR *)&err);
                       
                       break;
                    
                   case DATA_STM:                                             									     
									 
									     UsartSend(Reply_buf,Reply_length);
									 
									     ND_OSTaskQPost(Stimulate_Q,
                                      ND_OS_QUEUE_TYPE_STIMULATE,
                                      (ND_OS_MSG *)msg,
                                      (ND_OS_ERR *)&err);
                       
                       break;
                
                   case DATA_WF:
                       
                       ND_OSTaskQPost(WriteFlash_Q,
                                      ND_OS_QUEUE_TYPE_WRITEFLASH,
                                     (ND_OS_MSG *)msg,
                                     (ND_OS_ERR *)&err);
                       
                       break;
									 
									 case DATA_CONNECT:
										 
									     ND_OSMsgPut(Msg_Pool_Table,
                                   (ND_OS_MSG *)msg,
                                   (ND_OS_ERR *)&err);									 
										 
									     UsartSend(Reply_buf,Reply_length);
									 
									     break;
									 
									 case DATA_CHARGE:
										 
									     ND_OSMsgPut(Msg_Pool_Table,
                                   (ND_OS_MSG *)msg,
                                   (ND_OS_ERR *)&err);									 
									 
									     ND_OSTaskSemPost(Charge_Sem,
                                   ND_OS_SEM_TYPE_CHARGE,
                                   (ND_OS_ERR *)&err);
									 
									     break;
									 
									 case DATA_RESEND:
										 
									     ND_OSMsgPut(Msg_Pool_Table,
                                   (ND_OS_MSG *)msg,
                                   (ND_OS_ERR *)&err);									
										 
									     UsartSend(Reply_buf,Reply_length);
									 
									     break;
									 
									 case KEEP_LIVE:
										   
									     ND_OSMsgPut(Msg_Pool_Table,
                                   (ND_OS_MSG *)msg,
                                   (ND_OS_ERR *)&err);									 
										 
									     UsartSend(Reply_buf,Reply_length);
									 
									     break;
									 									 
									 case DATA_EEG:
										 
									     ND_OSMsgPut(Msg_Pool_Table,
                                   (ND_OS_MSG *)msg,
                                   (ND_OS_ERR *)&err);				
									 
									     UsartSend(Reply_buf,Reply_length);
									 
										   break;
               }    
               
               break;
        
           case NONE :      
               
               PendAndRunNextTask();
               
               break;
					 
       }
    }
}

void WriteFlashTask()
{
    ND_OS_ERR err = NONE;
    ND_DATA_ERR data_err = DATA_NONE;
    
    while(1)
    {
        ND_OS_MSG const *msg = ND_OSTaskQPend(WriteFlash_Q,
                                              ND_OS_QUEUE_TYPE_WRITEFLASH,
                                              (ND_OS_ERR *)&err);
        
        switch(err)
        {
            case EXIST:
                
                if(NULL == msg)
               {                    
                   PendAndRunNextTask();
                   
                   break;
               }
                
                ND_WriteFlash(msg,(ND_DATA_ERR *)&data_err);
                
                ND_OSMsgPut(Msg_Pool_Table,
                            (ND_OS_MSG *)msg,
                            (ND_OS_ERR *)&err);
                
                switch(data_err)
                {
                    case WRITE_ERR:break;
                    
                    case WRITE_SUCCESS:break;
                }
                
                break;
                
            case NONE:
                
                PendAndRunNextTask();
                
                break;
        }
    }
}


void StimulateTask()
{
    ND_OS_ERR err = NONE;
    ND_DATA_ERR data_err = DATA_NONE;
    
    while(1)
    {
        ND_OS_MSG const *msg = ND_OSTaskQPend(Stimulate_Q,
                                              ND_OS_QUEUE_TYPE_STIMULATE,
                                              (ND_OS_ERR *)&err);
        
        switch(err)
        {
            case EXIST:
                
                if(NULL == msg)
               {                    
                   PendAndRunNextTask();
                   
                   break;
               }
                
                ND_Stimulate(msg,(ND_DATA_ERR *)&data_err,&err);
                
                ND_OSMsgPut(Msg_Pool_Table,
                            (ND_OS_MSG *)msg,
                            (ND_OS_ERR *)&err);

                break;
                
            case NONE:
                
                PendAndRunNextTask();
                
                break;
        }
    }
}


void ReadFlashTask()
{
    ND_OS_ERR err = NONE;
    ND_DATA_ERR data_err = DATA_NONE;
    
    while(1)
    {        
        ND_OS_MSG const *msg = ND_OSTaskQPend(ReadFlash_Q,
                                              ND_OS_QUEUE_TYPE_READFLASH,
                                              (ND_OS_ERR *)&err);
        
        switch(err)
        {
            case EXIST:
                
                if(NULL == msg)
               {                    
                   PendAndRunNextTask();
                   
                   break;
               }
                
                ND_ReadFlash(msg,(ND_DATA_ERR *)&data_err);
                
                ND_OSMsgPut(Msg_Pool_Table,
                            (ND_OS_MSG *)msg,
                            (ND_OS_ERR *)&err);
                
                break;
                
            case NONE:
                
                PendAndRunNextTask();
                
                break;
        }
    }
}

void ManageErrTask()
{
    ND_OS_ERR err = NONE;
    ND_DATA_ERR data_err = DATA_NONE;
    
    while(1)
    {      
        ND_OS_MSG const *msg = ND_OSTaskQPend(ErrCheck_Q,
                                              ND_OS_QUEUE_TYPE_ERRMANAGE,
                                              (ND_OS_ERR *)&err);
        
        switch(err)
        {
            case EXIST:
                
                 if(NULL == msg)
               {                    
                   PendAndRunNextTask();
                   
                   break;
               }
                 
                 ND_OSMsgPut(Msg_Pool_Table,
                             (ND_OS_MSG *)msg,
                             (ND_OS_ERR *)&err);
                
                break;
                
            case NONE:
                
                PendAndRunNextTask();
                
                break;
        }
    }
}


void EleAndImpTask()
{
    ND_OS_ERR err = NONE;
    ND_DATA_ERR data_err = DATA_NONE;
    
    while(1)
    {       
        ND_OSTaskSemPend(EleImp_Sem,
                         ND_OS_SEM_TYPE_ELEANDIMP,
                         (ND_OS_ERR *)&err);
        
        switch(err)
        {
            case EXIST:
                
                ND_EleAndImpSend((ND_DATA_ERR *)&data_err);
                
                break;
                
            case NONE:
                
                PendAndRunNextTask();
                
                break;
        }
    }
}

void ChargeTask()
{
    ND_OS_ERR err = NONE;
    ND_DATA_ERR data_err = DATA_NONE;
	  INT8U  pre_charge_state = 0;
	  INT8U  post_charge_state = 0;
	
	  Charge_Init();
    
    while(1)
    {       
        ND_OSTaskSemPend(Charge_Sem,
                         ND_OS_SEM_TYPE_CHARGE,
                         (ND_OS_ERR *)&err);
        
        switch(err)
        {
            case EXIST:
							
						pre_charge_state = GetChargeState();
							  
						if(pre_charge_state)
						{
							
						  Charge_start();
							
							post_charge_state = pre_charge_state - 1;
							
						}
						else
						{
							
						  Charge_stop();
							
							post_charge_state = pre_charge_state + 1;
							
						}
						
						//post_charge_state = GetChargeState();
						
						ReplyInf(Rep_charge,post_charge_state);
												
						UsartSend(Reply_buf,Reply_length);

                break;
                
            case NONE:
                
                PendAndRunNextTask();
                
                break;
        }
        
    }
}

void NoticeEle()
{
    ND_OS_ERR err = NONE;
    ND_DATA_ERR data_err = DATA_NONE;
    
    while(1)
    {       
        ND_OSTaskSemPend(EleLack_Sem,
                         ND_OS_SEM_TYPE_ELELACK,
                         (ND_OS_ERR *)&err);
        
        switch(err)
        {
            case EXIST:break;
            
            case NONE:
                
                PendAndRunNextTask();
                
                break;
        }
    }
}



void SampleAndErrChkTask()
{
    ND_OS_ERR err = NONE;
    ND_DATA_ERR data_err =DATA_NONE;
    
    Mem_head = (EEG_ADDR  *)ND_OSMemGet(Mem_Table_One,(ND_OS_ERR *)&err);
	
	 if(Mem_head == NULL )
	 {
	    while(1);
	 }
	 
    Mem_tail = Mem_head;
	  Head_buf = Mem_head;
	
	  MEMSIZE    mem_size = Mem_Table_One -> BlkSize;
	  INT16U     num      = mem_size/sizeof(EEG_ADDR);
	             End_buf  = Head_buf + num;
	
	 SPI1_Init();
	 uart_init(19200);
	 ItanCLEAR();	 
	 TIM3_Int_Init();
	 TIM4_Int_Init();
	 ItanInit();
    
    while(1)
    {       
        ND_OSTaskSemPend(SampleAndErrChk_Sem,
                         ND_OS_SEM_TYPE_SampleAndErrChk,
                         (ND_OS_ERR *)&err);
        
        switch(err)
        {
            case EXIST:
                
                PendAndRunNextTask();
                
                break;
                
            case NONE:
							                
                ND_GetSample((ND_OS_ERR *)&err);						    						 
                
                ND_ErrCheck((ND_DATA_ERR *)&data_err);
                
               /* switch(data_err)
                {
                    case SAMPLE_ERR:
                        
                    {
                        ND_OS_MSG *msg = NULL;
                        
                        ND_OSTaskQPost(Stimulate_Q,
                                       ND_OS_QUEUE_TYPE_STIMULATE,
                                       msg,
                                       (ND_OS_ERR *)&err);
                    }                      
                        
                        break;
                    case NONE:break;
                } */
                
                break;
        }
    }
}


void SleepModeTask()
{
    ND_OS_ERR err = NONE;
    
    ND_OSTaskCreate((ND_OS_TCB *)Stimulate_Tcb,
                    (CHAR      *)"Stimulate_Task",
                    (TASK_PTR   )StimulateTask,
                    (ARG       *)NULL,
                    (ND_OS_PRIO )ND_OS_PRIO_LEVEL_2,
                    (STACK_PTR *)&Stimulate_Stk[0],
                    (STACK_SIZE )TASK_STACK_SIZE,
                    (ND_OS_SEM *)NULL,
                    (ND_OS_Q   *)Stimulate_Q,
                    (TIME_CNT   )4,
                    (ND_OS_ERR *)&err);
    
    ND_OSTaskCreate((ND_OS_TCB *)WriteFlash_Tcb,
                    (CHAR      *)"WriteFlash_Task",
                    (TASK_PTR   )WriteFlashTask,
                    (ARG       *)NULL,
                    (ND_OS_PRIO )ND_OS_PRIO_LEVEL_3,
                    (STACK_PTR *)&WriteFlash_Stk[0],
                    (STACK_SIZE )TASK_STACK_SIZE,
                    (ND_OS_SEM *)NULL,
                    (ND_OS_Q   *)WriteFlash_Q,
                    (TIME_CNT   )4,
                    (ND_OS_ERR *)&err);
    
    ND_OSTaskCreate((ND_OS_TCB *)EleAndImp_Tcb,
                    (CHAR      *)"EleAndImp_Task",
                    (TASK_PTR   )EleAndImpTask,
                    (ARG       *)NULL,
                    (ND_OS_PRIO )ND_OS_PRIO_LEVEL_3,
                    (STACK_PTR *)&EleAndImp_Stk[0],
                    (STACK_SIZE )TASK_STACK_SIZE,
                    (ND_OS_SEM *)EleImp_Sem,
                    (ND_OS_Q   *)NULL,
                    (TIME_CNT   )4,
                    (ND_OS_ERR *)&err);
    
    ND_OSTaskCreate((ND_OS_TCB *)ReadFlash_Tcb,
                    (CHAR      *)"ReadFlash_Task",
                    (TASK_PTR   )ReadFlashTask,
                    (ARG       *)NULL,
                    (ND_OS_PRIO )ND_OS_PRIO_LEVEL_3,
                    (STACK_PTR *)&ReadFlash_Stk[0],
                    (STACK_SIZE )TASK_STACK_SIZE,
                    (ND_OS_SEM *)NULL,
                    (ND_OS_Q   *)ReadFlash_Q,
                    (TIME_CNT   )4,
                    (ND_OS_ERR *)&err);
    
    ND_OSTaskCreate((ND_OS_TCB *)DataCheck_Tcb,
                    (CHAR      *)"DataCheck_Task",
                    (TASK_PTR   )DataCheckTask,
                    (ARG       *)NULL,
                    (ND_OS_PRIO )ND_OS_PRIO_LEVEL_3,
                    (STACK_PTR *)&DataCheck_Stk[0],
                    (STACK_SIZE )TASK_STACK_SIZE,
                    (ND_OS_SEM *)NULL,
                    (ND_OS_Q   *)DataCheck_Q,
                    (TIME_CNT   )4,
                    (ND_OS_ERR *)&err);
    
    ND_OSTaskCreate((ND_OS_TCB *)Charge_Tcb,
                    (CHAR      *)"Charge_Task",
                    (TASK_PTR   )ChargeTask,
                    (ARG       *)NULL,
                    (ND_OS_PRIO )ND_OS_PRIO_LEVEL_3,
                    (STACK_PTR *)&Charge_Stk[0],
                    (STACK_SIZE )TASK_STACK_SIZE,
                    (ND_OS_SEM *)Charge_Sem,
                    (ND_OS_Q   *)NULL,
                    (TIME_CNT   )4,
                    (ND_OS_ERR *)&err);
    
    ND_OSTaskCreate((ND_OS_TCB *)EleLack_Tcb,
                    (CHAR      *)"EleLack_Task",
                    (TASK_PTR   )ManageErrTask,
                    (ARG       *)NULL,
                    (ND_OS_PRIO )ND_OS_PRIO_LEVEL_0,
                    (STACK_PTR *)&EleLack_Stk[0],
                    (STACK_SIZE )TASK_STACK_SIZE,
                    (ND_OS_SEM *)EleLack_Sem,
                    (ND_OS_Q   *)NULL,
                    (TIME_CNT   )4,
                    (ND_OS_ERR *)&err);
    
    ND_OSTaskCreate((ND_OS_TCB *)ErrCheck_Tcb,
                    (CHAR      *)"ErrCheck_Task",
                    (TASK_PTR   )ManageErrTask,
                    (ARG       *)NULL,
                    (ND_OS_PRIO )ND_OS_PRIO_LEVEL_1,
                    (STACK_PTR *)&ErrCheck_Stk[0],
                    (STACK_SIZE )TASK_STACK_SIZE,
                    (ND_OS_SEM *)NULL,
                    (ND_OS_Q   *)ErrCheck_Q,
                    (TIME_CNT   )4,
                    (ND_OS_ERR *)&err);
    
    ND_OSTaskCreate((ND_OS_TCB *)SampleAndErrChk_Tcb,
                    (CHAR      *)"SampleAndErrChk_Task",
                    (TASK_PTR   )SampleAndErrChkTask,
                    (ARG       *)NULL,
                    (ND_OS_PRIO )ND_OS_PRIO_LEVEL_3,
                    (STACK_PTR *)&SampleAndErrChk_Stk[0],
                    (STACK_SIZE )TASK_STACK_SIZE,
                    (ND_OS_SEM *)SampleAndErrChk_Sem,
                    (ND_OS_Q   *)NULL,
                    (TIME_CNT   )4,
                    (ND_OS_ERR *)&err);
    
    while(1)
    {        
        ND_OSEnterSleepMode();
    }
}

void ND_OSSleepModeTaskCreate(ND_OS_ERR *err)
{
    ND_OSTaskCreate((ND_OS_TCB *)SleepMode_Tcb,
                    (CHAR      *)"SleepMode_Task",
                    (TASK_PTR   )SleepModeTask,
                    (ARG       *)NULL,
                    (ND_OS_PRIO )ND_OS_PRIO_LEVEL_4,
                    (STACK_PTR *)&SleepMode_Stk[0],
                    (STACK_SIZE )TASK_STACK_SIZE,
                    (ND_OS_SEM *)NULL,
                    (ND_OS_Q   *)NULL,
                    (TIME_CNT   )4,
                    (ND_OS_ERR *)err);
}

/* *****************************************************************************
 End of File
 */
