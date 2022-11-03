#ifndef _FONT_H
#define _FONT_H

#include "stm32f4xx.h"


#define DMABUFFSIZE 4096

//状态标志  FREE:空闲态  DMAIT:传输态  USARTIT:完成态
typedef enum
{
	FREE,
	DMAIT,
	USARTIT,
}sta;


typedef struct
{
	u8 dma_buff[2][4096];//双缓冲数组
	u8 flag;//传输状态标志
	u8 buff_flag;//数组下标切换标志
	u16 len;//数据长度
	u32 addr;//地址
}REV;
extern REV rev;

void Font_Dma_Config(void);
void Font_Load(void);

#endif

