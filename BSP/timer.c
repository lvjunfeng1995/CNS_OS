#include "timer.h"
#include "stm32f10x.h"
//#include "led.h" 
#include "nd_os.h"
u16 TimeFlag=0;
u8 TriggerFlag = 0;
u32 shortFlag=0;
void TIM3_Int_Init()
{
    
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 

 TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	TIM_TimeBaseStructure.TIM_Period = 9; 
	TIM_TimeBaseStructure.TIM_Prescaler = 719;  
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 
	
	TIM_ITConfig( 
		TIM3, 
		TIM_IT_Update ,
		ENABLE 
		);
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);						 
} 

void TIM3_Delay10us(u32 times)
{
	TIM3->CR1 = TIM_CR1_CEN;
	
	while(times--)
	{
		while((TIM3->SR &TIM_FLAG_Update)== RESET);
		TIM3->SR = ~TIM_CR1_CEN;
	}
	
	TIM3->CR1 &=~ TIM_CR1_CEN;
}

void TIM3_STOP()
{
	
	TIM_SetCounter(TIM3,0);
	
  TIM_Cmd(TIM3,DISABLE);
}

void TIM3_Start()
{
  TIM3->CR1 |= TIM_CR1_CEN;
}

/*void TIM3_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 

	TIM_TimeBaseStructure.TIM_Period = arr; 
	TIM_TimeBaseStructure.TIM_Prescaler =psc;  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 
 
	TIM_ITConfig( 
		TIM3, 
		TIM_IT_Update ,
		ENABLE 
		);
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 

	TIM_Cmd(TIM3, ENABLE);
							 

}*/


void TIM3_IRQHandler(void) 
{
	 INTERRUPT_DISABLE;
		
		ND_OSIntNestingCtr++;
		
		INTERRUPT_ENABLE;
	
	ND_OS_ERR err;

	switch(Stimulate_state)
	{
	  case Stimulate_Start:
			
		  Stimulate_not_Ctr--;
		
		  if(Stimulate_not_Ctr == 0)
			{
			  ItanTriWRITE(44, 0x80FF);		
					 
			  ItanTriWRITE(32, 0xAAAA);
			  ItanTriWRITE(33, 0x00FF);
		
		    ItanSTIMULATE(1);
				
				Stimulate_state = Stimulate_Positive;
				Stimulate_100us_Ctr = Stimulate_100us_Cnt;
			}
		
			break;
		
		case Stimulate_Positive:
			
		  Stimulate_100us_Ctr--;
		
		  if(Stimulate_100us_Ctr == 0)
			{
			  ItanTriWRITE(44, 0x0000);		       	
							
	 	    ItanSTIMULATE(1);
				
				Stimulate_state = Stimulate_Negative;
				Stimulate_100us_Ctr = Stimulate_100us_Cnt;
			}
			
			break;
		
		case Stimulate_Negative:
			
		  Stimulate_100us_Ctr--;
		
		  if(Stimulate_100us_Ctr == 0)
			{
			  ItanTriWRITE(32, 0x0000);
			  ItanTriWRITE(33, 0x0000);
				
				Stimulate_state = Stimulate_End;
				Stimulate_not_Ctr = Stimulate_not_Cnt;
			}
			
			break;
		
		case Stimulate_End:
			
		  Stimulate_not_Ctr--;
		
		  if(Stimulate_not_Ctr == 0)
			{
			  Stimulatethorugh++;
				
				if(Stimulatethorugh>StimulateBurst)
		    {			
			    TIM3->CR1 &=~ TIM_CR1_CEN;
		    }
				
				Stimulate_state   = Stimulate_Start;
			  Stimulate_not_Ctr = Stimulate_not_Cnt;		
			}					
			
			break;
	}
		  
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) 
		{
			TIM_ClearITPendingBit(TIM3, TIM_IT_Update  ); 
		}
		
		INTERRUPT_DISABLE;
		 
		  ND_OSExitInt(&err);
		
		ND_OSIntNestingCtr--;
		
		INTERRUPT_ENABLE;

}



void TIM4_Int_Init()
{
    
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); 

 TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	TIM_TimeBaseStructure.TIM_Period = 9; 
	TIM_TimeBaseStructure.TIM_Prescaler = 71;  
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); 
	
	/*TIM_ITConfig( 
		TIM3, 
		TIM_IT_Update ,
		ENABLE 
		);
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);  */
							 
} 

void TIM4_Delay10us(u32 times)
{
	TIM4->CR1 = TIM_CR1_CEN;
	
	while(times--)
	{
		while((TIM4->SR &TIM_FLAG_Update)== RESET);
		TIM4->SR = ~TIM_CR1_CEN;
	}
	
	TIM4->CR1 &=~ TIM_CR1_CEN;
}










