#include "main.h"                  // Device header

/*
Function name:ATIIC_Start
Description:IIC起始信号函数
param:None
retval:None
Remarks:时钟线为高电平，数据线出现下降沿
*/
void ATIIC_Start(void)
{
	ATIIC_SCL_L;
	TIM5_Delay_Us(4);//拉低时钟线，可以动数据线

	ATIIC_SDA_H;
	TIM5_Delay_Us(4);
	ATIIC_SCL_H;
	TIM5_Delay_Us(4);
	ATIIC_SDA_L;
	TIM5_Delay_Us(4);
}

/*
Function name:ATIIC_Stop
Description:IIC停止信号函数
param:None
retval:None
Remarks:时钟线为高电平，数据线出现上升沿
*/
void ATIIC_Stop(void)
{
	ATIIC_SCL_L;
	TIM5_Delay_Us(4);//拉低时钟线，可以动数据线
	
	ATIIC_SDA_L;
	TIM5_Delay_Us(4);
	ATIIC_SCL_H;
	TIM5_Delay_Us(4);
	ATIIC_SDA_H;
	TIM5_Delay_Us(4);
}


/*
Function name:ATIIC_Send_Ack
Description:IIC发送应答
param:ack 待发送的应答
retval:None
Remarks:
SCL低电平周期发送数据
1:非应答   0:应答
*/
void ATIIC_Send_Ack(u8 ack)
{
	ATIIC_SCL_L;
	TIM5_Delay_Us(4);
	if(ack==NACK)
	{
		ATIIC_SDA_H;
	}
	else
	{
		ATIIC_SDA_L;
	}
	TIM5_Delay_Us(4);
	ATIIC_SCL_H;
	TIM5_Delay_Us(4);
}

/*
Function name:ATIIC_Rev_Ack
Description:IIC接收应答
param:None
retval:接收到的应答
Remarks:
SCL高电平周期接收数据
1:非应答   0:应答
*/
u8 ATIIC_Rev_Ack(void)
{
	u8 ack = ACK;
	ATIIC_SDA_H;//切断输出线，变成输入模式 即时动作，不可延时
	ATIIC_SCL_L;//主机帮助从机拉低时钟线，让从机可以动数据线
	TIM5_Delay_Us(4);
	ATIIC_SCL_H;//一个周期，从机发送应答
	if(ATIIC_SDA)//时钟线高电平读
	{
		ack=NACK;
	}
	TIM5_Delay_Us(4);
	
	return ack;
}

/*
Function name:ATIIC_Send_Data
Description:IIC发送数据
param:ack 待发送的数据
retval:应答
Remarks:
数据先发高位(MSB)
*/
u8 ATIIC_Send_Data(u8 data)
{
	u8 ack = ACK;
	u8 i;
	for(i=0;i<8;i++)
	{
		ATIIC_SCL_L;//让主机改变数据线以发送数据
		TIM5_Delay_Us(4);
		if(data&(0x80>>i))
		{
			ATIIC_SDA_H;
		}
		else
		{
			ATIIC_SDA_L;
		}
		TIM5_Delay_Us(4);
		ATIIC_SCL_H;
		TIM5_Delay_Us(4);
	}
	ack = ATIIC_Rev_Ack();
	return ack;
}
/*
Function name:ATIIC_Rev_Data
Description:IIC接收数据
param:应答
retval:接收到的数据
Remarks:
数据先发高位(MSB)
*/
u8 ATIIC_Rev_Data(u8 ack)
{
	u8 i;
	u8 data=0;
	ATIIC_SDA_H;//切断输出线，转为输入模式 即时动作，不可延时
	for(i=0;i<8;i++)
	{
		ATIIC_SCL_L;//主机帮助从机拉低时钟线，让从机可以动数据线
		TIM5_Delay_Us(4);
		ATIIC_SCL_H;
		data<<=1;
		if(ATIIC_SDA)//时钟线高电平可以读（接收）数据线
		{
			data|=(0x01);
		}
		TIM5_Delay_Us(4);
		
	}
	ATIIC_Send_Ack(ack);
	return data;
}
