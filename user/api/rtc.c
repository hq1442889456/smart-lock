#include "main.h"                  // Device header

/*
Function name:RTC_Clender_Config
Description:RTC日历初始化
param:None
retval:None
Remarks:
*/
void RTC_Clender_Config(void)
{
	//备份域访问
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);//使能电源接口时钟
	PWR_BackupAccessCmd(ENABLE);//使能对备份域的访问
	RCC_LSICmd(ENABLE);//使能LSI时钟
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);//选择LSI为RTC时钟源
	RCC_RTCCLKCmd(ENABLE);//使能RTC时钟
	//RTC日历初始化和配置
	RTC_WriteProtectionCmd(DISABLE);//关闭写保护,才可以对RTC寄存器进行配置
	RTC_EnterInitMode();//进入初始化模式
	RTC_InitTypeDef rtc_InitTypeDef;//配置预分频结构体，生成1Hz时钟
	rtc_InitTypeDef.RTC_AsynchPrediv=0x7f;//异步128分频
	rtc_InitTypeDef.RTC_HourFormat=RTC_HourFormat_24;//24小时形式
	rtc_InitTypeDef.RTC_SynchPrediv=0xf9;//同步250分频  32K经过128*250=32K分频后是1Hz,即1次/s
	RTC_Init(&rtc_InitTypeDef);//初始化结构体
	Set_RTC_Time(RTC_H12_AM,1,2,3);//设置日历时间
	Set_RTC_Date(22,8,2,3);//设置日历日期
	RTC_ExitInitMode();//退出初始化模式
	RTC_WriteProtectionCmd(ENABLE);//打开写保护,不可以对RTC寄存器进行写值
}


/*
Function name:Set_RTC_Time
Description:设置RTC时间
param:u8 h12,u8 hour,u8 min,u8 sec
retval:ERROR = 0, SUCCESS = !ERROR
Remarks:
h12-->
RTC_H12_AM
RTC_H12_PM

*/
ErrorStatus Set_RTC_Time(u8 h12,u8 hour,u8 min,u8 sec)
{
	RTC_TimeTypeDef rtc_TimeTypeDef;
	rtc_TimeTypeDef.RTC_H12=h12;//RTC_H12_AM  RTC_H12_PM
	rtc_TimeTypeDef.RTC_Hours=hour;//时
	rtc_TimeTypeDef.RTC_Minutes=min;//分
	rtc_TimeTypeDef.RTC_Seconds=sec;//秒
	return RTC_SetTime(RTC_Format_BIN,&rtc_TimeTypeDef);//初始化时间结构体
}

/*
Function name:Set_RTC_Date
Description:设置RTC日期
param:u8 year,u8 mon,u8 date,u8 week
retval:ERROR = 0, SUCCESS = !ERROR
Remarks:year：0-99 
*/
ErrorStatus Set_RTC_Date(u8 year,u8 mon,u8 date,u8 week)
{
	RTC_DateTypeDef rtc_DateTypeDef;
	rtc_DateTypeDef.RTC_Date=date;//日
	rtc_DateTypeDef.RTC_Month=mon;//月
	rtc_DateTypeDef.RTC_WeekDay=week;//周
	rtc_DateTypeDef.RTC_Year=year;//年
	return RTC_SetDate(RTC_Format_BIN,&rtc_DateTypeDef);//初始化日期结构体
}




/*
Function name:RTC_Alarm_Config
Description:RTC闹钟初始化
param:None
retval:None
Remarks:
*/
void RTC_Alarm_Config(void)
{
	//闹钟
	RTC_AlarmCmd(RTC_Alarm_A,DISABLE);//禁止闹钟 A
	Set_Rtc_Alarm(15,RTC_H12_PM,20,56,30);//设置闹钟A的时间
	RTC_ITConfig(RTC_IT_ALRA,ENABLE);//使能闹钟A中断
//	RTC_ClearITPendingBit(RTC_IT_ALRA);//闹钟A中断标志位，防止初始化时标志为1
	RTC_AlarmCmd(RTC_Alarm_A,ENABLE);//使能闹钟A
	RTC_WriteProtectionCmd(ENABLE);//打开写保护,不可以对RTC寄存器进行写值 除了ISR状态寄存器等其它寄存器
	//中断配置
	EXTI_InitTypeDef exti_InitTypeDef;
	exti_InitTypeDef.EXTI_Line =EXTI_Line17;//EXTI 线 17 
	exti_InitTypeDef.EXTI_LineCmd=ENABLE;//使能
	exti_InitTypeDef.EXTI_Mode = EXTI_Mode_Interrupt;//中断模式
	exti_InitTypeDef.EXTI_Trigger =EXTI_Trigger_Rising;//上升沿有效
	EXTI_Init(&exti_InitTypeDef);//外部中断结构体初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置优先级分组
	NVIC_InitTypeDef nvic_InitTypeDef;
	nvic_InitTypeDef.NVIC_IRQChannel = RTC_Alarm_IRQn;//中断源
	nvic_InitTypeDef.NVIC_IRQChannelCmd = ENABLE;
	nvic_InitTypeDef.NVIC_IRQChannelPreemptionPriority = 2;//抢占优先级为2
	nvic_InitTypeDef.NVIC_IRQChannelSubPriority = 2;//次级优先级为2
	NVIC_Init(&nvic_InitTypeDef);//NVIC结构体初始化
}


/*
Function name:Set_Rtc_Alarm
Description:设置RTC闹钟
param:u8 date,u8 h12,u8 hour,u8 min,u8 sec
retval:None
Remarks:
h12-->
RTC_H12_AM
RTC_H12_PM

*/
void Set_Rtc_Alarm(u8 date,u8 h12,u8 hour,u8 min,u8 sec)
{
	RTC_AlarmTypeDef rtc_AlarmTypeDef;//编程闹钟A结构体
	rtc_AlarmTypeDef.RTC_AlarmDateWeekDay = date;//1-31的日期
	rtc_AlarmTypeDef.RTC_AlarmDateWeekDaySel=RTC_AlarmDateWeekDaySel_Date;//基于日期
//	rtc_AlarmTypeDef.RTC_AlarmMask=RTC_AlarmMask_Seconds;//只与除了秒以外设置的的日期时间有关，即与秒无关，其他日期时间匹配上那么每秒进一次闹钟中断
	rtc_AlarmTypeDef.RTC_AlarmMask=RTC_AlarmMask_Minutes;//只与除了分以外设置的的日期时间有关，即与分无关，其他日期时间匹配上每分进一次闹钟中断
//	rtc_AlarmTypeDef.RTC_AlarmMask=RTC_AlarmMask_None;//只有所有时间日期对上才会有闹钟
	//	RTC_TimeTypeDef rtc_AlarmTime;
//	rtc_AlarmTime.RTC_H12 = h12;
//	rtc_AlarmTime.RTC_Hours =hour;
//	rtc_AlarmTime.RTC_Minutes=min;
//	rtc_AlarmTime.RTC_Seconds=sec;
//	rtc_AlarmTypeDef.RTC_AlarmTime = rtc_AlarmTime;
	rtc_AlarmTypeDef.RTC_AlarmTime.RTC_H12=h12;//结构体中的结构体成员的成员赋值
	rtc_AlarmTypeDef.RTC_AlarmTime.RTC_Hours=hour;
	rtc_AlarmTypeDef.RTC_AlarmTime.RTC_Minutes=min;
	rtc_AlarmTypeDef.RTC_AlarmTime.RTC_Seconds=sec;
	RTC_SetAlarm(RTC_Format_BIN,RTC_Alarm_A,&rtc_AlarmTypeDef);//闹钟结构体初始化
}

/*
Function name:RTC_Alarm_IRQHandler
Description:RTC闹钟中断服务函数
param:None
retval:None
Remarks:闹钟中断标志位有两个，分别是外部中断挂起标志位
				一个是闹钟状态寄存器的闹钟标志位
*/
void RTC_Alarm_IRQHandler(void)
{
	if(RTC_GetITStatus(RTC_IT_ALRA))
	{
		RTC_ClearITPendingBit(RTC_IT_ALRA);//清除闹钟A中断标志位  因为写保护不包括RTC的ISR状态寄存器，所以该位可以清0
		printf("闹钟A\r\n");
		LED3_OVERTURN;
		LED4_OVERTURN;
	}
	EXTI_ClearITPendingBit(EXTI_Line17);//清除中断线标志位   如果不清该中断，他就会一直进这个服务函数，然后1s进一次这个的判断，此时主程序跑不了一直在进中断
}




