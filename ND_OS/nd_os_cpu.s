

 


;*********************************************************************************************************
;                                          PUBLIC FUNCTIONS
;********************************************************************************************************


 IMPORT  ND_TCBHighRdyPtr
 IMPORT  ND_TCBCurPtr
 IMPORT  OS_CPU_ExceptStkBase
	
 EXPORT  ND_OSCtxSw
 EXPORT  ND_OSIntCtxSw
 EXPORT  ND_OSStartHighRdy
 EXPORT  PendSV_Handler

;********************************************************************************************************
;                           CONSTANTS USED TO ACCESS TASK CONTEXT STACK
;********************************************************************************************************


NVIC_INT_CTRL   EQU     0xE000ED04                              ; Interrupt control state register.
NVIC_SYSPRI14   EQU     0xE000ED22                              ; System priority register (priority 14).
NVIC_PENDSV_PRI EQU     0xFF                                    ; PendSV priority value (lowest).
NVIC_PENDSVSET  EQU     0x10000000                              ; Value to trigger PendSV exception.


;*******************************************************************************************************
;                                    CODE GENERATION DIRECTIVES
;*******************************************************************************************************

    AREA |.text|, CODE, READONLY, ALIGN=2
    THUMB
    REQUIRE8
    PRESERVE8


;********************************************************************************************************


ND_OSStartHighRdy
    LDR     R0, =NVIC_SYSPRI14                                  ; Set the PendSV exception priority
    LDR     R1, =NVIC_PENDSV_PRI
    STRB    R1, [R0]

    MOVS    R0, #0                                              ; Set the PSP to 0 for initial context switch call
    MSR     PSP, R0

    LDR     R0, =OS_CPU_ExceptStkBase                           ; Initialize the MSP to the OS_CPU_ExceptStkBase
    LDR     R1, [R0]
    MSR     MSP, R1	                                             

    LDR     R0, =NVIC_INT_CTRL                                  ; Trigger the PendSV exception (causes context switch)
    LDR     R1, =NVIC_PENDSVSET
    STR     R1, [R0]
    
    CPSIE   I                                                   ; Enable interrupts at processor level

OSStartHang
    B       OSStartHang                                         ; Should never get here
	
ND_OSCtxSw
    LDR     R0, =NVIC_INT_CTRL                                  ; Trigger the PendSV exception (causes context switch)
    LDR     R1, =NVIC_PENDSVSET
    STR     R1, [R0]
		
    BX      LR
	
	
ND_OSIntCtxSw
    LDR     R0, =NVIC_INT_CTRL                                  ; Trigger the PendSV exception (causes context switch)
    LDR     R1, =NVIC_PENDSVSET
    STR     R1, [R0]
	
    BX      LR

PendSV_Handler
    CPSID   I                                                   ; Prevent interruption during context switch
    MRS     R0, PSP                                             ; PSP is process stack pointer
    CBZ     R0, OS_CPU_PendSVHandler_nosave                     ; Skip register save the first time

    SUBS    R0, R0, #0x20                                       ; Save remaining regs r4-11 on process stack
    STM     R0, {R4-R11}

    LDR     R1, =ND_TCBCurPtr                                    ; ND_OSTCBCurPtr->OSTCBStkPtr = SP;
    LDR     R1, [R1]
    STR     R0, [R1]                                            ; R0 is SP of process being switched out

                                                                ; At this point, entire context of process has been saved
OS_CPU_PendSVHandler_nosave 
    LDR     R0, =ND_TCBCurPtr                                    ; OSTCBCurPtr = OSTCBHighRdyPtr;
    LDR     R1, =ND_TCBHighRdyPtr
    LDR     R2, [R1]
    STR     R2, [R0]
	MOV     R0, #0
	STR     R0, [R1]
	MOV     R1, #0

    LDR     R0, [R2]                                            ; R0 is new process SP; SP = OSTCBHighRdyPtr->StkPtr;
    LDM     R0, {R4-R11}                                        ; Restore r4-11 from new process stack
    ADDS    R0, R0, #0x20
    MSR     PSP, R0                                             ; Load PSP with new process SP
    ORR     LR, LR, #0x04                                       ; Ensure exception  return uses process stack
    CPSIE   I
    BX      LR                                                  ; Exception return will restore remaining context
    
	ALIGN
		
	END




