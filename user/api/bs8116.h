#ifndef _BS8116_H
#define _BS8116_H

#include "stm32f4xx.h"

#define BS_ADDR_R 0xa1
#define BS_ADDR_W 0xa0

extern u8 rev_buf[15];//�洢�����������RFID��IC����
extern u8 key_buf[15];//�洢���������ַ���
extern u8 passward[15];//���ڴ���޸ĺ�ĵ�һ�����������
extern u8 passward1[15];//���ڴ���޸ĺ�ĵڶ�����������
extern u8 password_flag;//�޸ĵ��������벻һ�»���һ

extern u8 page_flag;


void BS8116_Config(void);
u8 Get_BsKey_Value(void);


void BS_Open_Lock(void);
void BS_Change_Password(void);

#endif

