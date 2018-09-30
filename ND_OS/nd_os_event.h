/* ************************************************************************** */
/** Descriptive File Name

  @Company
    NewDoon

  @File Name
    nd_os_event.h

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

#ifndef _ND_OS_EVENT_H    /* Guard against multiple inclusion */
#define _ND_OS_EVENT_H

#include "nd_os.h"

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

    extern   void     UsartSend(INT8U * p_Send, MSG_SIZE len);
	
	  extern   INT32U   ItanTriWRITE(INT32U  reg, INT16U  dat);
	
    extern   INT32U   ItanWRITE(INT32U reg, INT16U dat);
	
	  extern   MSG_SIZE RestoreData(INT8U *mem,MSG_SIZE msg_size);
	
	  extern   void     ReplyConfirm(void);
	
	  extern   void     ReplyInf(ND_OS_REPLY reply,INT8U request);
	
	  extern   void     ND_DataErrCheck(ND_OS_MSG const *msg,ND_DATA_ERR  *data_err);
	
	  extern   void     ND_WriteFlash(ND_OS_MSG const *msg,ND_DATA_ERR *data_err);
	
	  extern   INT32U   ItanSTIMULATE(INT32U ch);
	
	  extern   void     ND_Stimulate(ND_OS_MSG const *msg,ND_DATA_ERR *data_err,ND_OS_ERR *err);
		
		extern   void     ND_ReadFlash(ND_OS_MSG const *msg,ND_DATA_ERR *data_err);
		
		extern   void     ND_EleAndImpSend(ND_DATA_ERR *data_err);
		
		extern   void     Charge_Init(void);
		
		extern   void     Charge_start (void);
		
		extern   void     Charge_stop (void);
		
		extern   INT8U    GetChargeState (void);
		
		extern   INT32U   ItanCONVERT(INT8U ch);
		
		extern   void     ND_ErrCheck(ND_DATA_ERR *data_err);
		
		extern   INT32U   ItanCLEAR(void);
		
		extern   void     ItanInit(void);
		
		extern   void     ND_OSEnterSleepMode(void);
		
		extern   void     ND_GetSample(ND_OS_ERR *err);

    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _EXAMPLE_FILE_NAME_H */

/* *****************************************************************************
 End of File
 */