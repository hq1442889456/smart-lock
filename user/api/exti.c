#include "main.h"                  // Device header
/*
Function name:EXTI_Config
Description:外部中断配置
param:None
retval:None
Remarks:
MG200_DETECT--PC0--输入  (高电平电平表示按键按下)
BS8116_IRQ--PA1--输入    (低电平表示按键按下，资料手册IRQ功能那一章有说)
*/
void EXTI_Config(void)
{
	//时钟
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);//打开PC时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);//打开PA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);//-EXTI控制器时钟使能
	//IO
	GPIO_InitTypeDef GPIO_InitStruct={0};//定义IO口初始化结构体变量
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;//输入模式
//	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;//PC0引脚
//	GPIO_Init(GPIOC,&GPIO_InitStruct);//使能IO口初始化结构体
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;//PA1引脚
	GPIO_Init(GPIOA,&GPIO_InitStruct);//使能IO口初始化结构体
	//EXTI控制器
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
	//NVIC中断优先级分组
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef nvic_InitTypeDef;
//	nvic_InitTypeDef.NVIC_IRQChannel = EXTI0_IRQn;//不可以|上EXTI1_IRQn，因为上EXTI1_IRQn是6，EXTI1_IRQn是7，或上的结果不是正确的结果
	nvic_InitTypeDef.NVIC_IRQChannelCmd = ENABLE;
	nvic_InitTypeDef.NVIC_IRQChannelPreemptionPriority = 2;
	nvic_InitTypeDef.NVIC_IRQChannelSubPriority = 2;
//	NVIC_Init(&nvic_InitTypeDef);
	nvic_InitTypeDef.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_Init(&nvic_InitTypeDef);
}


//u8 key_flag;
//u8 lock_flag = 1;
//u8 lcd_flag;//清屏标志位
//u8 key2_flag;
//u8 gif_flag=1;
//u8 key_v;
void EXTI1_IRQHandler(void)//按键中断服务函数
{
	if(EXTI_GetITStatus(EXTI_Line1)==SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line1);//清除标志位
		if(key_flag==0)//开锁方式按键
		{
			while(BS_IRQ);//等待按键按下
			if(!BS_IRQ)
			{
				key_v=Get_BsKey_Value();
				printf("3=%c\r\n",key_v);//打印键值并判断位置
				Voice_Work(0X1d);//嘟   要放在获取键值之后否则会干扰IIC时序
				switch(key_v)//根据键值选择开锁方式
				{
					case '1':key_flag=1;break;
					case '2':key_flag=2;break;
					case '3':key_flag=3;break;
				}
				while(!BS_IRQ);//抬起才能出等待
			}
		}
		if(!lock_flag)//功能选择按键
		{
			key2_flag=1;
		}
	}
	
}












