#include "main.h"                  // Device header

/*
Function name:RTC_Clender_Config
Description:RTC������ʼ��
param:None
retval:None
Remarks:
*/
void RTC_Clender_Config(void)
{
	//���������
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);//ʹ�ܵ�Դ�ӿ�ʱ��
	PWR_BackupAccessCmd(ENABLE);//ʹ�ܶԱ�����ķ���
	RCC_LSICmd(ENABLE);//ʹ��LSIʱ��
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);//ѡ��LSIΪRTCʱ��Դ
	RCC_RTCCLKCmd(ENABLE);//ʹ��RTCʱ��
	//RTC������ʼ��������
	RTC_WriteProtectionCmd(DISABLE);//�ر�д����,�ſ��Զ�RTC�Ĵ�����������
	RTC_EnterInitMode();//�����ʼ��ģʽ
	RTC_InitTypeDef rtc_InitTypeDef;//����Ԥ��Ƶ�ṹ�壬����1Hzʱ��
	rtc_InitTypeDef.RTC_AsynchPrediv=0x7f;//�첽128��Ƶ
	rtc_InitTypeDef.RTC_HourFormat=RTC_HourFormat_24;//24Сʱ��ʽ
	rtc_InitTypeDef.RTC_SynchPrediv=0xf9;//ͬ��250��Ƶ  32K����128*250=32K��Ƶ����1Hz,��1��/s
	RTC_Init(&rtc_InitTypeDef);//��ʼ���ṹ��
	Set_RTC_Time(RTC_H12_AM,1,2,3);//��������ʱ��
	Set_RTC_Date(22,8,2,3);//������������
	RTC_ExitInitMode();//�˳���ʼ��ģʽ
	RTC_WriteProtectionCmd(ENABLE);//��д����,�����Զ�RTC�Ĵ�������дֵ
}


/*
Function name:Set_RTC_Time
Description:����RTCʱ��
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
	rtc_TimeTypeDef.RTC_Hours=hour;//ʱ
	rtc_TimeTypeDef.RTC_Minutes=min;//��
	rtc_TimeTypeDef.RTC_Seconds=sec;//��
	return RTC_SetTime(RTC_Format_BIN,&rtc_TimeTypeDef);//��ʼ��ʱ��ṹ��
}

/*
Function name:Set_RTC_Date
Description:����RTC����
param:u8 year,u8 mon,u8 date,u8 week
retval:ERROR = 0, SUCCESS = !ERROR
Remarks:year��0-99 
*/
ErrorStatus Set_RTC_Date(u8 year,u8 mon,u8 date,u8 week)
{
	RTC_DateTypeDef rtc_DateTypeDef;
	rtc_DateTypeDef.RTC_Date=date;//��
	rtc_DateTypeDef.RTC_Month=mon;//��
	rtc_DateTypeDef.RTC_WeekDay=week;//��
	rtc_DateTypeDef.RTC_Year=year;//��
	return RTC_SetDate(RTC_Format_BIN,&rtc_DateTypeDef);//��ʼ�����ڽṹ��
}




/*
Function name:RTC_Alarm_Config
Description:RTC���ӳ�ʼ��
param:None
retval:None
Remarks:
*/
void RTC_Alarm_Config(void)
{
	//����
	RTC_AlarmCmd(RTC_Alarm_A,DISABLE);//��ֹ���� A
	Set_Rtc_Alarm(15,RTC_H12_PM,20,56,30);//��������A��ʱ��
	RTC_ITConfig(RTC_IT_ALRA,ENABLE);//ʹ������A�ж�
//	RTC_ClearITPendingBit(RTC_IT_ALRA);//����A�жϱ�־λ����ֹ��ʼ��ʱ��־Ϊ1
	RTC_AlarmCmd(RTC_Alarm_A,ENABLE);//ʹ������A
	RTC_WriteProtectionCmd(ENABLE);//��д����,�����Զ�RTC�Ĵ�������дֵ ����ISR״̬�Ĵ����������Ĵ���
	//�ж�����
	EXTI_InitTypeDef exti_InitTypeDef;
	exti_InitTypeDef.EXTI_Line =EXTI_Line17;//EXTI �� 17 
	exti_InitTypeDef.EXTI_LineCmd=ENABLE;//ʹ��
	exti_InitTypeDef.EXTI_Mode = EXTI_Mode_Interrupt;//�ж�ģʽ
	exti_InitTypeDef.EXTI_Trigger =EXTI_Trigger_Rising;//��������Ч
	EXTI_Init(&exti_InitTypeDef);//�ⲿ�жϽṹ���ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�������ȼ�����
	NVIC_InitTypeDef nvic_InitTypeDef;
	nvic_InitTypeDef.NVIC_IRQChannel = RTC_Alarm_IRQn;//�ж�Դ
	nvic_InitTypeDef.NVIC_IRQChannelCmd = ENABLE;
	nvic_InitTypeDef.NVIC_IRQChannelPreemptionPriority = 2;//��ռ���ȼ�Ϊ2
	nvic_InitTypeDef.NVIC_IRQChannelSubPriority = 2;//�μ����ȼ�Ϊ2
	NVIC_Init(&nvic_InitTypeDef);//NVIC�ṹ���ʼ��
}


/*
Function name:Set_Rtc_Alarm
Description:����RTC����
param:u8 date,u8 h12,u8 hour,u8 min,u8 sec
retval:None
Remarks:
h12-->
RTC_H12_AM
RTC_H12_PM

*/
void Set_Rtc_Alarm(u8 date,u8 h12,u8 hour,u8 min,u8 sec)
{
	RTC_AlarmTypeDef rtc_AlarmTypeDef;//�������A�ṹ��
	rtc_AlarmTypeDef.RTC_AlarmDateWeekDay = date;//1-31������
	rtc_AlarmTypeDef.RTC_AlarmDateWeekDaySel=RTC_AlarmDateWeekDaySel_Date;//��������
//	rtc_AlarmTypeDef.RTC_AlarmMask=RTC_AlarmMask_Seconds;//ֻ��������������õĵ�����ʱ���йأ��������޹أ���������ʱ��ƥ������ôÿ���һ�������ж�
	rtc_AlarmTypeDef.RTC_AlarmMask=RTC_AlarmMask_Minutes;//ֻ����˷��������õĵ�����ʱ���йأ�������޹أ���������ʱ��ƥ����ÿ�ֽ�һ�������ж�
//	rtc_AlarmTypeDef.RTC_AlarmMask=RTC_AlarmMask_None;//ֻ������ʱ�����ڶ��ϲŻ�������
	//	RTC_TimeTypeDef rtc_AlarmTime;
//	rtc_AlarmTime.RTC_H12 = h12;
//	rtc_AlarmTime.RTC_Hours =hour;
//	rtc_AlarmTime.RTC_Minutes=min;
//	rtc_AlarmTime.RTC_Seconds=sec;
//	rtc_AlarmTypeDef.RTC_AlarmTime = rtc_AlarmTime;
	rtc_AlarmTypeDef.RTC_AlarmTime.RTC_H12=h12;//�ṹ���еĽṹ���Ա�ĳ�Ա��ֵ
	rtc_AlarmTypeDef.RTC_AlarmTime.RTC_Hours=hour;
	rtc_AlarmTypeDef.RTC_AlarmTime.RTC_Minutes=min;
	rtc_AlarmTypeDef.RTC_AlarmTime.RTC_Seconds=sec;
	RTC_SetAlarm(RTC_Format_BIN,RTC_Alarm_A,&rtc_AlarmTypeDef);//���ӽṹ���ʼ��
}

/*
Function name:RTC_Alarm_IRQHandler
Description:RTC�����жϷ�����
param:None
retval:None
Remarks:�����жϱ�־λ���������ֱ����ⲿ�жϹ����־λ
				һ��������״̬�Ĵ��������ӱ�־λ
*/
void RTC_Alarm_IRQHandler(void)
{
	if(RTC_GetITStatus(RTC_IT_ALRA))
	{
		RTC_ClearITPendingBit(RTC_IT_ALRA);//�������A�жϱ�־λ  ��Ϊд����������RTC��ISR״̬�Ĵ��������Ը�λ������0
		printf("����A\r\n");
		LED3_OVERTURN;
		LED4_OVERTURN;
	}
	EXTI_ClearITPendingBit(EXTI_Line17);//����ж��߱�־λ   ���������жϣ����ͻ�һֱ�������������Ȼ��1s��һ��������жϣ���ʱ�������ܲ���һֱ�ڽ��ж�
}




