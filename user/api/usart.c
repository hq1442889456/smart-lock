#include "main.h"

/*
Function name:Usart_Config
Description:���ڳ�ʼ��
param:brr ������
retval:None
Remarks:
PA9:�����������
PA10:��������
*/
void Usart_Config(u32 brr)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);//��PAʱ��
	GPIO_InitTypeDef GPIO_InitStruct;//����IO�ڳ�ʼ���ṹ�����
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;//����ģʽ
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;//PA9��PA10����
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;//��������
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;//����ٶ�
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);//���Ÿ���
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);//���Ÿ���   Ҫ�ڳ�ʼ��GPIO�ṹ��֮ǰ���ã�������ڴ�ӡǰ��һ����
	GPIO_Init(GPIOA,&GPIO_InitStruct);//ʹ��IO�ڳ�ʼ���ṹ��
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//����ʱ��ʹ��
	USART_InitTypeDef USART_InitStruct={0};//���崮�ڳ�ʼ���ṹ�����
	USART_InitStruct.USART_BaudRate = brr;//������
//	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//û��Ӳ�������ƣ�ϵͳĬ������Ҫ��������Ҫ�ر�
	USART_InitStruct.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;//���ͺͽ�����ʹ��
	USART_InitStruct.USART_Parity = USART_Parity_No;//����żУ��
	USART_InitStruct.USART_StopBits = USART_StopBits_1;//һλֹͣλ
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;//8λ����λ
	USART_Init(USART1,&USART_InitStruct);//ʹ�ܴ��ڳ�ʼ���ṹ��
	USART_Cmd(USART1,ENABLE);//����ʹ��
}

void Usart_Echo(void)
{
	u8 data;
	while(USART_GetFlagStatus(USART1,USART_FLAG_RXNE)==RESET);
	data=USART_ReceiveData(USART1);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
	USART_SendData(USART1,data);
}




//�ض���������
int fputc(int c, FILE * stream)
{
	while(!(USART1->SR & (1<<7)));
	USART1->DR=c;
	return c;
}

	




