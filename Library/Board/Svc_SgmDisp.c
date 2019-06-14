#include <stm32f10x.h> 
#include <OS.h> 
#include <Drv_SgmDisp.h> 

void Svc_SgmDisp_Worker(TASK Self, int rate); 

void Svc_SgmDisp_Init(int rate){ 
	TASK Task; 
	Task = OS_New(512, Svc_SgmDisp_Worker); 
	Task->Arg0 = rate; 
	OS_ChgPri(Task, -1); 
	OS_GenEvent(Task, 0); 
	OS_Preempt(); 
}

void Svc_SgmDisp_Worker(TASK Self, int rate){ 
	OS_Lock(); 
	Drv_SgmDisp_Init(); 
	OS_UnLock(); 
	OS_TBGperiod(rate); 
	for(;;){ 
		OS_Lock(); 
		Drv_SgmDisp_Rfsh(); 
		OS_UnLock(); 
		OS_Suspend(); 
	}
}

void Svc_SgmDisp_Write(int dat){ 
	OS_Lock(); 
	Drv_SgmDisp_WriteDec(dat); 
	OS_UnLock(); 
}

// End of file. 
