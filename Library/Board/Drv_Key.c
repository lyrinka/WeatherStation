
#include <stm32f10x.h> 
#include "Drv_Key.h" 

#define Filter 8 

u32 Drv_Key_Filter, Drv_Key_Fprev, Drv_Key_FilterRec; 
int Drv_Key_Fcount; 

void Drv_Key_Init(void){ 
#ifdef NPWR 
	int __IE = __get_PRIMASK(); 
	__disable_irq(); 
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPDEN; 
	GPIOA->CRL &= 0xFFFFFFF0; 
	GPIOA->CRL |= 0x00000008; 
	GPIOC->CRH &= 0xFFFFFF00; 
	GPIOC->CRH |= 0x00000088; 
	GPIOD->CRL &= 0xFFFFF0FF; 
	GPIOD->CRL |= 0x00000800; 
	GPIOA->BRR = 1; 
	GPIOC->BSRR = 0x3 << 8; 
	GPIOD->BSRR = 0x4; 
	__set_PRIMASK(__IE); 
#endif 
	Drv_Key_Filter = 0x0; 
	Drv_Key_FilterRec = 0x0; 
	Drv_Key_Fprev = 0x0; 
	Drv_Key_Fcount = 0; 
}

u32 Drv_Key_GetRaw(void){ 
	u32 s = 0x0; 
	int __IE = __get_PRIMASK(); 
	__disable_irq(); 
	s |= (GPIOA->IDR & 0x1) << 3; 
	s |= (~GPIOD->IDR & 0x4); 
	s |= ((~GPIOC->IDR >> 8) & 0x3); 
	__set_PRIMASK(__IE); 
	return s; 
}

void Drv_Key_Rfsh(void){ 
	u32 Curr = Drv_Key_GetRaw(); 
	if(Curr == Drv_Key_Fprev){ 
		Drv_Key_Fcount++; 
		if(Drv_Key_Fcount == Filter){ 
			Drv_Key_Filter = Curr; 
			int __IE = __get_PRIMASK(); 
			__disable_irq(); 
			Drv_Key_FilterRec |= Curr; 
			__set_PRIMASK(__IE); 
		}
	}
	else Drv_Key_Fcount = 0; 
	Drv_Key_Fprev = Curr; 
}

int Drv_Key_Read(void){ 
	return 31 - (int)__clz(Drv_Key_Filter); 
}

void Drv_Key_RecStart(void){ 
	Drv_Key_FilterRec = Drv_Key_Filter; 
}

u32 Drv_Key_RecGet(void){ 
	return Drv_Key_FilterRec; 
}

// End of file. 
