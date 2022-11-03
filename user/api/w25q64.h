#ifndef _W25Q64_H
#define _W25Q64_H

#include "stm32f4xx.h"




void W25Q_Read_ID(void);
void W25_Write_Enable(void);
void W25_Write_Disable(void);
u8 W25_Read_Status(void);
void W25Q_Page_Write(u32 addr,u8 *buf,u16 len);
void W25Q_Cross_Write(u32 addr,u8 *buf,u32 len);
void W25_Read_Data(u32 addr,u8 *buf,u32 len);
void W25Q_Sector_Erase(u32 addr);
void W25Q_Block_Erase(u32 addr);
void W25Q_Piece_Erase(void);



#endif




