/* ************************************************************************** */
/** Descriptive File Name

  @Company
    NewDoon

  @File Name
    nd_os.h

  @Description
    Contains all the methods and data structures
 */
/* ************************************************************************** */

#ifndef _ND_OS_H    /* Guard against multiple inclusion */
#define _ND_OS_H


#ifdef __cplusplus
extern "C" {
#endif
    
#include "nd_os_type.h"
    
#define         NULL                         ((void *)0)
    
#define         CUR_INT_NESTING_CNT_ONE       1                                  /*Number of the nesting ISR*/
#define         CUR_INT_NESTING_CNT_NONE      0

#define         CUR_RESEND_CNT_NONE           0
#define         CUR_RESEND_CNT_LIMIT          3

#define         REC_DATA_NONE                 0

#define         ND_CFG_CMD_NUM                6
    
#define         ND_CFG_PRIO_MAX               4                                 /*Lowest priority*/
    
#define         INTERRUPT_ENABLE              INTX_ENABLE()             /*Enable interrupt*/
#define         INTERRUPT_DISABLE             INTX_DISABLE()           /*Disable interrupt*/
    
#define         ND_CFG_MSG_POOL_SIZE          10                                /*Message pool size*/
#define         ND_CFG_MEM_4BYTE_ONE_SIZE     1280                              /*First memory partition capacity*/
#define         ND_CFG_MSG_QUEUE_SIZE         3                                 /*Message queue size*/
    
#define         ALLGN4MASK                    0xFFFFFFFC                        /*4 byte alignment*/
#define         ALLGN8MASK                    0xFFFFFFF8                        /*8 byte alignment*/
    
#define         TASKMSGQ_NUM                   5                                /*Number of task  message queue*/
#define         TASKSEM_NUM                    4                                /*Number of task  sem*/
#define         MSGPOOL_NUM                    1                                /*Number of message pool*/
#define         MEM_NUM                        1                                /*Number of management memory table*/
#define         TASK_NUM                       10                               /*Number of task*/
    
#define         TASK_STACK_SIZE                128                              /*Size of task stack*/
#define         INTERRUPT_STACK_SIZE           1024                             /*Size of interrupt stack*/

#define         SAMPLING_FREQUENCY             200                              /*Sampling frequency*/

#define         True                           1
#define         False                          0

      
			
    typedef     enum      event_type         EVENT_TYPE;
    typedef     enum      nd_os_state        ND_OS_STATE;
    typedef     enum      nd_os_prio         ND_OS_PRIO;
    typedef     enum      nd_os_queue_type   ND_OS_QUEUE_TYPE;
    typedef     enum      nd_os_sem_type     ND_OS_SEM_TYPE;
    typedef     enum      nd_os_err          ND_OS_ERR;
    typedef     enum      nd_data_err        ND_DATA_ERR;
		typedef     enum      nd_rec_data_state  ND_REC_DATA_STATE;
		typedef     enum      nd_os_request      ND_OS_REQUEST;
		typedef     enum      nd_os_reply        ND_OS_REPLY;
		typedef     enum      nd_stimulate_state STIMULATE_STATE;
    
    typedef     struct    nd_os_tcb       ND_OS_TCB;  
    typedef     struct    nd_os_msg       ND_OS_MSG;
    typedef     struct    nd_os_msg_pool  ND_OS_MSG_POOL;
    typedef     struct    nd_os_msg_q     ND_OS_MSG_Q;
    typedef     struct    nd_os_sem       ND_OS_SEM;
    typedef     struct    nd_os_mem       ND_OS_MEM;
    typedef     struct    nd_os_rdy_list  ND_OS_RDY_LIST;
    typedef     struct    nd_os_q         ND_OS_Q;
		typedef     struct    stimulate       STIMULATE;
    
    enum event_type{                                                            /*Type of event requested*/
        ND_OS_TYPE_SEM=1,
        ND_OS_TYPE_QUEUE
    };
    
    enum nd_os_state{                                                           /*Task state*/
        ND_OS_TASK_RUNNING=1,
        ND_OS_TASK_READY,
        ND_OS_TASK_WRITING,
        ND_OS_TASK_SLEEP
    };
		
		enum nd_rec_data_state{
		    ND_REC_DATA_START = 1,
			  ND_REC_DATA_FIRST,
			  ND_REC_DATA_CMD
		};
    
    enum nd_os_prio{                                                            /*Task priority*/
        ND_OS_PRIO_NONE    =-1,
        ND_OS_PRIO_LEVEL_0 =0,
        ND_OS_PRIO_LEVEL_1,
        ND_OS_PRIO_LEVEL_2,
        ND_OS_PRIO_LEVEL_3,
        ND_OS_PRIO_LEVEL_4
    };
    
    enum nd_os_queue_type{                                                      /*Task message queue type*/
        ND_OS_QUEUE_TYPE_DATACHECK=0,
        ND_OS_QUEUE_TYPE_WRITEFLASH,
        ND_OS_QUEUE_TYPE_STIMULATE,
        ND_OS_QUEUE_TYPE_READFLASH,
        ND_OS_QUEUE_TYPE_ERRMANAGE
    };
    
    enum nd_os_sem_type{                                                        /*Semaphore type*/
        ND_OS_SEM_TYPE_ELEANDIMP=0,
        ND_OS_SEM_TYPE_CHARGE,
        ND_OS_SEM_TYPE_ELELACK,
        ND_OS_SEM_TYPE_SampleAndErrChk
    };
    
    enum nd_os_err{                                                             /*Error type*/
        NONE=0,
        TYPE_ERR,
        EXIST,
        FULL,
        MSG_NULL,
        TABLE_NULL,
        MSGQ_NULL,
        TYPE_NULL,
        NAME_NULL,
        NUM_CANNOT_ZERO,
        MEM_NULL,
        OUT_OF_MEMORY,
        NUMBER_FEW,
        MEM_ADDR_NULL,
        MEM_BLK_FULL,
        POOL_NULL,
        POOL_NONE,
        MSG_ADDR_NULL,
        POOL_FULL,
        TCB_NULL,
        TASK_NULL,
        STACK_NULL,
        STACK_SIZE_FEW,
        SWITCH_ERR,
        QUEUE_NULL,
        SEM_NULL,
				MOUNTED
    };
    
    enum mem_table{                                                             /*Memory table*/
        MANAGEMENT_MEM_TABLE_1 = 0
    };
    
    enum nd_data_err                                                            /*Data err type*/
    {
        DATA_NONE=0,
        DATA_ERR,
        DATA_IMPCHK,
        DATA_STM,
        DATA_WF,
        DATA_RESEND,
			  DATA_CONNECT,
			  DATA_CHARGE,
			  WRITE_ERR,
			  WRITE_SUCCESS,
			  KEEP_LIVE,
			  DATA_EEG
    };
    
    enum nd_os_task                                                             /*Task type*/
    {
        ND_OS_TASK_DATACHECK=0,
        ND_OS_TASK_WRITEFLASH,
        ND_OS_TASK_STIMULATE,
        ND_OS_TASK_READFLASH,
        ND_OS_TASK_ELEANDIMP,
        ND_OS_TASK_CHARGE,
        ND_OS_TASK_ELELACK,
        ND_OS_TASK_SAMPLEANDERRCHK,
        ND_OS_TASK_ERRCHECK,
        ND_OS_TASK_SLEEPMODE
    };
		
		enum nd_os_request
		{
		    Req_connect = 0,
			  Req_stimulate,
			  Req_charge,
			  Req_EEG,
			  Req_resend,
			  Keep_live
		};
		
		enum nd_os_reply
		{
		    Rep_confirm = 0,
			  Rep_stimulate,
			  Rep_charge,
			  Rep_EEG,
			  Rep_resend,
			  Rep_alive,
		};
		
		enum nd_stimulate_state
		{
		    Stimulate_Start,
			  Stimulate_Positive,
			  Stimulate_Negative,
			  Stimulate_End
		};

    struct nd_os_tcb{                                                           /*Task control block data structure*/
        STACK_PTR           *StkPtr;
        
        ND_OS_TCB           *NextPtr;
        ND_OS_TCB           *PrevPtr;
        
        CHAR                *NamePtr;
        
        STACK_PTR           *StkBasePtr;
        
        TASK_PTR             TaskEntryAddr;
        ARG                 *TaskEntryArg;
        
        void                *PendDataTblPtr;
        EVENT_TYPE           Pendon;
        
        ND_OS_STATE          Task_state;
        
        ND_OS_PRIO           Prio;
        
        STACK_SIZE           StkSize;
        
        TIME_CNT             TimeQuanta;
        TIME_CNT             TimeQuantaCtr;      
    };
    
    struct nd_os_msg{                                                           /*Message data structure*/
        ND_OS_MSG           *NextPtr;
        
        MSG_SIZE             MsgSize;
        
        MEM_ADDR            *MsgPtr;
    };
    
    struct nd_os_msg_pool{                                                      /*Message idle pool data structure*/
        ND_OS_MSG           *NextPtr;
        
        NBRMSG               NbrFree;
        NBRMSG               NbrUsed;
    };
    
    struct nd_os_q{                                                             /*Task message queue table data structure*/
        ND_OS_QUEUE_TYPE     Type;
        
        CHAR                *NamePtr;
        
        ND_OS_TCB           *TCB;
        
        ND_OS_MSG_Q         *MsgQ;
    };
    
    struct nd_os_msg_q{                                                         /*Message queue table data structure*/
        ND_OS_MSG           *InPtr;
        ND_OS_MSG           *OutPtr;
        
        NBRMSG               NbrEntriesSize;
        NBRMSG               NbrEntries;
    };
    
    struct nd_os_sem{                                                           /*Semaphore table data structure*/
        ND_OS_SEM_TYPE       Type;
        
        CHAR                *NamePtr;
        
        ND_OS_TCB           *TCB;
        
        NBRCNT               Ctr;
    };
    
    struct nd_os_mem{                                                           /*Memory control block data structure*/
        CHAR                *NamePtr;
        
        MEM_ADDR            *FreeListPtr;
        
        MEMSIZE              BlkSize;
        
        NBRMEM               NbrMax;
        NBRMEM               NbrFree;
    };
    
    struct nd_os_rdy_list{                                                      /*Task ready table data structure*/
        NBRRDY               NbrEntries;
        
        ND_OS_TCB           *HeadPtr;
        ND_OS_TCB           *TailPtr;
    };
		
		struct stimulate{
			
	      INT32U Freq;
			
	      INT32U Pwper;
			
	      INT32U BurstDuration;
			
			  INT8U Current;
    };
    
    
            extern    volatile    NBRINT            ND_OSIntNestingCtr;  /*Nesting interrupt counter*/
		
            extern    volatile    ND_REC_DATA_STATE Rec_State;
            extern    volatile    INT8U             Cmd_Length; 
            extern    volatile    INT8U             Rec_Data_Ctr;	
            extern    volatile    MSG_SIZE          Reply_length;	
		
		        extern    volatile    INT8U             Stimulate_100us_Cnt;
            extern    volatile    INT8U             Stimulate_not_Cnt;  
            extern    volatile    INT8U             Stimulate_100us_Ctr;		
		        extern    volatile    INT8U             Stimulate_not_Ctr;
		        extern    volatile    STIMULATE_STATE   Stimulate_state;
            extern    volatile    INT32U            StimulateBurst;
            extern    volatile    INT32U            Stimulatethorugh;		
		
            extern    volatile    NBRRDY            NDPrioTbl[ND_CFG_PRIO_MAX+1];          /*Record which priority has a task ready*/
            extern      ND_OS_RDY_LIST*   NDRdyList[ND_CFG_PRIO_MAX+1];                /*Return task ready table*/
		
		        extern      INT8U             NDCmdTable[ND_CFG_CMD_NUM];
             
    extern const   ND_OS_TCB  volatile  *ND_TCBCurPtr;               /*Point to current task block*/
    extern const   ND_OS_TCB  volatile  *ND_TCBHighRdyPtr;               /*Point to the higher task block*/
		
		extern EXCEPT_ADDR *OS_CPU_ExceptStkBase;                          /*Point to the except stack*/
		extern INT8U    *CMN_buf;                                       /*Point to the USART interrupt data buffer*/
		extern EEG_ADDR *Mem_tail;                                      /*Point to the current eeg*/
    extern EEG_ADDR *Head_buf;                                      /*Point to the eeg buffer bound*/ 
    extern EEG_ADDR *End_buf;
    extern EEG_ADDR *Mem_head;    
    extern INT8U    *Reply_buf;
             
    extern  ND_OS_Q         * const   DataCheck_Q;                     /*Task queue table*/
    extern  ND_OS_Q         * const   WriteFlash_Q;
    extern  ND_OS_Q         * const   Stimulate_Q;
    extern  ND_OS_Q         * const   ReadFlash_Q;
    extern  ND_OS_Q         * const   ErrCheck_Q;
             
    extern  ND_OS_SEM       * const   EleImp_Sem;                      /*Task semaphore table*/
    extern  ND_OS_SEM       * const   EleLack_Sem;
    extern  ND_OS_SEM       * const   Charge_Sem;
    extern  ND_OS_SEM       * const   SampleAndErrChk_Sem;
    
    extern  ND_OS_MSG_POOL  * const   Msg_Pool_Table;                  /*Point to idle message pool  */
    
    extern  ND_OS_MEM       * const   Mem_Table_One;                   /*First memory table*/
		
		
      
    
    extern void ND_OSSemCreate (            ND_OS_SEM       * const   sem,
                                            ND_OS_SEM_TYPE            type,
                                            CHAR                     *name,
                                            NBRCNT                    cnt,
                                            ND_OS_ERR                *err );
    
    extern void ND_OSTaskSemPend (            ND_OS_SEM       * const   sem,
                                              ND_OS_SEM_TYPE           type,
                                              ND_OS_ERR               *err );
    
    extern void ND_OSTaskSemPost(            ND_OS_SEM       * const   sem,
                                             ND_OS_SEM_TYPE            type,
                                             ND_OS_ERR                *err );
    
    extern void ND_OSTaskSemSet (            ND_OS_SEM       * const   sem,
                                             ND_OS_SEM_TYPE            type,
                                             NBRCNT                    cnt,
                                             ND_OS_ERR                *err );
    
    extern void ND_OSSched      ( ND_OS_TCB        *tcb,
                                   ND_OS_ERR        *err );
    
    extern void ND_OSIntCtxSw      (void);
    
    extern void ND_OSCtxSw         (void);
    
    extern void ND_OSStartHighRdy (void);
    
    extern void ND_OSQCreate    (   ND_OS_Q              * const   queue,
                                    ND_OS_QUEUE_TYPE                type,
                                    CHAR                           *name,
                                    ND_OS_MSG_Q                    *msg_q, 
                                    NBRMSG                         entries_size,
                                    ND_OS_ERR                      *err);
    
    extern ND_OS_MSG const *ND_OSTaskQPend(   ND_OS_Q   * const          queue,
                                              ND_OS_QUEUE_TYPE             type,
                                             ND_OS_ERR                    *err);
    
    extern void ND_OSTaskQPost(   ND_OS_Q               * const   queue,
                                  ND_OS_QUEUE_TYPE                 type,
                                  ND_OS_MSG                       *msg,
                                  ND_OS_ERR                       *err);
    
    extern void ND_OSMemCreate(   ND_OS_MEM         * const       mem,
                                  CHAR                           *name,
                                  MEM_ADDR                       *addr,
                                  MEMSIZE                         blk_size,
                                  NBRMEM                          nbr_size,
                                  ND_OS_ERR                      *err);
    
    extern MEM_ADDR *ND_OSMemGet(           ND_OS_MEM     * const  mem,
                                            ND_OS_ERR             *err);

    extern void ND_OSMemPut(           ND_OS_MEM     * const   mem,
                                       MEM_ADDR               *addr,
                                       ND_OS_ERR              *err);
    
    extern void ND_OSMsgPoolCreate(            ND_OS_MSG_POOL    * const pool,
                                               ND_OS_ERR                *err); 
    
    extern ND_OS_MSG *ND_OSMsgGet(  ND_OS_MSG_POOL    * const pool,
                                    ND_OS_ERR         *err);

    extern void ND_OSMsgPut(  ND_OS_MSG_POOL    * const pool,
                              ND_OS_MSG         *msg,
                              ND_OS_ERR         *err);
    
    extern void ND_OSTaskCreate( ND_OS_TCB           *p_tcb,
                                   CHAR                *p_name,
                                   TASK_PTR             p_task,
                                   ARG                 *p_arg,
                                   ND_OS_PRIO           prio,
                                   STACK_PTR           *p_stk_base,
                                   STACK_SIZE           stk_size,
                                   ND_OS_SEM    * const sem,
                                   ND_OS_Q      * const queue,
                                   TIME_CNT             time_quanta,
                                   ND_OS_ERR           *err);
    
    extern void RunNextHighPrioTask(ND_OS_ERR *err);
    
    extern ND_OS_TCB *GetHighRdyTcbPtr(void);
    
    extern void OSTable_InitAll(ND_OS_ERR *err);
    
    extern void ND_OSSleepModeTaskCreate(ND_OS_ERR *err);
													
		extern void PendAndRunNextTask(void);
													
		extern void INTX_DISABLE(void);
											
		extern void INTX_ENABLE(void); 
													
    extern ND_OS_TCB *GetSamePrioTask(void);
													
	  extern void ND_OSExitInt(ND_OS_ERR *err);
													
		extern void ND_OSSysTick(void);
		
		extern void TIM3_Start(void);
   
#ifdef __cplusplus
}
#endif

#endif 

/* *****************************************************************************
 End of File
 */
