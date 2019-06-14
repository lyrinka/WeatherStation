#include <stm32f10x.h> 
#include <UART.h> 

/* USART1 Facts: 
	Pinouts: 
		PA09 TXD AFPP 50M = 0xB+1 
		PA10 RXD InPu     = 0x8+1 
	USART Peripheral: 
		CR1 = 0x202B (RXNE Interrupts) 
		CR2 = 0x0 
		CR3 = 0x0 
		BRR = 0x271 (115200bps) 
		SR: TXE(7) TC(6) RXNE(5) 
	NVIC: 
		preempt = 1; sub = 0; 
		NVIC_SetPriority(USART1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), preempt, sub)); 
		NVIC_EnableIRQ(USART1_IRQn); 
*/

#define NULL ((void *)0) 
void (*UART_Putch_cbFunc_ptr)(void) = NULL; 
void (*UART_Getch_cbFunc_ptr)(char) = NULL; 

void UART_InitIO(void){ 
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; 
	GPIOA->BSRR = 0x0600; 
	GPIOA->CRH = (GPIOA->CRH & 0xFFFFF00F) | 0x000008B0; 
}

void UART_Init(u32 baud){ 
	UART_Putch_cbFunc_ptr = NULL; 
	UART_Putch_cbFunc_ptr = NULL; 
	
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN; 
	USART1->CR1 = 0x0; 
	USART1->CR2 = 0x0; 
	USART1->CR3 = 0x0; 
	USART1->BRR = baud; 
	USART1->CR1 = 0x202C; 
	
	const int preempt = 1, sub = 0; 
	NVIC_SetPriority(USART1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), preempt, sub)); 
	NVIC_EnableIRQ(USART1_IRQn); 
}
	

int UART_Putch(char ch){ 
	if((USART1->SR & 0x80) == 0){ // Busy 
		USART1->CR1 |= 0x80; // Enable TXE Interrupts 
		return -1; 
	}
	USART1->DR = ch; 
	return 0; 
}

void UART_Putch_cbFuncReg(void (*cbFunc)(void)){ 
	UART_Putch_cbFunc_ptr = cbFunc; 
}

void UART_Getch_cbFuncReg(void (*cbFunc)(char)){ 
	UART_Getch_cbFunc_ptr = cbFunc; 
}

void USART1_IRQHandler(void){ 
	if((USART1->CR1 & 0x80) && (USART1->SR & 0x80)){ // TXE 
		USART1->CR1 &= ~0x80; // Disable TXE Interrupt 
		if(UART_Putch_cbFunc_ptr != NULL) UART_Putch_cbFunc_ptr(); 
	}
	if(USART1->SR & 0x20){ // RXNE 
		char ch = USART1->DR; 
		if(UART_Getch_cbFunc_ptr != NULL) UART_Getch_cbFunc_ptr(ch); 
	}
}

// End of file. 
