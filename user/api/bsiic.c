#include "main.h"                  // Device header

/*
Function name:BSIIC_Start
Description:IIC起始信号函数
param:None
retval:None
Remarks:时钟线为高电平，数据线出现下降沿
*/
void BSIIC_Start(void)
{
	BSIIC_SCL_L;
	TIM5_Delay_Us(4);//拉低时钟线，可以动数据线

	BSIIC_SDA_H;
	TIM5_Delay_Us(4);
	BSIIC_SCL_H;
	TIM5_Delay_Us(4);
	BSIIC_SDA_L;
	TIM5_Delay_Us(4);
}

/*
Function name:BSIIC_Start
Description:IIC停止信号函数
param:None
retval:None
Remarks:时钟线为高电平，数据线出现上升沿
*/
void BSIIC_Stop(void)
{
	BSIIC_SCL_L;
	TIM5_Delay_Us(4);//拉低时钟线，可以动数据线
	
	BSIIC_SDA_L;
	TIM5_Delay_Us(4);
	BSIIC_SCL_H;
	TIM5_Delay_Us(4);
	BSIIC_SDA_H;
	TIM5_Delay_Us(4);
}


/*
Function name:BSIIC_Send_Ack
Description:IIC发送应答
param:ack 待发送的应答
retval:None
Remarks:
SCL低电平周期发送数据
1:非应答   0:应答
*/
void BSIIC_Send_Ack(u8 ack)
{
	BSIIC_SCL_L;
	TIM5_Delay_Us(4);
	if(ack==NACK)
	{
		BSIIC_SDA_H;
	}
	else
	{
		BSIIC_SDA_L;
	}
	TIM5_Delay_Us(4);
	BSIIC_SCL_H;
	TIM5_Delay_Us(4);
}

/*
Function name:BSIIC_Rev_Ack
Description:IIC接收应答
param:None
retval:接收到的应答
Remarks:
SCL高电平周期接收数据
1:非应答   0:应答
*/
u8 BSIIC_Rev_Ack(void)
{
	u8 ack = ACK;
	BSIIC_SDA_H;//切断输出线，变成输入模式
	BSIIC_SCL_L;//主机帮助从机拉低时钟线，让从机可以动数据线
	TIM5_Delay_Us(4);
	BSIIC_SCL_H;//一个周期，从机发送应答
	if(BSIIC_SDA) 
	{
		ack=NACK;
	}
	TIM5_Delay_Us(4);
	return ack;
}

/*
Function name:BSIIC_Send_Data
Description:IIC发送数据
param:ack 待发送的数据
retval:应答
Remarks:
数据先发高位(MSB)
*/
u8 BSIIC_Send_Data(u8 data)
{
	u8 ack = ACK;
	u8 i;
	for(i=0;i<8;i++)
	{
		BSIIC_SCL_L;//让主机改变数据线以发送数据
		TIM5_Delay_Us(4);
		if(data&(0x80>>i))
		{
			BSIIC_SDA_H;
		}
		else
		{
			BSIIC_SDA_L;
		}
		TIM5_Delay_Us(4);
		BSIIC_SCL_H;
		TIM5_Delay_Us(4);
	}
	ack = BSIIC_Rev_Ack();
	return ack;
}
/*
Function name:BSIIC_Rev_Data
Description:IIC接收数据
param:应答
retval:接收到的数据
Remarks:
数据先发高位(MSB)
*/
u8 BSIIC_Rev_Data(u8 ack)
{
	u8 i;
	u8 data=0;
	BSIIC_SDA_H;//切断输出线，转为输入模式
	for(i=0;i<8;i++)
	{
		BSIIC_SCL_L;
		TIM5_Delay_Us(4);
		BSIIC_SCL_H;
		data<<=1;
		if(BSIIC_SDA)   //不可放到延时之后，不然时序会出问题
		{
			data|=(0x01);
		}
		TIM5_Delay_Us(4);
		
	}
	BSIIC_Send_Ack(ack);
	return data;
}
