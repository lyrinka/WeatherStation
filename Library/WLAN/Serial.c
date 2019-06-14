#include <stm32f10x.h> 
#include <OS.h> 
#include <Serial.h> 
#include <UART.h> 

TASK Serial_Putch_cbTask = NULL; 
TASK Serial_Getch_cbTask = NULL; 
char Serial_recv; 

void Serial_Putch_cbFunc(void); 
void Serial_Getch_cbFunc(char); 

void Serial_Init(u32 baud){ 
	Serial_Putch_cbTask = NULL; 
	Serial_Getch_cbTask = NULL; 
	
	OS_Lock(); 
	UART_InitIO(); 
	UART_Init(baud); 
	UART_Putch_cbFuncReg(Serial_Putch_cbFunc); 
	UART_Getch_cbFuncReg(Serial_Getch_cbFunc); 
	OS_UnLock(); 
}

void Serial_Putch(char ch){ 
	Serial_Putch_cbTask = Lin_GetCurrTask(); 
	while(UART_Putch(ch)) OS_Suspend(); 
}

char Serial_Getch(void){ 
	OS_Lock(); 
	Serial_Getch_cbTask = Lin_GetCurrTask(); 
	OS_Suspend(); 
	Serial_Getch_cbTask = NULL; 
	return Serial_recv; 
}

void Serial_Putch_cbFunc(void){ 
	if(Serial_Putch_cbTask != NULL){ 
		OS_GenEvent(Serial_Putch_cbTask, 0); 
		OS_PreemptISR(); 
	}
}

void Serial_Getch_cbFunc(char ch){ 
	Serial_recv = ch; 
	if(Serial_Getch_cbTask != NULL){ 
		OS_GenEvent(Serial_Getch_cbTask, 0); 
		OS_PreemptISR(); 
	}
}


void Serial_CRLF(void){ 
	Serial_Putch(0x0D); 
	Serial_Putch(0x0A); 
}

void Serial_Puts(const char * str){ 
	int i = 0; 
	while(str[i] != 0) Serial_Putch(str[i++]); 
	Serial_CRLF(); 
}

void Serial_Print(const char * str, int len){ 
	if(len <= 0) return; 
	for(int i = 0; i < len; i++) Serial_Putch(str[i]); 
}

// End of file. 
