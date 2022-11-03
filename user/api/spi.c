#include "main.h"                  // Device header

/*
Function name:SPI1_Config
Description:SPI1初始化
param:None
retval:None
Remarks:
PA4--FLASH_SPI1_CS     通用推挽输出    片选线是不经过SPI的，通过直接IO输出高低控制从机的片选
PA5--FLASH_SPI1_SCLK	 复用推挽输出
PA6--FLASH_SPI1_MISO   复用输入
PA7--FLASH_SPI1_MOSI	 复用推挽输出	
*/
void SPI1_Config(void)
{
	//时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);//打开PA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);//SPI1时钟使能
	//IO
	GPIO_InitTypeDef GPIO_InitStruct={0};//定义IO口初始化结构体变量
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;//输出速度为高速
	GPIO_Init(GPIOA,&GPIO_InitStruct);//使能IO口初始化结构体
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;//复用模式
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;//PA5和PA6和PA7引脚
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF_SPI1);//引脚复用   
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_SPI1);//引脚复用
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_SPI1);//引脚复用   
	GPIO_Init(GPIOA,&GPIO_InitStruct);//使能IO口初始化结构体
	//SPI 库函数
//	SPI_InitTypeDef spi_InitTypeDef = {0};
//	spi_InitTypeDef.SPI_CPHA = SPI_CPHA_1Edge;//SPI模式0
//	spi_InitTypeDef.SPI_CPOL = SPI_CPOL_Low;
//	spi_InitTypeDef.SPI_Mode = SPI_Mode_Master;//主模式
//	spi_InitTypeDef.SPI_NSS = SPI_NSS_Soft;//软件从器件管理
//	SPI_Init(SPI1,&spi_InitTypeDef);
//	SPI_Cmd(SPI1,ENABLE);
//	//SPI 寄存器   比库函数方便
	SPI1->CR1 |= (1<<2);//主模式
	SPI1->CR1 |= (3<<8);//从器件管理  有从器件得使能，开始先软件给高，之后拉高拉低片选线IO电平控制片选
	SPI1->CR1 |= (1<<6);//使能SPI
}



/*
Function name:SPI1_Echo
Description:SPI1发送接收函数
param:u8 
retval:u8
Remarks:
先发送后接收 因为从器件不能主动发数据，需要主机先发指令在接受数据
*/
u8 SPI1_Echo(u8 data)
{
	while(!(SPI1->SR&(1<<1)));//等待发送完成
	SPI1->DR = data;
	while(!(SPI1->SR&(1<<0)));//等待接收完成
	return SPI1->DR;
}












