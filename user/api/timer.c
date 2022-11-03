#include "main.h"                  // Device header


/*
Function name:TIM5_Delay_Us
Description:定时器5 微秒延时
param:u16
retval:None
Remarks:APB1--84M 84分频 1次/us ARR32位 最大计数时间：42亿us==420s

*/
void TIM5_Delay_Us(u32 timeus)
{
	//时钟 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);
	//时基单元
	TIM_TimeBaseInitTypeDef TIM5_TimeBaseInitStruct={0};
//	TIM5_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;//数字滤波器不分频  最好配完所有结构体成员默认值配位为默认值，而不要不写，它不一定是默认值，可以不写的会有特别说明
	TIM5_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;//递增计数
	TIM5_TimeBaseInitStruct.TIM_Period = timeus*1-1;//重载值
	TIM5_TimeBaseInitStruct.TIM_Prescaler = 84 -1;//预分频值
	TIM_TimeBaseInit(TIM5,&TIM5_TimeBaseInitStruct);//初始化结构体
//	TIM_SelectOnePulseMode(TIM5,TIM_OPMode_Single);//单脉冲模式
//	TIM_SetCounter(TIM5,0);//计数器清零
	TIM_Cmd(TIM5,ENABLE);//打开计数器
	TIM_ClearFlag(TIM5,TIM_FLAG_Update);//清除标志位   防止标志位异常
	while(TIM_GetFlagStatus(TIM5,TIM_FLAG_Update)==RESET);//等待计数完成
	TIM_Cmd(TIM5,DISABLE);//关闭计数器
	
}


/*
Function name:TIM5_Delay_Ms
Description:定时器5 毫秒延时
param:u16
retval:None
Remarks:APB1--84M 84分频 --1000次/ms--1次/us ARR32位 最大计数时间：42亿us==420000ms

*/
void TIM5_Delay_Ms(u32 timems)
{
	//时钟 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);
	//时基单元
	TIM_TimeBaseInitTypeDef TIM5_TimeBaseInitStruct={0};
	TIM5_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;//数字滤波器不分频  最好配完所有结构体成员默认值为默认值，而不要不写，它不一定是默认值，可以不写的会有特别说明
	TIM5_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;//递增计数
	TIM5_TimeBaseInitStruct.TIM_Period = timems*1000-1;//重载值
	TIM5_TimeBaseInitStruct.TIM_Prescaler = 84 -1;//预分频值
	TIM_TimeBaseInit(TIM5,&TIM5_TimeBaseInitStruct);//初始化结构体
//	TIM_SelectOnePulseMode(TIM5,TIM_OPMode_Single);//单脉冲模式
//	TIM_SetCounter(TIM5,0);//计数器清零
	TIM_ClearFlag(TIM5,TIM_FLAG_Update);//清除标志位  防止标志位异常
	TIM_Cmd(TIM5,ENABLE);//打开计数器
	while(TIM_GetFlagStatus(TIM5,TIM_FLAG_Update)==RESET);//等待计数完成
	TIM_Cmd(TIM5,DISABLE);//关闭计数器
}



void TIM9_MsIT_Config(void)
{
	TIM_TimeBaseInitTypeDef TIM9_TimeBaseInitStruct={0};
	NVIC_InitTypeDef nvic_InitStruct={0};
	
	//时钟 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9,ENABLE);
	
	TIM9_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
	TIM9_TimeBaseInitStruct.TIM_Period = 1000-1;//重载值
	TIM9_TimeBaseInitStruct.TIM_Prescaler = 84 -1;//预分频值
	TIM_TimeBaseInit(TIM9,&TIM9_TimeBaseInitStruct);//初始化结构体
	TIM_ClearFlag(TIM9,TIM_FLAG_Update);//清除标志位  防止标志位异常
	
	TIM_ITConfig(TIM9,TIM_IT_Update,ENABLE);
	nvic_InitStruct.NVIC_IRQChannel=TIM1_BRK_TIM9_IRQn;
	nvic_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	nvic_InitStruct.NVIC_IRQChannelPreemptionPriority=1;
	nvic_InitStruct.NVIC_IRQChannelSubPriority=2;
	NVIC_Init(&nvic_InitStruct);
	
	TIM_Cmd(TIM9,DISABLE);//关闭计数器，当需要计数的时候在打开
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
			if(count==10000)//十秒没操作自动退出
			{
				count=0;
				wait_flag1=1;
				TIM9->CNT=0;
				TIM_Cmd(TIM9,DISABLE);//关闭计数器，当需要计数的时候在打开
			}
		}
		
	}
}

