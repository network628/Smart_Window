/*-------------------------------------------------*/
/*            ��γ����STM32ϵ�п�����              */
/*-------------------------------------------------*/
/*                                                 */
/*             ʵ�ִ���2���ܵ�Դ�ļ�               */
/*                                                 */
/*-------------------------------------------------*/
//#include "usart.h"
#include "stm32f10x.h"  //������Ҫ��ͷ�ļ�
#include "usart2.h"     //������Ҫ��ͷ�ļ�
#include "string.h"     //������Ҫ��ͷ�ļ�
#include "wifi.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"

#if  USART2_RX_ENABLE                   //���ʹ�ܽ��չ���
char Usart2_RxCompleted = 0;            //����һ������ 0����ʾ����δ��� 1����ʾ������� 
unsigned int Usart2_RxCounter = 0;      //����һ����������¼����2�ܹ������˶����ֽڵ�����
unsigned char Usart2_RxBuff[USART2_RXBUFF_SIZE]; //����һ�����飬���ڱ��洮��2���յ�������   	
#endif

/*-------------------------------------------------*/
/*����������ʼ������2���͹���                      */
/*��  ����bound��������                            */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void Usart2_Init(unsigned int bound)
{  	 	
    GPIO_InitTypeDef GPIO_InitStructure;     //����һ������GPIO���ܵı���
	USART_InitTypeDef USART_InitStructure;   //����һ�����ô��ڹ��ܵı���
#if USART2_RX_ENABLE                         //���ʹ�ܽ��չ���
	NVIC_InitTypeDef NVIC_InitStructure;     //���ʹ�ܽ��չ��ܣ�����һ�������жϵı���
#endif

#if USART2_RX_ENABLE                                 //���ʹ�ܽ��չ���
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);  //�����ж��������飺��2�� �������ȼ���0 1 2 3 �����ȼ���0 1 2 3
#endif	
 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);  //ʹ�ܴ���2ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);   //ʹ��GPIOAʱ��
	USART_DeInit(USART2);                                  //����2�Ĵ�����������ΪĬ��ֵ
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;            //׼������PA2  tx
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      //IO����50M
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	       //����������������ڴ���2�ķ���
    GPIO_Init(GPIOA, &GPIO_InitStructure);                 //����PA2
   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;              //׼������PA3  rx
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //�������룬���ڴ���2�Ľ���
    GPIO_Init(GPIOA, &GPIO_InitStructure);                 //����PA3
	
	USART_InitStructure.USART_BaudRate = bound;                                    //����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;                    //8������λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;                         //1��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;                            //����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
#if USART2_RX_ENABLE               												   //���ʹ�ܽ���ģʽ
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;	               //�շ�ģʽ
#else                                                                              //�����ʹ�ܽ���ģʽ
	USART_InitStructure.USART_Mode = USART_Mode_Tx ;	                           //ֻ��ģʽ
#endif        
    USART_Init(USART2, &USART_InitStructure);                                      //���ô���2	

#if USART2_RX_ENABLE  	         					        //���ʹ�ܽ���ģʽ
	USART_ClearFlag(USART2, USART_FLAG_RXNE);	            //������ձ�־λ
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);          //���������ж�
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;       //���ô���2�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1; //��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//�ж�ͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	                        //���ô���2�ж�
#endif  

	USART_Cmd(USART2, ENABLE);                              //ʹ�ܴ���2
}

/*-------------------------------------------------*/
/*������������2 printf����                         */
/*��  ����char* fmt,...  ��ʽ������ַ����Ͳ���    */
/*����ֵ����                                       */
/*-------------------------------------------------*/

__align(8) char USART2_TxBuff[USART2_TXBUFF_SIZE];  

void u2_printf(char* fmt,...) 
{  
	unsigned int i,length;
	
	va_list ap;
	va_start(ap,fmt);
	vsprintf(USART2_TxBuff,fmt,ap);
	va_end(ap);	
	
	length=strlen((const char*)USART2_TxBuff);		
	while((USART2->SR&0X40)==0);
	for(i = 0;i < length;i ++)
	{			
		USART2->DR = USART2_TxBuff[i];
		while((USART2->SR&0X40)==0);	
	}	
}

/*-------------------------------------------------*/
/*������������2���ͻ������е�����                  */
/*��  ����data������                               */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void u2_TxData(unsigned char *data,char len)
{
	int	i;	
	while((USART2->SR&0X40)==0);
	for(i = 0;i < len;i ++){			
		USART2->DR = data[i];
		while((USART2->SR&0X40)==0);	
	}
}

//void USART2_IRQHandler(void)   
//{                      
//  uint8_t ch;
//  
//  //�����ж�
//  if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
//  {
//    ch = USART_ReceiveData(USART2);

//    //�����յ��ֽ����ݴ���uart_receive_input
//    uart_receive_input(ch);                                                      //ʹ��TIMx
//  }
//}
 
void USART2_IRQHandler(void)
{ 
	 unsigned char Res=0;
	if((USART2->SR&USART_FLAG_RXNE) != 0)
    {
        Res=USART2->DR;
        uart_receive_input(Res);
    }
	
}

void UART2_SendByte(unsigned char data)
{
    while((USART2->SR&USART_FLAG_TXE)!=USART_FLAG_TXE);
    USART2->DR = data;
}
