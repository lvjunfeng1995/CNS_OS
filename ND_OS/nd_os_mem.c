/* ************************************************************************** */
/** Descriptive File Name

  @Company
    NewDoon

  @File Name
    nd_os_mem.c

  @Description
    Management system memory.
 */
/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

#include"nd_os.h"
int put1_num = 0;
int get1_num = 0; 
/* ************************************************************************** */
/* ************************************************************************** */
// Section: Local Functions                                                   */
/* ************************************************************************** */
/* ************************************************************************** */

/* ************************************************************************** */ 
/** 
  @Function
    void ND_OSMemCreate(          ND_OS_MEM const     *mem,
                                  CHAR                *name,
                                  MEM_ADDR            *addr,
                                  MEMSIZE              blk_size,
                                  NBRMEM               nbr_size,
                                  ND_OS_ERR           *err);

  @Description
    Initialize a new manage memory table;

  @Parameters
    @ *mem   Point to the manage memory table; 

    @ *name  Initialize name of the manage memory table;
 
    @ *addr  Point to the first memory block
 
    @ blk_size  Block size per memory blocks
 
    @ nbr_size  Number of memory blacks;
    
    @ *err  Initialization error type;

  @Returns
     void

  @Remarks
     Create the table before the system runs;

 */

void ND_OSMemCreate(            ND_OS_MEM  * const   mem,
                                CHAR                *name,
                                MEM_ADDR            *addr,
                                MEMSIZE              blk_size,
                                NBRMEM               nbr_size,
                                ND_OS_ERR           *err)
{
    if(NULL == name)
    {
        *err = NAME_NULL;
        
        return;
    }
    
    if(NULL == addr)
    {
        *err = MEM_ADDR_NULL;
        
        return;
    }
    
    if(blk_size < (MEMSIZE)4 && blk_size%((MEMSIZE)4) != 0)                                        /*Each memory block holds a 32-bit pointer to the next memory block and 4 byte alignment*/
    {
        *err = OUT_OF_MEMORY;                                                   
        
        return;
    }
    
    if(nbr_size < 1)
    {
        *err = NUMBER_FEW;
        
        return;
    }
    
    INT32U i = 1;
		INT32U add_size = blk_size/4;
    
    for(;i < nbr_size;i++)
    {
        *(addr+(i-1)*add_size) = (MEM_ADDR)(addr +(i * add_size));                          
    }
    
    mem -> FreeListPtr = addr;   
    mem -> NamePtr = name; 
    mem -> BlkSize= blk_size;    
    mem -> NbrMax = nbr_size;  
    mem -> NbrFree = nbr_size;
    
    *err = NONE;
}

/* ************************************************************************** */ 
/** 
  @Function
    MEM_ADDR  *ND_OSMemGet(          ND_OS_MEM const   *mem,
                                     ND_OS_ERR         *err);

  @Description
    Get memory block from manage memory table;

  @Parameters
    @ *mem   Point to the manage memory table; 
    
    @ *err  Get error type;

  @Returns
     Returns an empty block of memory;

  @Remarks
     The requested table must exist;

 */

MEM_ADDR  *ND_OSMemGet(           ND_OS_MEM  * const  mem,
                                  ND_OS_ERR          *err)
{
    MEM_ADDR *mem_get = NULL;
    get1_num ++;
    if(NULL == mem)
    {
        *err = MEM_NULL;
                
        return mem_get;
    }
    
    INTERRUPT_DISABLE;
    
    NBRMEM free_num = mem -> NbrFree;
    
    if(free_num == 0)
    {
        *err = OUT_OF_MEMORY;
        
        INTERRUPT_ENABLE;
        
        return mem_get;
    }
    
    mem_get = mem -> FreeListPtr;                                               /*Get the first memory block*/
    
    mem -> FreeListPtr = (MEM_ADDR *)(*mem_get);                                /*Pointer to the next memory block*/
    *mem_get = 0;
    
    mem -> NbrFree--;
    
    *err = NONE;
    
    INTERRUPT_ENABLE;
            
    return mem_get;   
}

/* ************************************************************************** */ 
/** 
  @Function
    void ND_OSMemPut(          ND_OS_MEM const    *mem,
                               MEM_ADDR           *addr,
                               ND_OS_ERR          *err);

  @Description
    Return memory block to manage memory table;

  @Parameters
    @ *mem   Point to the manage memory table; 
    
    @ *addr  Point to returned memory block;
  
    @ *err  Put error type;

  @Returns
     void;

  @Remarks
     The requested table must exist;

 */

void ND_OSMemPut(           ND_OS_MEM  * const  mem,
                            MEM_ADDR           *addr,
                            ND_OS_ERR          *err)
{
    if(NULL == mem)
    {
        *err = MEM_NULL;
        
        return;
    }
    put1_num++;
    if(NULL == addr)
    {
        *err = MEM_ADDR_NULL;
        
        return;
    }
    
    INTERRUPT_DISABLE;
     
    NBRMEM free_num = mem -> NbrFree;
    NBRMEM max_num = mem -> NbrMax;
    
    if(free_num > (max_num-1))                                                  /*Prevent memory block number overflow*/
    {
        mem -> NbrFree = max_num;
        
        *err = MEM_BLK_FULL;
        
        INTERRUPT_ENABLE;
                
        return;
    }
    
    *addr = (INT32U)(mem -> FreeListPtr);                                       /*Put a memory block into manage memory table*/
    mem -> FreeListPtr = addr;
    
    mem -> NbrFree++;
    
    *err = NONE;
    
    INTERRUPT_ENABLE;
}
/* *****************************************************************************
 End of File
 */
