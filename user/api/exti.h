#ifndef _EXTI_H 
#define _EXTI_H

#include "stm32f4xx.h"



void EXTI_Config(void);

extern	u8 rev_buf[15];
extern	u8 key_buf[15];
extern u8 mg200_flag;
extern u8 key_flag;
extern u8 lock_flag;
extern u8 lcd_flag;
extern u8 key2_flag;
extern u8 gif_flag;





#endif

