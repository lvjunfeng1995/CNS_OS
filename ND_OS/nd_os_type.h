/* ************************************************************************** */
/** Descriptive File Name

  @Company
    NewDoon

  @File Name
    nd_os_type.h

  @Description
    Contains all value type
 */
/* ************************************************************************** */

#ifndef _ND_OS_TYPE_H    /* Guard against multiple inclusion */
#define _ND_OS_TYPE_H 

#ifdef __cplusplus
extern "C" {
#endif
    
    typedef     unsigned char        INT8U;
    typedef     unsigned short       INT16U;
    typedef     unsigned int         INT32U;
    typedef     char                 CHAR;
    
    typedef     void(*TASK_PTR)      (void);
    typedef     void                 ARG;
    typedef     unsigned int         STACK_PTR; 
    typedef     unsigned short       STACK_SIZE;
    typedef     unsigned char        TIME_CNT;
    
    typedef     unsigned short       MSG_SIZE;
    typedef     unsigned int         MEM_ADDR;
    
    typedef     unsigned char        NBRMSG;
    
    typedef     unsigned char        NBRCNT;
    
    typedef     unsigned short       MEMSIZE;
    typedef     unsigned short       NBRMEM;
    
    typedef     unsigned char        NBRRDY;
    
    typedef     unsigned int        NBRINT;
		
		typedef     unsigned char        EXCEPT_ADDR;
		
		typedef     unsigned short      EEG_ADDR;
    

#ifdef __cplusplus
}
#endif

#endif 
/* *****************************************************************************
 End of File
 */
