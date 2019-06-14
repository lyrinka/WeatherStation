
#ifndef __SPI_H__ 
#define __SPI_H__ 

void SPI_InitIO(void); 
void SPI_CSel(void); 
void SPI_CDis(void); 

void SPI_Init(int mode, int baud); 
int  SPI_AsyncExch(char * dat, int len); 
void SPI_AsyncExch_cbFuncReg(void (*cbFunc)(void)); 

#endif 
