#ifndef __KEY_H
#define __KEY_H
#include "stm32f10x.h"
#include "sys.h"

#define KEY1  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3)//读取按键1
#define KEY2  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4)//读取按键2
#define KEY3  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)//读取按键3
#define KEY4  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14)//读取按键4

#define FAN_RAIN GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) ////风雨
#define Light    GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1) ////日光

#define KEY1_PRES	1		//PB3 
#define KEY2_PRES	2		//PB4
#define KEY3_PRES	3		//PB5
#define KEY4_PRES	4   //PB14
#define KEY5_PRES	5   //复位配网
#define KEY6_PRES	6   //自动老化



void KEY_Init(void);//IO初始化
u8 KEY_Scan(u8 mode);  	//按键扫描函数		
#endif

