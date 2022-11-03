#include "main.h"                  // Device header

/*
Function name:Voice_Config
Description:语音模块初始化
param:
retval:None
Remarks:
PC4--VOICE_BUSY -- 浮空输入
PC5--VOICE_DATA -- 通用输出
*/
void Voice_Config(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);//PC时钟使能
	GPIO_InitTypeDef gpio_InitTypeDef;
	gpio_InitTypeDef.GPIO_Mode = GPIO_Mode_IN;
	gpio_InitTypeDef.GPIO_Pin = GPIO_Pin_4;
	GPIO_Init(GPIOC,&gpio_InitTypeDef);//PC4 输入
	gpio_InitTypeDef.GPIO_Mode = GPIO_Mode_OUT;
	gpio_InitTypeDef.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOC,&gpio_InitTypeDef);//PC5 输出
	Send_L;//空闲低
}

/*
Function name:Voice_Work
Description:语音模块开始工作
param:手册中的八位协议码
retval:None
Remarks:
PC4--VOICE_BUSY -- 浮空输入
PC5--VOICE_DATA -- 通用输出
*/
void Voice_Work(u8 data)
{
	u8 i;
	
	Voice_Start();//同步头
	for(i=0;i<8;i++)
	{
		if(data&(0x80>>i))
		{
			Voice_Data(1);
		}
		else
		{
			Voice_Data(0);
		}
	}
	while(Recv_Busy);//等待语音结束
	TIM5_Delay_Ms(500);//播放语音后的延时
	
}
/*
Function name:Voice_Start
Description:MCU发送数据的同步头
param:None
retval:None
Remarks:
同步头为 8MS 高和 1MS 低组成
*/
void Voice_Start(void)
{
	Send_H;
	TIM5_Delay_Ms(8);//8ms高电平
	Send_L;
	TIM5_Delay_Ms(1);//1ms低电平
}

/*
Function name:Voice_Data
Description:MCU发送数据
param:u8
retval:None
Remarks:
数据 “0” ： 0.5MS 高电平和 1.5MS 低电平组成。
数据 “1” ： 1.5MS 高电平和 0.5MS 低电平组成。

*/
void Voice_Data(u8 data)
{
	if(data)//发送数据1
	{
		Send_H;
		TIM5_Delay_Us(1500);
		Send_L;
		TIM5_Delay_Us(500);
	}
	else if(data==0)//发送数据0
	{
		Send_H;
		TIM5_Delay_Us(500);
		Send_L;
		TIM5_Delay_Us(1500);
	}
}


void VOICE_Set_Volume(void)
{
	Voice_Work(0X09);//设置音量
	printf("设置音量");
	LCD_Clear(0,239,0,239,WHITE);//清屏
	LCDFONT_Display_Str(50,0,BLUE,WHITE,16,(u8 *)"按*键加大音量，按#号减小音量");
	LCDFONT_Display_Str(70,0,BLUE,WHITE,16,(u8 *)"按0键退出");
	while(1)
	{
		while(BS_IRQ);//等待按键按下
		
		if(!BS_IRQ)
		{	
			u8 key=Get_BsKey_Value();
			if(key=='*')
			{
				Voice_Work(volume--);
				if(volume<=0x20)
				{
					volume=0x20;
				}
			}
			else if(key=='#')
			{
				Voice_Work(volume++);
				if(volume>=0x27)
				{
					volume=0x27;
				}
			}
			else if(key=='0')
			{
				break;
			}
			Voice_Work(0X1d);//嘟   要放在获取键值之后否则会干扰IIC时序
			printf("%#x ",volume);
			while(!BS_IRQ);//抬起才能出等待
		}			
	}
	Voice_Work(0X11);//操作成功
	LCD_Clear(0,239,0,239,WHITE);//清屏
	LCDFONT_Display_Str(50,0,BLUE,WHITE,32,(u8 *)"操作成功");
	Delay_Ms(500);
	lcd_flag=1;
}

