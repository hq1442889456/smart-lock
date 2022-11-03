#include "main.h"                  // Device header

/*
Function name:KEY_Config
Description:按键初始化
param:None
retval:None
Remarks:
PC13--KEY2  按下为低电平
*/
void KEY_Config(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Pin =  GPIO_Pin_13;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL; 
	GPIO_Init(GPIOC,&GPIO_InitStruct);
}

/*
Function name:KEY_Scan
Description:按键检测
param:None
retval:u8
Remarks:
PC13--KEY2  按下为低电平
*/
u8 KEY_Scan(void)
{
	u8 key_v = 0;
	static u8 key_flag = 1;
	if(KEY2&&key_flag)
	{
		key_flag = 0;
		Delay_Ms(20);
		if(KEY2)
		{
			key_v = 2;
		}
	}
	if(!KEY2&&!key_flag)
	{
		key_flag = 1;
	}
	return key_v;
}






