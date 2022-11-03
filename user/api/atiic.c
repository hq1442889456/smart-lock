#include "main.h"                  // Device header

/*
Function name:ATIIC_Start
Description:IIC��ʼ�źź���
param:None
retval:None
Remarks:ʱ����Ϊ�ߵ�ƽ�������߳����½���
*/
void ATIIC_Start(void)
{
	ATIIC_SCL_L;
	TIM5_Delay_Us(4);//����ʱ���ߣ����Զ�������

	ATIIC_SDA_H;
	TIM5_Delay_Us(4);
	ATIIC_SCL_H;
	TIM5_Delay_Us(4);
	ATIIC_SDA_L;
	TIM5_Delay_Us(4);
}

/*
Function name:ATIIC_Stop
Description:IICֹͣ�źź���
param:None
retval:None
Remarks:ʱ����Ϊ�ߵ�ƽ�������߳���������
*/
void ATIIC_Stop(void)
{
	ATIIC_SCL_L;
	TIM5_Delay_Us(4);//����ʱ���ߣ����Զ�������
	
	ATIIC_SDA_L;
	TIM5_Delay_Us(4);
	ATIIC_SCL_H;
	TIM5_Delay_Us(4);
	ATIIC_SDA_H;
	TIM5_Delay_Us(4);
}


/*
Function name:ATIIC_Send_Ack
Description:IIC����Ӧ��
param:ack �����͵�Ӧ��
retval:None
Remarks:
SCL�͵�ƽ���ڷ�������
1:��Ӧ��   0:Ӧ��
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
Description:IIC����Ӧ��
param:None
retval:���յ���Ӧ��
Remarks:
SCL�ߵ�ƽ���ڽ�������
1:��Ӧ��   0:Ӧ��
*/
u8 ATIIC_Rev_Ack(void)
{
	u8 ack = ACK;
	ATIIC_SDA_H;//�ж�����ߣ��������ģʽ ��ʱ������������ʱ
	ATIIC_SCL_L;//���������ӻ�����ʱ���ߣ��ôӻ����Զ�������
	TIM5_Delay_Us(4);
	ATIIC_SCL_H;//һ�����ڣ��ӻ�����Ӧ��
	if(ATIIC_SDA)//ʱ���߸ߵ�ƽ��
	{
		ack=NACK;
	}
	TIM5_Delay_Us(4);
	
	return ack;
}

/*
Function name:ATIIC_Send_Data
Description:IIC��������
param:ack �����͵�����
retval:Ӧ��
Remarks:
�����ȷ���λ(MSB)
*/
u8 ATIIC_Send_Data(u8 data)
{
	u8 ack = ACK;
	u8 i;
	for(i=0;i<8;i++)
	{
		ATIIC_SCL_L;//�������ı��������Է�������
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
Description:IIC��������
param:Ӧ��
retval:���յ�������
Remarks:
�����ȷ���λ(MSB)
*/
u8 ATIIC_Rev_Data(u8 ack)
{
	u8 i;
	u8 data=0;
	ATIIC_SDA_H;//�ж�����ߣ�תΪ����ģʽ ��ʱ������������ʱ
	for(i=0;i<8;i++)
	{
		ATIIC_SCL_L;//���������ӻ�����ʱ���ߣ��ôӻ����Զ�������
		TIM5_Delay_Us(4);
		ATIIC_SCL_H;
		data<<=1;
		if(ATIIC_SDA)//ʱ���߸ߵ�ƽ���Զ������գ�������
		{
			data|=(0x01);
		}
		TIM5_Delay_Us(4);
		
	}
	ATIIC_Send_Ack(ack);
	return data;
}
