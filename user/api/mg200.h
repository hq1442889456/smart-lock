#ifndef _MG_200_H
#define _MG_200_H

#include "stm32f4xx.h"

#define MG200_PWR_ON   PCout(1)=1
#define MG200_PWR_OFF  PCout(1)=0
#define MG200_DETECT   PCin(0)

extern u8 page_flag;
extern u8 key_v;//µçÈÝ°´¼ü¼üÖµ
extern u8 lcd_flag;

void Usart6_Config(u32 brr);
void Usart6_Send_Byte(u8 data);
u8 Usart6_Rev_Byte(void);

void MG200_Config(void);
void MG200_Init(void);
void MG_Send_Pack(u8 cmd,u8 param);
u8 MG_Rev_Pack(u8 cmd,u8 *res,u8 *param);
u8 CaptureAndExtract(u8 parameter);
u8 Enroll(u8 parameter);
u8 EraseAll(void);
u8 Match1N(void);



void MG_Open_lock(void);
void MG_Enroll(void);
void MG_Delect_All(void);
u8 MG_Del_By_Id(void);


#endif




