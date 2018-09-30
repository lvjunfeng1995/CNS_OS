#include "stm32f10x.h"	
#include "usart.h"	
#include "nd_os.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//串口1初始化		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/8/18
//版本：V1.5
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//********************************************************************************
//V1.3修改说明 
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
//V1.4修改说明
//1,修改串口初始化IO的bug
//2,修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
//3,增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
//4,修改了EN_USART1_RX的使能方式
//V1.5修改说明
//1,增加了对UCOSII的支持
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 0
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

 
 
#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	  
u8 BufferCount = 0;  
u8 ReceiveFlag = 0;


void uart_init(u32 bound){
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_USART1, ENABLE);	//使能USART1，GPIOA时钟
  
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
   
  //USART1_RX	  GPIOA.10初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  

  //Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART1, &USART_InitStructure); //初始化串口1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(USART1, ENABLE);                    //使能串口1 

}

/*void USART1_IRQHandler(void)                	//串口1中断服务程序
	{
		INTERRUPT_DISABLE;
		
		ND_OSIntNestingCtr++;
		
		INTERRUPT_ENABLE;
		
	u8 Res = 0;
  u8 i;
	ND_OS_ERR err;
		
		if (USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)//
    {
        USART_ReceiveData(USART1);
    }
		
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
	   	Res =USART_ReceiveData(USART1);	
			
			if(USART_RX_STA&0x8000)
			{
				if(Res == 0xff)
				{
					if(USART_RX_STA > 0x8000)
					{
					    USART_RX_STA |=0x4000;
					}
					else
					{
					   INTERRUPT_DISABLE;
		 
		        ND_OSExitInt(&err);
		
		        ND_OSIntNestingCtr--;
		
		        INTERRUPT_ENABLE;
						
						return;
					}
				}
				if(USART_RX_STA&0x4000)
				{
					for(i=0 ; i< (USART_RX_STA & 0x3FFF); i++)
					{
							if(USART_RX_BUF[i] == 0xFE)
							{
								if(USART_RX_BUF[i+1] == 0x0D)
								{
									*(CMN_buf+BufferCount) = 0xFF;
								//ReceiveData[BufferCount] = 0xFF;
									i++;
									BufferCount++;
								}
								if(USART_RX_BUF[i+1] == 0x0E)
								{
									*(CMN_buf+BufferCount) = 0xFE;
								//	ReceiveData[BufferCount] = 0xFE;
									i++;
									BufferCount++;
								}											
							}
							else
							{
								*(CMN_buf+BufferCount) = USART_RX_BUF[i];
							//	ReceiveData[BufferCount] = USART_RX_BUF[i];
								BufferCount++;
							}
					}
					
					for(i=0;i < (USART_RX_STA & 0x3FFF);i++)
					{
						USART_RX_BUF[i]=0;
					}

				//	LightData = 0x00;
				//	LightData |= ReceiveData[0];
				//	LightData = LightData << 8;
				//	LightData |= ReceiveData[1];
					
					ND_OS_MSG *msg = ND_OSMsgGet(Msg_Pool_Table,&err);
					
					msg -> MsgSize = (MSG_SIZE )BufferCount;
					msg -> MsgPtr = (MEM_ADDR *)CMN_buf;
					
					CMN_buf = (INT8U *)ND_OSMemGet(Mem_Table_One,&err);
					
					if(CMN_buf == NULL )
	       {
	          while(1);
	       }				 					
					
					ReceiveFlag = 1;
					BufferCount = 0;
					USART_RX_STA=0;
					
					ND_OSTaskQPost(DataCheck_Q,ND_OS_QUEUE_TYPE_DATACHECK,msg,&err);
					
					INTERRUPT_DISABLE;	 
		
		      ND_OSIntNestingCtr--;
		
		      INTERRUPT_ENABLE;
					
					return;
				}
				else if(USART_RX_STA&0x8000)
				{
						USART_RX_BUF[USART_RX_STA & 0X3FFF] = Res;
						USART_RX_STA++;
				}
				
			}
			else
			{
					if(Res == 0xff)
					{
						USART_RX_STA |= 0x8000;
					}

			}
				
		
     } 
		
		 INTERRUPT_DISABLE;
		 
		  ND_OSExitInt(&err);
		
		ND_OSIntNestingCtr--;
		
		INTERRUPT_ENABLE;

} */

void PostTaskToDataChk(ND_OS_ERR *err)
{
    ND_OS_MSG *msg = ND_OSMsgGet(Msg_Pool_Table,err);
					
	  msg -> MsgSize = (MSG_SIZE )Rec_Data_Ctr;
	  msg -> MsgPtr = (MEM_ADDR *)CMN_buf;
					
	  CMN_buf = (INT8U *)ND_OSMemGet(Mem_Table_One,err);
					
	  if(CMN_buf == NULL )
    {
	      while(1);
    }
 
    Rec_State = ND_REC_DATA_START;					 
					
	  ND_OSTaskQPost(DataCheck_Q,ND_OS_QUEUE_TYPE_DATACHECK,msg,err);
					
	  INTERRUPT_DISABLE;	 
		
	  ND_OSIntNestingCtr--;
		 
	  INTERRUPT_ENABLE;					
}

void USART1_IRQHandler(void)
{
    INTERRUPT_DISABLE;
		
		ND_OSIntNestingCtr++;
		
		INTERRUPT_ENABLE; 
	
	  INT8U Res = 0;
	  ND_OS_ERR err;	 
	
		if (USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)
    {
        USART_ReceiveData(USART1);
    }
		
	  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  
	  {
	      Res =USART_ReceiveData(USART1);
		
		    if(0xAA == Res)
		    {
		        Rec_State = ND_REC_DATA_FIRST;
					
					  Rec_Data_Ctr = REC_DATA_NONE;										 
		    }
		
		    switch(Rec_State)
		    {
		        case ND_REC_DATA_FIRST:
			 	
                if(Res < ND_CFG_CMD_NUM)
				        {
				            Cmd_Length = NDCmdTable[Res];										
					
					         *(CMN_buf + Rec_Data_Ctr) = 0xAA;
					          Rec_Data_Ctr ++;
					
					         *(CMN_buf + Rec_Data_Ctr) = Res;
					          Rec_Data_Ctr ++;
					
					          Rec_State = ND_REC_DATA_CMD;
				         }
								else if(Res != 0xAA)
								{
								    Rec_State = ND_REC_DATA_START;
								}
				
				     break;
			
			       case ND_REC_DATA_CMD:
				
			           *(CMN_buf + Rec_Data_Ctr) = Res;
					        Rec_Data_Ctr ++;
			
			           if(Cmd_Length == Rec_Data_Ctr)
				         {
				             PostTaskToDataChk(&err);
											 
										 Rec_Data_Ctr = REC_DATA_NONE;								                
					
					           return;
				         }
				         else if(0xBB == Res)
				         {
				             *(CMN_buf + Rec_Data_Ctr-1) = 0xAB;
											 
					           PostTaskToDataChk(&err);
											 
										 Rec_Data_Ctr = REC_DATA_NONE;									                
					
					           return;
				          }  
			
				      break;
			
			        case ND_REC_DATA_START:
				      break;
		      }
	    }
	
	    INTERRUPT_DISABLE;
		 
		  ND_OSExitInt(&err);
		
		  ND_OSIntNestingCtr--;
		
		  INTERRUPT_ENABLE;
}

void SysTick_Init(void)
{
   
  //if (SysTick_Config(SystemFrequency / 1000))  //ST3.0.0???
    if (SysTick_Config(7200))  //ST3.5.0???
  {
   
    while (1);
  }
}

#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif

#endif	

