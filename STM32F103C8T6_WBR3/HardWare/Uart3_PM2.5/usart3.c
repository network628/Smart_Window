/*-------------------------------------------------*/
/*            超纬电子STM32系列开发板              */
/*-------------------------------------------------*/
/*                                                 */
/*             实现串口2功能的源文件               */
/*                                                 */
/*-------------------------------------------------*/
#include "usart.h"
#include "stm32f10x.h"  //包含需要的头文件
#include "usart3.h"     //包含需要的头文件
#include "string.h"     //包含需要的头文件
#include "main.h"

#if  USART3_RX_ENABLE                   //如果使能接收功能
char Usart3_RxCompleted = 0;            //定义一个变量 0：表示接收未完成 1：表示接收完成 
unsigned int Usart3_RxCounter = 0;      //定义一个变量，记录串口2总共接收了多少字节的数据
unsigned char Usart3_RxBuff[USART3_RXBUFF_SIZE]; //定义一个数组，用于保存串口2接收到的数据   	
#endif

/*-------------------------------------------------*/
/*函数名：初始化串口2发送功能                      */
/*参  数：bound：波特率                            */
/*返回值：无                                       */
/*-------------------------------------------------*/
void Usart3_Init(unsigned int bound)
{  	 	
    GPIO_InitTypeDef GPIO_InitStructure;     //定义一个设置GPIO功能的变量
	USART_InitTypeDef USART_InitStructure;   //定义一个设置串口功能的变量
#if USART3_RX_ENABLE                         //如果使能接收功能
	NVIC_InitTypeDef NVIC_InitStructure;     //如果使能接收功能，定义一个设置中断的变量
#endif

#if USART3_RX_ENABLE                                 //如果使能接收功能
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //设置中断向量分组：第2组 抢先优先级：0 1 2 3 子优先级：0 1 2 3
#endif	
      
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);  //使能串口2时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);   //使能GPIOA时钟
	USART_DeInit(USART3);                                  //串口2寄存器重新设置为默认值
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;              //准备设置PA2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      //IO速率50M
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	       //复用推挽输出，用于串口2的发送
    GPIO_Init(GPIOB, &GPIO_InitStructure);                 //设置PA2
   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;              //准备设置PA3
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //浮空输入，用于串口2的接收
    GPIO_Init(GPIOB, &GPIO_InitStructure);                 //设置PA3
	
	USART_InitStructure.USART_BaudRate = bound;                                    //波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;                    //8个数据位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;                         //1个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;                            //无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
#if USART3_RX_ENABLE               												   //如果使能接收模式
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;	               //收发模式
#else                                                                              //如果不使能接收模式
	USART_InitStructure.USART_Mode = USART_Mode_Tx ;	                           //只发模式
#endif        
    USART_Init(USART3, &USART_InitStructure);                                      //设置串口2	

#if USART3_RX_ENABLE  	         					        //如果使能接收模式
	USART_ClearFlag(USART3, USART_FLAG_RXNE);	            //清除接收标志位
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);          //开启接收中断
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;       //设置串口2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0; //抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//中断通道使能
	NVIC_Init(&NVIC_InitStructure);	                        //设置串口2中断
#endif  

	USART_Cmd(USART3, ENABLE);                              //使能串口2
}

/*-------------------------------------------------*/
/*函数名：串口2 printf函数                         */
/*参  数：char* fmt,...  格式化输出字符串和参数    */
/*返回值：无                                       */
/*-------------------------------------------------*/

__align(8) char USART3_TxBuff[USART3_TXBUFF_SIZE];  

void u3_printf(char* fmt,...) 
{  
	unsigned int i,length;
	
	va_list ap;
	va_start(ap,fmt);
	vsprintf(USART3_TxBuff,fmt,ap);
	va_end(ap);	
	
	length=strlen((const char*)USART3_TxBuff);		
	while((USART3->SR&0X40)==0);
	for(i = 0;i < length;i ++)
	{			
		USART3->DR = USART3_TxBuff[i];
		while((USART3->SR&0X40)==0);	
	}	
}

/*-------------------------------------------------*/
/*函数名：串口2发送缓冲区中的数据                  */
/*参  数：data：数据                               */
/*返回值：无                                       */
/*-------------------------------------------------*/
void u3_TxData(unsigned char *data,char len)
{
	int	i;	
	while((USART3->SR&0X40)==0);
	for(i = 0;i < len;i ++){			
		USART3->DR = data[i];
		while((USART3->SR&0X40)==0);	
	}
}


 
unsigned char LCD_str[10]; 
//extern TYPE_BUFFER_S FlashBuffer;
//TYPE_BUFFER_S FlashBuffer; 
void USART3_IRQHandler(void)   
{                      
  u8 Res;	
//	u8 i;
  u8 t,len;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res =USART_ReceiveData(USART3);	//读取接收到的数据
    if(Res == 0x16) 
		{
		  Usart3_RxCounter = 0;
		}
		Usart3_RxBuff[Usart3_RxCounter]=Res ;
    Usart3_RxCounter++;
	}
 
	if(Usart3_RxBuff[0] != 0x16 && Usart3_RxBuff[1] != 0x11) 
	{
	  Usart3_RxCounter=0;
	}
//          第0个数据为0x5A            每一次包要大于4个数      -------------------------------- 验证校验和 -------------------------------------
  if( Usart3_RxBuff[0] == 0x16 && Usart3_RxBuff[2] == 0x0B && (Usart3_RxCounter==9)) 
	{ 
 
       len = Usart3_RxCounter;
			 for(t=0;t<len;t++)  //复制字符串
			 {
				 LCD_str[t] = Usart3_RxBuff[t];
			 }	
 
		   Usart3_RxCounter=0;
			 
			 if(LCD_str[1]==0x11)   
			 {
//				 printf("------  PM2.5NUMBER  OK  --->\r\n");
//				 for(i=0;i<7;i++)
//				 printf("%X-",LCD_str[i]);
//				 PM2.5测量值 = DF3*256 + DF4
				 FlashBuffer.pm_25 = LCD_str[5]*256 + LCD_str[6];
//				 printf("->\r\n");
//			    lcd.Set_time = LCD_str[8];
//				  Work_Cnt = 0;
//				  lcd.DeviceStatus =1;
			 }	
 
	 }	
}
 
  


