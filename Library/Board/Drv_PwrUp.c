
#include <stm32f10x.h> 
#include "Drv_PwrUp.h" 

enum PortMode{ 
		Analog 		= 0x0, 
	outPP10M 		= 0x1, 
	outPP2M 		= 0x2, 
	outPP30M 		= 0x3, 
		Floating 	= 0x4, 
	outOD10M 		= 0x5, 
	outOD2M 		= 0x6, 
	outOD30M 		= 0x7, 
		PuPd 			= 0x8, 
	AFPP10M 		= 0x9, 
	AFPP2M 			= 0xA, 
	AFPP30M 		= 0xB,
		
	AFOD10M	 		= 0xD, 
	AFOD2M 			= 0xE, 
	AFOD30M 		= 0xF 
}; 

#define PA0  PuPd 		// * KeyUp 		- WKUP 
#define PA1  outPP10M 
//#define PA1  PuPd 		// * 
#define PA2  PuPd 		// * 
#define PA3  PuPd 		// * 
#define PA4  PuPd 		// * 
#define PA5  PuPd 		// * 
#define PA6  PuPd 		// * 
#define PA7  PuPd 		// * 
#define PA8  PuPd 		// * 
#define PA9  AFPP2M 	// * UART_Tx 	- UART1 
#define PA10 PuPd 		// * UART_Rx 	- UART1 
#define PA11 Analog 	//   USB_DM 	- USB 
#define PA12 Analog 	//   USB_DP 	- USB 
#define PA13 PuPd 		//   SWD 			- DEBUG 
#define PA14 PuPd 		//   SWC 			- DEBUG 
#define PA15 PuPd 		// * 

#define PB0  PuPd 		//   IrDA 		- TIM3_3 
#define PB1  Analog 	// * ADC_in		- ADC_9 
#define PB2  PuPd 		// * 					- BOOT1 
#define PB3  outPP30M //   SgmDat 
#define PB4  outPP10M //   SgmLck 
#define PB5  outPP30M //   SgmClk 
#define PB6  AFOD2M 	//   EESCL 		- I2C1 
#define PB7  AFOD2M 	//   EESDA 		- I2C1 
#define PB8  outOD2M 	//   Beeper 
#define PB9  outOD2M 	// * 1Wire 		- TIM4_4 
#define PB10 PuPd 		// * 					- UART3, I2C2 
#define PB11 PuPd 		// * 					- UART3, I2C2 
#define PB12 AFPP2M 	//   FlaCS 		- SPI2 
#define PB13 AFPP30M	//   FlaSCK 	- SPI2 
#define PB14 PuPd 		//   FlaMISO 	- SPI2 
#define PB15 AFPP30M 	//   FlaMOSI 	- SPI2 

#define PC0  outOD2M 	//   DS0 
#define PC1  outOD2M 	//   DS1 
#define PC2  outOD2M 	//   DS2 
#define PC3  outOD2M 	//   DS3 
#define PC4  outOD2M 	//   DS4 
#define PC5  outOD2M 	//   DS5 
#define PC6  outOD2M 	//   DS6 
#define PC7  outOD2M 	//   DS7 
#define PC8  PuPd 		// * Key0 
#define PC9  PuPd 		// * Key1 
#define PC10 outPP2M 	//   SgmA0 
#define PC11 outPP2M 	//   SgmA1 
#define PC12 outPP2M 	//   SgmA2 
#define PC13 outPP2M 	// * 
#define PC14 Analog 	//   HSE 
#define PC15 Analog 	//   HSE 

#define PD0  Analog 	//   LSE 
#define PD1  Analog 	//   LSE 
#define PD2  PuPd 		// * Key2 
#define PD3  Floating 
#define PD4  Floating 
#define PD5  Floating 
#define PD6  Floating 
#define PD7  Floating 
#define PD8  Floating 
#define PD9  Floating 
#define PD10 Floating 
#define PD11 Floating 
#define PD12 Floating 
#define PD13 Floating 
#define PD14 Floating 
#define PD15 Floating 

#define PAres 1 	// KeyUp is Pull-Down 
#define PBres 0 	
#define PCres 0 	
#define PDres 0 	

void Drv_PwrUp_Init(){ 
	int __IE = __get_PRIMASK(); 
	__disable_irq(); 
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPDEN | RCC_APB2ENR_AFIOEN | RCC_APB2ENR_AFIOEN; 
	RCC->APB2RSTR = RCC_APB2RSTR_AFIORST; 
	RCC->APB2RSTR &= ~RCC_APB2RSTR_AFIORST; 
	AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE; 
	DBGMCU->CR &= ~DBGMCU_CR_TRACE_IOEN; 
	GPIOA->ODR = 0xFFFF; 
	GPIOB->ODR = 0xFFFF; 
	GPIOC->ODR = 0xFFFF; 
	GPIOD->ODR = 0xFFFF; 
	GPIOA->CRL = PA0 | PA1<<4 | PA2 <<8 | PA3 <<12 | PA4 <<16 | PA5 <<20 | PA6 <<24 | (u32)PA7 <<28; 
	GPIOA->CRH = PA8 | PA9<<4 | PA10<<8 | PA11<<12 | PA12<<16 | PA13<<20 | PA14<<24 | (u32)PA15<<28; 
	GPIOB->CRL = PB0 | PB1<<4 | PB2 <<8 | PB3 <<12 | PB4 <<16 | PB5 <<20 | PB6 <<24 | (u32)PB7 <<28;  
	GPIOB->CRH = PB8 | PB9<<4 | PB10<<8 | PB11<<12 | PB12<<16 | PB13<<20 | PB14<<24 | (u32)PB15<<28; 
	GPIOC->CRL = PC0 | PC1<<4 | PC2 <<8 | PC3 <<12 | PC4 <<16 | PC5 <<20 | PC6 <<24 | (u32)PC7 <<28; 
	GPIOC->CRH = PC8 | PC9<<4 | PC10<<8 | PC11<<12 | PC12<<16 | PC13<<20 | PC14<<24 | (u32)PC15<<28; 
	GPIOD->CRL = PD0 | PD1<<4 | PD2 <<8 | PD3 <<12 | PD4 <<16 | PD5 <<20 | PD6 <<24 | (u32)PD7 <<28; 
	GPIOD->CRH = PD8 | PD9<<4 | PD10<<8 | PD11<<12 | PD12<<16 | PD13<<20 | PD14<<24 | (u32)PD15<<28; 
	GPIOA->BRR = PAres; 
	GPIOB->BRR = PBres; 
	GPIOC->BRR = PCres; 
	GPIOD->BRR = PDres; 
	__set_PRIMASK(__IE); 
}

// End of file. 
