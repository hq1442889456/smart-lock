#include "main.h"
u8 send_buf[15]={'1','2','3','4','5','6','\0'};//密码
u8 card_id[4];  //当前IC卡号  我的IC卡卡号:0xAC AD 84 21  存在EEPROM中，不在这个数组中
unsigned char CT[2];//卡类型
u8 key_v;//电容按键键值
u8 errorCode;//mqtt错误码
char string[520] = {0};//存要发布上传数据的数组
u8 rev_buf[15]={0};//存储按键密码或者RFID的IC卡号
u8 key_buf[15]={0};//存储按键输入字符串

RTC_TimeTypeDef RTC_Time;//时间结构体变量
RTC_DateTypeDef RTC_Date;//日期结构体变量
u8 display_buf[100]={0};//存储显示在LCD屏的字符串的数组
u8 passward[15]={0};//用于存放修改后的第一次输入的密码
u8 passward1[15]={0};//用于存放修改后的第二次输入密码
u8 password_flag;//修改的两次密码不一致会置一
u8 volume = 0x27;//起始音量最小

u8 lcd_flag;//清屏标志位
u8 gif_flag;

int main(void)
{
	Led_Config();//LED灯
	Usart_Config(115200);//串口
	BS8116_Config();//电容按键
	AT24C04_Config();//EEPROM存储芯片
	Voice_Config();//语音模块
	Voice_Work(volume);//设置音量
	Motor_Config();//电机模块
	MG200_Init();//指纹
	RFID_Init();//RFID
	SPI1_Config();//SPI控制器初始化  用于和W25Q64进行通信
	LCD_Init();//LCD屏幕
	RTC_Clender_Config();//RTC日历
//	TIM9_MsIT_Config();//1MS定时中断
	Esp_Init();//esp32初始化
	printf("初始化成功!\r\n");
	
	AT_Cross_Write(62,send_buf,sizeof(send_buf));//向EEPROM写入初始密码
	AT_Random_Read(62,rev_buf,sizeof(rev_buf));//从EEPROM读出密码放在rev_buf[]数组中
	printf("%s\n",rev_buf);

	ESP32_Subscription();//esp32连接WIFI并订阅阿里云
	ESP32_Publish(0);//esp32上报阿里云门锁状态：关
mm:	
	while(1)
	{
		if(lcd_flag)
		{
			LCD_Clear(0,239,0,239,WHITE);//清屏
			lcd_flag =0;
		}
		LCD_Dis_GUI();
		if(!BS_IRQ)
		{
			key_v=Get_BsKey_Value();
			while(!BS_IRQ);
			printf("key_v1=%c\r\n",key_v);//打印键值并判断位置
			Voice_Work(0X1d);//嘟   要放在获取键值之后否则会干扰IIC时序
			switch(key_v)//根据键值选择开锁方式
			{
				case '1':BS_Open_Lock();break;
				case '2':MG_Open_lock();break;
				case '3':RFID_Open_lock();break;
			}
		}
		if(esp32rec.flag == 1)//云开锁
		{
			ESP32_Open_Lock();
		}
		while(page_flag==1)
		{
			if(lcd_flag)
			{
				LCD_Clear(0,239,0,239,WHITE);//清屏
				lcd_flag =0;
			}
			//RTC及功能选项屏幕显示
			LCD_Dis_RTC();
			if(!BS_IRQ)
			{
				key_v=Get_BsKey_Value();
				printf("key_v2=%c\r\n",key_v);//打印键值并判断位置
				Voice_Work(0X1d);//嘟   要放在获取键值之后否则会干扰IIC时序
				switch(key_v)//根据键值选择开锁方式
				{
					case '1':BS_Change_Password();break;
					case '2':MG_Enroll();break;
					case '3':MG_Delect_All();break;
					case '4':VOICE_Set_Volume();break;
					case '5':MG_Del_By_Id();break;
					case '0':page_flag=0;lcd_flag=1;esp32rec.flag = 0;goto mm;
				}
			}
			if(esp32rec.flag == 1)//云关锁
			{
				ESP32_Close_Lock();
				lcd_flag=1;
			}
		}
	}
}



