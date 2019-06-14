
#ifndef __Drv_LED_H__ 
#define __Drv_LED_H__ 

void Drv_LED_Init(void); 

void Drv_LED_Set(int idx); 
void Drv_LED_Clr(int idx); 
void Drv_LED_Togl(int idx); 

void Drv_LED_Write(u8 dat); 
u8 Drv_LED_Read(void); 

#endif 
