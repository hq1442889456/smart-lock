#include "main.h"                  // Device header

/*
Function name:SPI1_Config
Description:SPI1��ʼ��
param:None
retval:None
Remarks:
PA4--FLASH_SPI1_CS     ͨ���������    Ƭѡ���ǲ�����SPI�ģ�ͨ��ֱ��IO����ߵͿ��ƴӻ���Ƭѡ
PA5--FLASH_SPI1_SCLK	 �����������
PA6--FLASH_SPI1_MISO   ��������
PA7--FLASH_SPI1_MOSI	 �����������	
*/
void SPI1_Config(void)
{
	//ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);//��PAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);//SPI1ʱ��ʹ��
	//IO
	GPIO_InitTypeDef GPIO_InitStruct={0};//����IO�ڳ�ʼ���ṹ�����
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;//����ٶ�Ϊ����
	GPIO_Init(GPIOA,&GPIO_InitStruct);//ʹ��IO�ڳ�ʼ���ṹ��
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;//����ģʽ
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;//PA5��PA6��PA7����
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF_SPI1);//���Ÿ���   
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_SPI1);//���Ÿ���
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_SPI1);//���Ÿ���   
	GPIO_Init(GPIOA,&GPIO_InitStruct);//ʹ��IO�ڳ�ʼ���ṹ��
	//SPI �⺯��
//	SPI_InitTypeDef spi_InitTypeDef = {0};
//	spi_InitTypeDef.SPI_CPHA = SPI_CPHA_1Edge;//SPIģʽ0
//	spi_InitTypeDef.SPI_CPOL = SPI_CPOL_Low;
//	spi_InitTypeDef.SPI_Mode = SPI_Mode_Master;//��ģʽ
//	spi_InitTypeDef.SPI_NSS = SPI_NSS_Soft;//�������������
//	SPI_Init(SPI1,&spi_InitTypeDef);
//	SPI_Cmd(SPI1,ENABLE);
//	//SPI �Ĵ���   �ȿ⺯������
	SPI1->CR1 |= (1<<2);//��ģʽ
	SPI1->CR1 |= (3<<8);//����������  �д�������ʹ�ܣ���ʼ��������ߣ�֮����������Ƭѡ��IO��ƽ����Ƭѡ
	SPI1->CR1 |= (1<<6);//ʹ��SPI
}



/*
Function name:SPI1_Echo
Description:SPI1���ͽ��պ���
param:u8 
retval:u8
Remarks:
�ȷ��ͺ���� ��Ϊ�������������������ݣ���Ҫ�����ȷ�ָ���ڽ�������
*/
u8 SPI1_Echo(u8 data)
{
	while(!(SPI1->SR&(1<<1)));//�ȴ��������
	SPI1->DR = data;
	while(!(SPI1->SR&(1<<0)));//�ȴ��������
	return SPI1->DR;
}












