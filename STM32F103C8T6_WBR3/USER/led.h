#ifndef __LED_H
#define __LED_H
#include "stm32f10x.h"
#include "sys.h"
////////////////////////////////////////////////////////////////////////////////// 
#define LED1 PAout(8)	// PA8
#define LED2 PBout(12)	// PD2	
#define LED3 PBout(13)	// PD2
#define LED4 PBout(9)	// PD2

void LED_GPIO_Config(void);
#endif

