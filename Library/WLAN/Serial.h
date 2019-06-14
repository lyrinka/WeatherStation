
#ifndef __Serial_H__ 
#define __Serial_H__ 

void Serial_Init(unsigned int baud); 
void Serial_Putch(char ch); 
char Serial_Getch(void); 

#define putch Serial_Putch 
#define getch Serial_Getch 

void Serial_CRLF(void); 
void Serial_Puts(const char * str); 
void Serial_Print(const char * str, int len); 

#endif 
