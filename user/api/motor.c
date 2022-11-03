#include "main.h"                  // Device header

/*
Function name:Motor_Config
Description:电机初始化
param:None
retval:None
Remarks:
IB--PB3
IA--PB4      给低低电平灯亮
*/
void Motor_Config(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);//PB时钟使能
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	Close_Door();//初始是关
//	IA_L;//开始是停止转动，不可以让他一直转
//	IB_L;
}

/*
Function name:Motor_State
Description:电机开关门状态
param:u8 ia,u8 ib
retval:None
Remarks:
IB--PB3
IA--PB4      给低低电平灯亮
*/
void Motor_State(u8 ia,u8 ib)
{
	if(ia==1&&ib==0)
	{
		IA_H;
		IB_L;
	}
	else if(ia==0&&ib==1)
	{
		IA_L;
		IB_H;
	}
	Delay_Ms(300);//等待门转动完成
	IA_L;//停止转动，不可以让他一直转
	IB_L;
}

/*
Function name:Motor_Control
Description:电机不停的切换状态
param:u8 ia,u8 ib
retval:None
Remarks:
IB--PB3
IA--PB4      给低低电平灯亮
*/
void Motor_Control(u8 time)
{
	Motor_State(1,0);
	Delay_Ms(1000*time);
	Motor_State(0,1);
	Delay_Ms(1000*time);
}


void Open_Door(void)
{
	Motor_State(1,0);
}

void Close_Door(void)
{
	Motor_State(0,1);
}
