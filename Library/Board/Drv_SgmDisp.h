
#ifndef __Drv_SgmDisp_H__ 
#define __Drv_SgmDisp_H__ 

void Drv_SgmDisp_Init(void); 

void Drv_SgmDisp_Rfsh(void); 

void Drv_SgmDisp_WriteRaw(int idx, u8 dat); 
//u8 Drv_SgmDisp_Read(int idx); 
void Drv_SgmDisp_Clear(void); 

void Drv_SgmDisp_Write(int idx, int dat); 
void Drv_SgmDisp_WriteHex(u32 dat); 
void Drv_SgmDisp_WriteDec(int dat); 


#endif 
