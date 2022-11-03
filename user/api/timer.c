#include "main.h"                  // Device header


/*
Function name:TIM5_Delay_Us
Description:��ʱ��5 ΢����ʱ
param:u16
retval:None
Remarks:APB1--84M 84��Ƶ 1��/us ARR32λ ������ʱ�䣺42��us==420s

*/
void TIM5_Delay_Us(u32 timeus)
{
	//ʱ�� 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);
	//ʱ����Ԫ
	TIM_TimeBaseInitTypeDef TIM5_TimeBaseInitStruct={0};
//	TIM5_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;//�����˲�������Ƶ  ����������нṹ���ԱĬ��ֵ��λΪĬ��ֵ������Ҫ��д������һ����Ĭ��ֵ�����Բ�д�Ļ����ر�˵��
	TIM5_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;//��������
	TIM5_TimeBaseInitStruct.TIM_Period = timeus*1-1;//����ֵ
	TIM5_TimeBaseInitStruct.TIM_Prescaler = 84 -1;//Ԥ��Ƶֵ
	TIM_TimeBaseInit(TIM5,&TIM5_TimeBaseInitStruct);//��ʼ���ṹ��
//	TIM_SelectOnePulseMode(TIM5,TIM_OPMode_Single);//������ģʽ
//	TIM_SetCounter(TIM5,0);//����������
	TIM_Cmd(TIM5,ENABLE);//�򿪼�����
	TIM_ClearFlag(TIM5,TIM_FLAG_Update);//�����־λ   ��ֹ��־λ�쳣
	while(TIM_GetFlagStatus(TIM5,TIM_FLAG_Update)==RESET);//�ȴ��������
	TIM_Cmd(TIM5,DISABLE);//�رռ�����
	
}


/*
Function name:TIM5_Delay_Ms
Description:��ʱ��5 ������ʱ
param:u16
retval:None
Remarks:APB1--84M 84��Ƶ --1000��/ms--1��/us ARR32λ ������ʱ�䣺42��us==420000ms

*/
void TIM5_Delay_Ms(u32 timems)
{
	//ʱ�� 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);
	//ʱ����Ԫ
	TIM_TimeBaseInitTypeDef TIM5_TimeBaseInitStruct={0};
	TIM5_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;//�����˲�������Ƶ  ����������нṹ���ԱĬ��ֵΪĬ��ֵ������Ҫ��д������һ����Ĭ��ֵ�����Բ�д�Ļ����ر�˵��
	TIM5_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;//��������
	TIM5_TimeBaseInitStruct.TIM_Period = timems*1000-1;//����ֵ
	TIM5_TimeBaseInitStruct.TIM_Prescaler = 84 -1;//Ԥ��Ƶֵ
	TIM_TimeBaseInit(TIM5,&TIM5_TimeBaseInitStruct);//��ʼ���ṹ��
//	TIM_SelectOnePulseMode(TIM5,TIM_OPMode_Single);//������ģʽ
//	TIM_SetCounter(TIM5,0);//����������
	TIM_ClearFlag(TIM5,TIM_FLAG_Update);//�����־λ  ��ֹ��־λ�쳣
	TIM_Cmd(TIM5,ENABLE);//�򿪼�����
	while(TIM_GetFlagStatus(TIM5,TIM_FLAG_Update)==RESET);//�ȴ��������
	TIM_Cmd(TIM5,DISABLE);//�رռ�����
}



void TIM9_MsIT_Config(void)
{
	TIM_TimeBaseInitTypeDef TIM9_TimeBaseInitStruct={0};
	NVIC_InitTypeDef nvic_InitStruct={0};
	
	//ʱ�� 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9,ENABLE);
	
	TIM9_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
	TIM9_TimeBaseInitStruct.TIM_Period = 1000-1;//����ֵ
	TIM9_TimeBaseInitStruct.TIM_Prescaler = 84 -1;//Ԥ��Ƶֵ
	TIM_TimeBaseInit(TIM9,&TIM9_TimeBaseInitStruct);//��ʼ���ṹ��
	TIM_ClearFlag(TIM9,TIM_FLAG_Update);//�����־λ  ��ֹ��־λ�쳣
	
	TIM_ITConfig(TIM9,TIM_IT_Update,ENABLE);
	nvic_InitStruct.NVIC_IRQChannel=TIM1_BRK_TIM9_IRQn;
	nvic_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	nvic_InitStruct.NVIC_IRQChannelPreemptionPriority=1;
	nvic_InitStruct.NVIC_IRQChannelSubPriority=2;
	NVIC_Init(&nvic_InitStruct);
	
	TIM_Cmd(TIM9,DISABLE);//�رռ�����������Ҫ������ʱ���ڴ�
}


u8 wait_flag1;
u8 wait_flag2;
u32 count;
u16 cnt;
void TIM1_BRK_TIM9_IRQHandler(void)
{	
	cnt++;
	if(TIM_GetITStatus(TIM9,TIM_IT_Update)==SET)
	{
		TIM_ClearITPendingBit(TIM9,TIM_IT_Update);
//		if(cnt==2000)
//		{
//			cnt=0;
//			LED3_OVERTURN;
//		}
		
		if(wait_flag2)
		{
			count++;
			if(count==10000)//ʮ��û�����Զ��˳�
			{
				count=0;
				wait_flag1=1;
				TIM9->CNT=0;
				TIM_Cmd(TIM9,DISABLE);//�رռ�����������Ҫ������ʱ���ڴ�
			}
		}
		
	}
}

