#include "main.h"                  // Device header


/*
Function name:LCD_Pin_Config
Description:LCD���ų�ʼ��
param:None
retval:None
Remarks:
LCD_CS��Ƭѡ  ����Ч                 PB12
LCD_SPI2_SCLK��ʱ����  �����շ�����  PB13
LCD_SPI2_MOSI��MCU�����������       PB15   Ӳ����ֻ����һ�������
LCD_LEDK������Դ ����Ч              PB1
LCD_RESET������Ч                    PB10
LCD_D/C��                            PB14
		DC=1---->�������ݻ��߲���
		DC=0---->��������

*/
void LCD_Pin_Config(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);//PBʱ��ʹ��
	GPIO_InitTypeDef GPIO_InitStruct={0};
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_10|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	//Ĭ����Ч
	LCD_CS =1;
	LCD_LEDK=0;
	LCD_RESET=1;
}

/*
Function name:SPI_Send_Data
Description:ģ��SPI
param:data �����͵�����
retval:None
Remarks:SP0ģʽ �½��ط��� �����ؽ��� �ȷ��͸�λ
*/
void SPI_Send_Data(u8 data)
{
	LCD_SCLK=1;
	for(u8 i=0;i<8;i++)//�����½���
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
Description:�������ݸ�LCD
param:data �����͵�����
retval:None
Remarks:DC=1 ��������
*/
void Send_Data_LCD(u8 data)
{
	LCD_CS = 0;//����Ƭѡ��ʼͨ��
	LCD_DC = 1;//DC=1---->�������ݻ��߲���
	SPI_Send_Data(data);
	LCD_CS = 1;//����Ƭѡ������ͨ��
}

/*
Function name:Send_Cmd_LCD
Description:����ָ���LCD
param:cmd �����͵�ָ��
retval:None
Remarks:DC=0 ����ָ��
*/
void Send_Cmd_LCD(u8 Cmd)
{
	LCD_CS = 0;//����Ƭѡ��ʼͨ��
	LCD_DC = 0;//DC=0---->����ָ��
	SPI_Send_Data(Cmd);
	LCD_CS = 1;//����Ƭѡ������ͨ��
}

/*
Function name:Send_Data_16b_LCD
Description:����ʮ��λ����
param:data �����͵�����
retval:None
Remarks:DC=1 ��������
Ҳ�����÷�16λ��ģ��SPI����
*/
void Send_Data_16b_LCD(u16 data)
{
	LCD_CS = 0;//����Ƭѡ��ʼͨ��
	LCD_DC = 1;//DC=1---->�������ݻ��߲���
	SPI_Send_Data(data>>8);//�ȷ���8λ
	SPI_Send_Data(data);//�ٷ���8λ
	LCD_CS = 1;//����Ƭѡ������ͨ��
}

/*
Function name:LCD_Set_Positon
Description:��������(λ��)�ĺ���
param:s_row-->��ʼ�� e_row-->������ s_col-->��ʼ�� e_col-->������
retval:None
Remarks:
*/
void LCD_Set_Positon(u16 s_row,u16 e_row,u16 s_col,u16 e_col)
{
	Send_Cmd_LCD(0x2a);//�����з�Χָ��
	Send_Data_16b_LCD(s_col);
	Send_Data_16b_LCD(e_col);
	
	Send_Cmd_LCD(0x2b);//�����з�Χָ��
	Send_Data_16b_LCD(s_row);
	Send_Data_16b_LCD(e_row);
	
	Send_Cmd_LCD(0x2c);//����д��
}

/*
Function name:LCD_Clear
Description:��������
param:s_row-->��ʼ�� e_row-->������ 
			s_col-->��ʼ�� e_col-->������
      color-->��������ɫ
retval:None
Remarks:
*/
void LCD_Clear(u16 s_row,u16 e_row,u16 s_col,u16 e_col,u16 color)
{
	LCD_Set_Positon(s_row,e_row,s_col,e_col);
	for(u16 i=0;i<(e_row-s_row+1)*(e_col-s_col+1);i++)//ÿ�����ص������䣬�ϱߵĺ����Ѿ����ú÷�Χ��Խ���趨�ı߽���Զ����д���
	{
		Send_Data_16b_LCD(color);//16λ�ٲʣ���ɫ����ռ16λ ����һ�����һ�����ص�
	}
}
//��ʼ������
//�����ǹٷ����ģ������
void LCD_Init(void)
{
	LCD_Pin_Config();
	
	/* ��λST7789VM������ */
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
	
	/* �����Ļ��ʾ */
	LCD_Clear(0,239,0,239,WHITE);//��ʼ��ɫ��ɫ
	LCDFONT_Display_Str(200,30,BLUE,WHITE,32,(u8 *)"Loading����");
	lcd_flag =1;
	/* �������� */
	LCD_LEDK=1;
}

/*
Function name:LCD_Display_Point
Description:��ʾ��ĺ���
param:row-->�� col-->�� 
      color-->��������ɫ
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
Description:��ʾ�ַ��ĺ���
param:row-->��ʼ�� col-->��ʼ�� font_color-->������ɫ
      back_color-->����ɫ fontsize-->�����С  data-->�����׵�ַ
retval:None
Remarks:
ȡģ��ʽ-->���� ����ʽ ��λ��ǰ
24��С������  ��ռ12λ--->��Ϊ16λ
*/
void LCD_Display_Char(u16 row,u16 col,u16 font_color,u16 back_color,u8 fontsize,u8 *data)
{
	u8 width=0;//��  16�У�fontsize��������Ϊ8����ôΪһ���ֽڣ�widthΪ1��32�У�����Ϊ16����ô�ֽ�Ϊ2������widthΪ2��24�У�����Ϊ12���ֽ�Ϊ12/8=1.5�ֽڣ�������ʾ�ǲ�����ģ�����Ҫ�Զ�����Ϊ2�ֽ�
	width = fontsize/2/8;//���ֽ�Ϊ��λ
	if(fontsize/2%8)//�������ֽ� ��Ҫ����
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
Description:��ʾ���ֵĺ���
param:row-->��ʼ�� col-->��ʼ�� font_color-->������ɫ
      back_color-->����ɫ fontsize-->�����С  data-->�����׵�ַ
retval:None
Remarks:
ȡģ��ʽ-->���� ����ʽ ��λ��ǰ
24��С������  ��ռ12λ--->��Ϊ16λ
*/
void LCD_Display_Hz(u16 row,u16 col,u16 font_color,u16 back_color,u8 fontsize,u8 *data)
{
	u8 width=0;//��  16�У�fontsize��������Ϊ8����ôΪһ���ֽڣ�widthΪ1��32�У�����Ϊ16����ô�ֽ�Ϊ2������widthΪ2��24�У�����Ϊ12���ֽ�Ϊ12/8=1.5�ֽڣ�������ʾ�ǲ�����ģ�����Ҫ�Զ�����Ϊ2�ֽ�
	width = fontsize/8;//���ֽ�Ϊ��λ
//	if(fontsize%8)//�������ֽ� ��Ҫ����  �ַ���Ҫ����������Ҫ
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
Description:��ʾͼƬ�ĺ���
param:row-->��ʼ��  col-->��ʼ��  data-->��ʾ���ݵ���ʼ��ַ
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

//////////////////////////�ֿ�//////////////////////////////

/*
Function name:LCDFONT_Display_ASCII
Description:��ʾ�ַ��ĺ���
param:row-->��ʼ�� col-->��ʼ�� font_color-->������ɫ
      back_color-->����ɫ fontsize-->�����С  data-->����
retval:None
Remarks:
24��С������  ��ռ12λ--->��Ϊ16λ
*/
void LCDFONT_Display_ASCII(u16 row,u16 col,u16 font_color,u16 back_color,u8 fontsize,u8 data)
{
	u16 i,j,k;
	u32 addr=0;
	u8 dma_asc[100]={0};//�����洢�ַ�ģ��
	u16 width=0;//���ݿ��
	width=fontsize/2/8;//���ֽ�Ϊ��λ
	if(fontsize/2%8)//�������ֽ� ��Ҫ����
	{
		width+=1;
	}
	//��ͬ�����ַ���ĵ�ַ
	switch(fontsize)
	{
		case 16:addr+=0x00000000;break;
		case 24:addr+=0x0004064C;break;
		case 32:addr+=0x000D1A68;break;
	}
	//Ҫ��ʾ�ַ��ĵ�ַ
	addr=addr+data*fontsize*width;//�ֿ���һ���ַ����ֽڴ�С��������32�ֳ�����ô��(fontsize)��32�У�16�У�����Ϊһ�ֽڣ�����һ����2�ֽڣ���width=2,һ��2*32=64�ֽ�,��Ϊ�ַ����ֿ��ǰ�ASCII�����еģ�ǰ����ASCII���ַ��ֿ����ݣ���ô��ƫ��ASCII*fontsize*width����ַ
	W25_Read_Data(addr,(u8 *)dma_asc,fontsize*width);//��ȡ���ַ����ֿ�����
	
	for(i=0;i<fontsize;i++)//��
	{
		for(j=0;j<width;j++)//��
		{
			for(k=0;k<8;k++)//λ ��λ�ȷ�
			{
				if(dma_asc[i*width+j] & (0x80>>k))//����1 ��ʾ������ɫ
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
Description:��ʾ���ֵĺ���
param:row-->��ʼ�� col-->��ʼ�� font_color-->������ɫ
      back_color-->����ɫ fontsize-->�����С  data-->�����׵�ַ
retval:None
Remarks:
24��С������  ��ռ12λ--->��Ϊ16λ
*/
void LCDFONT_Display_Font(u16 row,u16 col,u16 font_color,u16 back_color,u8 fontsize,u8 *data)
{
	u16 i,j,k;
	u8 dma_font[200]={0};//��С����128
	u32 addr=0;
	u16 width=0;//���ݿ��
	width=fontsize/8;//���ֽ�Ϊ��λ
//	if(fontsize/2%8)//�������ֽ� ��Ҫ����   ���ַ���Ҫ  ���ֲ���Ҫ
//	{
//		width+=1;
//	}
//	
	//��ͬ�����ַ���ĵ�ַ
	switch(fontsize)
	{
		case 16:addr+=0x00000806;break;
		case 24:addr+=0x00041E52;break;
		case 32:addr+=0x000D3A6E;break;
	}
	//�ֿ��ڵ��ǻ����롢��Ҫ����λ��
	addr=addr+((*data-0xa1)*94+*(data+1)-0xa1)*fontsize*width;//ǰ�ֽڼ�0xa0�õ���ǰ���ţ���Ҫ�����ǰ��ʣ����ٸ���������-1��ÿ����94�����֣����ֽڼ�0xa0�õ���ǰλ�ţ���Ҫ�����ǰ��ʣ����ٸ�λ����Ҫ��-1����*�����ַ��ֽ����Ϳ������ǰ���ֵĵ�ַ
	W25_Read_Data(addr,(u8 *)dma_font,fontsize*width);
	for(i=0;i<fontsize;i++)//��
	{
		for(j=0;j<width;j++)//��
		{
			for(k=0;k<8;k++)//λ ��λ�ȷ�
			{
				if(dma_font[i*width+j] & (0x80>>k))//����1 ��ʾ������ɫ
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
Description:��ʾ�ַ����ĺ���
param:row-->��ʼ�� col-->��ʼ�� font_color-->������ɫ
      back_color-->����ɫ fontsize-->�����С  str-->�ַ����׵�ַ
retval:None
Remarks:����0xa0�Ǻ���  С��0xa0���ַ�
*/
void LCDFONT_Display_Str(u16 row,u16 col,u16 font_color,u16 back_color,u8 fontsize,u8 *str)
{
	while(*str)
	{
		if(*str>0xa0)//����
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
	LCDFONT_Display_Str(35,20,RED,WHITE,24,(u8 *)"������������ϵͳ");
	LCDFONT_Display_Str(74,10,LIGHTGREEN,WHITE,16,(u8 *)"��ѡ���ŷ�ʽ��");
	LCDFONT_Display_Str(101,20,BRED,WHITE,16,(u8 *)"1.���뿪��");
	LCDFONT_Display_Str(128,20,BRED,WHITE,16,(u8 *)"2.ָ�ƿ���");
	LCDFONT_Display_Str(155,20,BRED,WHITE,16,(u8 *)"3.RFID������");
}


void LCD_Dis_RTC(void)
{
	RTC_GetTime(RTC_Format_BIN,&RTC_Time);
	RTC_GetDate(RTC_Format_BIN,&RTC_Date);
	sprintf((char *)display_buf,"20%d��%02d��%02d����%d %02dʱ%02d��%02d��",RTC_Date.RTC_Year,RTC_Date.RTC_Month,RTC_Date.RTC_Date,RTC_Date.RTC_WeekDay,RTC_Time.RTC_Hours,RTC_Time.RTC_Minutes,RTC_Time.RTC_Seconds);//�����ݴ���������
	LCDFONT_Display_Str(35,0,RED,WHITE,16,display_buf);
	LCDFONT_Display_Str(60,0,BLUE,WHITE,16,(u8 *)"����ѡ���ܣ�");
	LCDFONT_Display_Str(80,0,BRED,WHITE,16,(u8 *)"1.�޸�����");
	LCDFONT_Display_Str(100,0,BRED,WHITE,16,(u8 *)"2.ע��ָ��");
	LCDFONT_Display_Str(120,0,BRED,WHITE,16,(u8 *)"3.ɾ������ָ��");
	LCDFONT_Display_Str(140,0,BRED,WHITE,16,(u8 *)"4.��������");
	LCDFONT_Display_Str(160,0,BRED,WHITE,16,(u8 *)"5.ɾ��ָ��ָ��");
	LCDFONT_Display_Str(180,0,BRED,WHITE,16,(u8 *)"0.�˳�");
	TIM5_Delay_Ms(500);//0.5sˢ��һ��

}

void LCD_Dis_GIF(void)
{
	if(gif_flag)
	{
		//��������
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
		LCD_Clear(0,239,0,239,WHITE);//����
		lcd_flag =0;
	}
}

