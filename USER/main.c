/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/main.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */  
	
#include "nd_os.h"


volatile    NBRINT            ND_OSIntNestingCtr  =   CUR_INT_NESTING_CNT_NONE;
volatile    ND_REC_DATA_STATE Rec_State           =   ND_REC_DATA_START; 
volatile    INT8U             Cmd_Length          =   REC_DATA_NONE;
volatile    INT8U             Rec_Data_Ctr        =   REC_DATA_NONE;
volatile    INT8U             Stimulate_100us_Cnt =   0;
volatile    INT8U             Stimulate_not_Cnt   =   0;
volatile    INT8U             Stimulate_100us_Ctr =   0;
volatile    INT8U             Stimulate_not_Ctr   =   0;
volatile    INT32U            StimulateBurst      =   0;
volatile    INT32U            Stimulatethorugh    =   0;
volatile    STIMULATE_STATE   Stimulate_state     =   Stimulate_Start;
volatile    MSG_SIZE          Reply_length        =   0;
const   ND_OS_TCB  volatile  *ND_TCBCurPtr       =         NULL;             
const   ND_OS_TCB  volatile  *ND_TCBHighRdyPtr   =         NULL;
        INT8U             *CMN_buf            =         NULL;
				INT8U             *Reply_buf          =         NULL;

int main(void)
 {
	 ND_OS_ERR err = NONE; 
	
	 SysTick_Init();
	 
   OSTable_InitAll((ND_OS_ERR *)&err);
	
	 CMN_buf   = (INT8U *)ND_OSMemGet(Mem_Table_One,&err); 
	 Reply_buf = (INT8U *)ND_OSMemGet(Mem_Table_One,&err);
	 
	 if(CMN_buf == NULL || Reply_buf == NULL )
	 {
	    while(1);
	 }
    
   ND_OSSleepModeTaskCreate((ND_OS_ERR *)&err);
	
   while(1);
}


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
