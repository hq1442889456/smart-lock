#include "main.h"                  // Device header


/*
Function name:LCD_Pin_Config
Description:LCD引脚初始化
param:None
retval:None
Remarks:
LCD_CS：片选  低有效                 PB12
LCD_SPI2_SCLK：时钟线  决定收发周期  PB13
LCD_SPI2_MOSI：MCU的数据输出线       PB15   硬件上只给了一个输出脚
LCD_LEDK：背光源 高有效              PB1
LCD_RESET：低有效                    PB10
LCD_D/C：                            PB14
		DC=1---->发送数据或者参数
		DC=0---->发送命令

*/
void LCD_Pin_Config(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);//PB时钟使能
	GPIO_InitTypeDef GPIO_InitStruct={0};
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_10|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	//默认无效
	LCD_CS =1;
	LCD_LEDK=0;
	LCD_RESET=1;
}

/*
Function name:SPI_Send_Data
Description:模拟SPI
param:data 待发送的数据
retval:None
Remarks:SP0模式 下降沿发送 上升沿接收 先发送高位
*/
void SPI_Send_Data(u8 data)
{
	LCD_SCLK=1;
	for(u8 i=0;i<8;i++)//产生下降沿
	{
		LCD_SCLK=0;
		if(data&0x80>>i)
		{
			LCD_MOSI=1;
		}
		else
		{
			LCD_MOSI=0;
		}
		LCD_SCLK=1;
	}
}

/*
Function name:Send_Data_LCD
Description:发送数据给LCD
param:data 待发送的数据
retval:None
Remarks:DC=1 发送数据
*/
void Send_Data_LCD(u8 data)
{
	LCD_CS = 0;//拉低片选开始通信
	LCD_DC = 1;//DC=1---->发送数据或者参数
	SPI_Send_Data(data);
	LCD_CS = 1;//拉高片选，结束通信
}

/*
Function name:Send_Cmd_LCD
Description:发送指令给LCD
param:cmd 待发送的指令
retval:None
Remarks:DC=0 发送指令
*/
void Send_Cmd_LCD(u8 Cmd)
{
	LCD_CS = 0;//拉低片选开始通信
	LCD_DC = 0;//DC=0---->发送指令
	SPI_Send_Data(Cmd);
	LCD_CS = 1;//拉高片选，结束通信
}

/*
Function name:Send_Data_16b_LCD
Description:发送十六位数据
param:data 待发送的数据
retval:None
Remarks:DC=1 发送数据
也可以用发16位的模拟SPI代替
*/
void Send_Data_16b_LCD(u16 data)
{
	LCD_CS = 0;//拉低片选开始通信
	LCD_DC = 1;//DC=1---->发送数据或者参数
	SPI_Send_Data(data>>8);//先发高8位
	SPI_Send_Data(data);//再发低8位
	LCD_CS = 1;//拉高片选，结束通信
}

/*
Function name:LCD_Set_Positon
Description:设置区域(位置)的函数
param:s_row-->起始行 e_row-->结束行 s_col-->起始列 e_col-->结束列
retval:None
Remarks:
*/
void LCD_Set_Positon(u16 s_row,u16 e_row,u16 s_col,u16 e_col)
{
	Send_Cmd_LCD(0x2a);//设置列范围指令
	Send_Data_16b_LCD(s_col);
	Send_Data_16b_LCD(e_col);
	
	Send_Cmd_LCD(0x2b);//设置行范围指令
	Send_Data_16b_LCD(s_row);
	Send_Data_16b_LCD(e_row);
	
	Send_Cmd_LCD(0x2c);//允许写入
}

/*
Function name:LCD_Clear
Description:清屏函数
param:s_row-->起始行 e_row-->结束行 
			s_col-->起始列 e_col-->结束列
      color-->待填充的颜色
retval:None
Remarks:
*/
void LCD_Clear(u16 s_row,u16 e_row,u16 s_col,u16 e_col,u16 color)
{
	LCD_Set_Positon(s_row,e_row,s_col,e_col);
	for(u16 i=0;i<(e_row-s_row+1)*(e_col-s_col+1);i++)//每个像素点进行填充，上边的函数已经设置好范围，越过设定的边界会自动换行处理
	{
		Send_Data_16b_LCD(color);//16位假彩：颜色数据占16位 所以一次填充一个像素点
	}
}
//初始化函数
//命令是官方给的，不用深究
void LCD_Init(void)
{
	LCD_Pin_Config();
	
	/* 复位ST7789VM驱动器 */
	LCD_RESET=0;
	TIM5_Delay_Ms(100);
	LCD_RESET=1;
	TIM5_Delay_Ms(100);

	/* start initial sequence */ 
	Send_Cmd_LCD(0x36);
	Send_Data_LCD(0x00);

	Send_Cmd_LCD(0x3A); 
	Send_Data_LCD(0x05);

	Send_Cmd_LCD(0xB2);
	Send_Data_LCD(0x0C);
	Send_Data_LCD(0x0C);
	Send_Data_LCD(0x00);
	Send_Data_LCD(0x33);
	Send_Data_LCD(0x33); 

	Send_Cmd_LCD(0xB7); 
	Send_Data_LCD(0x35);  

	Send_Cmd_LCD(0xBB);
	Send_Data_LCD(0x19);

	Send_Cmd_LCD(0xC0);
	Send_Data_LCD(0x2C);

	Send_Cmd_LCD(0xC2);
	Send_Data_LCD(0x01);

	Send_Cmd_LCD(0xC3);
	Send_Data_LCD(0x12);   

	Send_Cmd_LCD(0xC4);
	Send_Data_LCD(0x20);  

	Send_Cmd_LCD(0xC6); 
	Send_Data_LCD(0x0F);    

	Send_Cmd_LCD(0xD0); 
	Send_Data_LCD(0xA4);
	Send_Data_LCD(0xA1);

	Send_Cmd_LCD(0xE0);
	Send_Data_LCD(0xD0);
	Send_Data_LCD(0x04);
	Send_Data_LCD(0x0D);
	Send_Data_LCD(0x11);
	Send_Data_LCD(0x13);
	Send_Data_LCD(0x2B);
	Send_Data_LCD(0x3F);
	Send_Data_LCD(0x54);
	Send_Data_LCD(0x4C);
	Send_Data_LCD(0x18);
	Send_Data_LCD(0x0D);
	Send_Data_LCD(0x0B);
	Send_Data_LCD(0x1F);
	Send_Data_LCD(0x23);

	Send_Cmd_LCD(0xE1);
	Send_Data_LCD(0xD0);
	Send_Data_LCD(0x04);
	Send_Data_LCD(0x0C);
	Send_Data_LCD(0x11);
	Send_Data_LCD(0x13);
	Send_Data_LCD(0x2C);
	Send_Data_LCD(0x3F);
	Send_Data_LCD(0x44);
	Send_Data_LCD(0x51);
	Send_Data_LCD(0x2F);
	Send_Data_LCD(0x1F);
	Send_Data_LCD(0x1F);
	Send_Data_LCD(0x20);
	Send_Data_LCD(0x23);

	Send_Cmd_LCD(0x21); 
	Send_Cmd_LCD(0x11); 
	Send_Cmd_LCD(0x29); 
	
	/* 清空屏幕显示 */
	LCD_Clear(0,239,0,239,WHITE);//初始颜色白色
	LCDFONT_Display_Str(200,30,BLUE,WHITE,32,(u8 *)"Loading……");
	lcd_flag =1;
	/* 开启背光 */
	LCD_LEDK=1;
}

/*
Function name:LCD_Display_Point
Description:显示点的函数
param:row-->行 col-->列 
      color-->待填充的颜色
retval:None
Remarks:
*/
void LCD_Display_Point(u16 row,u16 col,u16 color)
{
	LCD_Set_Positon(row,row,col,col);
	Send_Data_16b_LCD(color);
}

/*
Function name:LCD_Display_Char
Description:显示字符的函数
param:row-->起始行 col-->起始列 font_color-->字体颜色
      back_color-->背景色 fontsize-->字体大小  data-->数据首地址
retval:None
Remarks:
取模方式-->阴码 逐行式 高位在前
24大小的字体  宽占12位--->补为16位
*/
void LCD_Display_Char(u16 row,u16 col,u16 font_color,u16 back_color,u8 fontsize,u8 *data)
{
	u8 width=0;//宽  16行（fontsize），则列为8，那么为一个字节，width为1，32行，则列为16，那么字节为2，所以width为2，24行，则列为12，字节为12/8=1.5字节，但是显示是不允许的，所以要自动补齐为2字节
	width = fontsize/2/8;//以字节为单位
	if(fontsize/2%8)//宽不是整字节 需要补齐
	{
		width++;
	}
	for(u8 i=0;i<fontsize;i++)
	{
		for(u8 j=0;j<width;j++)
		{
			for(u8 k=0;k<8;k++)
			{
				if(data[i*width+j]&(0x80>>k))
				{
					LCD_Display_Point(row+i,col+k+8*j,font_color);
				}
				else
				{
					LCD_Display_Point(row+i,col+k+8*j,back_color);
				}
			}
		}
	}
}

/*
Function name:LCD_Display_Hz
Description:显示汉字的函数
param:row-->起始行 col-->起始列 font_color-->字体颜色
      back_color-->背景色 fontsize-->字体大小  data-->数据首地址
retval:None
Remarks:
取模方式-->阴码 逐行式 高位在前
24大小的字体  宽占12位--->补为16位
*/
void LCD_Display_Hz(u16 row,u16 col,u16 font_color,u16 back_color,u8 fontsize,u8 *data)
{
	u8 width=0;//宽  16行（fontsize），则列为8，那么为一个字节，width为1，32行，则列为16，那么字节为2，所以width为2，24行，则列为12，字节为12/8=1.5字节，但是显示是不允许的，所以要自动补齐为2字节
	width = fontsize/8;//以字节为单位
//	if(fontsize%8)//宽不是整字节 需要补齐  字符需要，汉字则不需要
//	{
//		width++;
//	}
	for(u8 i=0;i<fontsize;i++)
	{
		for(u8 j=0;j<width;j++)
		{
			for(u8 k=0;k<8;k++)
			{
				if(data[i*width+j]&(0x80>>k))
				{
					LCD_Display_Point(row+i,col+k+8*j,font_color);
				}
				else
				{
					LCD_Display_Point(row+i,col+k+8*j,back_color);
				}
			}
		}
	}
}

/*
Function name:LCD_Display_Pic
Description:显示图片的函数
param:row-->起始行  col-->起始列  data-->显示数据的起始地址
retval:None
Remarks:
*/
void LCD_Display_Pic(u16 row,u16 col,u8 *data)
{
	u16 w,h,i;
	w=data[2]<<8|data[3];
	h=data[4]<<8|data[5];
	LCD_Set_Positon(row,row+h,col,col+w);
	for(i=0;i<w*h;i++)
	{
		Send_Data_16b_LCD(data[8+2*i]<<8|data[8+2*i+1]);
	}
}

//////////////////////////字库//////////////////////////////

/*
Function name:LCDFONT_Display_ASCII
Description:显示字符的函数
param:row-->起始行 col-->起始列 font_color-->字体颜色
      back_color-->背景色 fontsize-->字体大小  data-->数据
retval:None
Remarks:
24大小的字体  宽占12位--->补为16位
*/
void LCDFONT_Display_ASCII(u16 row,u16 col,u16 font_color,u16 back_color,u8 fontsize,u8 data)
{
	u16 i,j,k;
	u32 addr=0;
	u8 dma_asc[100]={0};//用来存储字符模的
	u16 width=0;//数据宽度
	width=fontsize/2/8;//以字节为单位
	if(fontsize/2%8)//宽不是整字节 需要补齐
	{
		width+=1;
	}
	//不同类型字符库的地址
	switch(fontsize)
	{
		case 16:addr+=0x00000000;break;
		case 24:addr+=0x0004064C;break;
		case 32:addr+=0x000D1A68;break;
	}
	//要显示字符的地址
	addr=addr+data*fontsize*width;//字库中一个字符的字节大小：假如是32字长，那么高(fontsize)是32行，16列，八列为一字节，所以一行是2字节，即width=2,一共2*32=64字节,因为字符的字库是按ASCII码排列的，前面有ASCII个字符字库数据，那么就偏移ASCII*fontsize*width个地址
	W25_Read_Data(addr,(u8 *)dma_asc,fontsize*width);//读取该字符的字库数据
	
	for(i=0;i<fontsize;i++)//行
	{
		for(j=0;j<width;j++)//列
		{
			for(k=0;k<8;k++)//位 高位先发
			{
				if(dma_asc[i*width+j] & (0x80>>k))//数据1 显示字体颜色
				{
					LCD_Display_Point(row+i,col+k+8*j,font_color);
				}
				else
				{
					LCD_Display_Point(row+i,col+k+8*j,back_color);
				}
			}
		}
	}
}



/*
Function name:LCDFONT_Display_Font
Description:显示汉字的函数
param:row-->起始行 col-->起始列 font_color-->字体颜色
      back_color-->背景色 fontsize-->字体大小  data-->数据首地址
retval:None
Remarks:
24大小的字体  宽占12位--->补为16位
*/
void LCDFONT_Display_Font(u16 row,u16 col,u16 font_color,u16 back_color,u8 fontsize,u8 *data)
{
	u16 i,j,k;
	u8 dma_font[200]={0};//最小大于128
	u32 addr=0;
	u16 width=0;//数据宽度
	width=fontsize/8;//以字节为单位
//	if(fontsize/2%8)//宽不是整字节 需要补齐   对字符需要  汉字不需要
//	{
//		width+=1;
//	}
//	
	//不同类型字符库的地址
	switch(fontsize)
	{
		case 16:addr+=0x00000806;break;
		case 24:addr+=0x00041E52;break;
		case 32:addr+=0x000D3A6E;break;
	}
	//字库内的是机内码、需要找区位码
	addr=addr+((*data-0xa1)*94+*(data+1)-0xa1)*fontsize*width;//前字节减0xa0得到当前区号，而要求的是前面剩余多少个区，故再-1，每个区94个汉字，后字节减0xa0得到当前位号，而要求的是前面剩余多少个位，需要再-1，再*单个字符字节数就可求出当前汉字的地址
	W25_Read_Data(addr,(u8 *)dma_font,fontsize*width);
	for(i=0;i<fontsize;i++)//行
	{
		for(j=0;j<width;j++)//列
		{
			for(k=0;k<8;k++)//位 高位先发
			{
				if(dma_font[i*width+j] & (0x80>>k))//数据1 显示字体颜色
				{
					LCD_Display_Point(row+i,col+k+8*j,font_color);
				}
				else
				{
					LCD_Display_Point(row+i,col+k+8*j,back_color);
				}
			}
		}
	}
}


/*
Function name:LCDFONT_Display_Str
Description:显示字符串的函数
param:row-->起始行 col-->起始列 font_color-->字体颜色
      back_color-->背景色 fontsize-->字体大小  str-->字符串首地址
retval:None
Remarks:大于0xa0是汉字  小于0xa0是字符
*/
void LCDFONT_Display_Str(u16 row,u16 col,u16 font_color,u16 back_color,u8 fontsize,u8 *str)
{
	while(*str)
	{
		if(*str>0xa0)//汉字
		{
			LCDFONT_Display_Font(row,col,font_color,back_color,fontsize,str);
			str+=2;
			col+=fontsize;
		}
		else
		{
			LCDFONT_Display_ASCII(row,col,font_color,back_color,fontsize,*str);
			str+=1;
			col+=fontsize/2;
		}
	}
}



void LCD_Dis_GUI(void)
{
	LCDFONT_Display_Str(35,20,RED,WHITE,24,(u8 *)"智能门锁管理系统");
	LCDFONT_Display_Str(74,10,LIGHTGREEN,WHITE,16,(u8 *)"请选择开门方式：");
	LCDFONT_Display_Str(101,20,BRED,WHITE,16,(u8 *)"1.密码开锁");
	LCDFONT_Display_Str(128,20,BRED,WHITE,16,(u8 *)"2.指纹开锁");
	LCDFONT_Display_Str(155,20,BRED,WHITE,16,(u8 *)"3.RFID卡开锁");
}


void LCD_Dis_RTC(void)
{
	RTC_GetTime(RTC_Format_BIN,&RTC_Time);
	RTC_GetDate(RTC_Format_BIN,&RTC_Date);
	sprintf((char *)display_buf,"20%d年%02d月%02d日周%d %02d时%02d分%02d秒",RTC_Date.RTC_Year,RTC_Date.RTC_Month,RTC_Date.RTC_Date,RTC_Date.RTC_WeekDay,RTC_Time.RTC_Hours,RTC_Time.RTC_Minutes,RTC_Time.RTC_Seconds);//将数据存入数组中
	LCDFONT_Display_Str(35,0,RED,WHITE,16,display_buf);
	LCDFONT_Display_Str(60,0,BLUE,WHITE,16,(u8 *)"按键选择功能：");
	LCDFONT_Display_Str(80,0,BRED,WHITE,16,(u8 *)"1.修改密码");
	LCDFONT_Display_Str(100,0,BRED,WHITE,16,(u8 *)"2.注册指纹");
	LCDFONT_Display_Str(120,0,BRED,WHITE,16,(u8 *)"3.删除所有指纹");
	LCDFONT_Display_Str(140,0,BRED,WHITE,16,(u8 *)"4.设置音量");
	LCDFONT_Display_Str(160,0,BRED,WHITE,16,(u8 *)"5.删除指定指纹");
	LCDFONT_Display_Str(180,0,BRED,WHITE,16,(u8 *)"0.退出");
	TIM5_Delay_Ms(500);//0.5s刷新一次

}

void LCD_Dis_GIF(void)
{
	if(gif_flag)
	{
		//开机动画
		LCD_Display_Pic(0,0,(u8 *)gImage_1);
		Delay_Ms(150);
		LCD_Display_Pic(0,0,(u8 *)gImage_2);
		Delay_Ms(150);
		LCD_Display_Pic(0,0,(u8 *)gImage_3);
		Delay_Ms(150);

		gif_flag=0;
		lcd_flag = 1;
	}
	if(lcd_flag)
	{
		LCD_Clear(0,239,0,239,WHITE);//清屏
		lcd_flag =0;
	}
}

