
#ifndef __BMP280_H__ 
#define __BMP280_H__ 

// Internal 
#define s64 signed long long int 
struct __BMP280_Meas{ 
	u16 dig_T1; 
	s16 dig_T2; 
	s16 dig_T3; 
	u16 dig_P1; 
	s16 dig_P2; 
	s16 dig_P3; 
	s16 dig_P4; 
	s16 dig_P5; 
	s16 dig_P6; 
	s16 dig_P7; 
	s16 dig_P8; 
	s16 dig_P9; 
	s32 t_fine; 
	s32 T; 
	u32 P; 
}; 

void BMP280_Calc(const u8 * data, s32 * temp, u32 * pressure); 
void BMP280_Compensation(const u8 * data); 
s32  BMP280_Temperature(s32 adc_T); 
u32  BMP280_Pressure(s32 adc_P); 

// External 
int BMP280_Init(void); 
int BMP280_Off(void); 
int BMP280_On(void); 
int BMP280_Read(int * temp, unsigned int * pressure); 

#endif 
