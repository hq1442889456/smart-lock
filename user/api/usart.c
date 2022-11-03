#include "main.h"

/*
Function name:Usart_Config
Description:串口初始化
param:brr 波特率
retval:None
Remarks:
PA9:复用推挽输出
PA10:复用输入
*/
void Usart_Config(u32 brr)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);//打开PA时钟
	GPIO_InitTypeDef GPIO_InitStruct;//定义IO口初始化结构体变量
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;//复用模式
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;//PA9和PA10引脚
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;//无上下拉
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;//输出速度
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);//引脚复用
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);//引脚复用   要在初始化GPIO结构体之前配置，否则会在打印前多一个？
	GPIO_Init(GPIOA,&GPIO_InitStruct);//使能IO口初始化结构体
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//串口时钟使能
	USART_InitTypeDef USART_InitStruct={0};//定义串口初始化结构体变量
	USART_InitStruct.USART_BaudRate = brr;//波特率
//	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//没有硬件流控制，系统默认是需要，现在需要关闭
	USART_InitStruct.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;//发送和接收器使能
	USART_InitStruct.USART_Parity = USART_Parity_No;//无奇偶校验
	USART_InitStruct.USART_StopBits = USART_StopBits_1;//一位停止位
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;//8位数据位
	USART_Init(USART1,&USART_InitStruct);//使能串口初始化结构体
	USART_Cmd(USART1,ENABLE);//串口使能
}

void Usart_Echo(void)
{
	u8 data;
	while(USART_GetFlagStatus(USART1,USART_FLAG_RXNE)==RESET);
	data=USART_ReceiveData(USART1);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
	USART_SendData(USART1,data);
}




//重定向数据流
int fputc(int c, FILE * stream)
{
	while(!(USART1->SR & (1<<7)));
	USART1->DR=c;
	return c;
}

	




