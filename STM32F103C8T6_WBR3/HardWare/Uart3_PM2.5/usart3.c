/*-------------------------------------------------*/
/*            ��γ����STM32ϵ�п�����              */
/*-------------------------------------------------*/
/*                                                 */
/*             ʵ�ִ���2���ܵ�Դ�ļ�               */
/*                                                 */
/*-------------------------------------------------*/
#include "usart.h"
#include "stm32f10x.h"  //������Ҫ��ͷ�ļ�
#include "usart3.h"     //������Ҫ��ͷ�ļ�
#include "string.h"     //������Ҫ��ͷ�ļ�
#include "main.h"

#if  USART3_RX_ENABLE                   //���ʹ�ܽ��չ���
char Usart3_RxCompleted = 0;            //����һ������ 0����ʾ����δ��� 1����ʾ������� 
unsigned int Usart3_RxCounter = 0;      //����һ����������¼����2�ܹ������˶����ֽڵ�����
unsigned char Usart3_RxBuff[USART3_RXBUFF_SIZE]; //����һ�����飬���ڱ��洮��2���յ�������   	
#endif

/*-------------------------------------------------*/
/*����������ʼ������2���͹���                      */
/*��  ����bound��������                            */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void Usart3_Init(unsigned int bound)
{  	 	
    GPIO_InitTypeDef GPIO_InitStructure;     //����һ������GPIO���ܵı���
	USART_InitTypeDef USART_InitStructure;   //����һ�����ô��ڹ��ܵı���
#if USART3_RX_ENABLE                         //���ʹ�ܽ��չ���
	NVIC_InitTypeDef NVIC_InitStructure;     //���ʹ�ܽ��չ��ܣ�����һ�������жϵı���
#endif

#if USART3_RX_ENABLE                                 //���ʹ�ܽ��չ���
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //�����ж��������飺��2�� �������ȼ���0 1 2 3 �����ȼ���0 1 2 3
#endif	
      
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);  //ʹ�ܴ���2ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);   //ʹ��GPIOAʱ��
	USART_DeInit(USART3);                                  //����2�Ĵ�����������ΪĬ��ֵ
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;              //׼������PA2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      //IO����50M
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	       //����������������ڴ���2�ķ���
    GPIO_Init(GPIOB, &GPIO_InitStructure);                 //����PA2
   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;              //׼������PA3
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //�������룬���ڴ���2�Ľ���
    GPIO_Init(GPIOB, &GPIO_InitStructure);                 //����PA3
	
	USART_InitStructure.USART_BaudRate = bound;                                    //����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;                    //8������λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;                         //1��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;                            //����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
#if USART3_RX_ENABLE               												   //���ʹ�ܽ���ģʽ
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;	               //�շ�ģʽ
#else                                                                              //�����ʹ�ܽ���ģʽ
	USART_InitStructure.USART_Mode = USART_Mode_Tx ;	                           //ֻ��ģʽ
#endif        
    USART_Init(USART3, &USART_InitStructure);                                      //���ô���2	

#if USART3_RX_ENABLE  	         					        //���ʹ�ܽ���ģʽ
	USART_ClearFlag(USART3, USART_FLAG_RXNE);	            //������ձ�־λ
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);          //���������ж�
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;       //���ô���2�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0; //��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//�ж�ͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	                        //���ô���2�ж�
#endif  

	USART_Cmd(USART3, ENABLE);                              //ʹ�ܴ���2
}

/*-------------------------------------------------*/
/*������������2 printf����                         */
/*��  ����char* fmt,...  ��ʽ������ַ����Ͳ���    */
/*����ֵ����                                       */
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
/*������������2���ͻ������е�����                  */
/*��  ����data������                               */
/*����ֵ����                                       */
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
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		Res =USART_ReceiveData(USART3);	//��ȡ���յ�������
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
//          ��0������Ϊ0x5A            ÿһ�ΰ�Ҫ����4����      -------------------------------- ��֤У��� -------------------------------------
  if( Usart3_RxBuff[0] == 0x16 && Usart3_RxBuff[2] == 0x0B && (Usart3_RxCounter==9)) 
	{ 
 
       len = Usart3_RxCounter;
			 for(t=0;t<len;t++)  //�����ַ���
			 {
				 LCD_str[t] = Usart3_RxBuff[t];
			 }	
 
		   Usart3_RxCounter=0;
			 
			 if(LCD_str[1]==0x11)   
			 {
//				 printf("------  PM2.5NUMBER  OK  --->\r\n");
//				 for(i=0;i<7;i++)
//				 printf("%X-",LCD_str[i]);
//				 PM2.5����ֵ = DF3*256 + DF4
				 FlashBuffer.pm_25 = LCD_str[5]*256 + LCD_str[6];
//				 printf("->\r\n");
//			    lcd.Set_time = LCD_str[8];
//				  Work_Cnt = 0;
//				  lcd.DeviceStatus =1;
			 }	
 
	 }	
}
 
  


