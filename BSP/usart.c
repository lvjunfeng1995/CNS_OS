#include "stm32f10x.h"	
#include "usart.h"	
#include "nd_os.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//����1��ʼ��		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/8/18
//�汾��V1.5
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//V1.5�޸�˵��
//1,�����˶�UCOSII��֧��
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 0
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

 
 
#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	  
u8 BufferCount = 0;  
u8 ReceiveFlag = 0;


void uart_init(u32 bound){
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_USART1, ENABLE);	//ʹ��USART1��GPIOAʱ��
  
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
   
  //USART1_RX	  GPIOA.10��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  

  //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1 

}

/*void USART1_IRQHandler(void)                	//����1�жϷ������
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
		
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
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

#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif

#endif	

