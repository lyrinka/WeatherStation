#include <stm32f10x.h> 
#include <SPI.h> 

/* SPI1 Facts: 
	Pinouts: 
		PA4 CS   IOPP  2M = 0x2+1 
		PA5 SCK  AFPP 50M = 0xB+1 
		PA6 MISO InPu     = 0x8+1 
		PA7 MOSI AFPP 50M = 0xB+1 
	DMA Peripheral: 
		RXDMAREQ CH2 
		TXDMAREQ CH3 
		ISR/IFCR: TEIF HTIF TCIF GIF 
		CCRx = 0x1081 | rdmem:1<<4 | tcie:1<<1 (normal priority) 
		CCR2 = 0x1081 | 0<<4 | 1<<1 
		CCR3 = 0x1081 | 1<<4 | 0<<1 
		IFCR = 0x1 << 4 | 0x1 << 8 
		DMA1_Channel2_IRQHandler 
	SPI Peripheral: 
		CR1 = 0x344 | br:3<<3 | mode:2 	(master) 
		CR2 = 0x3 											(DMA on) 
		SR: BSY(7) OVR(6) TXE(1) RXNE(0) 
		DR <=> Data 
	NVIC: 
		preempt = 1; sub = 0; 
		NVIC_SetPriority(DMA1_Channel2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), preempt, sub)); 
		NVIC_EnableIRQ(DMA1_Channel2_IRQn); 
*/
#define NULL ((void *)0) 

void (*SPI_AsyncExch_cbFunc_ptr)(void) = NULL; 

void SPI_InitIO(void){ 
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; 
	GPIOA->BSRR = 0x00F0; 
	GPIOA->CRL = (GPIOA->CRL & 0x0000FFFF) | 0xB8B20000; 
}

void SPI_CSel(void){ 
	GPIOA->BRR = 0x10; // Pin 4 
}

void SPI_CDis(void){ 
	GPIOA->BSRR = 0x10; // Pin 4 
}

void SPI_Init(int mode, int baud){ 
	SPI_AsyncExch_cbFunc_ptr = NULL; 
	
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; 
	SPI1->CR1 = 0x0; 
	SPI1->CR2 = 0x3; 
	SPI1->CR1 = 0x344 | (baud & 0x7) << 3 | (mode & 0x3); 
	
	RCC->AHBENR |= RCC_AHBENR_DMA1EN; 
	DMA1->IFCR = 0x0000110; 
	
	DMA1_Channel2->CCR  = 0x0; 
	DMA1_Channel2->CPAR = (u32)&SPI1->DR; 
	
	DMA1_Channel3->CCR  = 0x0; 
	DMA1_Channel3->CPAR = (u32)&SPI1->DR; 
	
	DMA1_Channel2->CCR  = 0x1080 | 0x02; // RX 
	DMA1_Channel3->CCR  = 0x1080 | 0x10; // TX 
	
	const int preempt = 1, sub = 0; 
	NVIC_SetPriority(DMA1_Channel2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), preempt, sub)); 
	NVIC_EnableIRQ(DMA1_Channel2_IRQn); 
}

int SPI_AsyncExch(char * dat, int len){ 
	if(DMA1_Channel2->CNDTR != 0) return -1; 
	DMA1_Channel2->CMAR  = (u32)dat; 
	DMA1_Channel2->CNDTR =      len; 
	DMA1_Channel3->CMAR  = (u32)dat; 
	DMA1_Channel3->CNDTR =      len; 
	
	DMA1_Channel2->CCR  |= 0x1; // Start RX DMA 
	DMA1_Channel3->CCR  |= 0x1; // Start TX DMA 
	
	return 0; 
}

void SPI_AsyncExch_cbFuncReg(void (*cbFunc)(void)){ 
	SPI_AsyncExch_cbFunc_ptr = cbFunc; 
}

void DMA1_Channel2_IRQHandler(void){ 
	while((SPI1->SR & SPI_SR_TXE) == 0); 
	while((SPI1->SR & SPI_SR_BSY) != 0); 
	
	DMA1_Channel2->CCR &= ~0x1; // Stop RX DMA 
	DMA1_Channel3->CCR &= ~0x1; // Stop TX DMA 
	DMA1->IFCR = 0x0000110; 
	
	if(SPI_AsyncExch_cbFunc_ptr != NULL) SPI_AsyncExch_cbFunc_ptr(); 
}

// End of file. 
