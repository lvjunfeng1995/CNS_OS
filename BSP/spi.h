#ifndef __SPI_H
#define __SPI_H

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//Mini STM32������
//SPI ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2010/6/13 
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  

#include "stm32f10x.h"	
 				  	    													  
void SPI1_Init(void);			 //��ʼ��SPI��
void SPI1_SetSpeed(u8 SpeedSet); //����SPI�ٶ�   
u16 SPI1_ReadWriteByte(u16 TxData);//SPI���߶�дһ���ֽ�
uint16_t SPIWriteRead(uint16_t dat);		 
u16 SPI2_ReadWriteByte(u16 TxData);
#endif

