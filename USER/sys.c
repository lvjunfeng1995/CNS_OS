//���ж�
void INTX_DISABLE(void)
{		  
	__asm volatile("cpsid i");
}
//���ж�
void INTX_ENABLE(void)
{
	__asm volatile("cpsie i");		  
}