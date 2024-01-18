#ifndef __KEY_H
#define __KEY_H
#include "stm32f10x.h"
#include "sys.h"

#define KEY1  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3)//��ȡ����1
#define KEY2  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4)//��ȡ����2
#define KEY3  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)//��ȡ����3
#define KEY4  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14)//��ȡ����4

#define FAN_RAIN GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) ////����
#define Light    GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1) ////�չ�

#define KEY1_PRES	1		//PB3 
#define KEY2_PRES	2		//PB4
#define KEY3_PRES	3		//PB5
#define KEY4_PRES	4   //PB14
#define KEY5_PRES	5   //��λ����
#define KEY6_PRES	6   //�Զ��ϻ�



void KEY_Init(void);//IO��ʼ��
u8 KEY_Scan(u8 mode);  	//����ɨ�躯��		
#endif

