#ifndef _LCD_H
#define _LCD_H

#include "stm32f4xx.h"

#define LCD_CS     PBout(12)
#define LCD_SCLK   PBout(13)
#define LCD_MOSI   PBout(15)
#define LCD_LEDK   PBout(1)
#define LCD_RESET  PBout(10)
#define LCD_DC     PBout(14)


/* ��ɫ���忪ʼ */
#define WHITE        0xFFFF
#define BLACK        0x0000	  
#define BLUE         0x001F  
#define BRED         0XF81F
#define GRED 			   0XFFE0
#define GBLUE			   0X07FF
#define RED          0xF800
#define MAGENTA      0xF81F
#define GREEN        0x07E0
#define CYAN         0x7FFF
#define YELLOW       0xFFE0
#define BROWN 			 0XBC40 //��ɫ
#define BRRED 			 0XFC07 //�غ�ɫ
#define GRAY  			 0X8430 //��ɫ
#define DARKBLUE     0X01CF	//����ɫ
#define LIGHTBLUE    0X7D7C	//ǳ��ɫ  
#define GRAYBLUE     0X5458 //����ɫ
#define LIGHTGREEN   0X841F //ǳ��ɫ
#define LGRAY 			 0XC618 //ǳ��ɫ(PANNEL),���屳��ɫ
#define LGRAYBLUE    0XA651 //ǳ����ɫ(�м����ɫ)
#define LBBLUE       0X2B12 //ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)
/* ��ɫ������� */
	
	
extern RTC_TimeTypeDef RTC_Time;//ʱ��ṹ�����
extern RTC_DateTypeDef RTC_Date;//���ڽṹ�����
extern u8 display_buf[100];
extern u8 gif_flag;
	
void LCD_Pin_Config(void);
void SPI_Send_Data(u8 data);
void Send_Data_LCD(u8 data);
void Send_Cmd_LCD(u8 Cmd);
void Send_Data_16b_LCD(u16 data);
void LCD_Set_Positon(u16 s_row,u16 e_row,u16 s_col,u16 e_col);
void LCD_Clear(u16 s_row,u16 e_row,u16 s_col,u16 e_col,u16 color);
void LCD_Init(void);
void LCD_Display_Point(u16 row,u16 col,u16 color);
void LCD_Display_Char(u16 row,u16 col,u16 font_color,u16 back_color,u8 fontsize,u8 *data);
void LCD_Display_Hz(u16 row,u16 col,u16 font_color,u16 back_color,u8 fontsize,u8 *data);
void LCD_Display_Pic(u16 row,u16 col,u8 *data);
void LCDFONT_Display_ASCII(u16 row,u16 col,u16 font_color,u16 back_color,u8 fontsize,u8 data);
void LCDFONT_Display_Font(u16 row,u16 col,u16 font_color,u16 back_color,u8 fontsize,u8 *data);
void LCDFONT_Display_Str(u16 row,u16 col,u16 font_color,u16 back_color,u8 fontsize,u8 *str);
	
void LCD_Dis_GUI(void);
void LCD_Dis_RTC(void);
void LCD_Dis_GIF(void);
	
#endif
