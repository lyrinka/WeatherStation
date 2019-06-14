#include <stm32f10x.h> 
#include <OS.h> 
#include <BMP280.h> 
#include <SPI.h> 

struct __BMP280_Meas BMP280_Meas; 
TASK SPI_cbTask = NULL; 

void SPI_BufferExch(u8 * stream, int size){ 
	SPI_CSel(); 
	SPI_cbTask = Lin_GetCurrTask(); 
	SPI_AsyncExch((char *)stream, size); 
	do{ 
		OS_Suspend(); 
	}while(SPI_cbTask->WkupSrc != Src_Gen); 
	SPI_CDis(); 
}

void SPI_AsyncExch_cbFunc(void){ 
	if(SPI_cbTask){ 
		OS_GenEvent(SPI_cbTask, 0); 
		OS_PreemptISR(); 
	}
}

int BMP280_Init(void){ 
	SPI_cbTask = NULL; 
	OS_Lock(); 
	SPI_InitIO(); 
	SPI_CDis(); 
	SPI_Init(0, 2); 
	SPI_AsyncExch_cbFuncReg(SPI_AsyncExch_cbFunc); 
	OS_UnLock(); 
	
	u8 Stream[25]; 
	Stream[0] = 0x88; 
	SPI_BufferExch(Stream, 25); 
	BMP280_Compensation(Stream + 1); 
	
	return 0; 
}

int BMP280_Off(void){ 
	u8 Stream[2]; 
	Stream[0] = 0x74; 
	Stream[1] = 0x00; 
	SPI_BufferExch(Stream, 2); 
	return 0; 
}

int BMP280_On(void){ 
	u8 Stream[4]; 
	Stream[0] = 0x74; 
	Stream[1] = 0x57; 
	Stream[2] = 0x75; 
	Stream[3] = 0x30; 
	SPI_BufferExch(Stream, 4); 
	return 0; 
}

int BMP280_Read(int * temp, u32 * pres){ 
	u8 Stream[7]; 
	Stream[0] = 0xF7; 
	SPI_BufferExch(Stream, 7); 
	BMP280_Calc(Stream + 1, temp, pres); 
	return 0; 
}


// Internal Functions 
// Calculates the raw measurement result from chip. 
void BMP280_Calc(const u8 * data, s32 * temp, u32 * pres){ 
	int pres_raw = data[2] >> 4| data[1] << 4 | data[0] << 12; 
	int temp_raw = data[5] >> 4| data[4] << 4 | data[3] << 12; 
	*temp = BMP280_Temperature(temp_raw); 
	*pres = BMP280_Pressure(pres_raw); 
}

// Feeds in compensation value read from chip. 
void BMP280_Compensation(const u8 * data){ 
	u8 * temp = (u8 *)&BMP280_Meas; 
	for(int i = 0; i < 24; i++) temp[i] = data[i]; 
}

// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC. 
s32 BMP280_Temperature(s32 adc_T){ 
	s32 var1, var2, T; 
	s32 temp_T1 = (s32)BMP280_Meas.dig_T1; 
	var1 = (((adc_T >> 3) - (temp_T1 << 1)) * ((s32)BMP280_Meas.dig_T2)) >> 11; 
	var2 = (((((adc_T >> 4) - temp_T1) * ((adc_T >> 4) - temp_T1)) >> 12) * ((s32)BMP280_Meas.dig_T3)) >> 14; 
	T = var1 + var2; 
	BMP280_Meas.t_fine = T; 
	T = (T * 5 + 128) >> 8; 
	BMP280_Meas.T = T; 
	return T; 
}

// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits). 
u32 BMP280_Pressure(s32 adc_P){  
	s64 var1, var2, p; 
	var1 = ((s64)BMP280_Meas.t_fine) - 128000; 
	var2 = var1 * var1 * (s64)BMP280_Meas.dig_P6; 
	var2 = var2 + ((var1 * (s64)BMP280_Meas.dig_P5) << 17); 
	var2 = var2 + (((s64)BMP280_Meas.dig_P4) << 35); 
	var1 = ((var1 * var1 * (s64)BMP280_Meas.dig_P3) >> 8) + ((var1 * (s64)BMP280_Meas.dig_P2) << 12); 
	var1 = (((((s64)1) << 47) + var1)) * ((s64)BMP280_Meas.dig_P1) >> 33; 
	if (var1 == 0) return 0; 
	p = 1048576 - adc_P; 
	p = (((p << 31)- var2) * 3125) / var1; 
	var1 = (((s64)BMP280_Meas.dig_P9) * (p >> 13) * (p >> 13)) >> 25; 
	var2 = (((s64)BMP280_Meas.dig_P8) * p) >> 19; 
	p = ((p + var1 + var2) >> 8) + (((s64)BMP280_Meas.dig_P7 )<< 4); 
	BMP280_Meas.P = (u32)p; 
	return (u32)p;
}

// End of file. 
