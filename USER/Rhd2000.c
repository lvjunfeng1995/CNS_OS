#include "stm32f10x.h"
#include "stm32f10x_spi.h"				
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "spi.h"
#include "stm32f10x.h"
#include "Rhd2000.h"

/*void delays(int tck)
{
   while(tck -- > 1)
	 {
	    IWDG_ReloadCounter();
	 }
}*/






u32 ItanCalibrate(void)
{
	 u32 rhdrxdat=0;
	 u32 rhdtxdat=0;
		
	 rhdtxdat = 0x55000000;
   rhdrxdat = ItanSend(rhdtxdat);
	
	 return rhdrxdat;
}




u32 ItanREAD(u32 reg)
{
	
	 u32 rhdrxdat=0;
	 u32 rhdtxdat=0;
	 rhdtxdat = (u32)((0xc0<<24)|((reg)<<16)|0x00);
	 rhdrxdat = ItanSend(rhdtxdat);
	 return rhdrxdat;

}



u32 ItanWirteRead(u8 reg, u16 dat)
{
	
	 u32 rhdrxdat;
	 u32 rhdtxdat;
	
	 rhdtxdat = (u32)((0x60<<16)+((reg)<<16)+dat);
	 rhdrxdat = ItanSend(rhdtxdat);
	/*
   rhdrxdat = SPI1_ReadWriteByte((u16)(rhdtxdat&0x0000ffff));
	 rhdrxdat	<< 16;
	 rhdrxdat |= SPI1_ReadWriteByte((u16)((rhdtxdat&0xffff0000)>>16));*/
		 //delays(129);		
	 //rhdrxdat = SPI1_ReadWriteByte(rhdtxdat);
	 //GPIO_WriteBit(GPIOA, GPIO_Pin_4, 1);	
	
	 return rhdrxdat;

}

