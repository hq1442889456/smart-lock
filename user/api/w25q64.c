#include "main.h"                  // Device header
/*
Function name:W25Q_Read_ID
Description:读取厂商ID
param:None
retval:None
Remarks:
指令--->0x90
*/
void W25Q_Read_ID(void)
{
	u16 id;
	W25_CS = 0;//拉低片选,开始通信
	SPI1_Echo(0x90);//指令
	SPI1_Echo(0x00);//24 位地址
	SPI1_Echo(0x00);
	SPI1_Echo(0x00);
	id = SPI1_Echo(0xff);//发送1同时接收id
	id = id<<8 | SPI1_Echo(0xff);
	W25_CS = 1;//拉高片选，结束通信
	printf("厂商ID：%#x\r\n",id);//以16进制打印，且打印符号
}

//写使能
void W25_Write_Enable(void)
{
	W25_CS = 0;//拉低片选，开始通信
	SPI1_Echo(0x06);//写使能指令
	W25_CS = 1;//拉高片选，结束通信
}

//禁止写使能
void W25_Write_Disable(void)
{
	W25_CS = 0;//拉低片选，开始通信
	SPI1_Echo(0x04);//禁止写使能指令
	W25_CS = 1;//拉高片选，结束通信
}
/*
Function name:W25_Read_Status
Description:W25Q64读取状态寄存器最低位
param:None
retval:u8 (1 代表忙，  0代表不忙 )
Remarks:
指令--->0x05
*/
u8 W25_Read_Status(void)
{
	u8 sta = 0;
	W25_CS = 0;//拉低片选，开始通信
	SPI1_Echo(0x05);//读取状态寄存器指令
	sta = SPI1_Echo(0xff);
	W25_CS = 1;//拉高片选，结束通信
	if(sta&0x01)
		return 1;
	return 0; 
}
/*
Function name:W25Q_Page_Write
Description:页写数据
param:addr-->待写入数据的flash的地址
			buf-->待读数据的数组首地址
      len--->写的数据长度(一次最多写入256个数据,u8范围是0-255 所以用u16)
retval:None
Remarks:
指令--->0x02
flash的地址：第一页 0-255，第二页256-511   ……
编程之前必须保证内存空间是 0XFF  即得进行擦除，可用全片擦除
*/
void W25Q_Page_Write(u32 addr,u8 *buf,u16 len)
{
	while(W25_Read_Status());//等待不忙
	W25_Write_Enable();//写使能
	W25_CS = 0;//拉低片选，开始通信
	SPI1_Echo(0x02);//页编程指令
	SPI1_Echo(addr>>16);//24 位地址 先发高八位
	SPI1_Echo(addr>>8);	//再发中八位
	SPI1_Echo(addr);    //再发低八位
	while(len--)
	{
		SPI1_Echo(*buf++);
	}
	W25_CS = 1;//结束通信
	while(W25_Read_Status());//等待写入完成
	printf("写入完成\r\n");
	W25_Write_Disable();//禁止写使能
}
/*
Function name:W25Q_Cross_Write
Description:跨页写数据
param:addr-->待写入数据的flash的地址
			buf-->待读数据的数组首地址
      len--->写的数据长度
retval:None
Remarks:
指令--->0x02
flash的地址：第一页 0-255，第二页256-511   ……
编程之前必须保证内存空间是 0XFF  即得进行擦除，可用全片擦除
*/
void W25Q_Cross_Write(u32 addr,u8 *buf,u32 len)
{
	u16 len_rest;
	len_rest = 256-addr%256;//剩余量 可用赋值法验证
	if(len<=len_rest)
	{
		W25Q_Page_Write(addr,buf,len);
	}
	else
	{
		while(1)
		{
			W25Q_Page_Write(addr,buf,len_rest);
			if(len_rest==len)
			{
				break;
			}
			addr+=len_rest;
			buf+=len_rest;
			len-=len_rest;
			if(len<=256)
			{
				len_rest = len;
			}
			else
			{
				len_rest=256;
			}
		}
	}
}




/*
Function name:W25_Read_Data
Description:读取数据
param:addr-->待读数据flash的地址
			buf-->存储读出数据数组的首地址
      len--->读数据的长度
retval:None
Remarks:
指令--->0x03
*/
void W25_Read_Data(u32 addr,u8 *buf,u32 len)
{
	W25_CS = 0;//拉低片选
	SPI1_Echo(0x03);//读数据指令
	SPI1_Echo(addr>>16);//24 位地址
	SPI1_Echo(addr>>8);
	SPI1_Echo(addr);
	while(len--)
	{
		*buf++=SPI1_Echo(0xff);
	}
	W25_CS = 1;//结束通信
//	printf("读取完成\r\n");
}
/*
Function name:W25Q_Sector_Erase
Description:扇区擦除
param:addr
retval:None
Remarks:
指令--->0x20
*/
void W25Q_Sector_Erase(u32 addr)
{
	W25_Write_Enable();//写使能
	while(W25_Read_Status());//等待不忙
	W25_CS = 0;//拉低片选，开始通信
	SPI1_Echo(0x20);//发送指令
	SPI1_Echo(addr>>16);//发送地址
	SPI1_Echo(addr>>8);
	SPI1_Echo(addr);
	W25_CS = 1;//拉高片选，结束通信
	while(W25_Read_Status());//等待擦除结束
	printf("擦除扇区结束\r\n");
}

/*
Function name:W25Q_Block_Erase
Description:块擦除
param:addr
retval:None
Remarks:
指令--->0xd8 64KB块擦除，因为Q64是16个扇区（4KB）为1块，16*4=64KB
*/
void W25Q_Block_Erase(u32 addr)
{
	W25_Write_Enable();//写使能
	while(W25_Read_Status());//等待不忙
	W25_CS = 0;//拉低片选，开始通信
	SPI1_Echo(0xd8);//发送指令
	SPI1_Echo(addr>>16);//发送地址
	SPI1_Echo(addr>>8);
	SPI1_Echo(addr);
	W25_CS = 1;//拉高片选，结束通信
	while(W25_Read_Status());//等待擦除结束
	printf("擦除块区结束\r\n");
}


/*
Function name:W25Q_Piece_Erase
Description:全片擦除
param:None
retval:None
Remarks:
指令--->0xC7/0x60
*/
void W25Q_Piece_Erase(void)
{
	W25_Write_Enable();//写使能
	while(W25_Read_Status());//等待不忙
	W25_CS=0;
	SPI1_Echo(0xc7);
	W25_CS=1;
	while(W25_Read_Status());//等待擦除完成
	printf("擦除完成\r\n");
}
