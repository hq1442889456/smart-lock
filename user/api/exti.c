#include "main.h"                  // Device header
/*
Function name:EXTI_Config
Description:�ⲿ�ж�����
param:None
retval:None
Remarks:
MG200_DETECT--PC0--����  (�ߵ�ƽ��ƽ��ʾ��������)
BS8116_IRQ--PA1--����    (�͵�ƽ��ʾ�������£������ֲ�IRQ������һ����˵)
*/
void EXTI_Config(void)
{
	//ʱ��
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);//��PCʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);//��PAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);//-EXTI������ʱ��ʹ��
	//IO
	GPIO_InitTypeDef GPIO_InitStruct={0};//����IO�ڳ�ʼ���ṹ�����
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;//����ģʽ
//	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;//PC0����
//	GPIO_Init(GPIOC,&GPIO_InitStruct);//ʹ��IO�ڳ�ʼ���ṹ��
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;//PA1����
	GPIO_Init(GPIOA,&GPIO_InitStruct);//ʹ��IO�ڳ�ʼ���ṹ��
	//EXTI������
//	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC,EXTI_PinSource0);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource1);
	EXTI_InitTypeDef exti_InitTypeDef;
//	exti_InitTypeDef.EXTI_Line = EXTI_Line0;
	exti_InitTypeDef.EXTI_LineCmd = ENABLE;
	exti_InitTypeDef.EXTI_Mode = EXTI_Mode_Interrupt;
//	exti_InitTypeDef.EXTI_Trigger = EXTI_Trigger_Rising;
//	EXTI_Init(&exti_InitTypeDef);//Line0
	exti_InitTypeDef.EXTI_Line = EXTI_Line1;
	exti_InitTypeDef.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&exti_InitTypeDef);//Line1
	//NVIC�ж����ȼ�����
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef nvic_InitTypeDef;
//	nvic_InitTypeDef.NVIC_IRQChannel = EXTI0_IRQn;//������|��EXTI1_IRQn����Ϊ��EXTI1_IRQn��6��EXTI1_IRQn��7�����ϵĽ��������ȷ�Ľ��
	nvic_InitTypeDef.NVIC_IRQChannelCmd = ENABLE;
	nvic_InitTypeDef.NVIC_IRQChannelPreemptionPriority = 2;
	nvic_InitTypeDef.NVIC_IRQChannelSubPriority = 2;
//	NVIC_Init(&nvic_InitTypeDef);
	nvic_InitTypeDef.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_Init(&nvic_InitTypeDef);
}


//u8 key_flag;
//u8 lock_flag = 1;
//u8 lcd_flag;//������־λ
//u8 key2_flag;
//u8 gif_flag=1;
//u8 key_v;
void EXTI1_IRQHandler(void)//�����жϷ�����
{
	if(EXTI_GetITStatus(EXTI_Line1)==SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line1);//�����־λ
		if(key_flag==0)//������ʽ����
		{
			while(BS_IRQ);//�ȴ���������
			if(!BS_IRQ)
			{
				key_v=Get_BsKey_Value();
				printf("3=%c\r\n",key_v);//��ӡ��ֵ���ж�λ��
				Voice_Work(0X1d);//�   Ҫ���ڻ�ȡ��ֵ֮���������IICʱ��
				switch(key_v)//���ݼ�ֵѡ������ʽ
				{
					case '1':key_flag=1;break;
					case '2':key_flag=2;break;
					case '3':key_flag=3;break;
				}
				while(!BS_IRQ);//̧����ܳ��ȴ�
			}
		}
		if(!lock_flag)//����ѡ�񰴼�
		{
			key2_flag=1;
		}
	}
	
}












