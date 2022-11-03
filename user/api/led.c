#include "led.h"

/*
Function name:Led_Config
Description:灯初始化
param:None
retval:None
Remarks:
LED_4--PB8
LED_3--PB9       给低低电平灯亮
*/
void Led_Config(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);//PB时钟使能
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	LED3_OFF;//起始灭
	LED4_OFF;
}

