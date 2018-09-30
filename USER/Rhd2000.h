#ifndef  __Rhd2000_H_
#define  __Rhd2000_H_

#include "stm32f10x.h"
#include <string.h>
//#include "userdef.h"
u32 ItanSend(u32 data);
u32 ItanCalibrate(void);
u32 ItanCLEAR(void);
u32 ItanWRITE(u32 reg, u16 dat);
u32 ItanREAD(u32 reg);
u32 ItanCONVERT(u8 ch);
u32 ItanWirteRead(u8 reg, u16 dat);
u32 ItanTriWRITE(u32 reg, u16 dat);
u32 ItanSTIMULATE(u32 ch);
#endif




