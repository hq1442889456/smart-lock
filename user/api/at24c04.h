#ifndef _AT24C04_H 
#define _AT24C04_H

#include "stm32f4xx.h"

#define AT_ADDR_R    0xa1
#define AT_ADDR_W    0xa0

void AT24C04_Config(void);
u8 AT_Page_Write(u8 addr,u8 *buf,u8 len);
u8 AT_Random_Read(u8 addr,u8 *buf,u8 len);
u8 AT_Cross_Write(u8 addr,u8 *buf,u8 len);


#endif

