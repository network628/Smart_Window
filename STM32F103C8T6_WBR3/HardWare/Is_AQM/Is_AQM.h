#ifndef __IS_AQM_H
#define __IS_AQM_H  
#include "i2c.h"

typedef struct
{
  u16 eCO2;
	float Temp_C;
	float Hum_RH;
	u16 TVOC;
	u16 HCHO;
}Is_AQM_Dat;


FunctionalState Is_AQM_D(void);
void Is_AQM_D_out(void);

void Is_AQM_Init(void); //≥ı ºªØIIC
#endif
















