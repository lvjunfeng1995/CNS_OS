/* ************************************************************************** */
/** Descriptive File Name

  @Company
    NewDoon

  @File Name
    nd_os_table.c

  @Description
    Initialize all tables of the system.
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
/* Section: Global Data                                         */
/* ************************************************************************** */
/* ************************************************************************** */

static ND_OS_MSG_Q    Msg_q     [TASKMSGQ_NUM]               =     {0};
static ND_OS_Q        Q_table   [TASKMSGQ_NUM]               =     {0};
static ND_OS_SEM      Sem_table [TASKSEM_NUM]                =     {0};
static ND_OS_RDY_LIST Rdy_table [ND_CFG_PRIO_MAX+1]          =     {0}; 
static ND_OS_MSG_POOL Pool_table[MSGPOOL_NUM]                =     {0};
static ND_OS_MEM      Mem_table [MEM_NUM]                    =     {0};
static MEM_ADDR       Mem_addr  [ND_CFG_MEM_4BYTE_ONE_SIZE]  =     {0};
static EXCEPT_ADDR    ExceptBase[INTERRUPT_STACK_SIZE ];     


         ND_OS_Q        *   const  DataCheck_Q          = &Q_table[ND_OS_QUEUE_TYPE_DATACHECK];
         ND_OS_Q        *   const  WriteFlash_Q         = &Q_table[ND_OS_QUEUE_TYPE_WRITEFLASH];
         ND_OS_Q        *   const  Stimulate_Q          = &Q_table[ND_OS_QUEUE_TYPE_STIMULATE];
         ND_OS_Q        *   const  ReadFlash_Q          = &Q_table[ND_OS_QUEUE_TYPE_READFLASH];
         ND_OS_Q        *   const  ErrCheck_Q           = &Q_table[ND_OS_QUEUE_TYPE_ERRMANAGE];
             
         ND_OS_SEM      *   const  EleImp_Sem           = &Sem_table[ND_OS_SEM_TYPE_ELEANDIMP];
         ND_OS_SEM      *   const  EleLack_Sem          = &Sem_table[ND_OS_SEM_TYPE_ELELACK];
         ND_OS_SEM      *   const  Charge_Sem           = &Sem_table[ND_OS_SEM_TYPE_CHARGE];
         ND_OS_SEM      *   const  SampleAndErrChk_Sem  = &Sem_table[ND_OS_SEM_TYPE_SampleAndErrChk];

         ND_OS_MSG_POOL *   const  Msg_Pool_Table       = &Pool_table[0]; 

         ND_OS_MEM      *   const  Mem_Table_One        = &Mem_table[MANAGEMENT_MEM_TABLE_1]; 

         ND_OS_RDY_LIST * NDRdyList[ND_CFG_PRIO_MAX+1]={&Rdy_table[ND_OS_PRIO_LEVEL_0],
                                                        &Rdy_table[ND_OS_PRIO_LEVEL_1],
                                                        &Rdy_table[ND_OS_PRIO_LEVEL_2],
                                                        &Rdy_table[ND_OS_PRIO_LEVEL_3],
                                                        &Rdy_table[ND_OS_PRIO_LEVEL_4]};
				 
				 EXCEPT_ADDR *OS_CPU_ExceptStkBase = &ExceptBase[0];
																												
				 INT8U        NDCmdTable[ND_CFG_CMD_NUM] = {5,31,7,9,11,13};
				 
				 volatile    NBRRDY            NDPrioTbl[ND_CFG_PRIO_MAX+1] = {0}; 
				 

/* ************************************************************************** */
/* ************************************************************************** */
// Section: Local Functions                                                   */
/* ************************************************************************** */
/* ************************************************************************** */

static void InitRdyList()
{
    INT32U i=0;
    
    for(;i< ND_CFG_PRIO_MAX+1;i++)
    {
        ND_OS_RDY_LIST *rdylist = NDRdyList[i];
        
        rdylist -> HeadPtr = NULL;
        rdylist -> TailPtr = NULL;
        rdylist -> NbrEntries = 0;
    }
}

void OSTable_InitAll(ND_OS_ERR *err)
{
    ND_OSQCreate(              DataCheck_Q,
                               ND_OS_QUEUE_TYPE_DATACHECK,
                (CHAR *       )"DataCheck_Q",
                (ND_OS_MSG_Q *)&Msg_q[ND_OS_QUEUE_TYPE_DATACHECK],
                (NBRMSG       )ND_CFG_MSG_QUEUE_SIZE,
                               err);
    
    ND_OSQCreate(              WriteFlash_Q,
                               ND_OS_QUEUE_TYPE_WRITEFLASH,
                (CHAR        *)"WriteFlash_Q",
                (ND_OS_MSG_Q *)&Msg_q[ND_OS_QUEUE_TYPE_WRITEFLASH],
                (NBRMSG       )ND_CFG_MSG_QUEUE_SIZE,
                              err);
    
    ND_OSQCreate(              Stimulate_Q,
                               ND_OS_QUEUE_TYPE_STIMULATE,
                (CHAR        *)"Stimulate_Q",
                (ND_OS_MSG_Q *)&Msg_q[ND_OS_QUEUE_TYPE_STIMULATE],
                (NBRMSG       )ND_CFG_MSG_QUEUE_SIZE,
                               err);
    
    ND_OSQCreate(              ReadFlash_Q,
                               ND_OS_QUEUE_TYPE_READFLASH,
                (CHAR        *)"ReadFlash_Q",
                (ND_OS_MSG_Q *)&Msg_q[ND_OS_QUEUE_TYPE_READFLASH],
                (NBRMSG       )ND_CFG_MSG_QUEUE_SIZE,
                              err);
    
    ND_OSQCreate(              ErrCheck_Q,
                               ND_OS_QUEUE_TYPE_ERRMANAGE,
                (CHAR        *)"ErrCheck_Q",
                (ND_OS_MSG_Q *)&Msg_q[ND_OS_QUEUE_TYPE_ERRMANAGE],
                (NBRMSG       )ND_CFG_MSG_QUEUE_SIZE,
                               err);
    
    ND_OSSemCreate(        EleImp_Sem,
                           ND_OS_SEM_TYPE_ELEANDIMP,
                   (CHAR *)"EleImp_Sem",
                   (NBRCNT)0,
                           err);
    
    ND_OSSemCreate(        EleLack_Sem,
                           ND_OS_SEM_TYPE_ELELACK,
                   (CHAR *)"EleLack_Sem",
                   (NBRCNT)0,
                           err);
    
    ND_OSSemCreate(        Charge_Sem,
                           ND_OS_SEM_TYPE_CHARGE,
                   (CHAR *)"Charge_Sem",
                   (NBRCNT)0,
                           err);
    
    ND_OSSemCreate(        SampleAndErrChk_Sem,
                           ND_OS_SEM_TYPE_SampleAndErrChk,
                   (CHAR *)"SampleAndErrChk_Sem",
                   (NBRCNT)0,
                           err);
    
    ND_OSMsgPoolCreate(Msg_Pool_Table,
                       err);
    
    ND_OSMemCreate(            Mem_Table_One,
                   (CHAR     *)"1280 Int capacity",
                   (MEM_ADDR *)&Mem_addr[0],
                   (MEMSIZE   )512,
                   (NBRMEM    )10,
                               err);
    
    InitRdyList();
}

/* *****************************************************************************
 End of File
 */
