
#include <stm32f10x.h> 
#include "Drv_SgmDisp.h" 

u8 Drv_SgmDisp_Data[8]; 
const u8 Drv_SgmDisp_Tabl[16] = {	0xc0, 0xf9, 0xa4, 0xb0, \
																	0x99, 0x92, 0x82, 0xf8, \
																	0x80, 0x90, 0x88, 0x83, \
																	0xc6, 0xa1, 0x86, 0x8e  \
}; 
int Drv_SgmDisp_DispIdx; 

void Drv_SgmDisp_Init(void){ 
#ifdef NPWR 
	int __IE = __get_PRIMASK(); 
	__disable_irq(); 
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_AFIOEN; 
	AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE; 
	DBGMCU->CR &= ~DBGMCU_CR_TRACE_IOEN; 
	GPIOB->CRL &= 0xFF000FFF; 
	GPIOB->CRL |= 0x00313000; 
	GPIOC->CRH &= 0xFFF000FF; 
	GPIOC->CRH |= 0x00022200; 
	__set_PRIMASK(__IE); 
#endif 
	GPIOB->BRR = 7 << 3; 
	GPIOC->BRR = 7 << 10; 
	Drv_SgmDisp_DispIdx = 0; 
	for(int i = 0; i < 8; i++) Drv_SgmDisp_Data[i] = 0x0; 
	for(int i = 0; i < 8; i++) Drv_SgmDisp_Rfsh(); 
}

void Drv_SgmDisp_Rfsh(void){ 
	int x  = Drv_SgmDisp_DispIdx; 
	int d = Drv_SgmDisp_Data[x]; 
	x <<= 10; 
	int y = (~x)&0x1C00; 
	GPIOC->BSRR = y << 16 | x; // Set decoder 
	for(int j = 0; j < 8; j++){ 
		GPIOB->BSRR = d & 0x80 ? 0x00200008 : 0x00280000; 	// Set data, clear clock 
		GPIOB->BSRR = 0x0020; 															// Set clock; 
		d <<= 1; 
	}
	GPIOB->BSRR = 0x00200010; // Clear clock, set lock 
	GPIOB->BRR = 0x0010; 			// clear lock; 
	if(++Drv_SgmDisp_DispIdx >= 8) Drv_SgmDisp_DispIdx = 0; 
}

void Drv_SgmDisp_WriteRaw(int idx, u8 dat){ 
	if(idx < 0 || idx >= 8) return; 
	Drv_SgmDisp_Data[idx] = dat; 
}
u8 Drv_SgmDisp_Read(int idx){ 
	if(idx < 0 || idx >= 8) return 0x0; 
	return Drv_SgmDisp_Data[idx]; 
}
void Drv_SgmDisp_Clear(void){ 
	for(int i = 7; i >= 0; i--) Drv_SgmDisp_Data[i] = 0x00; 
}

void Drv_SgmDisp_Write(int idx, int dat){ 
	if(idx < 0 || idx >= 8 || dat < 0) return; 
	Drv_SgmDisp_Data[idx] = ~Drv_SgmDisp_Tabl[dat & 0xF]; 
}

void Drv_SgmDisp_WriteHex(u32 dat){ 
	for(int i = 7; i >= 0; i--){ 
		Drv_SgmDisp_Data[i] = ~Drv_SgmDisp_Tabl[dat & 0xF]; 
		dat >>= 4; 
	}
}

void Drv_SgmDisp_WriteDec(int dat){ 
	if(dat > 99999999 || dat < -9999999){ 
		for(int i = 7; i >= 0; i--) Drv_SgmDisp_Data[i] = 0x40; 
		return; 
	}
	int j = 0; 
	if(dat < 0){ 
		Drv_SgmDisp_Data[0] = 0x40; 
		j++; 
		dat = -dat; 
	}
	for(int i = 7; i >= j; i--){ 
		int q = dat / 10; 
		Drv_SgmDisp_Data[i] = ~Drv_SgmDisp_Tabl[(dat-q*10) & 0xF]; 
		dat = q; 
	}
}


// End of file. 
