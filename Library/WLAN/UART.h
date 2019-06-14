
#ifndef __UART_H__ 
#define __UART_H__ 

void UART_InitIO(void); 
void UART_Init(unsigned int baud); 
int  UART_Putch(char ch); 
void UART_Putch_cbFuncReg(void (*cbFunc)(void)); 
void UART_Getch_cbFuncReg(void (*cbFunc)(char)); 

#endif
