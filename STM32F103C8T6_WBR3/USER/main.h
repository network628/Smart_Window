#ifndef _MAIN_H_
#define _MAIN_H_
 
#include	<stdint.h>

#define         ENABLE_BOOT                             //使能BOOT(固件升级功能)
//(7+1K)+28K+28K
#define         BASIC_FLASH_ADDR                        0x08000000

#ifdef ENABLE_BOOT
  #define         OFFSET_PARA                             0x1c00        
#else
  #define         OFFSET_PARA                             0xfc00         
#endif

#define         OFFSET_FIRMWARE_L                       0x2000                  //固件正常运行地址
#define         OFFSET_FIRMWARE_H                       0x9000                  //升级固件存储地址

#define         PARA_ADDR                               (BASIC_FLASH_ADDR + OFFSET_PARA)
#define         FIREWARE_ADDR_L                         (BASIC_FLASH_ADDR + OFFSET_FIRMWARE_L)   //
#define         FIREWARE_ADDR_H                         (BASIC_FLASH_ADDR + OFFSET_FIRMWARE_H)   //

#define         FIREWARE_UPDATE_FLAG                           0x55555555
 

typedef struct {
  uint8_t power_switch;        //开关
  uint8_t wind_rain;           //风雨检测
	uint16_t pm_25;              //PM2.5  
	int8_t temper;               //温度
  uint8_t humidity;            //湿度
  uint16_t tvoc;               //TVOC
	uint16_t ch2o;               //甲醛
	uint16_t co2;                //二氧化碳

} TYPE_BUFFER_S;

extern TYPE_BUFFER_S FlashBuffer; 
    /*
    //此代码为平台自动生成，请按照实际数据修改每个可下发可上报函数和只上报函数
    mcu_dp_value_update(DPID_PM25_VALUE,当前PM2.5检测值); //VALUE型数据上报;
    mcu_dp_value_update(DPID_TEMP_CURRENT,当前当前温度); //VALUE型数据上报;
    mcu_dp_value_update(DPID_HUMIDITY_VALUE,当前当前湿度); //VALUE型数据上报;
    mcu_dp_value_update(DPID_CH2O_VALUE,当前甲醛检测值); //VALUE型数据上报;
    mcu_dp_value_update(DPID_VOC_VALUE,当前VOC检测值); //VALUE型数据上报;
    mcu_dp_value_update(DPID_CO2_VALUE,当前CO2检测值); //VALUE型数据上报;
    mcu_dp_bool_update(DPID_OPEN_WINDOW,当前窗户); //BOOL型数据上报;
    mcu_dp_bool_update(DPID_WIND_RAIN,当前风雨检测); //BOOL型数据上报;

    */


void Open_Window(char state);  //0-关窗    1-开窗


#endif
