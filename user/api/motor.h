#ifndef _MOTOR_H 
#define _MOTOR_H

#include "stm32f4xx.h"

#define IA_L  (PBout(4)=0)
#define IA_H  (PBout(4)=1)
#define IB_L  (PBout(3)=0)
#define IB_H  (PBout(3)=1)

void Motor_Config(void);
void Motor_State(u8 ia,u8 ib);
void Motor_Control(u8 time);
void Open_Door(void);
void Close_Door(void);

#endif

