#ifndef  __TimeSet_H_
#define  __TimeSet_H_
#include "stm32f10x.h"	
void delay_init(void);
void TIM3_IRQHandler(void);
u16 SUM16(u16 *pBuffer, u16 length);
void delay_ms(u16 nms);
#endif
