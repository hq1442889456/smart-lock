//#ifndef _ESP32_H
//#define _ESP32_H

//#include "stm32f4xx.h"


//typedef struct
//{
//	u8 buff[1024];//接收数据的数组
//	u8 flag;//接收标志
//	u16 len;
//}USA2_REC;


//extern USA2_REC esp32rec;


//void USART2_Config(void);
//void USART2_Send_Str(u8 *data);
//u8 Esp_Send_And_Ack(u8 *cmd,u8 *eack,u32 timeout);
//void Esp_Exit(void);
//u8 Esp_RESTORE(void);
//void Esp_Init(void);
//u8 Esp_Connect_AP(u8 *ssid,u8 *pwd);
//u8 Esp_Connect_Tcp(u8 *ip,u32 id);


//#endif



#ifndef _ESP32_H
#define _ESP32_H


#include "stm32f4xx.h"
#include "string.h"
#include "stdio.h"
#include "delay.h"
#include "mqtt.h"
#include "led.h"
#include "lcd.h"
#include "voice.h"
#include "motor.h"

typedef struct
{
	u8 buff[1024];//接收数据的数组
	u8 flag;//接收标志
	u16 len;
}USA2_REC;

extern USA2_REC esp32rec;
extern u8 errorCode;//mqtt错误码
extern char string[520];
extern u8 lock_flag;
extern u8 lcd_flag;//清屏标志位
extern u8 page_flag;

void usart2_config(void);
void usart2_send_str(u8 *data);
u8 Esp_Send_And_Ack(u8 *cmd,u8 *eack,u32 timeout);
void Esp_Exit(void);
u8 Esp_RESTORE(void);
void Esp_Init(void);
u8 Esp_Connect_AP(u8 *ssid,u8 *pwd);
u8 Esp_Connect_Tcp(u8 *ip,u32 id);


void ESP32_Subscription(void);
void ESP32_Publish(u8 status);
void ESP32_Open_Lock(void);
void ESP32_Close_Lock(void);

#endif


