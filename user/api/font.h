#ifndef _FONT_H
#define _FONT_H

#include "stm32f4xx.h"


#define DMABUFFSIZE 4096

//״̬��־  FREE:����̬  DMAIT:����̬  USARTIT:���̬
typedef enum
{
	FREE,
	DMAIT,
	USARTIT,
}sta;


typedef struct
{
	u8 dma_buff[2][4096];//˫��������
	u8 flag;//����״̬��־
	u8 buff_flag;//�����±��л���־
	u16 len;//���ݳ���
	u32 addr;//��ַ
}REV;
extern REV rev;

void Font_Dma_Config(void);
void Font_Load(void);

#endif

