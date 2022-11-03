#ifndef _BS8116_H
#define _BS8116_H

#include "stm32f4xx.h"

#define BS_ADDR_R 0xa1
#define BS_ADDR_W 0xa0

extern u8 rev_buf[15];//存储按键密码或者RFID的IC卡号
extern u8 key_buf[15];//存储按键输入字符串
extern u8 passward[15];//用于存放修改后的第一次输入的密码
extern u8 passward1[15];//用于存放修改后的第二次输入密码
extern u8 password_flag;//修改的两次密码不一致会置一

extern u8 page_flag;


void BS8116_Config(void);
u8 Get_BsKey_Value(void);


void BS_Open_Lock(void);
void BS_Change_Password(void);

#endif

