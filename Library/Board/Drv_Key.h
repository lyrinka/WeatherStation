
#ifndef __Drv_Key_H__ 
#define __Drv_Key_H__ 

void Drv_Key_Init(void); 

void Drv_Key_Rfsh(void); 

int Drv_Key_Read(void); 
void Drv_Key_RecStart(void); 
u32 Drv_Key_RecGet(void); 

#endif 
