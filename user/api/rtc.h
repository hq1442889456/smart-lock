#ifndef _RTC_H
#define _RTC_H

#include "stm32f4xx.h"

void RTC_Clender_Config(void);
ErrorStatus Set_RTC_Time(u8 h12,u8 hour,u8 min,u8 sec);
ErrorStatus Set_RTC_Date(u8 year,u8 mon,u8 date,u8 week);
void RTC_Alarm_Config(void);
void Set_Rtc_Alarm(u8 date,u8 h12,u8 hour,u8 min,u8 sec);








#endif
