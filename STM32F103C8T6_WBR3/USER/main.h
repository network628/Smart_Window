#ifndef _MAIN_H_
#define _MAIN_H_
 
#include	<stdint.h>

#define         ENABLE_BOOT                             //ʹ��BOOT(�̼���������)
//(7+1K)+28K+28K
#define         BASIC_FLASH_ADDR                        0x08000000

#ifdef ENABLE_BOOT
  #define         OFFSET_PARA                             0x1c00        
#else
  #define         OFFSET_PARA                             0xfc00         
#endif

#define         OFFSET_FIRMWARE_L                       0x2000                  //�̼��������е�ַ
#define         OFFSET_FIRMWARE_H                       0x9000                  //�����̼��洢��ַ

#define         PARA_ADDR                               (BASIC_FLASH_ADDR + OFFSET_PARA)
#define         FIREWARE_ADDR_L                         (BASIC_FLASH_ADDR + OFFSET_FIRMWARE_L)   //
#define         FIREWARE_ADDR_H                         (BASIC_FLASH_ADDR + OFFSET_FIRMWARE_H)   //

#define         FIREWARE_UPDATE_FLAG                           0x55555555
 

typedef struct {
  uint8_t power_switch;        //����
  uint8_t wind_rain;           //������
	uint16_t pm_25;              //PM2.5  
	int8_t temper;               //�¶�
  uint8_t humidity;            //ʪ��
  uint16_t tvoc;               //TVOC
	uint16_t ch2o;               //��ȩ
	uint16_t co2;                //������̼

} TYPE_BUFFER_S;

extern TYPE_BUFFER_S FlashBuffer; 
    /*
    //�˴���Ϊƽ̨�Զ����ɣ��밴��ʵ�������޸�ÿ�����·����ϱ�������ֻ�ϱ�����
    mcu_dp_value_update(DPID_PM25_VALUE,��ǰPM2.5���ֵ); //VALUE�������ϱ�;
    mcu_dp_value_update(DPID_TEMP_CURRENT,��ǰ��ǰ�¶�); //VALUE�������ϱ�;
    mcu_dp_value_update(DPID_HUMIDITY_VALUE,��ǰ��ǰʪ��); //VALUE�������ϱ�;
    mcu_dp_value_update(DPID_CH2O_VALUE,��ǰ��ȩ���ֵ); //VALUE�������ϱ�;
    mcu_dp_value_update(DPID_VOC_VALUE,��ǰVOC���ֵ); //VALUE�������ϱ�;
    mcu_dp_value_update(DPID_CO2_VALUE,��ǰCO2���ֵ); //VALUE�������ϱ�;
    mcu_dp_bool_update(DPID_OPEN_WINDOW,��ǰ����); //BOOL�������ϱ�;
    mcu_dp_bool_update(DPID_WIND_RAIN,��ǰ������); //BOOL�������ϱ�;

    */


void Open_Window(char state);  //0-�ش�    1-����


#endif
