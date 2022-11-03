#ifndef _SPI_H
#define _SPI_H

#include "stm32f4xx.h"

#define W25_CS PAout(4) 



void SPI1_Config(void);
u8 SPI1_Echo(u8 data);







#endif




