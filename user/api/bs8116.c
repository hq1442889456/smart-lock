#include "main.h"                  // Device header


/*
Function name:BS8116_IO_Config
Description:电容按键BS8116 IO初始化
param:None
retval:None
Remarks:
PB6--BS8116--SCLK -- 通用推挽输出  （只有MCU可以输出时钟，故只有MCU可以拉时钟线）
PB7--BS8116--SDA -- 通用开漏输出  （可以切换为输入）
PA1--BS8116--IRQ--  输入         (低电平表示按键按下，资料手册IRQ功能那一章有说)
*/
void BS8116_Config(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);//PB时钟使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);//PA时钟使能
	//PB6
	GPIO_InitTypeDef gpio_InitTypeDef = {0};//定义并清零  因为IO结构体很容易之前有配置过
	gpio_InitTypeDef.GPIO_Mode = GPIO_Mode_OUT;
	gpio_InitTypeDef.GPIO_OType = GPIO_OType_PP;//通用推挽输出
	gpio_InitTypeDef.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOB,&gpio_InitTypeDef);
	//PB7
	gpio_InitTypeDef.GPIO_OType = GPIO_OType_OD;//通用开漏输出
	gpio_InitTypeDef.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOB,&gpio_InitTypeDef);
	//PA1
	gpio_InitTypeDef.GPIO_Mode = GPIO_Mode_IN;//通用输入
	gpio_InitTypeDef.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOA,&gpio_InitTypeDef);
}



/*
Function name:Get_BsKey_Value
Description:BS8116获取键值
param:None
retval:错误码或键值
Remarks:资料手册IIC接口章节有时序图，更具时序图来写

*/
u8 Get_BsKey_Value(void)
{
	u16 key = 0;
	BSIIC_Start();
	if(BSIIC_Send_Data(BS_ADDR_W))
	{
		printf("应答1错误\r\n");
		return 1;
	}
	if(BSIIC_Send_Data(0x08))
	{
		printf("应答2错误\r\n");
		return 2;
	}
	BSIIC_Start();
	if(BSIIC_Send_Data(BS_ADDR_R))
	{
		printf("应答3错误\r\n");
		return 3;
	}
	key = BSIIC_Rev_Data(ACK);//接收低位键值寄存器数据
	key = BSIIC_Rev_Data(NACK)<<8 | key;//接收高位键值寄存器数据并和低位键值数据组合
	BSIIC_Stop();
	switch(key)//BS8116手册没给一个重要的信息，就是它的按键高低位寄存器，高位的和低位的寄存器的最高位都是1，即复位值是0X8080
	{
		case 0x8081:return '1';//TK1  根据硬件判断
		case 0x8480:return '2';//TK11
		case 0x8080:return '3';//TK8
		case 0x8082:return '4';//TK2
		case 0x8880:return '5';//TK12
		case 0x80c0:return '6';//TK7
		case 0x8088:return '7';//TK4
		case 0x8180:return '8';//TK9
		case 0x80a0:return '9';//TK6
		case 0x8280:return '0';//TK10
		case 0x8084:return '*';//TK3
		case 0x8090:return '#';//TK5
	}
	return 0;
}


u8 page_flag;

void BS_Open_Lock(void)
{
	u8 i;
	LCD_Clear(0,239,0,239,WHITE);//清屏
	LCDFONT_Display_Str(50,0,BLUE,WHITE,32,(u8 *)"请输入密码：");
	printf("请输入密码：\r\n");
	Voice_Work(0X0c);//请输入管理员密码
	TIM_Cmd(TIM9,ENABLE);//打开计数器
	for(i=0;i<6;i++)
	{
		TIM9->CNT =0;
		count=0;
		while(BS_IRQ)
		{
			wait_flag2=1;
			if(wait_flag1)
			{
				wait_flag1=0;
				wait_flag2=0;
				lcd_flag=1;
				return;//10s未完操作将结束函数
			}
		}//等待按键按下
		if(!BS_IRQ)
		{
			
			key_buf[i]=Get_BsKey_Value();
			printf("%c\r\n",key_buf[i]);
			Voice_Work(0X1d);//嘟   要放在获取键值之后否则会干扰IIC时序
			LCDFONT_Display_ASCII(100,16*i,BLACK,WHITE,32,'*');//在LCD屏上显示*
			while(!BS_IRQ);//抬起才能出等待
		}
	}
	key_buf[i]='\0';//字符串结束标志
	if(!strcmp((char *)key_buf,(char *)rev_buf))//密码比对
	{
		Voice_Work(0X18);//欢迎回家
		printf("欢迎回家\r\n");
		LCD_Clear(0,239,0,239,WHITE);//清屏
		LCDFONT_Display_Str(50,0,BLUE,WHITE,32,(u8 *)"欢迎回家！");
		Delay_Ms(500);
		lcd_flag = 1;//清屏标志位
		page_flag = 1;
		Open_Door();
		TIM9->CNT =0;
		TIM_Cmd(TIM9,DISABLE);//打开计数器
		
	}
	else
	{
		Voice_Work(0X10);//密码不一致
		printf("密码不一致\r\n");
		LCD_Clear(0,239,0,239,WHITE);//清屏
		LCDFONT_Display_Str(50,0,BLUE,WHITE,32,(u8 *)"密码不一致");
		Delay_Ms(500);
		lcd_flag=1;//清屏
		TIM9->CNT =0;
		TIM_Cmd(TIM9,DISABLE);//打开计数器
	}
	
}





void BS_Change_Password(void)
{
	u8 i;
	LCD_Clear(0,239,0,239,WHITE);//清屏
	LCDFONT_Display_Str(50,0,BLUE,WHITE,32,(u8 *)"请输入新密码：");
	printf("请输入新密码：\r\n");
	Voice_Work(0X0d);//请输入新密码
	for(i=0;i<6;i++)
	{
		while(BS_IRQ);//等待按键按下
		if(!BS_IRQ)
		{
//								LCD_Display_Char(100,16*i,BLACK,WHITE,32,passward_buf);
			LCDFONT_Display_ASCII(100,16*i,BLACK,WHITE,32,'*');
			passward[i]=Get_BsKey_Value();
			printf("%c\r\n",passward[i]);
			Voice_Work(0X1d);//嘟   要放在获取键值之后否则会干扰IIC时序
			while(!BS_IRQ);//抬起才能出等待
		}
	}
	passward[i] = '\0';
	LCD_Clear(0,239,0,239,WHITE);//清屏
	LCDFONT_Display_Str(50,0,BLUE,WHITE,24,(u8 *)"请再次输入新密码");
	printf("请再次输入新密码:\r\n");
	Voice_Work(0X0e);//请再次输入新密码
	for(i=0;i<6;i++)
	{
		while(BS_IRQ);//等待按键按下
		if(!BS_IRQ)
		{
//								LCD_Display_Char(100,16*i,BLACK,WHITE,32,passward_buf);
			LCDFONT_Display_ASCII(100,16*i,BLACK,WHITE,32,'*');
			passward1[i]=Get_BsKey_Value();
			printf("%c\r\n",passward1[i]);
			Voice_Work(0X1d);//嘟   要放在获取键值之后否则会干扰IIC时序
			while(!BS_IRQ);//抬起才能出等待
		}
	}
	passward1[i] = '\0';
	if(strcmp((char *)passward1,(char *)passward))
	{
		printf("密码不一致\r\n");
		LCD_Clear(0,239,0,239,WHITE);//清屏
		LCDFONT_Display_Str(50,0,BLUE,WHITE,32,(u8 *)"密码不一致！");
		LCDFONT_Display_Str(80,0,BLUE,WHITE,32,(u8 *)"修改失败！");
		Delay_Ms(500);
		Voice_Work(0X10);//密码不一致！
		lcd_flag=1;
		password_flag=1;
	}
	if(!password_flag)
	{
		AT_Cross_Write(62,passward,sizeof(passward));//向EEPROM写入密码
		LCD_Clear(0,239,0,239,WHITE);//清屏
		printf("修改密码成功\r\n");
		LCDFONT_Display_Str(50,0,BLUE,WHITE,32,(u8 *)"修改密码成功");
		Delay_Ms(500);
		Voice_Work(0X11);//操作成功！
		lcd_flag=1;
	
	}	
}




