#include "main.h"                  // Device header

/*
Function name:BSIIC_Start
Description:IIC��ʼ�źź���
param:None
retval:None
Remarks:ʱ����Ϊ�ߵ�ƽ�������߳����½���
*/
void BSIIC_Start(void)
{
	BSIIC_SCL_L;
	TIM5_Delay_Us(4);//����ʱ���ߣ����Զ�������

	BSIIC_SDA_H;
	TIM5_Delay_Us(4);
	BSIIC_SCL_H;
	TIM5_Delay_Us(4);
	BSIIC_SDA_L;
	TIM5_Delay_Us(4);
}

/*
Function name:BSIIC_Start
Description:IICֹͣ�źź���
param:None
retval:None
Remarks:ʱ����Ϊ�ߵ�ƽ�������߳���������
*/
void BSIIC_Stop(void)
{
	BSIIC_SCL_L;
	TIM5_Delay_Us(4);//����ʱ���ߣ����Զ�������
	
	BSIIC_SDA_L;
	TIM5_Delay_Us(4);
	BSIIC_SCL_H;
	TIM5_Delay_Us(4);
	BSIIC_SDA_H;
	TIM5_Delay_Us(4);
}


/*
Function name:BSIIC_Send_Ack
Description:IIC����Ӧ��
param:ack �����͵�Ӧ��
retval:None
Remarks:
SCL�͵�ƽ���ڷ�������
1:��Ӧ��   0:Ӧ��
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
Description:IIC����Ӧ��
param:None
retval:���յ���Ӧ��
Remarks:
SCL�ߵ�ƽ���ڽ�������
1:��Ӧ��   0:Ӧ��
*/
u8 BSIIC_Rev_Ack(void)
{
	u8 ack = ACK;
	BSIIC_SDA_H;//�ж�����ߣ��������ģʽ
	BSIIC_SCL_L;//���������ӻ�����ʱ���ߣ��ôӻ����Զ�������
	TIM5_Delay_Us(4);
	BSIIC_SCL_H;//һ�����ڣ��ӻ�����Ӧ��
	if(BSIIC_SDA) 
	{
		ack=NACK;
	}
	TIM5_Delay_Us(4);
	return ack;
}

/*
Function name:BSIIC_Send_Data
Description:IIC��������
param:ack �����͵�����
retval:Ӧ��
Remarks:
�����ȷ���λ(MSB)
*/
u8 BSIIC_Send_Data(u8 data)
{
	u8 ack = ACK;
	u8 i;
	for(i=0;i<8;i++)
	{
		BSIIC_SCL_L;//�������ı��������Է�������
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
Description:IIC��������
param:Ӧ��
retval:���յ�������
Remarks:
�����ȷ���λ(MSB)
*/
u8 BSIIC_Rev_Data(u8 ack)
{
	u8 i;
	u8 data=0;
	BSIIC_SDA_H;//�ж�����ߣ�תΪ����ģʽ
	for(i=0;i<8;i++)
	{
		BSIIC_SCL_L;
		TIM5_Delay_Us(4);
		BSIIC_SCL_H;
		data<<=1;
		if(BSIIC_SDA)   //���ɷŵ���ʱ֮�󣬲�Ȼʱ��������
		{
			data|=(0x01);
		}
		TIM5_Delay_Us(4);
		
	}
	BSIIC_Send_Ack(ack);
	return data;
}
