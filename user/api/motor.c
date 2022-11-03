#include "main.h"                  // Device header

/*
Function name:Motor_Config
Description:�����ʼ��
param:None
retval:None
Remarks:
IB--PB3
IA--PB4      ���͵͵�ƽ����
*/
void Motor_Config(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);//PBʱ��ʹ��
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	Close_Door();//��ʼ�ǹ�
//	IA_L;//��ʼ��ֹͣת��������������һֱת
//	IB_L;
}

/*
Function name:Motor_State
Description:���������״̬
param:u8 ia,u8 ib
retval:None
Remarks:
IB--PB3
IA--PB4      ���͵͵�ƽ����
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
	Delay_Ms(300);//�ȴ���ת�����
	IA_L;//ֹͣת��������������һֱת
	IB_L;
}

/*
Function name:Motor_Control
Description:�����ͣ���л�״̬
param:u8 ia,u8 ib
retval:None
Remarks:
IB--PB3
IA--PB4      ���͵͵�ƽ����
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
