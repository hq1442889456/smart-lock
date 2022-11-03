#include "main.h"                  // Device header

/*
Function name:AT24C04_Config
Description:EEPROM AT24C04 IO初始化
param:None
retval:None
Remarks:
PA8--AT24C04--SCLK -- 通用推挽输出  （只有MCU可以输出时钟，故只有MCU可以拉时钟线）
PC9--AT24C04--SDA -- 通用开漏输出  （可以切换为输入）
*/
void AT24C04_Config(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);//PC时钟使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);//PA时钟使能
	
	//PA8
	GPIO_InitTypeDef gpio_InitTypeDef;
	gpio_InitTypeDef.GPIO_Mode = GPIO_Mode_OUT;
	gpio_InitTypeDef.GPIO_OType = GPIO_OType_PP;//通用推挽输出
	gpio_InitTypeDef.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOA,&gpio_InitTypeDef);
	
	//PC9
	gpio_InitTypeDef.GPIO_OType = GPIO_OType_OD;//通用开漏输出
	gpio_InitTypeDef.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOC,&gpio_InitTypeDef);
}

/*
Function name:AT_Page_Write
Description:AT24C04 页写
param:None
retval:错误码
Remarks:addr-->写入地址:0~15--第一页   16~31--第二页 ……
				buf-->待写入数据的首地址
				len-->数据长度
*/
u8 AT_Page_Write(u8 addr,u8 *buf,u8 len)
{
	u8 i;
	ATIIC_Start();
	if(ATIIC_Send_Data(AT_ADDR_W))
	{
		printf("应答1错误\r\n");
		return 1;
	}
	if(ATIIC_Send_Data(addr))
	{
		printf("应答2错误\r\n");
		return 2;
	}
	
	//老师的（效率）
//	while(len--)
//	{
//		if(ATIIC_Send_Data(*buf++))
//		{
//			printf("应答3错误\r\n");
//			return 3;
//		}
//	}
	
	//自己的（易懂）
	for(i=0;i<len;i++)//写入len个数据
	{
		if(ATIIC_Send_Data(*buf))
		{
			printf("应答3错误\r\n");
			return 3;
		}
		buf++;
	}
	ATIIC_Stop();
	TIM5_Delay_Ms(5);
	return 0;
}

/*
Function name:AT_Random_Read
Description:随机读
param:addr-->待读数据的首地址 ：0~15--第一页   16~31--第二页 ……
			buf-->读出数据存储的首地址
			len-->读的数据长度
retval:错误码
Remarks:
*/
u8 AT_Random_Read(u8 addr,u8 *buf,u8 len)
{
	u8 i;
	ATIIC_Start();
	if(ATIIC_Send_Data(AT_ADDR_W))
	{
		printf("应答4错误\r\n");
		return 1;
	}
	if(ATIIC_Send_Data(addr))
	{
		printf("应答5错误\r\n");
		return 2;
	}
	ATIIC_Start();
	if(ATIIC_Send_Data(AT_ADDR_R))
	{
		printf("应答6错误\r\n");
		return 3;
	}
	//老师的
//	len--;
//	while(len--)//让前len-1个数据接收后发送应答
//	{
//		*buf++ = ATIIC_Rev_Data(ACK);
//	}
//	*buf = ATIIC_Rev_Data(NACK);//让接收最后一个数据后发送应答非
	
	//自己的
	for(i=0;i<len-1;i++)//让前len-1个数据接收后发送应答
	{
		*buf = ATIIC_Rev_Data(ACK);
		buf++;
	}
	*buf = ATIIC_Rev_Data(NACK);//让接收最后一个数据后发送应答非
	
	ATIIC_Stop();
	return 0;
}

/*
Function name:AT_Cross_Write
Description:跨页写
param:addr-->写入地址 ：0~15--第一页   16~31--第二页 ……
			buf-->待写入数据的首地址
			len-->数据长度
retval:错误码
Remarks:
*/
u8 AT_Cross_Write(u8 addr,u8 *buf,u8 len)
{
	u8 len_rest;
	len_rest=16-addr%16;//本页剩余量
	
	//老师的
//	if(len_rest>=len)//不需要跨页
//	{
//		len_rest=len;
//	}
//	while(1)
//	{
//		AT_Page_Write(addr,buf,len_rest);
//		if(len==len_rest)
//		{
//			break;
//		}
//		len-=len_rest;
//		addr+=len_rest;
//		buf+=len_rest;
//		if(len>16)
//		{
//			len_rest=16;
//		}
//		else
//		{
//			len_rest=len;
//		}
//	}

	//自己写
	if(len_rest>=len)//不需要跨页
	{
		AT_Page_Write(addr,buf,len);//剩余量足够写数据个数，直接写给剩余空间即可
	}
	else
	{
		while(1)
		{
			AT_Page_Write(addr,buf,len_rest);//第一次进来把剩余的空间填满
			if(len==len_rest)//结束条件
			{
				break;
			}
			len-=len_rest;//页写函数结束指针偏移量会被销毁 所以需要偏移
			addr+=len_rest;
			buf+=len_rest;
			if(len>16)
			{
				len_rest = 16;
			}
			else
			{
				len_rest = len;
			}
		}
	}
	
	
	return 0;
}




