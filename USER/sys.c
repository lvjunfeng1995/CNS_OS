//关中断
void INTX_DISABLE(void)
{		  
	__asm volatile("cpsid i");
}
//开中断
void INTX_ENABLE(void)
{
	__asm volatile("cpsie i");		  
}