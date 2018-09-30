/* ************************************************************************** */
/** Descriptive File Name

  @Company
     NewDoon

  @File Name
    nd_os_event.c

  @Description
    Task event
 */
/* ************************************************************************** */




/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

#include "nd_os.h"
#include "nd_os_event.h"
#include "stm32f10x.h"
#include <string.h>

/* ************************************************************************** */
/* ************************************************************************** */
// Section: Local Functions                                                   */
/* ************************************************************************** */
/* ************************************************************************** */

STIMULATE stm = {0x1F4,2,0x64,0x50};

void UsartSend(INT8U * p_Send, MSG_SIZE len)
{
	  int i = 0;
	  for(i = 0;i < len;i++)
    {
		  USART1->DR = (*p_Send);
	    while((USART1->SR&0X40)==0);
		  p_Send++;
	  }
}

INT32U ItanSend(INT32U data)
{
	INT32U FromItan=0;
	 GPIO_WriteBit(GPIOA, GPIO_Pin_4, 0);
	
	 FromItan = (SPI1_ReadWriteByte((INT16U)((data&0xffff0000)>>16)));
	
	 FromItan = FromItan<<16;
	FromItan |= SPI1_ReadWriteByte((INT16U)(data&0x0000ffff));
	 
	 GPIO_WriteBit(GPIOA, GPIO_Pin_4, 1);	
	
	 return FromItan;
}

INT32U ItanCLEAR(void)
{
	 INT32U rhdrxdat=0;
	 INT32U rhdtxdat=0;
	 rhdtxdat = 0x6a000000;
   rhdrxdat = SPI1_ReadWriteByte(rhdtxdat);
	 return rhdrxdat;
}

INT32U ItanSTIMULATE(INT32U ch)
{
	 INT32U rhdrxdat;
	 INT32U rhdtxdat = 0;
	// rhdtxdat += ch;
	 //rhdtxdat <<= 16;
	 rhdtxdat = (INT32U)((0x08<<24)|((ch)<<16)|0x00);
	 rhdrxdat = ItanSend(rhdtxdat);
	 return rhdrxdat;	
}

INT32U ItanTriWRITE(INT32U reg, INT16U dat)
{
	
	 INT32U rhdrxdat=0;
	 INT32U rhdtxdat=0;
	
	 rhdtxdat = (INT32U)((0xA0<<24)|((reg)<<16)|dat);
	 rhdrxdat = ItanSend(rhdtxdat);

	 return rhdrxdat;

}

INT32U ItanWRITE(INT32U reg, INT16U dat)
{
	
	 INT32U rhdrxdat=0;
	 INT32U rhdtxdat=0;
	
	 rhdtxdat = (INT32U)((0x80<<24)|((reg)<<16)|dat);
	 rhdrxdat = ItanSend(rhdtxdat);

	 return rhdrxdat;

}

void ItanInit(void)
{
	INT32U i;
	
	ItanWRITE(0, 0x0828); 
	ItanWRITE(1, 0x0595);	
	ItanWRITE(2, 0x0040);	
	ItanWRITE(3, 0x0080);		
	ItanWRITE(4, 0x06a6);	
  ItanWRITE(5, 0x03e5);
  ItanWRITE(6, 0x08A3);
  ItanWRITE(7, 0x0188);
  ItanWRITE(8, 0xFFFF);
  ItanWRITE(10, 0xFFFF);
  ItanWRITE(12, 0x000F);
	ItanTriWRITE(10, 0xFFFF);
	ItanWRITE(10, 0x0000);
	ItanTriWRITE(10, 0x0000);	
	
	
	ItanWRITE(32, 0x0000);
	ItanWRITE(33, 0x0000);	
  ItanWRITE(34, 0x000F);	
  ItanWRITE(35, 0x00FF);
  ItanWRITE(36, 0x0080);	
  ItanWRITE(37, 0x0009);	
  ItanWRITE(38, 0xFFFF);
  ItanWRITE(42, 0x0002);
  ItanWRITE(44, 0xFFFE);	
 // ItanWRITE(44, 0x0001);
	ItanWRITE(46, 0x0000);	
  ItanWRITE(48, 0x0003);	
	for(i=64 ;i<80; i++)
	{
		ItanWRITE(i, 0x80FF);
	}
	for(i=96 ;i<112; i++)
	{
		ItanWRITE(i, 0x80FF);
	}
}

MSG_SIZE RestoreData(INT8U *mem,MSG_SIZE msg_size)
{
    INT8U index = 2;
	
	  MSG_SIZE length = 2;
	
	  INT8U high = 0;
	  INT8U low  = 0;
	
	  for(;index < msg_size-2;index=index+2)
	  {
		    high = *(mem + index);
			  low  = *(mem + index+1);
			
			  *(mem + length) = (high << 4)|(low & 0x0F);
			
			  length ++;
		}
		
		*(mem + length) = 0xBB;
		
		length ++;
		
		return length;
}

void ModifyData(INT8U * const mem,INT8U *reply_buf,ND_OS_REPLY reply)
{
    MEMSIZE length = Reply_length;
	  Reply_length   = 0;
	
	  *(reply_buf + Reply_length) = 0xAA;
	  Reply_length ++;
	
	  *(reply_buf + Reply_length) = reply;
	  Reply_length ++;
	
	  MEMSIZE i=2;
	
	  for(;i< length - 1;i++)
	  {
	      INT8U num = *(mem + i);
			
			  *(reply_buf + Reply_length) = 0x30|(num >> 4);
			  Reply_length ++;
			
			  *(reply_buf + Reply_length) = 0x30|(0x0F & num);
			  Reply_length ++;
	  }
		
		*(reply_buf + Reply_length) = 0xBB;
		Reply_length ++;
	  
}

void ReplyConfirm(void)
{
    Reply_length = 0;
	
	  *(Reply_buf + Reply_length) = 0xAA;
	  Reply_length ++;
	
	  *(Reply_buf + Reply_length) = Rep_confirm;
	  Reply_length ++;
	
	  *(Reply_buf + Reply_length) = 0x01;
	  Reply_length ++;
	
	  *(Reply_buf + Reply_length) = 0x02;
	  Reply_length ++;
	
	  *(Reply_buf+ Reply_length) = 0x03;
	  Reply_length ++;
	 
	  *(Reply_buf + Reply_length) = 0x04;
	  Reply_length ++;
		
		INT8U index = 1;
		INT8U sum   = 0; 
		
		for(;index < Reply_length;index++)
		{
		    sum += *(Reply_buf + index);
		}
		
		*(Reply_buf + Reply_length) = sum;
	  Reply_length ++;
		
		*(Reply_buf+ Reply_length) = 0xBB;
	  Reply_length ++;
		
		//ModifyData(mem,Reply_buf,Rep_confirm);
}

void ReplyInf(ND_OS_REPLY reply,INT8U request)
{
    Reply_length = 0;
	
	  *(Reply_buf + Reply_length) = 0xAA;
	  Reply_length ++;
	
	  *(Reply_buf + Reply_length) = reply;
	  Reply_length ++;
	
	  *(Reply_buf + Reply_length) = request;
	  Reply_length ++;		 
		
		INT8U index = 1;
		INT8U sum   = 0; 
		
		for(;index < Reply_length;index++)
		{
		    sum += *(Reply_buf + index);
		}
		
		*(Reply_buf + Reply_length) = sum;
	  Reply_length ++;
		
		*(Reply_buf + Reply_length) = 0xBB;
	  Reply_length ++;
		
		//ModifyData(mem,Reply_buf,reply);
}

void ReplyEEG(void)
{
    Reply_length = 0;
	
	  *(Reply_buf + Reply_length) = 0xAA;
	  Reply_length ++;
	
	  *(Reply_buf + Reply_length) = Rep_EEG;
	  Reply_length ++;
	
	  if(Mem_tail > Mem_head)
	{
		INT32U i;
		EEG_ADDR *addr;
		addr = Mem_tail;
		
		for(i=0;i < SAMPLING_FREQUENCY;i++)
		{
		  *((EEG_ADDR *)(Reply_buf+2+i*sizeof(EEG_ADDR))) = *addr;
			
			addr ++;
			Reply_length += sizeof(EEG_ADDR);
			
			if(addr == End_buf)
			{
			  addr = Head_buf;
			}
		}
	}
	else 
	{
	  INT32U i;
		EEG_ADDR *addr;
		addr = Mem_tail;
		
		for(i=0;i < SAMPLING_FREQUENCY;i++)
		{
		  *((EEG_ADDR *)(Reply_buf+2+i*sizeof(EEG_ADDR))) = *addr;
			
			addr --;
			Reply_length += sizeof(EEG_ADDR);
			
			if(addr == Head_buf-1)
			{
			  addr = End_buf;
				addr--;
			}
		}
	}	 
		
		MSG_SIZE index = 1;
		INT8U    sum   = 0; 
		
		for(;index < Reply_length;index++)
		{
		    sum += *(Reply_buf + index);
		}
		
		*(Reply_buf + Reply_length) = sum;
	  Reply_length ++;
		
		*(Reply_buf + Reply_length) = 0xBB;
	  Reply_length ++;
		
		//ModifyData(mem,Send_EegData_buf,Rep_EEG);
}

void ND_DataErrCheck(ND_OS_MSG const *msg,
                     ND_DATA_ERR     *data_err)
{
	  ND_OS_ERR err = NONE;
	
    MSG_SIZE msg_size = msg -> MsgSize;
    INT8U *mem = (INT8U *)(msg -> MsgPtr);
	
	  //Dll_unpack((ND_OS_MSG *)msg);
	  INT8U first_byte = *(mem);
	  INT8U end_byte = *(mem + msg_size-1);
	  
	  if(0xAA == first_byte && 0xBB == end_byte)
		{
		    msg_size = RestoreData(mem,msg_size);
			  
			  INT8U    sum = 0;
			  MSG_SIZE index = 1;
			  INT8U    checkcode = *(mem + msg_size - 2);
        
        for(;index < msg_size-2;index++)
			  {
				    sum += *(mem + index);
				}
				
				if(sum == checkcode)
				{
				    ND_OS_REQUEST request = *(mem + 1);
					
					  switch(request)
						{
							case Req_connect:							
							
							    ReplyConfirm();
							    
							    *(data_err) = DATA_CONNECT;
							
								break;
							
							case Req_stimulate:
								
							    ReplyInf(Rep_stimulate,0x01);
							
							    *(data_err) = DATA_STM;
							
                  for(index=2;index<msg_size;index++)
                  {
										  *(mem + index-2) = *(mem + index);
									}							
							
								break;
									
							case Req_charge:
								
							    *(data_err) = DATA_CHARGE;
									
								break;
									
							case Req_EEG:
								
							    ReplyEEG();								
										
									*(data_err) = DATA_EEG;
									
								break;
							case Req_resend:
								
							     *(data_err) = DATA_RESEND;
									
								break;
									
							case Keep_live:
								
							    ReplyInf(Rep_alive,0x01);
										
								  *(data_err) = KEEP_LIVE;
									
								break;
						}
				}
				else
				{
				    ReplyInf(Rep_resend,0xFF);
					
				    *data_err = DATA_ERR;
				}
		}
		else
		{
		    ReplyInf(Rep_resend,0xFF);
			
		    *data_err = DATA_ERR;
		}
	// ND_OSMemPut(Mem_Table_One,(MEM_ADDR *)mem,&err);
			
	//ND_OSMsgPut(Msg_Pool_Table,
                              //  (ND_OS_MSG *)msg,
                            //    (ND_OS_ERR *)&err);
	//put_num++;
}

void ND_WriteFlash(ND_OS_MSG const *msg,
                        ND_DATA_ERR *data_err)
{
    
}

void ND_Stimulate(ND_OS_MSG const *msg,ND_DATA_ERR *data_err,ND_OS_ERR *err)
{
	INT8U *stm_mem = (INT8U *)msg->MsgPtr;
	
	STIMULATE Stm_Data;
	
	memcpy(&Stm_Data,stm_mem,sizeof(Stm_Data));
	
	//INT32U  StimulateBurst=0;
	INT32U  i;
	//INT32U Stimulatethorugh=0;
	
	if(Stm_Data.Freq != 0){
		      //TIM3_Int_Init(10000/Stm_Data.Freq,7199);		      
				  StimulateBurst = (Stm_Data.BurstDuration * Stm_Data.Freq)/1000;
				  Stimulatethorugh = 0;
			  }
	
//	INT32U Postive_delay = (100000/(Stm_Data.Freq*2))-Stm_Data.Pwper;
//	INT32U Negative_delay = Stm_Data.Pwper;
				
	Stimulate_100us_Cnt = Stm_Data.Pwper;
	Stimulate_not_Cnt   =  (10000/(Stm_Data.Freq*2))-Stm_Data.Pwper;
				
	INT16U  Current_size = Stm_Data.Current | 0x8000;
				
	 for(i=96 ;i<112; i++)
			{
				ItanWRITE(i, Current_size);
			}			
			
	for(i=64 ;i<80; i++)
	    {
		    ItanWRITE(i, Current_size); 
	    }
			
      ItanTriWRITE(32, 0x0000);
			ItanTriWRITE(33, 0x0000); 			
						
	TIM3_Start();	
	/*while(1)
	{
		INTERRUPT_DISABLE;
		
		TIM3_Delay10us(Postive_delay-2);
		
			
			ItanTriWRITE(44, 0x80FF);		
					 
			ItanTriWRITE(32, 0xAAAA);
			ItanTriWRITE(33, 0x00FF);
		
		  ItanSTIMULATE(1);
										
		  TIM3_Delay10us(Negative_delay-1);
		
		  //ItanTriWRITE(32, 0x0000);
		//	ItanTriWRITE(33, 0x0000); 
			
		  ItanTriWRITE(44, 0x0000);		       	

     // ItanTriWRITE(32, 0xAAAA);
		//	ItanTriWRITE(33, 0x00FF);
							
	 	  ItanSTIMULATE(1);	
		
		TIM3_Delay10us(Negative_delay-1);
			
			
			ItanTriWRITE(32, 0x0000);
			ItanTriWRITE(33, 0x0000); 
				 								
		TIM3_Delay10us(Postive_delay-2);
			
			
		  Stimulatethorugh++;
		
    
					 
					/* if(Stimulatethorugh%2)
           {
						 ItanTriWRITE(44, 0x0000);
						 	for(i=64 ;i<80; i++)
	            {
		             ItanWRITE(i, 0x80FF); 
	            }
					 }
           else
           {
						 ItanTriWRITE(44, 0x80FF);
						 	for(i=96 ;i<112; i++)
							{
								ItanWRITE(i, 0x80FF);
							}
					 }
					 
					    ItanTriWRITE(32, 0xAAAA);
							ItanTriWRITE(33, 0x00FF);
							
							ItanSTIMULATE(1);	
							ItanWRITE(200,0x00);	
							ItanWRITE(200,0x00);
					 
					 TIM3_Delay10us(dd);
					 
					 Stimulatethorugh++;
					 
					 ItanTriWRITE(32, 0x0000);
					 ItanTriWRITE(33, 0x0000); */
					
		
		/*if(Stimulatethorugh>StimulateBurst)
		{
			//ND_OSMemPut(Mem_Table_One,(MEM_ADDR *)stm_mem,err);
			
		//	ND_OSMsgPut(Msg_Pool_Table,
                               //    (ND_OS_MSG *)msg,
                              //     (ND_OS_ERR *)err);			
			//INTERRUPT_ENABLE;
			
			return;
		}
	} */
}


void ND_ReadFlash(ND_OS_MSG const *msg,ND_DATA_ERR *data_err)
{
}

void ND_EleAndImpSend(ND_DATA_ERR *data_err)
{
}

void Charge_Init(void)
{
GPIO_InitTypeDef GPIO_InitStructure;
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); 
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; 
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_Init(GPIOC, &GPIO_InitStructure);
GPIO_SetBits (GPIOC, GPIO_Pin_11);
}

INT8U GetChargeState (void)
{
  INT8U byte =GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_10);
	
	return byte;
}

void Charge_start (void)
{
  GPIO_ResetBits (GPIOC, GPIO_Pin_11);
	
}

void Charge_stop (void)
{
  GPIO_SetBits(GPIOC, GPIO_Pin_11);
}


INT32U ItanCONVERT(INT8U ch)
{
	 INT32U rhdrxdat;
	 //u32 rhdtxdat = 0;
	 INT32U rhdtxdat = 0x00003000;
	 rhdtxdat += ch;
	 rhdtxdat <<= 16;
	 //rhdtxdat = (u32)((0x00<<24)+((ch)<<16)+0x00);
	 rhdrxdat = ItanSend(rhdtxdat);
	 return rhdrxdat;	
}

void ND_GetSample(ND_OS_ERR *err)
{
	   TIM4_Delay10us(100000/SAMPLING_FREQUENCY);
	
	   ItanCONVERT(0);
	   ItanWRITE(200,0x00);
	    
	    Mem_tail ++;
	
	   if(Mem_tail == End_buf)
			{
			  Mem_tail = Head_buf;
			}
	
			INT32U OREEG = ItanWRITE(200,0x00);
	   *Mem_tail = OREEG >> 16;
		//	PCOut = *Mem_tail;		
	
	    if(Mem_tail == Mem_head)
			{
			  Mem_head ++;
			}	   	    
			
			if(Mem_head == End_buf)
			{
			  Mem_head = Head_buf;
			}
						
}
INT32U Line = 0;
int GetSampleCtr = 0;
void ND_ErrCheck(ND_DATA_ERR *data_err)
{
	  if(GetSampleCtr < SAMPLING_FREQUENCY)
		{
		   GetSampleCtr ++;
			 
			 return;
		}
		
	  ND_OS_ERR err = NONE;		  
    int n = 0;
		GetSampleCtr = 0;
			
		while((Head_buf + n) != End_buf)
		{
			n++;
						
			Line += (int)(*(Head_buf + n));
		} 
			
		if(Line > 0x00800000)
		{
			MEM_ADDR *mem_addr = ND_OSMemGet(Mem_Table_One,&err);
				
			memcpy(mem_addr,&stm,sizeof(stm));
				
			ND_OS_MSG *msg = ND_OSMsgGet(Msg_Pool_Table,&err);
			  
			msg -> MsgPtr = mem_addr;
		  msg -> MsgSize = (NBRMSG)sizeof(stm);
				
			ND_OSTaskQPost(Stimulate_Q,
                    ND_OS_QUEUE_TYPE_STIMULATE,
                    (ND_OS_MSG *)msg,
              			            &err);
		} 
			
		Line = 0;
}

void ND_OSEnterSleepMode(void)
{
}

