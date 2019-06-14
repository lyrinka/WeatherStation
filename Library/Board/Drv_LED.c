
#include <stm32f10x.h> 
#include "Drv_LED.h" 

void Drv_LED_Init(void){ 
#ifdef NPWR 
	int __IE = __get_PRIMASK(); 
	__disable_irq(); 
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN; 
	GPIOC->CRL = 0x66666666; 
	GPIOC->BSRR = 0xFF; 
	__set_PRIMASK(__IE); 
#endif 
}

void Drv_LED_Write(u8 dat){ 
	GPIOC->BSRR = (u32)dat << 16 | (u32)~dat; 
}

u8 Drv_LED_Read(void){ 
	return (u32)(~GPIOC->ODR & 0xFF); 
}

void Drv_LED_Set(int idx){ 
	if(idx < 0 || idx >= 8) return; 
	GPIOC->BRR = 1 << idx; 
}

void Drv_LED_Clr(int idx){ 
	if(idx < 0 || idx >= 8) return; 
	GPIOC->BSRR = 1 << idx; 
}

void Drv_LED_Togl(int idx){ 
	if(idx < 0 || idx >= 8) return; 
	int __IE = __get_PRIMASK(); 
	__disable_irq(); 
	GPIOC->ODR ^= 1 << idx; 
	__set_PRIMASK(__IE); 
}

// End of file. 
