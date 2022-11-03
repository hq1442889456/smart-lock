#ifndef _VOICE_H 
#define _VOICE_H

#include "stm32f4xx.h"

#define Send_H (PCout(5)=1)
#define Send_L (PCout(5)=0)
#define Recv_Busy  (PCin(4))

extern u8 volume;//起始音量最小

void Voice_Config(void);
void Voice_Work(u8 data);
void Voice_Start(void);
void Voice_Data(u8 data);

void VOICE_Set_Volume(void);

#endif

