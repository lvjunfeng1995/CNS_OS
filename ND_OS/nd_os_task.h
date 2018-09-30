/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.h

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

#ifndef _ND_OS_TASK_H    /* Guard against multiple inclusion */
#define _ND_OS_TASK_H


/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

    extern void DataCheckTask(void);
    
    extern void WriteFlashTask(void);
    
    extern void StimulateTask(void);
    
    extern void ReadFlashTask(void);
    
    extern void ManageErrTask(void);
    
    extern void EleAndImpTask(void);
    
    extern void ChargeTask(void);
    
    extern void NoticeEleTask(void);
    
    extern void SampleAndErrChkTask(void);
    
    extern void SleepModeTask(void);

    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _EXAMPLE_FILE_NAME_H */

/* *****************************************************************************
 End of File
 */
