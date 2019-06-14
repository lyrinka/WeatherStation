
#include <stm32f10x.h> 
#include "Drivers.h" 

void Drv_Init(void){ 
	Drv_PwrUp_Init(); 

	Drv_Beep_Init(); 
#ifndef BeepEN 
	Drv_Beep_Mute(); 
#endif 
	
	Drv_LED_Init(); 
	
	Drv_SgmDisp_Init(); 
	
	Drv_Key_Init(); 
	
}

void Drv_Rfsh(u32 Ctrl){ 
	if(Ctrl & Drv_Rfsh_SgmDisp) Drv_SgmDisp_Rfsh(); 
	if(Ctrl & Drv_Rfsh_Key) Drv_Key_Rfsh(); 
}

// End of file. 
