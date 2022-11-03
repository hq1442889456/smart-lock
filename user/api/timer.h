#ifndef _TIMER_H
#define _TIMER_H

#include "stm32f4xx.h"


extern u32 count;
extern u8 wait_flag1;
extern u8 wait_flag2;

void TIM5_Delay_Us(u32 timeus);
void TIM5_Delay_Ms(u32 timems);
void TIM9_MsIT_Config(void);



#endif
