#include "stm32f10x.h"
#include "usart.h"
#include "usart2.h"
#include "usart3.h"     //包含需要的头文件
#include "key.h"
#include "led.h"
#include "Time_test.h"
#include "mcu_api.h"
#include "protocol.h"
#include "system.h"
#include "wifi.h"
#include "delay.h"
#include "main.h"
#include "Is_AQM.h" 
#include "Motor.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_it.h"

//  volatile u32 rst_cnt;
volatile u32 time_cnt; 
volatile u32 tt; 
volatile unsigned char key;
u16 Hand_AD,Window_AD;
static char In_place_state = 0;  //0 无状态 1 开  2 关 //0-没到位   1-窗户电机到位了 2-执手电机到位了

unsigned char PM2_5_HEX[5]={0x11,0x02,0x0B,0x01,0xE1};

static void wifi_work_state_led(void);
void key_proces(void);
void Window_current_handle(void);
void Auto_Test(void);

int main(void)
{	
//  u8 rotation=0; //卡位回转 1：执手 2：窗户
	SystemInit();
	LED_GPIO_Config();
	uart1_init(9600);  // 调试打印
	Usart2_Init(9600); // Tuya wifi
	Usart3_Init(9600); // pm2.5
//	EXTI_PE4_Config();
	KEY_Init();
	Is_AQM_Init();			//IIC初始化 
  Motor_IO_Init();	 
  Adc_Init();		  		//ADC初始化	
	TIM2_NVIC_Configuration();
  TIM2_Configuration();
	time_cnt=0;
	printf("Usart1_Init ok\r\n");
	wifi_protocol_init();
	while(1)
	{	
		wifi_uart_service();
		wifi_work_state_led(); 
	  key_proces();
		Window_current_handle();

		
		
		if((FlashBuffer.pm_25 >1000 || FlashBuffer.ch2o>80 || FlashBuffer.tvoc>600 || FlashBuffer.co2>1000) && FlashBuffer.power_switch == 0)
		{
		    FlashBuffer.power_switch = 1;
				Open_Window(1);  //0-停止  1-开窗  2-关窗
				printf("------FlashBuffer.sonsor ok------\r\n");
		}
	  else if( FAN_RAIN == 1 && FlashBuffer.power_switch == 1)
		{
			delay_ms(50);	
			if( FAN_RAIN == 1 && FlashBuffer.power_switch == 1)
			{
				 
				FlashBuffer.power_switch = 0;
				Open_Window(2);  //0-停止  1-开窗  2-关窗
				printf("------FAN_RAIN ok------\r\n");
			}
		}



		if(time_cnt%30==0)  //s
		{
			u3_TxData(PM2_5_HEX,5);
			Is_AQM_D_out();
//			mcu_dp_bool_update(DPID_OPEN_WINDOW,FlashBuffer.power_switch); //BOOL型数据上报; //开关
//			mcu_dp_bool_update(DPID_WIND_RAIN,FlashBuffer.wind_rain); //BOOL型数据上报; //风雨检测
//			mcu_dp_value_update(DPID_PM25_VALUE,FlashBuffer.pm_25); //VALUE型数据上报;     100ug/m3
//			mcu_dp_value_update(DPID_TEMP_CURRENT,FlashBuffer.temper); //VALUE型数据上报;
//			mcu_dp_value_update(DPID_HUMIDITY_VALUE,FlashBuffer.humidity); //VALUE型数据上报;
//			mcu_dp_value_update(DPID_VOC_VALUE,FlashBuffer.tvoc); //VALUE型数据上报;   //有机气态物质 挥发性有机物 0.6mg/m3
//      mcu_dp_value_update(DPID_CH2O_VALUE,FlashBuffer.ch2o); //VALUE型数据上报;  //甲醛     0.1mg/m3
//	    mcu_dp_value_update(DPID_CO2_VALUE,FlashBuffer.co2); //VALUE型数据上报;     2000ppm
			mcu_dp_enum_update(DPID_STATUS,FlashBuffer.power_switch); //枚举型数据上报;
			mcu_dp_value_update(DPID_TEMP_CURRENT,FlashBuffer.temper); //VALUE型数据上报;
	    mcu_dp_bool_update(DPID_SWITCH_DOOR_WIN,FlashBuffer.power_switch); //BOOL型数据上报;
		}
		if(time_cnt%10==0)  //s
		{
		  printf("\r\n--->power_switch=%d  --->wind_rain=%d---> pm_25=%d--->\
			temper=%d---> humidity=%d--->  tvoc=%d---> ch2o=%d --->  co2=%d\r\n",
			FlashBuffer.power_switch,FlashBuffer.wind_rain,FlashBuffer.pm_25,
			FlashBuffer.temper,FlashBuffer.humidity,FlashBuffer.tvoc,FlashBuffer.ch2o,FlashBuffer.co2);
		}
	}
}

void key_proces(void)
{
		u8 i;
		key=KEY_Scan(0);
		switch(key)
		{
		  case KEY1_PRES:  //开窗
			FlashBuffer.power_switch = 1; //开窗
       		Open_Window(1);  //0-停止  1-开窗  2-关窗 
			printf("---KEY1_PRES\r\n");
			break;
		  case KEY2_PRES:  //关窗
			FlashBuffer.power_switch = 0; //关窗
       		Open_Window(2);  //0-停止  1-开窗  2-关窗 
			printf("---KEY2_PRES\r\n");
			break;
			case KEY3_PRES://停止
			{
                Open_Window(0);  //0-停止  1-开窗  2-关窗
				printf("---KEY3_PRES\r\n");
			}
			break;
			case KEY4_PRES://半窗 half
			{

				printf("---KEY4_PRES\r\n");
			}
			break;
			case KEY5_PRES:  //复位配网
			{
			mcu_reset_wifi();
				for(i=0;i<3;i++)
				{
					LED1 = 1;LED2 = 1;LED3 = 1;LED4 = 1;delay_ms(500);//1
					LED1 = 0;LED2 = 0;LED3 = 0;LED4 = 0;delay_ms(500);//0
				}
				printf("--->KEY5_PRES--->mcu_reset_wifi--->\r\n");
			}
			break;
			case KEY6_PRES:  //自动老化
			{
				Auto_Test();
			}
			break;
			default:  break;	
		}
		
		
}

/*****************************************************************************
函数名称 : wifi_work_state_led
功能描述 : wifi状态led控制
输入参数 : 无
返回参数 : 无
使用说明 : 无
*****************************************************************************/
static void wifi_work_state_led(void)
{
    uint8_t wifi_state = mcu_get_wifi_work_state();
		switch(wifi_state)
		{
			case 0x00:  //状态1	smartconfig配置状态	0x00
				if(tt%2==0)
				{
					LED1 = 0;LED4 = 0;LED2 = 0;LED3 = 0;
				}
				else
				{
					LED1 = 1;LED4 = 0;LED2 = 0;LED3 = 0;
				}			  				
				    break;
			case 0x01:  //状态2	AP配置状态	0x01
				if(tt==0)
				{
					LED2 = 0;LED1 = 0;LED4 = 0;LED3 = 0;
				}
				else
				{
					LED2 = 1;LED1 = 0;LED4 = 0;LED3 = 0;
				} 
				    break;
			case 0x02:  //状态3	Wi-Fi已配置但未连上路由器	0x02
				 if(tt%2==0)
				{
					LED3 = 0;LED1 = 0;LED2 = 0;LED4 = 0;
				}
				else
				{
					LED3 = 1;LED1 = 0;LED2 = 0;LED4 = 0;
				}			  								
			  break;
			case 0x03:  //状态4	Wi-Fi已配置且连上路由器	0x03
				 if(tt==0) 
				{
					LED4 = 0;LED1 = 0;LED2 = 0;LED3 = 0;
				}
				else
				{
					LED4 = 1;LED1 = 0;LED2 = 0;LED3 = 0;
				} 
				break;
			case 0x04:  //状态5	已连上路由器且连接到云端	0x04
				LED1 = 0;LED2 = 0;LED3 = 0;LED4 = 0;
//			  if(time_cnt%1000==0)
//				printf("<---wifi_state(-%d-)--->\r\n",wifi_state);
				break;
			default:
        break;		
			}     
//		  if(time_cnt%5000==0)
//		   printf("---wifi_state(-%d-)--time=%d---%d\r\n",wifi_state,time_cnt,tt);
}

////////////////////////////////////////////////////////////////////////////////
 
void Open_Window(char state)  //0-停止  1-开窗  2-关窗 
{

	if(state == 1) //0-停止  1-开窗  2-关窗 
	{  //先开执手电机  再开窗户电机
		In_place_state = 1;  //0 无状态 1 开  2 关  
		Hand_Motor_Contrl(1);  //0 关   1  正转  2  反转
//		printf("<---开窗   打开执手电机--->\r\n");
	}
	else if(state == 2) //0-停止  1-开窗  2-关窗 
	{  //先关窗户电机  再关执手电机
		In_place_state = 2;  //0 无状态 1 开  2 关 
		Window_Motor_Contrl(2); //0 关   1  正转  2  反转
//		printf("<---关窗   关闭窗户电机--->\r\n");
	}
  else if(state == 0) //0-停止  1-开窗  2-关窗 
  {
		if(In_place_state == 1)  //0 无状态 1 开  2 关   //如果是开窗就反转500ms
		{
			Window_Motor_Contrl(2);  //0 关   1  正转  2  反转
			delay_ms(500);
		}
		else if(In_place_state == 2)  //0 无状态 1 开  2 关   //如果是关窗就正转500ms
		{
			Window_Motor_Contrl(1);  //0 关   1  正转  2  反转
			delay_ms(500);
		}
		Hand_Motor_Contrl(0);  //0 关   1  正转  2  反转
		Window_Motor_Contrl(0); //0 关   1  正转  2  反转
		In_place_state = 0;  //0 无状态 1 开  2 关 
	}
}
 

////////////////////////////////////////////////////////////////////////////////
u16 Hand_AD_base=900;
u16 Window_AD_base = 1500;
void Window_current_handle(void)
{
	Window_AD   = Get_Adc_Average(ADC_Channel_4,10);  //Window_AD
	Hand_AD = Get_Adc_Average(ADC_Channel_5,10);   //Hand_AD
	//关窗过程
	if(Window_AD>Window_AD_base && In_place_state == 2) //  完全关闭 窗户电机
	{
		Window_Motor_Contrl(1); //打开窗户电机 //0 关   1  正转  2  反转
		printf("<---电机回调>900--->\r\n");
		delay_ms(500);
		Window_Motor_Contrl(0);  //0 关   1  正转  2  反转
		printf("<---关闭执手电机--->\r\n");
		Hand_Motor_Contrl(2); // //0 关   1  正转  2  反转
	}
  if(Hand_AD>Hand_AD_base && In_place_state == 2) //关窗过程中执手电机 完全关闭
	{
	  In_place_state = 0;//0 无状态 1 开  2 关
		Hand_Motor_Contrl(1); //  //0 关   1  正转  2  反转
		printf("<---电机回调>900--->\r\n");
		delay_ms(500);
		Hand_Motor_Contrl(0); //  //0 关   1  正转  2  反转 
	} 
	
  //开窗过程
	if(Hand_AD>Hand_AD_base && In_place_state == 1)// 完全打开 执手电机
	{
		Hand_Motor_Contrl(2); //   //0 关   1  正转  2  反转
		printf("<---电机回调--->\r\n");
		delay_ms(500);
		Hand_Motor_Contrl(0);  //0 关   1  正转  2  反转
		printf("<---打开窗户电机--->\r\n");
		Window_Motor_Contrl(1); //打开窗户电机 //0 关   1  正转  2  反转
	}	
	
  if(Window_AD>Window_AD_base && In_place_state == 1) //开窗过程中窗户电机 完全打开
	{
	  In_place_state = 0;//0 无状态 1 开  2 关
		Window_Motor_Contrl(2); //  //0 关   1  正转  2  反转
		printf("<---电机回调--->\r\n");
		delay_ms(500);
		Window_Motor_Contrl(0); //  //0 关   1  正转  2  反转
	} 
}
//////////////////////////////////////////////////////////////////////////////


void Auto_Test(void)
{
	u8 i=1;
	u8 exit=1;
	Open_Window(1);  //0-停止  1-开窗  2-关窗 
  while(exit)
	{
		key=KEY_Scan(0);
		switch(key)
		{
		  case KEY3_PRES:  //自动老化
			{
				 exit = 0;
			}
			break;
			default:  break;
		}	
		
	  Window_current_handle();
		
		if(rst_cnt%30==0)
		{
			if(i==1)
			{
				i=2;
			Open_Window(i);  //0-停止  1-开窗  2-关窗 
//		  printf("--->Auto_Test 1-开窗--->\r\n");
			delay_ms(2000);
			}
			else if(i==2)
			{
				i=1;
			Open_Window(i);  //0-停止  1-开窗  2-关窗 
		  printf("--->Auto_Test 2-关窗--->\r\n");
			delay_ms(2000);
			}
//			rst_cnt = 0;
		}
		
		if(time_cnt%10==0)
		printf("---> rst_cnt(%d)--->\r\n",rst_cnt);
	}//rst_cnt
}
//////////////////////////////////////////////////////////////////////////////


