#ifndef _KEY_H
#define _KEY_H

#include "stm32f4xx.h"  //M4м╥нд╪Ч

#define KEY2  (!(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13)))

void KEY_Config(void);
u8 KEY_Scan(void);

#endif




