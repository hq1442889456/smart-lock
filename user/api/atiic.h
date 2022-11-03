#ifndef _ATIIC_H 
#define _ATIIC_H

#include "stm32f4xx.h"



#define ATIIC_SCL_H PAout(8)=1
#define ATIIC_SCL_L PAout(8)=0
#define ATIIC_SDA_H PCout(9)=1
#define ATIIC_SDA_L PCout(9)=0

#define ATIIC_SDA   PCin(9)


#define ACK  0
#define NACK 1



void ATIIC_Start(void);
void ATIIC_Stop(void);
void ATIIC_Send_Ack(u8 ack);
u8 ATIIC_Rev_Ack(void);
u8 ATIIC_Send_Data(u8 data);
u8 ATIIC_Rev_Data(u8 ack);


#endif

