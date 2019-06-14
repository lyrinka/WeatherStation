#include <stm32f10x.h> 
#include <OS.h> 
#include <string.h> 
#include <Serial.h> 
#include <Networking_RxStateMachine.h> 

#include <Networking.h> 


TASK Net_RxAck_cbTask = NULL; 
TASK Net_RxCon_cbTask = NULL; 
TASK Net_RxDat_cbTask = NULL; 

void Net_Init(u32 baud){ 
	Serial_Init(baud); 
	Net_RxStateMachine_Init(); 
	
	Net_RxRply_Reg(); 
	Net_RxRply(); // "ready" 
	
	Serial_Puts("ATE0"); 
	Net_RxRply(); // "ATE0" 
	Net_RxRply(); // "OK" 
	
	Serial_Puts("AT+CIPMUX=1"); 
	Net_RxRply(); // "OK" 
	
	Serial_Puts("AT+CIPSERVER=1,333"); 
	Net_RxRply(); // "OK" 
	
	Net_RxRply_UnReg(); 
}

void Net_RxRply_Reg(void){ 
	Net_RxAck_cbTask = Lin_GetCurrTask(); 
}
void Net_RxRply(void){ 
	if(Lin_MsgQty() == 0) OS_Suspend(); 
	Lin_MsgRecv(); 
}
void Net_RxRply_UnReg(void){ 
	Net_RxAck_cbTask = NULL; 
}

void Net_RxConx_Reg(void){ 
	Net_RxCon_cbTask = Lin_GetCurrTask(); 
}
void Net_RxConx(int * id, int * dir){ 
	if(Lin_MsgQty() == 0) OS_Suspend(); 
	MSG Msg = Lin_MsgRecv(); 
	*id = Msg.Src; 
	*dir = Msg.Cmd; 
}
void Net_RxConx_UnReg(void){ 
	Net_RxCon_cbTask = NULL; 
}

void Net_RxData_Reg(void){ 
	Net_RxDat_cbTask = Lin_GetCurrTask(); 
}
char * Net_RxData(int * id){ 
	if(Lin_MsgQty() == 0) OS_Suspend(); 
	MSG Msg = Lin_MsgRecv(); 
	*id = Msg.Src; 
	return Msg.Pld; 
}
void Net_RxData_UnReg(void){ 
	Net_RxDat_cbTask = NULL; 
}

char * Net_Recv(int * id){ 
	Net_RxData_Reg(); 
	char * str; 
	str = Net_RxData(id); 
	Net_RxData_UnReg(); 
	return str; 
}

void Net_Send(int id, const char * str){ 
	Net_RxRply_Reg(); 
	
	Net_Print("AT+CIPSENDEX=", 13); 
	Net_Putch(id + '0'); 
	Net_Puts(",1024"); 
	Net_RxRply(); 
	Net_RxRply(); 
	
	Net_Puts(str); 
	Net_Print("\\0", 2); 
	Net_RxRply(); 
	Net_RxRply(); 
	
	Net_RxRply_UnReg(); 
}

void Net_RxRply_cbFunc(char * str){ 
	int code = 0, aux; 
				if(String_Cmp(str, "WIFI", 4) == 0){ 
					code = -1; 
	}else if(String_Cmp(str + 1, ",CONNECT", 8) == 0){ 
					code = 1; 
					aux = str[0] - '0'; 
	}else if(String_Cmp(str + 1, ",CLOSED",  7) == 0){ 
					code = 2; 
					aux = str[0] - '0'; 
	}
	Lin_MemFree(str); 
	if(code < 0) return; 
	MSG Msg; 
	switch(code){ 
		case 0: { 
			if(Net_RxAck_cbTask != NULL){
				Lin_MsgPut(Net_RxAck_cbTask, Msg); 
				OS_GenEvent(Net_RxAck_cbTask, 0); 
			}
			break; 
		}
		case 1: { 
			if(Net_RxCon_cbTask != NULL){ 
				Msg.Cmd = 0; 
				Msg.Src = aux; 
				Lin_MsgPut(Net_RxCon_cbTask, Msg); 
				OS_GenEvent(Net_RxCon_cbTask, 0); 
			}
			break; 
		}
		case 2: { 
			if(Net_RxCon_cbTask != NULL){ 
				Msg.Cmd = 1; 
				Msg.Src = aux; 
				Lin_MsgPut(Net_RxCon_cbTask, Msg); 
				OS_GenEvent(Net_RxCon_cbTask, 0); 
			}
			break; 
		}
	}
}

void Net_RxData_cbFunc(int id, char * str){ 
	if(Net_RxDat_cbTask == NULL || str == NULL){ 
		Lin_MemFree(str); 
		return; 
	}
	MSG Msg; 
	Msg.Src = id; 
	Msg.Pld = str; 
	Lin_MsgPut(Net_RxDat_cbTask, Msg); 
	OS_GenEvent(Net_RxDat_cbTask, 0); 
}

// End of file. 
