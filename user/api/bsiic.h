#ifndef _BSIIC_H 
#define _BSIIC_H

#include "stm32f4xx.h"



#define BSIIC_SCL_H PBout(6)=1
#define BSIIC_SCL_L PBout(6)=0
#define BSIIC_SDA_H PBout(7)=1
#define BSIIC_SDA_L PBout(7)=0

#define BSIIC_SDA   PBin(7)
#define BS_IRQ      PAin(1)

#define ACK  0
#define NACK 1



void BSIIC_Start(void);
void BSIIC_Stop(void);
void BSIIC_Send_Ack(u8 ack);
u8 BSIIC_Rev_Ack(void);
u8 BSIIC_Send_Data(u8 data);
u8 BSIIC_Rev_Data(u8 ack);


#endif

