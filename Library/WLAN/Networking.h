
#ifndef __Networking_H__ 
#define __Networking_H__ 

void Net_Init(unsigned int baud); 

void Net_RxRply(void); 
void Net_RxRply_Reg(void); 
void Net_RxRply_UnReg(void); 

void Net_RxConx(int * id, int * dir); 
void Net_RxConx_Reg(void);
void Net_RxConx_UnReg(void); 

char * Net_RxData(int * id); 
void Net_RxData_Reg(void); 
void Net_RxData_UnReg(void); 

char * Net_Recv(int * id); 
void Net_Send(int id, const char * str); 

#include <Serial.h> 
#define Net_Putch Serial_Putch 
#define Net_Print Serial_Print 
#define Net_Puts  Serial_Puts 
#define Net_CRLF  Serial_CRLF 

#endif 
