#include <stm32f10x.h> 
#include <OS.h> 
#include <Drv_PwrUp.h> 

void Svc_PwrUp_Init(void){ 
	OS_Lock(); 
	Drv_PwrUp_Init(); 
	OS_UnLock(); 
}

// End of file. 
