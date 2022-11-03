#ifndef _LED_H
#define _LED_H

#include "stm32f4xx.h"



#define LED4_ON        GPIO_ResetBits(GPIOB,GPIO_Pin_8)
#define LED4_OFF       GPIO_SetBits(GPIOB,GPIO_Pin_8)
#define LED4_OVERTURN  GPIO_ToggleBits(GPIOB,GPIO_Pin_8)
#define LED3_ON        GPIO_ResetBits(GPIOB,GPIO_Pin_9)
#define LED3_OFF       GPIO_SetBits(GPIOB,GPIO_Pin_9)
#define LED3_OVERTURN  GPIO_ToggleBits(GPIOB,GPIO_Pin_9)

void Led_Config(void);

#endif

