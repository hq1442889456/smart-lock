#include "main.h"                  // Device header
/*
Function name:W25Q_Read_ID
Description:��ȡ����ID
param:None
retval:None
Remarks:
ָ��--->0x90
*/
void W25Q_Read_ID(void)
{
	u16 id;
	W25_CS = 0;//����Ƭѡ,��ʼͨ��
	SPI1_Echo(0x90);//ָ��
	SPI1_Echo(0x00);//24 λ��ַ
	SPI1_Echo(0x00);
	SPI1_Echo(0x00);
	id = SPI1_Echo(0xff);//����1ͬʱ����id
	id = id<<8 | SPI1_Echo(0xff);
	W25_CS = 1;//����Ƭѡ������ͨ��
	printf("����ID��%#x\r\n",id);//��16���ƴ�ӡ���Ҵ�ӡ����
}

//дʹ��
void W25_Write_Enable(void)
{
	W25_CS = 0;//����Ƭѡ����ʼͨ��
	SPI1_Echo(0x06);//дʹ��ָ��
	W25_CS = 1;//����Ƭѡ������ͨ��
}

//��ֹдʹ��
void W25_Write_Disable(void)
{
	W25_CS = 0;//����Ƭѡ����ʼͨ��
	SPI1_Echo(0x04);//��ֹдʹ��ָ��
	W25_CS = 1;//����Ƭѡ������ͨ��
}
/*
Function name:W25_Read_Status
Description:W25Q64��ȡ״̬�Ĵ������λ
param:None
retval:u8 (1 ����æ��  0����æ )
Remarks:
ָ��--->0x05
*/
u8 W25_Read_Status(void)
{
	u8 sta = 0;
	W25_CS = 0;//����Ƭѡ����ʼͨ��
	SPI1_Echo(0x05);//��ȡ״̬�Ĵ���ָ��
	sta = SPI1_Echo(0xff);
	W25_CS = 1;//����Ƭѡ������ͨ��
	if(sta&0x01)
		return 1;
	return 0; 
}
/*
Function name:W25Q_Page_Write
Description:ҳд����
param:addr-->��д�����ݵ�flash�ĵ�ַ
			buf-->�������ݵ������׵�ַ
      len--->д�����ݳ���(һ�����д��256������,u8��Χ��0-255 ������u16)
retval:None
Remarks:
ָ��--->0x02
flash�ĵ�ַ����һҳ 0-255���ڶ�ҳ256-511   ����
���֮ǰ���뱣֤�ڴ�ռ��� 0XFF  ���ý��в���������ȫƬ����
*/
void W25Q_Page_Write(u32 addr,u8 *buf,u16 len)
{
	while(W25_Read_Status());//�ȴ���æ
	W25_Write_Enable();//дʹ��
	W25_CS = 0;//����Ƭѡ����ʼͨ��
	SPI1_Echo(0x02);//ҳ���ָ��
	SPI1_Echo(addr>>16);//24 λ��ַ �ȷ��߰�λ
	SPI1_Echo(addr>>8);	//�ٷ��а�λ
	SPI1_Echo(addr);    //�ٷ��Ͱ�λ
	while(len--)
	{
		SPI1_Echo(*buf++);
	}
	W25_CS = 1;//����ͨ��
	while(W25_Read_Status());//�ȴ�д�����
	printf("д�����\r\n");
	W25_Write_Disable();//��ֹдʹ��
}
/*
Function name:W25Q_Cross_Write
Description:��ҳд����
param:addr-->��д�����ݵ�flash�ĵ�ַ
			buf-->�������ݵ������׵�ַ
      len--->д�����ݳ���
retval:None
Remarks:
ָ��--->0x02
flash�ĵ�ַ����һҳ 0-255���ڶ�ҳ256-511   ����
���֮ǰ���뱣֤�ڴ�ռ��� 0XFF  ���ý��в���������ȫƬ����
*/
void W25Q_Cross_Write(u32 addr,u8 *buf,u32 len)
{
	u16 len_rest;
	len_rest = 256-addr%256;//ʣ���� ���ø�ֵ����֤
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
Description:��ȡ����
param:addr-->��������flash�ĵ�ַ
			buf-->�洢��������������׵�ַ
      len--->�����ݵĳ���
retval:None
Remarks:
ָ��--->0x03
*/
void W25_Read_Data(u32 addr,u8 *buf,u32 len)
{
	W25_CS = 0;//����Ƭѡ
	SPI1_Echo(0x03);//������ָ��
	SPI1_Echo(addr>>16);//24 λ��ַ
	SPI1_Echo(addr>>8);
	SPI1_Echo(addr);
	while(len--)
	{
		*buf++=SPI1_Echo(0xff);
	}
	W25_CS = 1;//����ͨ��
//	printf("��ȡ���\r\n");
}
/*
Function name:W25Q_Sector_Erase
Description:��������
param:addr
retval:None
Remarks:
ָ��--->0x20
*/
void W25Q_Sector_Erase(u32 addr)
{
	W25_Write_Enable();//дʹ��
	while(W25_Read_Status());//�ȴ���æ
	W25_CS = 0;//����Ƭѡ����ʼͨ��
	SPI1_Echo(0x20);//����ָ��
	SPI1_Echo(addr>>16);//���͵�ַ
	SPI1_Echo(addr>>8);
	SPI1_Echo(addr);
	W25_CS = 1;//����Ƭѡ������ͨ��
	while(W25_Read_Status());//�ȴ���������
	printf("������������\r\n");
}

/*
Function name:W25Q_Block_Erase
Description:�����
param:addr
retval:None
Remarks:
ָ��--->0xd8 64KB���������ΪQ64��16��������4KB��Ϊ1�飬16*4=64KB
*/
void W25Q_Block_Erase(u32 addr)
{
	W25_Write_Enable();//дʹ��
	while(W25_Read_Status());//�ȴ���æ
	W25_CS = 0;//����Ƭѡ����ʼͨ��
	SPI1_Echo(0xd8);//����ָ��
	SPI1_Echo(addr>>16);//���͵�ַ
	SPI1_Echo(addr>>8);
	SPI1_Echo(addr);
	W25_CS = 1;//����Ƭѡ������ͨ��
	while(W25_Read_Status());//�ȴ���������
	printf("������������\r\n");
}


/*
Function name:W25Q_Piece_Erase
Description:ȫƬ����
param:None
retval:None
Remarks:
ָ��--->0xC7/0x60
*/
void W25Q_Piece_Erase(void)
{
	W25_Write_Enable();//дʹ��
	while(W25_Read_Status());//�ȴ���æ
	W25_CS=0;
	SPI1_Echo(0xc7);
	W25_CS=1;
	while(W25_Read_Status());//�ȴ��������
	printf("�������\r\n");
}
