#include "Is_AQM.h" 
#include "delay.h"
#include "usart.h"
#include "main.h"

//��ʼ��IIC�ӿ�
void Is_AQM_Init(void)
{
//	IIC_Init();
	 I2C_Configuration();
}

static u8 data[13];
FunctionalState Is_AQM_D(void)
{
	 u8 length=0;
//	  char i;
	 if(!I2C_Start())return DISABLE;
    I2C_SendByte(0xa2); 
    if(!I2C_WaitAck())
		{I2C_Stop(); return DISABLE;}
    I2C_SendByte(0x51);      
    I2C_WaitAck();
		I2C_delay();
	  I2C_Stop();
		do
		{
		I2C_Start();
    I2C_SendByte(0xa3);
		}
		while(!I2C_WaitAck());
		for(length=0;length<=12;length++)
		{
			data[length]=I2C_ReceiveByte();
			I2C_Ack(); 		
		};
		I2C_NoAck();		
		I2C_Stop();		

	  return ENABLE;
}


Is_AQM_Dat Is_AQM;
 
void Is_AQM_D_out(void)
{
	Is_AQM_D();
	if((data[0]==0xff) && (data[3]==0x00))
	{
		Is_AQM.eCO2 = (data[1]<<8) | data[2];       //������̼
		Is_AQM.Temp_C = (float)(data[4]*256 + data[5]-669)/10; 
		Is_AQM.Hum_RH = (float)(data[6]*256 + data[7]-125)/10; 
		Is_AQM.TVOC = (data[8]<<8) | data[9];       //�л���̬���� �ӷ����л���
		Is_AQM.HCHO = (data[10]<<8) | data[11];     //��ȩ
	}
//				mcu_dp_bool_update(DPID_OPEN_WINDOW,FlashBuffer.power_switch); //BOOL�������ϱ�; //����
//			mcu_dp_bool_update(DPID_WIND_RAIN,FlashBuffer.wind_rain); //BOOL�������ϱ�; //������
//			mcu_dp_value_update(DPID_PM25_VALUE,FlashBuffer.pm_25); //VALUE�������ϱ�;
//			mcu_dp_value_update(DPID_TEMP_CURRENT,FlashBuffer.temper); //VALUE�������ϱ�;
//			mcu_dp_value_update(DPID_HUMIDITY_VALUE,FlashBuffer.humidity); //VALUE�������ϱ�;
//			mcu_dp_value_update(DPID_VOC_VALUE,FlashBuffer.tvoc); //VALUE�������ϱ�;
//      mcu_dp_value_update(DPID_CH2O_VALUE,FlashBuffer.ch2o); //VALUE�������ϱ�;
//	    mcu_dp_value_update(DPID_CO2_VALUE,FlashBuffer.co2); //VALUE�������ϱ�; 
	FlashBuffer.co2 = Is_AQM.eCO2; 
	FlashBuffer.temper = Is_AQM.Temp_C;
	FlashBuffer.humidity = Is_AQM.Hum_RH;
	FlashBuffer.tvoc = Is_AQM.TVOC;
	FlashBuffer.ch2o = Is_AQM.HCHO;

// 	printf("--->Is_AQM.eCO2=%04dppm  Is_AQM.Temp_C=%3.2f'C  Is_AQM.Hum_RH=%3.2f%%RH  Is_AQM.TVOC=%04dug/m3  Is_AQM.HCHO=%04dug/m3\r\n",Is_AQM.eCO2,Is_AQM.Temp_C,Is_AQM.Hum_RH,Is_AQM.TVOC,Is_AQM.HCHO);	
}








